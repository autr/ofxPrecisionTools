#pragma once

#include "ofMain.h"
#include "ofxSVG.h"

class ofxKitCursor {
public:
    ofDirectory d;
    vector<string> names;
    map<string, ofxSVG> svg;
    map<string, ofTexture> t;
    string curr;
    ofxKitCursor();
    
    void random();
    
    void draw(float x, float y);
};
