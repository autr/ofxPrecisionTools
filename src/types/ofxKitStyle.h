#pragma once

struct ofxKitGridStyle {
    ofColor defaultColor;
    ofColor hoverColor;
    ofColor dropColor;
    float minWidth;
    float minHeight;
    ofxKitGridStyle() {
        defaultColor = ofColor(0,120,120, 255);
        hoverColor = ofColor(255, 255, 255, 255);
        dropColor = ofColor(150, 255, 255, 100);
        minWidth = 10;
        minHeight = 10;
    }
};
