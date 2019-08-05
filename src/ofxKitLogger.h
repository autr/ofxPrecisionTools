#pragma once

#include "ofMain.h"

class ofxKitLogger : public ofBaseLoggerChannel {
private:
    
    ofConsoleLoggerChannel consoleLogger;
    ofFileLoggerChannel fileLogger;
    ofFbo fbo;
    int cw;
    int ch;
    bool word;
    int max;
    int height;
    bool isnew;
    ofRectangle bounds;
    string console;
    static ofPtr<ofxKitLogger> logger;
    static ofxKitLogger *logPtr;
    ofxKitLogger() {
        cw = 8;
        ch = 11;
        console = "";
        word = true;
        max = 20;
        height = 0;
        isnew = false;
    }
    
public:

    virtual ~ofxKitLogger() {}
    
    static ofPtr<ofxKitLogger> &getLogger() {
        if(logPtr==NULL) {
            logPtr = new ofxKitLogger();
            logger = ofPtr<ofxKitLogger>(logPtr);
        }
        
        
        return logger;
    }
    
    ofRectangle getBounds() {
        return bounds;
    }
    
    void log(ofLogLevel level, const string & module, const string & message) {
        
        
        string l = "";
        if (level == OF_LOG_NOTICE) l = "[i] ";
        if (level == OF_LOG_ERROR) l = "[e] ";
        l += message;

        console += l + "\n";
        
        int w = bounds.getWidth();
        
        if (!w) return;
        if (w == 0) return;
        
        int ix = 0;
        
        vector<int> breaks;
        
        int wi = 0;
        
        for (int i = 0; i < console.size(); i++) {
            char c = console[i];
            if (c == ' ')  wi = i;
            if ( c == '\n' ) {
                ix = 0;
            } else if ( (ix + cw) > w ) {
                ix = (i - wi) * cw;
                breaks.push_back( (word) ? wi : i );
            } else {
                ix += cw;
            }
        }
        
        int iadd = 0;
        for(int i = 0; i < breaks.size(); i++) {
            int index = breaks[i];
            if (!word) console.insert( index  , "\n");
            if (word) console.replace(index, 1, "\n");
        }
        
        int il = ofSplitString(console, "\n").size();
        while ( il > max) {
            console.erase(0, console.find("\n") + 1);
            il -= 1;
        }
    
        height = (il - 1) * 8 * 1.7 - 1;
        
        isnew = true;
        
        consoleLogger.log(level,module, message);
        fileLogger.log(level,module, message);
        
    }
    
    void log(ofLogLevel logLevel, const string & module, const char* format, ...) {
        va_list args;
        va_start(args, format);
        log(logLevel, module, format);
        va_end(args);
        
    }
    
    void log(ofLogLevel logLevel, const string & module, const char* format, va_list args) {
        
        consoleLogger.log(logLevel,module, format, args);
        fileLogger.log(logLevel,module, format, args);
    }
    
    void draw(ofRectangle r) {
        return draw(r.x, r.y, r.getWidth(), r.getHeight());
    }
    
    void draw(int x, int y, int w, int h) {
        
        bounds.set(x,y,w, h);
        
        if (w <= 0) return;
        
        if (w != fbo.getWidth() || height != fbo.getHeight() ) {
            fbo.allocate(w, height, GL_RGBA, 1);
        }
        
        if (isnew) {
            fbo.begin();
            ofClear(255,0);
            ofSetColor(255);
            ofDrawBitmapString(console, 0, ch );
            fbo.end();
            isnew = false;
        }
        
        ofSetColor(255);
        ofNoFill();
        fbo.draw(x,y);
        ofDrawRectangle( x, y, fbo.getWidth(), fbo.getHeight() );
    }
    
};
