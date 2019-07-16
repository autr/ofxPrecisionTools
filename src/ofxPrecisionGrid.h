
#pragma once
#include "ofMain.h"
#include "ofxScrollBox.h"

#define PRECISION_COL 0
#define PRECISION_ROW 1
#define PRECISION_OBJ 2

#define PRECISION_RR 0 // resize r
#define PRECISION_RB 1 // resize b
#define PRECISION_RL 2 // resize l
#define PRECISION_RT 3 // resize t

#define PRECISION_IN 5 // inside

#define PRECISION_PL 6 // padding L
#define PRECISION_PT 7 // padding T
#define PRECISION_PR 8 // padding R
#define PRECISION_PB 9 // padding B

#define PRECISION_DT 10 // drop T
#define PRECISION_DR 11 // drop R
#define PRECISION_DB 12 // drop B
#define PRECISION_DL 13 // drop L

#define PRECISION_DH 14 // drop H
#define PRECISION_DV 15 // drop V

#include "ofxPrecisionStyle.h"
#include "ofxPrecisionError.h"

class ofxPrecisionGrid;


#include "ofxPrecisionEvent.h"

class ofxPrecisionGrid {
public:
    
    ofxPrecisionGridStyle style;
    ofRectangle bounds;
    
#ifdef OFXSCROLLBOX
    
    ofxScrollBox scrollBox;
    
#endif
    
    int type;
    float sideOffset;
    float mH, mW;
    bool fixed;
    bool ghost;
    bool scroll;
    string name;
    
