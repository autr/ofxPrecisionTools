#pragma once

#include "ofMain.h"

#define OFXKIT_WINDOWSTYLE_MAC 1
#define OFXKIT_WINDOWSTYLE_WIN 2
#define OFXKIT_WINDOWSTYLE_LINUX 3
#define OFXKIT_WINDOWSTYLE_PI 4

struct ofxKitWindowSettings {
    int titleBarHeight;
    int osStyle;
    float dpi;
    ofColor titleBarColor;
    ofColor backgroundColor;
    ofxKitWindowSettings() {
        titleBarHeight = 28;
#ifdef TARGET_OSX
        osStyle = OFXKIT_WINDOWSTYLE_MAC;
#elif defined(TARGET_LINUX)
        osStyle = OFXKIT_WINDOWSTYLE_LINUX;
#elif defined(TARGET_WIN32)
        osStyle = OFXKIT_WINDOWSTYLE_WIN;
#elif defined(TARGET_RASPBERRY_PI)
        osStyle = OFXKIT_WINDOWSTYLE_PI;
#endif
        dpi = ((ofAppGLFWWindow*)ofGetWindowPtr())->getPixelScreenCoordScale();
        titleBarColor = ofColor(0,0,0,0);
        backgroundColor = ofColor(40);
    }
};

class ofxKitWindow {
public:
    
    ofRectangle titleBar;
    ofRectangle bottomBar;
    ofRectangle rightBar;
    ofRectangle windowOrigin;
    ofPoint pressOrigin;
    ofxKitWindowSettings s;
    int w, h;
    bool isMoving, isResizingWidth, isResizingHeight;
    ofFbo actionsFbo;
    
    ofxKitWindow() {
        isMoving = false;
        isResizingWidth = false;
        isResizingHeight = false;
    }
    
    
    void shrink( ofRectangle & r, int i ) {
        r.x += i;
        r.y += i;
        r.width -= i * 2;
        r.height -= i * 2;
    }
    
    void draw() {
        
        if ((ofGetWidth() != w)||(ofGetHeight() != h)) {
            w = ofGetWidth();
            h = ofGetHeight();
            
            
            
            titleBar.set( 0, 0, w, s.titleBarHeight * s.dpi );
            float si = 10;
            bottomBar.set( 0, h - si, w, si);
            rightBar.set( w - si, 0, si, h);
        }
        
        
        ofSetColor( 0 );
        
        float dpi = s.dpi;
        float ro = 5 * dpi;
        
        ofFill();
        ofSetColor( s.backgroundColor );
        
        ofRectangle rect( 0, 0, ofGetWidth(), ofGetHeight() );
        ofDrawRectRounded( rect, ro );
        
        ofColor hi = s.backgroundColor;
        ofColor lo = s.backgroundColor;
        
        hi.setBrightness( hi.getBrightness() + 40 );
        lo.setBrightness( lo.getBrightness() - 40 );
        float lw = 0.5 * s.dpi;
        
        ofNoFill();
        ofSetLineWidth( lw * 2 );
        
        //        ofSetColor( lo );
        //        shrink(rect, lw);
        //        ofDrawRectRounded( rect, ro );
        ofSetColor( hi );
        shrink(rect, lw);
        ofDrawRectRounded( rect, ro );
        
        
        
        
        ofFill();
        ofSetColor( s.titleBarColor );
        ofDrawRectRounded( titleBar, ro, ro, 0, 0 );
        
        ofSetColor(255);
        if ((int)actionsFbo.getWidth() <= 0 || (int)actionsFbo.getHeight() <= 0) drawActions();
        float ax = 15 * dpi;
        float ay = s.titleBarHeight/2 * dpi;
        actionsFbo.draw(ax, ay);
        ofNoFill();
        
        
    }
    
    void drawActions() {
        
        float dpi = s.dpi;
        
        float r = 6 * dpi;
        float y = r;
        float d = 20 * dpi;
        
        float a = r;
        float b = r + d;
        float c = r + d + d;
        
        float ww = c + r;
        float hh = r * 2;
        actionsFbo.allocate((int)ww, (int)hh, GL_RGBA);
        
        if ((int)ww <= 0 || (int)hh <= 0) return;
        
        actionsFbo.begin();
        
        ofClear(255,0);
        ofFill();
        
        ofColor cr(252, 98, 93);
        ofColor co(254, 192, 65);
        ofColor cg(52, 203, 75);
        
        ofSetColor( cr );
        ofDrawCircle( a, y, r );
        ofSetColor( co );
        ofDrawCircle( b, y, r );
        ofSetColor( cg );
        ofDrawCircle( c, y, r );
        
        int da = 40;
        cr.setBrightness( cr.getBrightness() - da );
        co.setBrightness( co.getBrightness() - da );
        cg.setBrightness( cg.getBrightness() - da );
        
        
        ofNoFill();
        ofSetLineWidth( 1 * dpi );
        
       
//
//        ofSetColor( cr );
//        ofDrawCircle( a, y, r );
//        ofSetColor( co );
//        ofDrawCircle( b, y, r );
//        ofSetColor( cg );
//        ofDrawCircle( c, y, r );
        
        actionsFbo.end();
    }
    void dragged(int x, int y, int button){
        
        float wx = ofGetWindowPositionX();
        float wy = ofGetWindowPositionY();
        float ww = ofGetWindowWidth();
        float wh = ofGetWindowHeight();
        
        float px = pressOrigin.x;
        float py = pressOrigin.y;
        
        float ow = windowOrigin.width;
        float oh = windowOrigin.height;
        
        if (isMoving) ofSetWindowPosition(x + wx - px, y + wy - py);
        if (isResizingWidth) ofSetWindowShape( ow + x - px, wh );
        if (isResizingHeight) ofSetWindowShape( ww, oh + y - py );
        
    }
    
    void pressed(int x, int y, int button){
        
        isMoving = titleBar.inside( x, y);
        isResizingWidth = rightBar.inside( x, y) && !isMoving;
        isResizingHeight = bottomBar.inside( x, y) && !isMoving;
        
        float wx = ofGetWindowPositionX();
        float wy = ofGetWindowPositionY();
        float ww = ofGetWindowWidth();
        float wh = ofGetWindowHeight();
        
        windowOrigin.set( wx, wy, ww, wh );
        
        pressOrigin.x = x;
        pressOrigin.y = y;
        
    }
    void released(int x, int y, int button){
        
        if (isMoving) isMoving = false;
        if (isResizingWidth) isResizingWidth = false;
        if (isResizingHeight) isResizingHeight = false;
        
    }
    void moved( int x, int y ) {
        
    }
};
