#include "ofxPrecisionCursor.h"
#import <Cocoa/Cocoa.h>


ofxPrecisionCursor::ofxPrecisionCursor() {
    
//    ofHideCursor();
    [[NSCursor pointingHandCursor] set];

    curr = "openhand";
    d.open("mac-cursors/svg");
    for (auto & f : d.getFiles()) {
        
        string e = f.getExtension();
        string n = f.getFileName();
        
        if (e == "svg") {
            ofxSVG s;
            s.load( f.getAbsolutePath() );
            n = n.substr(0, n.size() - 4);
            svg[ n] = s;
            ofFbo f;
            ofEnableAlphaBlending();
            f.allocate( 70, 70, GL_RGBA, 8);
            f.begin();
            ofClear(0,0,0,0);
            ofNoFill();
            ofScale(2,2,1);
            ofSetColor(0,0,0,0);
            svg[ n ].draw();
            f.end();
            names.push_back(n);
            t[n] = f.getTexture();
        }
    }
}

void ofxPrecisionCursor::random() {
    curr = names[ ofRandom(0, names.size()) ];
}

void ofxPrecisionCursor::draw(float x, float y) {
    
//    [[NSCursor openHand] set];
//    [[NSCursor closedHand] set];
//    [[NSCursor arrowCursor] set];
//    [[NSCursor resizeLeftRight] set];

//    ofPushMatrix();
//    ofTranslate( x - 25, y - 25);
//    ofSetColor(255);
//    t[ curr ].draw( 0, 0 );
//    ofPopMatrix();
}
