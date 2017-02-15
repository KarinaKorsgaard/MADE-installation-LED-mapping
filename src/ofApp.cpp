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
    
    ofParameterGroup g;
    g.add(u_01.set("u_01",0.,0.,10.));
    g.add(u_02.set("u_02",0.,0.,10.));
    g.add(u_03.set("u_03",0.,0.,10.));
    g.add(u_04.set("u_04",0.,0.,10.));
    g.add(u_color.set("u_color",ofColor(255,255,255),ofColor(0,0,0),ofColor(255,255,255)));
    
    gui.setup(g);
    
    gui.loadFromFile("settings.xml");
    
    config.loadFile("config.xml");
    for(int i = 0; i<strips.size();i++){
        
        
        config.pushTag("strip"+ofToString(i));
        strips[i].pos.x = config.getValue("x", 0,0);
        strips[i].pos.y = config.getValue("y", 0,0);
        strips[i].rotation = config.getValue("rotation", 0,0);
        config.popTag();

    }
    
    sender.setup("localhost", 5000);
}

//--------------------------------------------------------------
void ofApp::update(){

    fbo.begin();
    
    shader.begin();
    shader.setUniform1f("iGlobalTime", ofGetElapsedTimef());
    shader.setUniform2f("iResolution", fbo.getWidth(),fbo.getHeight());
    
    shader.setUniform1f("u_01", u_01);
    shader.setUniform1f("u_02", u_02);
    shader.setUniform1f("u_03", u_03);
    shader.setUniform1f("u_04", u_04);
    shader.setUniform3f("u_color", ofVec3f(u_color->r/255.0f,u_color->g/255.0f,u_color->b/255.0f));
    fbo.draw(0, 0);
    
    shader.end();

    
    fbo.end();
    
    ofPixels pix;
    fbo.readToPixels(pix);
    for(int u = 0; u<strips.size();u++){
        for(int i = 0; i<strips[u].num_leds;i++){
            ofColor c = pix.getColor(strips[u].pos.x /10, ( strips[u].pos.y+i*5 + i*ledSpacing ) /10);
            colorRgbw rgbw = rgbToRgbw(c.r, c.g, c.b);
            strips[u].data[i*3+0]=rgbw.red;
            strips[u].data[i*3+1]=rgbw.green;
            strips[u].data[i*3+2]=rgbw.blue;
            strips[u].data[i*3+3]=rgbw.white;
        }
    }
    
    ofSetWindowTitle(ofToString(ofGetFrameRate(),1));
    
    
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

    ofBackground(0);
    
    if(debug){
        fbo.draw(fbo.getWidth()*10, 0,fbo.getWidth()*10,fbo.getHeight()*10);
        
        ofPushStyle();
        
        ofFill();
        
        for(auto s: strips){
            for(int i = 0; i<s.num_leds;i++){
                ofFill();
                ofSetColor(s.data[i*3] , s.data[i*3+1] , s.data[i*3+2], s.data[i*3+3] );
                ofDrawRectangle(s.pos.x, s.pos.y+i*5 + i*ledSpacing,5,5);
            }
            ofSetColor(ofColor::black);
            ofNoFill();
            ofRectangle r = ofRectangle(s.pos.x-1,s.pos.y,7,5*s.num_leds);
            ofDrawRectangle(r);
            
        }
        ofPopStyle();
    }
    
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
    if(mouseDown){
        for(int i = 0; i<strips.size();i++){
            ofRectangle r = ofRectangle(strips[i].pos.x-2,strips[i].pos.y,9,5*strips[i].num_leds);
            if(r.inside(x, y))moving=i;
        }
        if(moving!=-1)strips[moving].pos = ofVec2f(x,y);
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
