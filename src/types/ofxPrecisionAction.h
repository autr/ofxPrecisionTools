#pragma once


struct ofxPrecisionAction {
    ofRectangle oBounds;
    int type;
    ofPoint oPress;
    vector<float> mHeights;
    vector<float> mWidths;
    ofxPrecisionGrid * unit;
    
    ofxPrecisionAction(int t,  ofxPrecisionGrid * u, ofPoint p, vector<float> mW, vector<float> mH) {
        type = t;
        unit = u;
        oPress = p;
        mWidths = mW;
        mHeights = mH;
        oBounds = u->bounds;
    }
};