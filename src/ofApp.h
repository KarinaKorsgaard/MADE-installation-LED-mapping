#pragma once

#include "ofMain.h"
#include "ofxAutoReloadedShader.h"
#include "ofxPostProcessing.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

#define ledSpacing 1

struct LEDstrip{
    int num_leds;
    ofPoint pos;
    float rotation;
    unsigned char data[512];
    string ip;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofFbo fbo;
    vector<LEDstrip>strips;
    ofxAutoReloadedShader shader;
    ofxPostProcessing post;
    ofxXmlSettings config;
    
    ofxOscSender sender;
    int moving;
    bool mouseDown;
    
    ofxPanel gui;
    ofParameter<float>u_01,u_02,u_03,u_04;
    ofParameter<ofColor>u_color;
    bool debug;
    
    
    
    struct colorRgbw {
        unsigned int   red;
        unsigned int   green;
        unsigned int   blue;
        unsigned int   white;
    };
    
    // The saturation is the colorfulness of a color relative to its own brightness.
    unsigned int saturation(colorRgbw rgbw) {
        // Find the smallest of all three parameters.
        float low = min(rgbw.red, min(rgbw.green, rgbw.blue));
        // Find the highest of all three parameters.
        float high = max(rgbw.red, max(rgbw.green, rgbw.blue));
        // The difference between the last two variables
        // divided by the highest is the saturation.
        return round(100 * ((high - low) / high));
    }
    
    // Returns the value of White
    unsigned int getWhite(colorRgbw rgbw) {
        return (255 - saturation(rgbw)) / 255 * (rgbw.red + rgbw.green + rgbw.blue) / 3;
    }
    
    // Use this function for too bright emitters. It corrects the highest possible value.
    unsigned int getWhite(colorRgbw rgbw, int redMax, int greenMax, int blueMax) {
        // Set the maximum value for all colors.
        rgbw.red = (float)rgbw.red / 255.0 * (float)redMax;
        rgbw.green = (float)rgbw.green / 255.0 * (float)greenMax;
        rgbw.blue = (float)rgbw.blue / 255.0 * (float)blueMax;
        return (255 - saturation(rgbw)) / 255 * (rgbw.red + rgbw.green + rgbw.blue) / 3;
        return 0;
    }
    
    // Example function.
    colorRgbw rgbToRgbw(unsigned int red, unsigned int green, unsigned int blue) {
        unsigned int white = 0;
        colorRgbw rgbw = {red, green, blue, white};
        rgbw.white = getWhite(rgbw);
        return rgbw;
    }
    
    // Example function with color correction.
    colorRgbw rgbToRgbw(unsigned int red, unsigned int redMax,
                        unsigned int green, unsigned int greenMax,
                        unsigned int blue, unsigned int blueMax) {
        unsigned int white = 0;
        colorRgbw rgbw = {red, green, blue, white};
        rgbw.white = getWhite(rgbw, redMax, greenMax, blueMax);
        return rgbw;
    }
    
};



