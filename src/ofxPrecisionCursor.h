#pragma once

#include "ofMain.h"
#include "ofxSVG.h"

class ofxPrecisionCursor {
public:
    ofDirectory d;
    vector<string> names;
    map<string, ofxSVG> svg;
    map<string, ofTexture> t;
    string curr;
    ofxPrecisionCursor();
    
    void random();
    
    void draw(float x, float y);
};