    vector<int> location;
    ofxPrecisionGrid * parent;
    vector<ofxPrecisionGrid *> inner;
    vector<ofxPrecisionGrid *> global;
    vector<ofxPrecisionGrid *> ghosts;
    vector<ofxPrecisionError> errors;
    ofEvent<ofxPrecisionEvent> event;
    
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
        fixed = false;
        parent = nullptr;
        global.push_back(this);
        ghost = false;
        scroll = false;
        sideOffset = 0;
        amend();
#ifdef OFXSCROLLBOX
        ofAddListener(scrollBox.event , this, &ofxPrecisionGrid::scrollEvent);
#endif
        
        
        
    };
    ofxPrecisionGrid(int t) {
        
        /*-- child --*/
        
        mH = 1;
        mW = 1;
        fixed = false;
        ghost = false;
        scroll = false;
        type = t;
        sideOffset = 0;
#ifdef OFXSCROLLBOX
        ofAddListener(scrollBox.event , this, &ofxPrecisionGrid::scrollEvent);
#endif
    };
    
    int getScrollingGrids( vector<ofxPrecisionGrid *> & list ) {
        if (scroll) list.push_back(this);
        for (auto & ch : inner ) ch->getScrollingGrids( list );
        return list.size();
    }
    
    void setFixed( bool b ) {
        fixed = b;
        (parent) ? parent->amend() : amend();
        ofxPrecisionEvent e("setfixed", this);
        ofNotifyEvent(getRoot()->event, e);
    }
    void setScroll( bool b ) {
        scroll = b;
        (parent) ? parent->amend() : amend();
        ofxPrecisionEvent e("setscroll", this);
        ofNotifyEvent(getRoot()->event, e);
    }
    
    void scrollEvent(string & e) {
        if (e == "scrolled") amend();
    }
    void set(float x, float y, float w, float h) {
        ofRectangle r(x, y, w, h);
        set(r);
    }
    void set(ofRectangle & r) {
        
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
        
#ifdef OFXSCROLLBOX
        
        if (scrollBox.scrollY == 999999) scrollBox.scrollY = bounds.y;
        
        scrollBox.outer = bounds;
        scrollBox.inner = bounds;
        scrollBox.inner.y = scrollBox.scrollY;
        
#endif
        
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
        for (int i = 0; i < parent->inner.size(); i++) {
            ofxPrecisionGrid * ch = parent->inner[i];
            if (!ch->ghost) w.push_back(ch->mW);
        }
        return w;
    }
    vector<float> getHeights() {
        vector<float> h;
        if (!parent) return h;
        for (int i = 0; i < parent->inner.size(); i++) {
            ofxPrecisionGrid * ch = parent->inner[i];
            if (!ch->ghost) h.push_back(ch->mH);
        }
        return h;
    }
    
    
    
    void draw(bool iso = false) {
        
        
        ofColor c = style.defaultColor;
        
        ofSetColor(c);
        ofNoFill();
        ofSetLineWidth(2);
        
        ofPushMatrix();
        if (iso) {
            ofTranslate(0,0,depth() * 80);
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
        
        ofSetColor(255);
        if (inner.size() > 0) ofDrawBitmapString( ofToString( inner.size() ) , bounds.getCenter() );
        
        if (parent && iso) {
            ofSetColor(255,255,255,100);
            ofVec3f p = parent->bounds.getCenter();
            ofVec3f c = bounds.getCenter();
            p.z -= 80;
            ofDrawLine(c, p);
        }
        
        
        ofFill();
        ofSetColor(0);
        
        ofPopMatrix();
        
#ifdef OFXSCROLLBOX
        if (scroll) scrollBox.draw();
        if (scroll) {
            ofNoFill();
            ofSetColor(255,0,0);
            ofDrawRectangle( scrollBox.inner );
        }
#endif
        
        int i = 0;
        for (auto & ch : inner) ch->draw(iso);
        for (auto & ch : ghosts) ch->draw(iso);
        
    }
    
    void scrolled( ofMouseEventArgs & e ) {
        
#ifdef OFXSCROLLBOX
        for(auto & ch : global) {
            if (ch->scroll) ch->scrollBox.scrolled(e);
        }
#endif
    }
    
    void pressed( int x, int y ) {
#ifdef OFXSCROLLBOX
        for(auto & ch : global) {
            if (ch->scroll) ch->scrollBox.pressed(x,y);
        }
#endif
        
    }
    void dragged( int x, int y ) {
#ifdef OFXSCROLLBOX
        for(auto & ch : global) {
            if (ch->scroll) ch->scrollBox.dragged(x,y);
        }
#endif
    }
    void released( int x, int y ) {
#ifdef OFXSCROLLBOX
        for(auto & ch : global) {
            if (ch->scroll) ch->scrollBox.released(x,y);
        }
#endif
    }
    
    
    
    vector<int> & position(vector<int> & pos) {
        
        if (parent == this) {
            ofLogError("Parent is also this");
            return;
        }
        
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
        
        
        if (!ghost) tag();
        
        ofxPrecisionEvent e("amended", this);
        ofNotifyEvent(getRoot()->event, e);
        
        /*-- iterate width, height and position into pixels --*/
        
        float x = bounds.x;
        float y = bounds.y;
        float w = bounds.width;
        float h = bounds.height;
        
        float tX = x;
        float tY = y;
        float tW = 0;
        float tH = 0;
        
        bool isCol = (type == PRECISION_COL || type == PRECISION_OBJ);
        bool isRow = (type == PRECISION_ROW);
        bool isScroll = (scroll);
        
        /*-- get totals --*/
        
        for (auto & c : inner) {
            
            bool isFixed = c->fixed;
            bool isGhost = c->ghost;
            
            /*-- get total without fixed widths and heights --*/
            
            if (!isGhost) {
            
                if (isCol && isFixed) {
                    
                    tW += c->mW;
                    h -= c->bounds.height;
                    
                } else if (isRow && isFixed) {
                    
                    tH += c->mH;
                    w -= c->bounds.width;
                } else {
                    
                    tW += c->mW;
                    tH += c->mH;
                }
            }
        }
        
        
        /*-- set scrollbox fixed --*/
        
#ifdef OFXSCROLLBOX
        
        if (scroll) {
            for (auto & ch : inner) if (!ch->fixed) ch->fixed = true;
        }
        
#endif
        
        /*-- amend inner --*/
        
        for (auto & c : inner) {
            
            c->tag();
            
            bool isFixed = (c->fixed);
            bool isGhost = c->ghost;
            
            if (!isGhost) {
                
                float ww = ( w / tW ) * c->mW;
                float hh = ( h / tH ) * c->mH;
            
                float cx = c->bounds.x;
                float cy = c->bounds.y;
                float cw = c->bounds.width;
                float ch = c->bounds.height;
            
                float fx = x;
                float fy = y;
                float fw = w;
                float fh = h;
            
                if (isCol) {
                    fy = tY;
                    fh = hh;
                }
                if (isRow) {
                    fx = tX;
                    fw = ww;
                }
                if (isFixed && isCol) {
                    fh = ch;
                }
                if (isFixed && isRow) {
                    fw = cw;
                }
                
                /*-- set scroll offset --*/
                
#ifdef OFXSCROLLBOX
                
                if (scroll) {
                    float s = scrollBox.getScrollOffset();
                    fy -= s;
            
                }
                
                if (sideOffset != 0) {
                    fx += sideOffset;
                }
                
#endif
                
                
                
                /*-- only set if diff --*/
                
                bool isDiff = ( (fx != cx) || (fy != cy) || (fw != cw) || (fh != ch) );
                if (isDiff) c->set(fx, fy, fw, fh);
                
                tX += c->bounds.width;
                tY += c->bounds.height;
                
                c->amend();
            }
        }
        
        /*-- amend scrollbox --*/
        
#ifdef OFXSCROLLBOX
        
        if (scroll) {
            float h = 0;
            for (auto & ch : inner) h += ch->bounds.height;
            scrollBox.inner.height = h;
        }
        
#endif
    }
    
    
    int depth(int d = -1) {
//        if (d == -1) d = 0;
//        if (!parent) return d;
//        d += 1;
        return location.size();//parent->depth(d);
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
        
        
        clear();
        
        
        if (this == getRoot()) {
            
            float x = j["x"].get<float>();
            float y = j["y"].get<float>();
            float w = j["width"].get<float>();
            float h = j["height"].get<float>();
            set( x, y, w, h );
            
        }
        
        for (int i = 0; i < j["inner"].size(); i++) {
            add( j["inner"][i], i);
        }
        ofxPrecisionEvent e("loaded", this);
        ofNotifyEvent(getRoot()->event, e);
        
    }
    
    void clear() {
        
        ofxPrecisionGrid * r = getRoot();
        ofxPrecisionEvent e("clear", this);
        ofNotifyEvent(r->event, e);
        for (auto & u : collect() ) {
            remove( r->global, u);
            delete u;
        }
        inner.clear();
        global.push_back(this);
    }
    
    
    void remove() {
        ofxPrecisionGrid * r = getRoot();
        ofxPrecisionEvent e("removed", this);
        ofNotifyEvent(r->event, e);
        for (auto & u : collect() ) {
            remove( r->global, u);
            delete u;
        }
        if (parent) remove(parent->inner, this);
        remove(r->global, this);
        delete this;
    }
    
    vector<ofxPrecisionGrid *> collect() {
        vector<ofxPrecisionGrid *> a;
        a.insert( a.end(), inner.begin(), inner.end() );
        for (auto & u : inner) {
            vector<ofxPrecisionGrid *> b = u->collect();
            a.insert( a.end(), b.begin(), b.end() );
        }
        return a;
    }
    
    bool isInvalid( ofxPrecisionGrid * g ) {
        
        if ( g == this ) return true;
        
        bool invalid = true;
        
        for ( int i = 0; i < location.size(); i++ ) {
            int p = location[i]; // to
            if (i < g->location.size()) {
                int pp = g->location[i]; // from
                
//                ofLogVerbose("[ofxPrecisionGrid]") << "Checking if invalid:" << p << pp;
                
                if (p != pp) invalid = false;
            }
        }
//        if (invalid) ofLogVerbose("[ofxPrecisionGrid]") << "Invalid move!";
        return invalid;
    }
    
    bool move( ofxPrecisionGrid * u , int idx = -1) {
        
        if ( isInvalid(u) ) return false;
        
        ofLogVerbose("[ofxPrecisionGrid]") << "Moving from: " << u->getLocationString();
        ofLogVerbose("[ofxPrecisionGrid]") << "Moving to  : " << getLocationString();
        
        if (u->parent) remove( u->parent->inner, u ); // remove from old parent
        u->ghost = false;
        add( u, idx ); // add here
        return true;
    }
    
    ofxPrecisionGrid & add(ofJson & j, int idx = -1) {
        
        ofxPrecisionGrid * ch = new ofxPrecisionGrid( j["type"].get<int>() );
        ch->mH = j["height"].get<float>(); // default multiH
        ch->mW = j["width"].get<float>(); // default multiWidth
        
//        if (!j["fX"].is_null()) ch->fX = j["fX"].get<float>();
//        if (!j["fY"].is_null()) ch->fY = j["fY"].get<float>();
//        if (!j["fW"].is_null()) ch->fW = j["fW"].get<float>();
//        if (!j["fH"].is_null()) ch->fH = j["fH"].get<float>();
        
        ch->type = j["type"].get<int>();
        ch->parent = nullptr;
        add(ch, idx);
        
        for (int i = 0; i < j["inner"].size(); i++) ch->add( j["inner"][i], i);
        
        return * ch;
    }
    
    ofxPrecisionGrid & add(ofxPrecisionGrid * ch, int idx = -1) {
        
        ofxPrecisionGrid * root = getRoot();
        remove(root->global, ch);
        
        ch->parent = this;
        /*-- Insert new rect --*/
        
        if (idx > inner.size() || idx < 0) idx = inner.size(); // safe IDX
        
        int d = 0;
        inner.insert(inner.begin() + idx, ch); // insert @ IDX
        
        /*--- Generate position ---*/
        
        root->global.push_back(ch);
        amend();
        
        ofxPrecisionEvent e("added", ch);
        ofNotifyEvent(root->event, e);
        
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
    
    string getLocationString(string l = "") {
        if ( parent == nullptr ) return l;
        l = " > " +  ofToString( getIndex() ) + l ;
        return parent->getLocationString( l );
    }
    
    int getIndex() {
        if (!parent) return 0;
//        if (!ghost) return -1;
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
        
        if (!parent) {
            j["fX"] = bounds.x;
            j["fY"] = bounds.y;
            j["fW"] = bounds.width;
            j["fH"] = bounds.height;
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
    
    bool inside( ofxPrecisionGrid * u ) {
        if ( u == this ) return true;
        bool b = false;
        for (auto & el : inner) if (el->inside( u )) b = true;
        return b;
    }
    
    
};

