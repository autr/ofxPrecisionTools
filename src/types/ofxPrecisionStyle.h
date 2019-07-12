#pragma once

struct ofxPrecisionGridStyle {
    ofColor defaultColor;
    ofColor hoverColor;
    ofColor dropColor;
    float minWidth;
    float minHeight;
    ofxPrecisionGridStyle() {
        defaultColor = ofColor(0,120,120, 255);
        hoverColor = ofColor(255, 255, 255, 255);
        dropColor = ofColor(150, 255, 255, 100);
        minWidth = 10;
        minHeight = 10;
    }
};
