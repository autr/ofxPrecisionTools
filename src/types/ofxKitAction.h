#pragma once


struct ofxKitAction {
    ofRectangle oBounds;
    int type;
    ofPoint oPress;
    vector<float> mHeights;
    vector<float> mWidths;
    ofxKitGrid * unit;
    
    ofxKitAction(int t,  ofxKitGrid * u, ofPoint p, vector<float> mW = {}, vector<float> mH = {}) {
        type = t;
        unit = u;
        oPress = p;
        mWidths = mW;
        mHeights = mH;
        oBounds = u->bounds;
    }
};
