
#pragma once
#include "ofMain.h"

#define PRECISION_COL 0
#define PRECISION_ROW 1
#define PRECISION_OBJ 2

#define PRECISION_W 0 // width
#define PRECISION_H 1 // height
#define PRECISION_IN 2 // inside
#define PRECISION_PL 3 // padding L
#define PRECISION_PT 4 // padding T
#define PRECISION_PR 5 // padding R
#define PRECISION_PB 6 // padding B
#define PRECISION_DT 7 // drop T
#define PRECISION_DR 8 // drop R
#define PRECISION_DB 9 // drop B
#define PRECISION_DL 10 // drop L
#define PRECISION_DH 10 // drop H
#define PRECISION_DV 10 // drop V

/*-- TODO :
 - Add callbacks for ofxFluidUi
 - Add an "errors" vector = undo change if not valid
 - Differentiate "fixed" and "percentage" for scaling
 --*/


struct ofxPrecisionGridStyle {
    ofColor defaultColor;
    ofColor hoverColor;
    ofColor dropColor;
    float resizeOffset;
    float minWidth;
    float minHeight;
    ofxPrecisionGridStyle() {
        defaultColor = ofColor(0,120,120, 255);
        hoverColor = ofColor(255, 255, 255, 255);
        dropColor = ofColor(150, 255, 255, 100);
        resizeOffset = 0;
        minWidth = 10;
        minHeight = 10;
    }
};

struct ofxPrecisionError {
    string msg;
    string name;
    ofxPrecisionError(string m, string n) {
        msg = m;
        name = n;
    }
};


class ofxPrecisionGrid;


class ofxPrecisionGrid {
public:
    
    ofxPrecisionGridStyle style;
    ofRectangle bounds;
    
    int type;
    int depth;
    float mH, mW;
    string name;
    
    vector<int> location;
    ofxPrecisionGrid * parent;
    vector<ofxPrecisionGrid *> inner;
    vector<ofxPrecisionGrid *> global;
    vector<ofxPrecisionError> errors;
    map<string, ofxPrecisionGrid *> hash;
    
    ofEvent<string> added;
    ofEvent<string> amended;
    
    
    ofxPrecisionGrid *& operator[] (size_t i) {
        return inner[i];
    }
    
    ofxPrecisionGrid() {
        
        /*-- root --*/
        
        mH = 1;
        mW = 1;
        type = 0;
        parent = NULL;
        hash[name] = this;
        global.push_back(this);
        amend();
        
        
    };
    ofxPrecisionGrid(int t, ofxPrecisionGrid * p) {
        mH = 1;
        mW = 1;
        type = t;
        parent = p;
    };
    void set(float x, float y, float w, float h) {
        ofRectangle r(x, y, w, h);
        set(r);
    }
    void set(ofRectangle & r) {
//        name = getName(name);
        float p = 0;
        r.x += p;
        r.y += p;
        r.width -= p * 2;
        r.height -= p * 2;
        
        if ((r.width < style.minWidth) || (r.height < style.minHeight)) {
            ofxPrecisionError err("Bounds too small", name);
            getRoot()->errors.push_back(err);
        }
        
        bounds = r;
    }
    void setWidth(float w) {
        set(bounds.x, bounds.y, w, bounds.height);
    }
    void setHeight(float h) {
        set(bounds.x, bounds.y, bounds.width, h);
    }
    
