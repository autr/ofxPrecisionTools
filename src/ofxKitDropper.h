
#pragma once
#include "ofMain.h"
#include "ofxKitGrid.h"

class ofxKitDropper {
public:
    ofxKitRow * row;
    ofxKitRow * source;
    ofxKitDropper(ofxKitRow * r) {
        row = r;
        source = new ofxKitDropper();
    }
    
    
};
