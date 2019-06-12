
#pragma once
#include "ofMain.h"

#define PRECISION_COL 0
#define PRECISION_ROW 1

/*-- TODO :
 - Add callbacks for ofxFluidUi
 - Add an "errors" vector = undo change if not valid
 - Differentiate "fixed" and "percentage" for scaling
 
 --*/

class ofxPrecisionRow {
public:
    ofRectangle bounds;
    int type;
    vector<ofxPrecisionRow *> rows;
    vector<ofxPrecisionRow *> global;
    map<string, ofxPrecisionRow *> hash;
    float multiHeight, multiWidth;
    ofJson config;
    ofxPrecisionRow * parent;
    ofEvent<ofxPrecisionRow> added;
    int minPadding = 5;
    
    ofxPrecisionRow *& operator[] (size_t i) {
        return rows[i];
    }
    
    ofxPrecisionRow() {
        multiHeight = 1;
        multiWidth = 1;
        type = 0;
    };
    ofxPrecisionRow(int t, ofxPrecisionRow * p) {
        multiHeight = 1;
        multiWidth = 1;
        type = t;
        parent = p;
    };
    void set(float x, float y, float w, float h) {
        ofRectangle r(x, y, w, h);
        set(r);
    }
    void set(ofRectangle r) {
        bounds = r;
        if (type == PRECISION_ROW) amendWidths( (bounds.width - r.width) / rows.size() );
        if (type == PRECISION_COL) amendCols();
    }
    
    vector<vector<ofPoint>> points;
    
    void moved(int x, int y) {
        points.clear();
        for (auto & g : global) {
            ofPoint tr = g->bounds.getTopRight();
            ofPoint br = g->bounds.getBottomRight();
            ofPoint bl = g->bounds.getBottomLeft();
            
            bool inX = (x > tr.x - 10 && x < tr.x);
            bool inH = (y > tr.y && y < br.y );
            
            
            if (inX && inH) points.push_back( {tr, br} );
            
            bool inY = (y > bl.y - 10 && y < bl.y);
            bool inW = (x > bl.x && x < br.x);
            
            if (inY && inW) points.push_back( {bl, br} );
            
            
        }
    }
    
    
    void draw(int d = 0, bool iso = false) {
        
        
        ofColor c;
//        c.setHsb( ofMap(i, 0, rows.size(), 0, 255), 255, 255, 150);
        
        c = (type == PRECISION_COL) ? ofColor(0,255,255, 100) : ofColor(0,255,0, 100);
        
        ofSetColor(c);
        ofNoFill();
        ofSetLineWidth(2);
        
        ofPushMatrix();
        if (iso) ofTranslate(0,0,d * 60);
        ofRectangle r = bounds;
        r.x += 5;
        r.y += 5;
        r.width -= 10;
        r.height -= 10;
        ofDrawRectangle(r);
        string s = "x";
        s += ofToString(bounds.x) + " y";
        s += ofToString(bounds.y) + " w";
        s += ofToString(bounds.width) + " h";
        s += ofToString(bounds.height) + " ";
//        ofDrawBitmapString( s, bounds.getCenter() );
        
        if (parent) {
            ofSetColor(255,255,255,100);
            ofVec3f p = parent->bounds.getCenter();
            ofVec3f c = bounds.getCenter();
            p.z -= 60;
            ofDrawLine(c, p);
        }
        ofPopMatrix();
        
        int i = 0;
        d += 1;
        for (auto & ch : rows) ch->draw(d);
        
        for (auto & pp : points) {
            ofSetColor(255,0,0);
            ofDrawLine( pp[0].x, pp[0].y, pp[1].x, pp[1].y );
        }
    }
    
    void amendWidths(float size, int exclude = -1) {
        
        /*-- Iterate widths --*/
        
        int x = bounds.x;
        int y = bounds.y;
        int w;
        int h = bounds.height;
        for (int i = 0; i < rows.size(); i ++) {
            int w = rows[i]->bounds.width;
            if (i != exclude) w -= size;
            
            /*-- Trigger inner children --*/
            
            rows[i]->set(x, rows[i]->bounds.y, w, h);
            
            x += rows[i]->bounds.width;
        }
    }
    
    void amendCols() {
        
        float iter = 0;
        for (auto & c : rows) iter += c->multiHeight;
        float x = bounds.x;
        float y = bounds.y;
        float w = bounds.width;
        for (auto & c : rows) {
            float h = ( bounds.height / iter ) * c->multiHeight;
            c->set(x, y, w, h);
            y += c->bounds.height;
        }
    }
    
    void refresh() {
        
    }
    
    ofxPrecisionRow & add(int t, int idx = -1) {
        
        
        ofxPrecisionRow * ch = new ofxPrecisionRow(t, this);
        
        
        if (type == PRECISION_COL) {

            /*-- Insert new rect --*/
            
            if (idx > rows.size() || idx < 0) idx = rows.size();
            rows.insert(rows.begin() + idx, ch);
            amendCols();
        }
        
        if (type == PRECISION_ROW) {
        
            /*-- Create rect --*/
            
            float w = bounds.width / ( rows.size() + 1 );
            ch->set(bounds.x, bounds.y, w, bounds.height);
            
            /*-- Insert new rect --*/
            
            if (idx > rows.size() || idx < 0) idx = rows.size();
            rows.insert(rows.begin() + idx, ch);
            
            /*-- Iterate widths --*/
            
            amendWidths(( ch->bounds.width / (rows.size() - 1) ), idx);
        
            
        }
        
        getRoot()->global.push_back(ch);
        int depth = 0;
        getRoot()->hash[ofToString(getDepth(depth)) + "/" + ofToString(idx)] = ch;
        
        /*-- Return vector --*/
        
        return * ch;
    }
    
    int & getDepth(int & d) {
        if (!parent) return d;
        d += 1;
        return parent->getDepth(d);
    }
    
    ofxPrecisionRow * getRoot() {
        if (parent) return parent;
        return this;
    }
    
    void onAdded( ofxPrecisionRow & ch ) {
//        ofLog() << "ADDED";
//        ch.set(200,200, 400,400);
//        ofxPrecisionRow & ref = ch;
//        ofNotifyEvent(added, ref);
    }
    
    //    vector<ofxPrecisionRow> setWidth(int idx, float w) {
    //
    //        //        if (idx == cols.size() - 1) return {};
    //        //
    //        //        /*-- Set rect width --*/
    //        //
    //        //        w *= bounds.width; // new width
    //        //        float rm = w - rows[idx].width;
    //        //        cols[idx].width = w;
    //        //
    //        //        /*-- Iterate widths --*/
    //        //
    //        //        for (int i = idx + 1; i < cols.size(); i ++) {
    //        //            cols[i].width -= rm/ (cols.size() - 1);
    //        //        }
    //        //        /*-- Iterate X positions --*/
    //        //
    //        //        width = 0;
    //        //        for (auto & c : cols) {
    //        //            c.x = bounds.x + width;
    //        //            width += c.width;
    //        //        }
    //        return chil;
    //    }
};
