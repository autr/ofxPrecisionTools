
#pragma once
#include "ofMain.h"

#define PRECISION_COL 0
#define PRECISION_ROW 1

#define PRECISION_W 0 // width
#define PRECISION_H 1 // height
#define PRECISION_M 2 // margin
#define PRECISION_PL 3 // padding L
#define PRECISION_PT 4 // padding T
#define PRECISION_PR 5 // padding R
#define PRECISION_PB 6 // padding B

/*-- TODO :
 - Add callbacks for ofxFluidUi
 - Add an "errors" vector = undo change if not valid
 - Differentiate "fixed" and "percentage" for scaling
 
 --*/
class ofxPrecisionRow;

struct ofxPrecisionAction {
    ofRectangle oBounds;
    int type;
    ofPoint oPress;
    vector<float> mHeights;
    vector<float> mWidths;
    ofxPrecisionAction(int t, ofRectangle b, ofPoint p, vector<float> mW, vector<float> mH) {
        type = t;
        oBounds = b;
        oPress = p;
        mWidths = mW;
        mHeights = mH;
    }
};

class ofxPrecisionRow {
public:
    
    ofRectangle bounds;
    
    int type;
    float mH, mW;
    float minPadding;
    int uId;
    string name;
    
    ofxPrecisionRow * parent;
    vector<ofxPrecisionRow *> inner;
    vector<ofxPrecisionRow *> global;
    map<int, ofxPrecisionRow *> gHash;
    
    vector<ofxPrecisionAction> actions;
    
    ofEvent<ofxPrecisionRow> added;
    
    
    ofxPrecisionRow *& operator[] (size_t i) {
        return inner[i];
    }
    
    ofxPrecisionRow() {
        mH = 1;
        mW = 1;
        type = 0;
    };
    ofxPrecisionRow(int t, ofxPrecisionRow * p) {
        mH = 1;
        mW = 1;
        type = t;
        parent = p;
    };
    void set(float x, float y, float w, float h) {
        ofRectangle r(x, y, w, h);
        set(r);
    }
    void set(ofRectangle r) {
        minPadding = 5;
        uId = 0;
        name = getName(name);
        bounds = r;
        
        /*-- Important for Fluid --*/
        
//        if (parent) {
//            mH = r.height / parent->bounds.height;
//            mW = r.width / parent->bounds.width;
//
//        } else {
//            mH = 1;
//            mW = 1;
//        }
        
        
        /*-- /Fluid --*/
        
        if (type == PRECISION_ROW) amendWidths( (bounds.width - r.width) / inner.size() );
        if (type == PRECISION_COL) amendCols();
    }
    void setWidth(float w) {
        set(bounds.x, bounds.y, w, bounds.height);
    }
    void setHeight(float h) {
        set(bounds.x, bounds.y, bounds.width, h);
    }
    
    ofxPrecisionRow * get(vector<int> idx) {
        ofxPrecisionRow * u = getRoot();
        for (auto & i : idx) {
            int ii = i;
            if (ii < 0) ii = 0;
            if (ii >= u->inner.size()) ii = u->inner.size() - 1;
            if (u->inner.size() > 0) u = u->inner[ii];
        }
        return u;
    }
    
    
    vector<float> getWidths() {
        vector<float> w;
        if (!parent) return w;
        for (auto & u : parent->inner) {
            w.push_back(u->mW);
        }
        return w;
    }
    vector<float> getHeights() {
        vector<float> h;
        if (!parent) return h;
        for (auto & u : parent->inner) {
            h.push_back(u->mH);
        }
        return h;
    }
    
    void moved(int x, int y) {
        ofPoint p(x,y);
        for (auto & g : global) {
            g->actions.clear();
            ofPoint tr = g->bounds.getTopRight();
            ofPoint br = g->bounds.getBottomRight();
            ofPoint bl = g->bounds.getBottomLeft();
            
            /*-- width drag --*/
            
            bool inX = (x > tr.x - 10 && x < tr.x);
            bool inH = (y > tr.y && y < br.y );
            bool isLast = (g->getIndex() == g->parent->inner.size() - 1);
            
            if (inX && inH && !isLast) {
                vector<float> mWidths = g->getWidths();
                vector<float> mHeights = g->getHeights();
                ofxPrecisionAction ww(PRECISION_W, g->bounds, p, mWidths, mHeights);
                g->actions.push_back(ww);
            }
            
            /*-- height drag --*/
            
            bool inY = (y > bl.y - 10 && y < bl.y);
            bool inW = (x > bl.x && x < br.x);
            
            if (inY && inW && !isLast) {
                vector<float> mWidths = g->getWidths();
                vector<float> mHeights = g->getHeights();
                ofxPrecisionAction hh(PRECISION_H, g->bounds, p, mWidths, mHeights);
                g->actions.push_back(hh);
            }
            
        }
    }
    
    void pressed(int x, int y) {
        for (auto & g : global) {
            for (auto & a : g->actions) {
                a.oPress.x = x;
                a.oPress.y = y;
            }
        }
    }
    
