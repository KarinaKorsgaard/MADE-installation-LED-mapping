#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    for(int i = 0; i<5;i++){
        LEDstrip s;
        s.num_leds = floorf(512/4);
        s.pos = ofPoint(i*20,0);
        s.rotation = 0;
        s.ip = "2.0.0.0";
        strips.push_back(s);
    }
    fbo.allocate((floorf(512/4)*3 + 100*ledSpacing ) /10,  (floorf(512/4)*5 + floorf(512/4)*ledSpacing ) /10);
    
    shader.load("glow");
    
    // setup gui
    ofParameterGroup g;
    g.add(u_01.set("amount",0.001,0.0,10.));
    g.add(u_02.set("speed",0.,0.,10.));
    g.add(u_color.set("u_color",ofColor(255,255,255),ofColor(0,0,0),ofColor(255,255,255)));
    ofParameterGroup colorCorrection;
    colorCorrection.setName("color correction");
    colorCorrection.add(redmax.set("red max", 0, 0, 255));
    colorCorrection.add(greenmax.set("green max", 0, 0, 255));
    colorCorrection.add(bluemax.set("blue max", 0, 0, 255));
    gui.setup(g);
    gui.add(colorCorrection);
    gui.loadFromFile("settings.xml");
    
    // load led strip positions from xml
    config.loadFile("config.xml");
    for(int i = 0; i<strips.size();i++){
        config.pushTag("strip"+ofToString(i));
        strips[i].pos.x = config.getValue("x", 0,0);
        strips[i].pos.y = config.getValue("y", 0,0);
        strips[i].rotation = config.getValue("rotation", 0,0);
        config.popTag();
    }
    
    // setup osc sender
    sender.setup("localhost", 5000);
    
    
    debug = true;
}

//--------------------------------------------------------------
void ofApp::update(){

    ofSetWindowTitle(ofToString(ofGetFrameRate(),1));
    
    // draw to framebuffer. Here we can draw anything. Atm im just using the glow shader. This could also be a video, image or whatever.
    fbo.begin();
    
    shader.begin();
    shader.setUniform1f("iGlobalTime", ofGetElapsedTimef());
    shader.setUniform2f("iResolution", fbo.getWidth(),fbo.getHeight());
    shader.setUniform1f("u_01", u_01);
    shader.setUniform1f("u_02", u_02);
    shader.setUniform3f("u_color", ofVec3f(u_color->r/255.0f,u_color->g/255.0f,u_color->b/255.0f));
    fbo.draw(0, 0);
    shader.end();
    
    fbo.end();
    
    // read fbo pixelvalues to led-strip data
    ofPixels pix;
    fbo.readToPixels(pix);
    for(int u = 0; u<strips.size();u++){
        for(int i = 0; i<strips[u].num_leds;i++){
            ofColor c = pix.getColor(strips[u].pos.x /10, ( strips[u].pos.y+i*5 + i*ledSpacing ) /10);
            colorRgbw rgbw = rgbToRgbw(c.r, redmax,c.g, greenmax,c.b, bluemax);
            strips[u].data[i*3+0]=rgbw.red;
            strips[u].data[i*3+1]=rgbw.green;
            strips[u].data[i*3+2]=rgbw.blue;
            strips[u].data[i*3+3]=rgbw.white;
        }
    }
    
    // send osc messages:
    for(int i = 0;i<strips.size();i++){
        ofxOscMessage m;
        m.setAddress("strip"+ofToString(i));
        for(int j = 0; j<512;j++){
            m.addIntArg(strips[i].data[j]);
        }
        sender.sendMessage(m);
        m.clear();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(ofColor::darkGray);
    if(debug){
        // draw raw fbo
        fbo.draw(fbo.getWidth()*10, 0,fbo.getWidth()*10,fbo.getHeight()*10);
        ofPushStyle();
        ofFill();
        
        // draw each led
        for(auto s: strips){
            ofSetColor(ofColor::black);
            ofRectangle r = ofRectangle(s.pos.x-1,s.pos.y,7,5*s.num_leds+s.num_leds*ledSpacing);
            ofDrawRectangle(r);
            for(int i = 0; i<s.num_leds;i++){
                ofSetColor(s.data[i*3] , s.data[i*3+1] , s.data[i*3+2], s.data[i*3+3] );
                ofDrawRectangle(s.pos.x, s.pos.y+i*5 + i*ledSpacing,5,5);
            }
        }
        ofPopStyle();
    }
    
    // draw gui
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'd')debug = !debug;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    // change position of led strip
    if(mouseDown){
        for(int i = 0; i<strips.size();i++){
            ofRectangle r = ofRectangle(strips[i].pos.x-2,strips[i].pos.y,9,5*strips[i].num_leds);
            if(r.inside(x, y))moving=i;
        }
        if(moving!=-1)strips[moving].pos = ofVec2f(x,y-strips[moving].pos.y);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    mouseDown =true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    mouseDown = false;
    moving = -1;
    
    // save new position to config.xml
    config.clear();
    for(int i =0; i<strips.size();i++){
        config.addTag("strip"+ofToString(i));
        config.pushTag("strip"+ofToString(i));
        config.addValue("x", strips[i].pos.x);
        config.addValue("y", strips[i].pos.y);
        config.addValue("rotation", strips[i].rotation);
        config.popTag();
    }
    config.save("config.xml");
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
