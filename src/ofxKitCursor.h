#pragma once

#include "ofMain.h"

enum Cursor {
    openHand, closedHand, arrowCursor, resizeLeftRight
};
//#include "ofxSVG.h"

class ofxKitCursor {
private:
    static ofPtr<ofxKitCursor> instance;
    static ofxKitCursor *instancePtr;
public:
    ofDirectory d;
    vector<string> names;
    map<string, ofTexture> t;
    string curr;
    ofxKitCursor();
    
    void random();
    
    void draw(float x, float y);
    static void set(string s);
    
    virtual ~ofxKitCursor();
    static ofPtr<ofxKitCursor> &getInstance();
    
    
};