    void dragged(int x, int y) {
        
        bool isShift = ofGetKeyPressed(OF_KEY_SHIFT);
        bool isAlt = ofGetKeyPressed(OF_KEY_ALT);
        
        for (auto & u : global) {
            for (auto & a : u->actions) {
                if (a.type == PRECISION_W) {
//                    setWidth( a.oBounds.width - (a.oPress.x - x) );
                }
                if (a.type == PRECISION_H) {
                    float pH = u->parent->bounds.height; // parent height
                    float offset = (a.oPress.y - y) / pH; // Y multiple
                    int idx = u->getIndex();
                    
                    /*-- Adjust divider only --*/
                    
                    if (!isShift && !isAlt) {
                        for (int i = idx; i < a.mHeights.size(); i++) {
                            ofxPrecisionRow * sib = u->parent->inner[i];
                            float h = a.mHeights[i];
                            sib->mH = h;
                            if (i == idx) sib->mH = h - offset;
                            if (i == idx + 1) sib->mH = h + offset;
                        }
                    }

                    /*-- Squash following siblings --*/

                    if (isShift && !isAlt) {
                        for (int i = idx; i < a.mHeights.size(); i++) {
                            ofxPrecisionRow * sib = u->parent->inner[i];
                            float h = a.mHeights[i];
                            if (i == idx) {
                                h -= offset;
                                sib->mH = h;
                            } else {
                                h += offset / (float)( a.mHeights.size() - idx - 1 );
                                sib->mH = h;
                            }
                        }
                    }
                    
                    /*-- Squash preceding siblings --*/
                    
                    if (isShift && isAlt) {
                        for (int i = 0; i < a.mHeights.size(); i++) {
                            ofxPrecisionRow * sib = u->parent->inner[i];
                            float h = a.mHeights[i];
                            sib->mH = h;
                            if (i < idx + 1) sib->mH = h - (offset / (float)( idx + 1) );
                            if (i == idx + 1) sib->mH = h + offset;
                        }
                    }
                    u->parent->amendCols();
                }
            }
        }
    }
    void released(int x, int y) {
//        actions.clear();
    }
    
    
    void draw(int d = 0, bool iso = false) {
        
        
        ofColor c;
        //        c.setHsb( ofMap(i, 0, inner.size(), 0, 255), 255, 255, 150);
        
        c = (type == PRECISION_COL) ? ofColor(0,255,255, 100) : ofColor(0,255,0, 100);
        
        ofSetColor(c);
        ofNoFill();
        ofSetLineWidth(2);
        
        ofPushMatrix();
        if (iso) {
            ofTranslate(0,0,d * 60);
        }
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
        for (auto & ch : inner) ch->draw(d, iso);
        
    }
    
    void amendWidths(float size, int exclude = -1) {
        
        /*-- Iterate widths --*/
        
        int x = bounds.x;
        int y = bounds.y;
        int w;
        int h = bounds.height;
        for (int i = 0; i < inner.size(); i ++) {
            int w = inner[i]->bounds.width;
            if (i != exclude) w -= size;
            
            /*-- Trigger inner children --*/
            
            inner[i]->set(x, inner[i]->bounds.y, w, h);
            
            x += inner[i]->bounds.width;
        }
    }
    
    void amendCols() {
        
        float iter = 0;
        for (auto & c : inner) iter += c->mH;
        float x = bounds.x;
        float y = bounds.y;
        float w = bounds.width;
        for (auto & c : inner) {
            float hh = c->mH;
//            hh -= fmod(hh, 0.05);
            float h = ( bounds.height / iter ) * hh;
//            h -= fmod(h, 10);
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
            
            if (idx > inner.size() || idx < 0) idx = inner.size();
            inner.insert(inner.begin() + idx, ch);
            ch->mH = 1;
            amendCols();
        }
        
        if (type == PRECISION_ROW) {
            
            
            /*-- Insert new rect --*/
            
            if (idx > inner.size() || idx < 0) idx = inner.size();
            inner.insert(inner.begin() + idx, ch);
            
            /*-- Create rect --*/
            
            float w = bounds.width / ( inner.size() );
            ch->set(bounds.x, bounds.y, w, bounds.height);
            
            /*-- Iterate widths --*/
            
            amendWidths(( ch->bounds.width / (inner.size() - 1) ), idx);
            
            
        }
        
        ofxPrecisionRow * root = getRoot();
        root->global.push_back(ch);
        while ( root->gHash.find(uId) != root->gHash.end() ) uId += 1;
        getRoot()->gHash[uId] = ch;
        
        /*-- Return vector --*/
        
        return * ch;
    }
    
    int getIndex() {
        ofxPrecisionRow * t = this;
        auto it = find(parent->inner.begin(), parent->inner.end(), t);
        if (it != parent->inner.end()) {
            return distance(parent->inner.begin(), it);
            
        } else {
            return 0;
        }
    }
    
    string & getName(string & s) {
        if (!parent) return s;
        ofxPrecisionRow * t = this;
        auto it = find(parent->inner.begin(), parent->inner.end(), t);
        if (it != parent->inner.end()) {
            int i = distance(parent->inner.begin(), it);
            s = s + " -> " + ofToString(i);
            return parent->getName(s);
        } else {
            ofLogError("[ofxPrecision]") << "Could not find index in parent";
            return s;
        }
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
    //        //        float rm = w - inner[idx].width;
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