    ofxPrecisionGrid * get(vector<int> idx) {
        ofxPrecisionGrid * u = getRoot();
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
    
    
    
    void draw(bool iso = false) {
        
        
        ofColor c = style.defaultColor;
        
        ofSetColor(c);
        ofNoFill();
        ofSetLineWidth(2);
        
        ofPushMatrix();
        int d = depth + 1;
        float o = style.resizeOffset;
        ofTranslate(d * o, d * o, 0 );
        if (iso) {
            ofTranslate(0,0,d * 60);
        }
        
        
        ofRectangle r = bounds;
        ofDrawRectangle(r);
        string s = "x";
        s += ofToString(bounds.x) + " y";
        s += ofToString(bounds.y) + " w";
        s += ofToString(bounds.width) + " h";
        s += ofToString(bounds.height) + " ";
        
        
        if (type == PRECISION_OBJ) {
            ofDrawLine( bounds.getTopLeft(), bounds.getBottomRight() );
            ofDrawLine( bounds.getTopRight(), bounds.getBottomLeft() );
        }
        
        if (parent && iso) {
            ofSetColor(255,255,255,100);
            ofVec3f p = parent->bounds.getCenter();
            ofVec3f c = bounds.getCenter();
            p.z -= 60;
            ofDrawLine(c, p);
        }
        
        
        ofFill();
        ofSetColor(0);
        
//        ofDrawRectangle(bounds);
        
        ofPopMatrix();
        
        int i = 0;
        d += 1;
        for (auto & ch : inner) ch->draw(iso);
        
        
    }
    vector<int> & position(vector<int> & pos) {
        if (!parent) {
            return pos;
        }
        int i = getIndex();
        pos.insert(pos.begin(), i);
        return parent->position(pos);
    }
    
    void tag() {
        
        /*-- amend naming and hash to position --*/
        
        vector<int> pos;
        location = position(pos);
        string ss = "|";
        for (auto & i : location) {
            ss += ">"+ofToString(i + 1);
        }
        if (name != ss) getRoot()->hash[ss] = this;
        name = ss;
    }
    
    void amend() {
        
        
        tag();
        ofNotifyEvent(getRoot()->amended, name);
        
        /*-- iterate width, height and position into pixels --*/
        
        float x = bounds.x;
        float y = bounds.y;
        float w = bounds.width;
        float h = bounds.height;
        
        float tX = x;
        float tY = y;
        float tW = 0;
        float tH = 0;
        
        /*-- get max size multiples --*/
        
        for (auto & c : inner) {
            tW += c->mW;
            tH += c->mH;
        }
        
        for (auto & c : inner) {
            
            c->tag();
            
            float ww = ( bounds.width / tW ) * c->mW;
            float hh = ( bounds.height / tH ) * c->mH;
            
            float cx = c->bounds.x;
            float cy = c->bounds.y;
            float cw = c->bounds.width;
            float ch = c->bounds.height;
            
            float fx = x;
            float fy = y;
            float fw = w;
            float fh = h;
            
            if (type == PRECISION_COL || type == PRECISION_OBJ) {
                fy = tY;
                fh = hh;
            }
            if (type == PRECISION_ROW) {
                fx = tX;
                fw = ww;
            }
            
            bool isDiff = ( (fx != cx) || (fy != cy) || (fw != cw) || (fh != ch) );
            
            if (isDiff) c->set(fx, fy, fw, fh);
//            ofNotifyEvent(getRoot()->amended, c->name);
            
            tX += c->bounds.width;
            tY += c->bounds.height;
            c->amend();
        }
    }
    
    
    int & getDepth(int & d) {
        if (!parent) return d;
        d += 1;
        return parent->getDepth(d);
    }
    
    bool hasError() {
        return errors.size() > 0;
    }
    
    void erase() {
//        parent->inner.erase(getIndex());
    }
    
    ofxPrecisionGrid * getRoot() {
        if (!parent) return this;
        return parent->getRoot();
    }
    
    ofxPrecisionGrid & add(int t, int idx = -1) {
        
        ofxPrecisionGrid * ch = new ofxPrecisionGrid(t, this);
        ofxPrecisionGrid * root = ch->getRoot();
        
        if (root->hasError()) {
            ofLogError("ofxPrecisionGrid") << "Trying to add child, but root element has error";
            return * ch;
        }
        
        /*-- Insert new rect --*/
        
        if (idx > inner.size() || idx < 0) idx = inner.size(); // safe IDX
        
        int d = 0;
        inner.insert(inner.begin() + idx, ch); // insert @ IDX
        ch->mH = 1; // default multiH
        ch->mW = 1; // default multiWidth
        ch->depth = getDepth(d); // set depth
        
        /*--- Generate position ---*/
        
        root->global.push_back(ch);
        amend();
        
        /*-- delete new item if it generated errors --*/
        
        if (root->hasError()) {
            ofLogError("ofxPrecisionGrid") << "Added child, but this causes dimensions error ... deleting and returning";
            inner.erase(inner.begin() + idx);
            root->global.pop_back();
            root->hash.erase(ch->name);
            ch->parent = NULL;
            root->errors.clear();
            return * ch;
        }
        
        
        
        ofNotifyEvent(root->added, ch->name);
        
        /*-- Return vector --*/
        
        return * ch;
    }
    
    int getIndex() {
        if (!parent) return 0;
        ofxPrecisionGrid * t = this;
        auto it = find(parent->inner.begin(), parent->inner.end(), t);
        if (it != parent->inner.end()) {
            return distance(parent->inner.begin(), it);
            
        } else {
            
            ofLogError("Could not find index");
            return 0;
        }
    }
    
    
    int getFirstAndInColCount(int i) {
        if (!parent) return i;
        
        bool isFirst = (getIndex() == 0);
        bool isInCol = (parent->type == PRECISION_COL);
        
        if (isFirst && isInCol && parent->parent) i += 1;
        return parent->getFirstAndInColCount(i);
    }
    int getLastAndInColCount(int i) {
        if (!parent) return i;
        
        bool isLast = (getIndex() == parent->inner.size() - 1);
        bool isInCol = (parent->type == PRECISION_COL);
        
        if (isLast && isInCol && parent->parent) i += 1;
        return parent->getLastAndInColCount(i);
    }
    int getLastAndInRowCount(int i) {
        if (!parent) return i;
        
        bool isLast = (getIndex() == parent->inner.size() - 1);
        bool isInRow = (parent->type == PRECISION_ROW);
        
        if (isLast && isInRow) i += 1;
        return parent->getLastAndInRowCount(i);
    }
    
    
    
};

