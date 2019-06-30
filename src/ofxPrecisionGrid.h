
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


#include "ofxPrecisionStyle.h"
#include "ofxPrecisionError.h"


class ofxPrecisionGrid;


#include "ofxPrecisionEvent.h"

class ofxPrecisionGrid {
public:
    
    ofxPrecisionGridStyle style;
    ofRectangle bounds;
    
    int type;
    float mH, mW;
    bool fixed;
    int depth;
    string name;
    
    vector<int> location;
    ofxPrecisionGrid * parent;
    vector<ofxPrecisionGrid *> inner;
    vector<ofxPrecisionGrid *> global;
    vector<ofxPrecisionError> errors;
    map<string, ofxPrecisionGrid *> hash;
    
    ofEvent<ofxPrecisionEvent> added;
    ofEvent<ofxPrecisionEvent> amended;
    
    
    ofxPrecisionGrid *& operator[] (size_t i) {
        return inner[i];
    }
    
    ofxPrecisionGrid() {
        
        /*-- root --*/
        
        mH = 1;
        mW = 1;
        type = 0;
        parent = NULL;
        global.push_back(this);
        amend();
        
        
    };
    ofxPrecisionGrid(int t) {
        mH = 1;
        mW = 1;
        type = t;
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
        name = ss;
    }
    
    void amend() {
        
        
        tag();
        
        ofxPrecisionEvent e(this);
        ofNotifyEvent(getRoot()->amended, e);
        
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
    
    ofxPrecisionGrid * getRoot() {
        if (!parent) return this;
        return parent->getRoot();
    }
    
    void remove(vector<ofxPrecisionGrid *> & v, ofxPrecisionGrid * c) {
        auto idx = std::find(v.begin(), v.end(), c);
        if (idx != v.end()) v.erase(idx);
    }
    
    void load(ofJson & j) {
        
        if (this == getRoot()) {
            
            global.clear();
            inner.clear();
            
            float x = j["x"].get<float>();
            float y = j["y"].get<float>();
            float w = j["width"].get<float>();
            float h = j["height"].get<float>();
            set( x, y, w, h );
            
        }
        
        for (int i = 0; i < j["inner"].size(); i++) {
            add( j["inner"][i], i);
        }
        
    }
    
    void insert() {
        
    }
    
    void removeReferences() {
        ofxPrecisionGrid * r = getRoot();
        remove(r->global, this);
        remove(parent->inner, this);
        for (auto & u : inner) u->removeReferences();
    }
    
    ofxPrecisionGrid & add(ofJson & j, int idx = -1) {
        
        ofxPrecisionGrid * ch = new ofxPrecisionGrid( j["type"].get<int>() );
        ch->mH = j["height"].get<int>(); // default multiH
        ch->mW = j["width"].get<int>(); // default multiWidth
        ch->type = j["type"].get<int>();
        ch->parent = nullptr;
        add(ch, idx);
        
        for (int i = 0; i < j["inner"].size(); i++) ch->add( j["inner"][i], i);
        
        return * ch;
    }
    
    ofxPrecisionGrid & add(ofxPrecisionGrid * ch, int idx = -1) {
        
        ofxPrecisionGrid * root = getRoot();
        remove(root->global, ch);
        
        if (ch->parent) {
            ofLog() << "A" << ch->parent->inner.size();
            remove(ch->parent->inner, ch);
            ofLog() << "B" << ch->parent->inner.size();
        }
        
        ch->parent = this;
        
        
        if (root->hasError()) {
            ofLogError("ofxPrecisionGrid") << "Trying to add child, but root element has error";
            return * ch;
        }
        
        /*-- Insert new rect --*/
        
        if (idx > inner.size() || idx < 0) idx = inner.size(); // safe IDX
        
        int d = 0;
        inner.insert(inner.begin() + idx, ch); // insert @ IDX
        ch->depth = getDepth(d); // set depth
        
        /*--- Generate position ---*/
        
        root->global.push_back(ch);
        
        /*-- Add children --*/
        
        for (auto & u : ch->inner) ch->add(u);
        
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
        
        ofxPrecisionEvent e(ch);
        ofNotifyEvent(root->added, e);
        
        /*-- Return vector --*/
        
        return * ch;
    }
    
    ofxPrecisionGrid & add(int t, int idx = -1) {
        
        ofxPrecisionGrid * ch = new ofxPrecisionGrid(t);
        ch->mH = 1; // default multiH
        ch->mW = 1; // default multiWidth
        ch->parent = nullptr;
        add(ch, idx);
        
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
    
    ofJson json() {
        
        ofJson j;
        
        if (this == getRoot()) {
            j["x"] = bounds.x;
            j["y"] = bounds.y;
            j["width"] = bounds.width;
            j["height"] = bounds.height;
        } else {
            j["type"] = type;
            j["width"] = mW;
            j["height"] = mH;
        }
        
        for (int i = 0; i < inner.size(); i++) {
            j["inner"][i] = inner[i]->json();
        }
        
        return j;
    }
    
    
    
};

