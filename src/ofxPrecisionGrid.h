
#pragma once
#include "ofMain.h"

#define PRECISION_COL 0
#define PRECISION_ROW 1

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
    ofxPrecisionGridStyle() {
        defaultColor = ofColor(0,120,120, 255);
        hoverColor = ofColor(150, 255, 255, 255);
        dropColor = ofColor(150, 255, 255, 100);
    }
};

class ofxPrecisionGrid;

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

class ofxPrecisionGrid {
public:
    
    /*-- Statuses --*/
    
    bool bResizeHovered;
    bool bDropHovered;
    bool bHovered;
    
    ofxPrecisionGridStyle style;
    
    ofRectangle bounds;
    
    int type;
    float mH, mW;
    float minPadding;
    int uId;
    string name;
    
    ofxPrecisionGrid * parent;
    vector<ofxPrecisionGrid *> inner;
    vector<ofxPrecisionGrid *> global;
    map<int, ofxPrecisionGrid *> gHash;
    
    vector<ofxPrecisionAction> actions;
    
    ofEvent<ofxPrecisionGrid> added;
    
    
    ofxPrecisionGrid *& operator[] (size_t i) {
        return inner[i];
    }
    
    ofxPrecisionGrid() {
        mH = 1;
        mW = 1;
        type = 0;
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
    void set(ofRectangle r) {
        minPadding = 10;
        uId = 0;
        name = getName(name);
        float p = minPadding;
        r.x += p;
        r.y += p;
        r.width -= p * 2;
        r.height -= p * 2;
        bounds = r;
        amend();
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
    
    void moved(int x, int y) {
        ofPoint p(x,y);
        float padd = minPadding;
        for (auto & g : global) {
            
            vector<float> mWidths = g->getWidths();
            vector<float> mHeights = g->getHeights();
            
            g->bDropHovered = false;
            g->bResizeHovered = false;
            g->bHovered = false;
            
            g->actions.clear();
            
            ofPoint tl = g->bounds.getTopLeft();
            ofPoint tr = g->bounds.getTopRight();
            ofPoint br = g->bounds.getBottomRight();
            ofPoint bl = g->bounds.getBottomLeft();
            
            bool inR = (x > tr.x - padd && x < tr.x + padd);
            bool inB = (y > bl.y - padd && y < bl.y + padd);
            bool inL = (x > tl.x - padd && x < tl.x + padd);
            bool inT = (y > bl.y - padd && y < bl.y + padd);
            
            bool inH = (y > tr.y - padd && y < br.y + padd);
            bool inW = (x > bl.x - padd && x < br.x + padd);
            
            bool isLast = (g->getIndex() == g->parent->inner.size() - 1);
            bool isInRow = (g->parent->type == PRECISION_ROW);
            bool isInCol = (g->parent->type == PRECISION_COL);
            bool noInner = (g->inner.size() <= 1);
            
            ofRectangle shri(g->bounds);
            shri.x += padd;
            shri.y += padd;
            shri.width -= padd*2;
            shri.height -= padd*2;
            
            /*-- drop area --*/
            
            if (shri.inside(x, y) && noInner) {
                ofxPrecisionAction a(PRECISION_IN, g->bounds, p, mWidths, mHeights);
                g->actions.push_back(a);
                g->bHovered = true;
            }
            
            /*-- width drag --*/
            
            if (inR && inH && !isLast && !isInCol) {
                ofxPrecisionAction a(PRECISION_W, g->bounds, p, mWidths, mHeights);
                g->actions.push_back(a);
                g->bResizeHovered = true;
            }
            
            /*-- drop right --*/
            
            if (inR && inH && isInRow) {
                ofxPrecisionAction a(PRECISION_DR, g->bounds, p, mWidths, mHeights);
                g->actions.push_back(a);
                g->bDropHovered = true;
            }
            
            /*-- drop left --*/
            
            if (inL && inH && isInRow) {
                ofxPrecisionAction a(PRECISION_DL, g->bounds, p, mWidths, mHeights);
                g->actions.push_back(a);
                g->bDropHovered = true;
            }
            
            /*-- height drag --*/
            
            
            if (inB && inW && !isLast && !isInRow) {
                vector<float> mWidths = g->getWidths();
                vector<float> mHeights = g->getHeights();
                ofxPrecisionAction a(PRECISION_H, g->bounds, p, mWidths, mHeights);
                g->actions.push_back(a);
                g->bResizeHovered = true;
            }
            
            /*-- drop bottom --*/
            
            if (inB && inW && isInCol) {
                ofxPrecisionAction a(PRECISION_DB, g->bounds, p, mWidths, mHeights);
                g->actions.push_back(a);
                g->bDropHovered = true;
            }
            
            /*-- drop top --*/
            
            if (inT && inW && isInCol) {
                ofxPrecisionAction a(PRECISION_DT, g->bounds, p, mWidths, mHeights);
                g->actions.push_back(a);
                g->bDropHovered = true;
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
                
                
                float pW = u->parent->bounds.width; // parent width
                float pH = u->parent->bounds.height; // parent height
                float offX = (a.oPress.x - x) / pW; // X multiple
                float offY = (a.oPress.y - y) / pH; // Y multiple
                int idx = u->getIndex();
                
                for (int i = 0; i < u->parent->inner.size(); i++) {
                    ofxPrecisionGrid * sib = u->parent->inner[i];
                    float w = a.mWidths[i];
                    float h = a.mHeights[i];
                    
                    if (!isShift && !isAlt) {
                        if (a.type == PRECISION_W) {
                            sib->mW = w;
                            if (i == idx) sib->mW = w - offX;
                            if (i == idx + 1) sib->mW = w + offX;
                        } else {
                            sib->mH = h;
                            if (i == idx) sib->mH = h - offY;
                            if (i == idx + 1) sib->mH = h + offY;
                        }
                        
                    } // !shit + !alt
                    
                    if (isShift && !isAlt) {
                        
                        if (a.type == PRECISION_W) {
                            sib->mW = w;
                            if (i == idx) sib->mW = w - offX;
                            if (i > idx) sib->mW = w + offX / (float)( a.mWidths.size() - idx - 1 );
                        } else {
                            sib->mH = h;
                            if (i == idx) sib->mH = h - offY;
                            if (i > idx) sib->mH = h + offY / (float)( a.mHeights.size() - idx - 1 );
                        }
                    } // shift + !alt
                    
                    if (isShift && isAlt) {
                        if (a.type == PRECISION_W) {
                            sib->mW = w;
                            if (i < idx + 1) sib->mW = w - (offX / (float)( idx + 1) );
                            if (i == idx + 1) sib->mW = w + offX;
                        } else {
                            sib->mH = h;
                            if (i < idx + 1) sib->mH = h - (offY / (float)( idx + 1) );
                            if (i == idx + 1) sib->mH = h + offY;
                        }
                    } // shift + alt
                    
                } // for each sibling

                u->parent->amend();
                
            } // for each action
        }
    }
    void released(int x, int y) {
        //        actions.clear();
    }
    
    
    void draw(int d = 0, bool iso = false) {
        
        
        ofColor c = style.defaultColor;
        
        ofSetColor(c);
        ofNoFill();
        ofSetLineWidth(2);
        
        ofPushMatrix();
        if (iso) {
            ofTranslate(0,0,d * 60);
        }
        ofRectangle r = bounds;
//        r.x += 5;
//        r.y += 5;
//        r.width -= 10;
//        r.height -= 10;
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
        
        if (!parent) {
            for (auto & g : global) {
                for (auto & a : g->actions) {
                    ofSetColor(style.hoverColor);
                    ofNoFill();
                    if (a.type == PRECISION_W) {
                        ofDrawLine(g->bounds.getTopRight(), g->bounds.getBottomRight());
                    }
                    if (a.type == PRECISION_H) {
                        ofDrawLine(g->bounds.getBottomLeft(), g->bounds.getBottomRight());
                    }
                    ofSetColor(style.dropColor);
                    ofFill();
                    if (a.type == PRECISION_IN) {
                        ofDrawRectangle(g->bounds);
                    }
                    ofNoFill();
                }
            }
        }
        
    }
    
    void amend() {
        
        /*-- Iterate --*/
        
        float tW = 0;
        float tH = 0;
        for (auto & c : inner) {
            tW += c->mW;
            tH += c->mH;
        }
        float x = bounds.x;
        float y = bounds.y;
        float w = bounds.width;
        float h = bounds.height;
        
        float tX = x;
        float tY = y;
        
        float p = minPadding * 2;
        
        for (auto & c : inner) {
            float ww = ( bounds.width / tW ) * c->mW;
            float hh = ( bounds.height / tH ) * c->mH;
            if (type == PRECISION_COL) c->set(x, tY, w, hh);
            if (type == PRECISION_ROW) c->set(tX, y, ww, h);
            tX += c->bounds.width + p;
            tY += c->bounds.height + p;
        }
    }
    
    
    void refresh() {
        
    }
    
    ofxPrecisionGrid & add(int t, int idx = -1) {
        
        
        ofxPrecisionGrid * ch = new ofxPrecisionGrid(t, this);
        
        /*-- Insert new rect --*/
        
        if (idx > inner.size() || idx < 0) idx = inner.size();
        inner.insert(inner.begin() + idx, ch);
        ch->mH = 1;
        ch->mW = 1;
        
        amend();
        
        ofxPrecisionGrid * root = getRoot();
        root->global.push_back(ch);
        while ( root->gHash.find(uId) != root->gHash.end() ) uId += 1;
        getRoot()->gHash[uId] = ch;
        
        /*-- Return vector --*/
        
        return * ch;
    }
    
    int getIndex() {
        ofxPrecisionGrid * t = this;
        auto it = find(parent->inner.begin(), parent->inner.end(), t);
        if (it != parent->inner.end()) {
            return distance(parent->inner.begin(), it);
            
        } else {
            return 0;
        }
    }
    
    string & getName(string & s) {
        if (!parent) return s;
        ofxPrecisionGrid * t = this;
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
    
    ofxPrecisionGrid * getRoot() {
        if (parent) return parent;
        return this;
    }
    
    void onAdded( ofxPrecisionGrid & ch ) {
    }
    
};

