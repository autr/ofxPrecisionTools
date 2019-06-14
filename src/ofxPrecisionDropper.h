
#pragma once
#include "ofMain.h"
#include "ofxPrecisionGrid.h"

class ofxPrecisionDropper {
public:
    ofxPrecisionRow * row;
    ofxPrecisionRow * source;
    ofxPrecisionDropper(ofxPrecisionRow * r) {
        row = r;
        source = new ofxPrecisionDropper();
    }
    
    
};
