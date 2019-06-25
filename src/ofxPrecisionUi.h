
#pragma once
#include "ofMain.h"
#include "ofxPrecisionGrid.h"

struct ofxPrecisionAction {
    ofRectangle oBounds;
    int type;
    ofPoint oPress;
    vector<float> mHeights;
    vector<float> mWidths;
    ofxPrecisionGrid * unit;
    
    ofxPrecisionAction(int t,  ofxPrecisionGrid * u, ofPoint p, vector<float> mW, vector<float> mH) {
        type = t;
        unit = u;
        oPress = p;
        mWidths = mW;
        mHeights = mH;
        oBounds = u->bounds;
    }
};

#define PRECISION_M_NONE 0
#define PRECISION_M_RESIZE 1
#define PRECISION_M_MOVE 2
#define PRECISION_M_MARGIN 3

struct ofxPrecisionAreas {
    ofRectangle t, r, b, l;
    bool isLast, isFirst, isInCol, isInRow, noInner;
    ofxPrecisionAreas() {
        
    }
    ofxPrecisionAreas(ofxPrecisionGrid * u) {
        
        int padd = 5;
        
        ofRectangle bo(u->bounds);
        
        isLast = false;
        isFirst = (u->getIndex() == 0);
        isInCol = false;
        isInRow = false;
        
        if (u->parent) {
            isLast = (u->getIndex() == u->parent->inner.size() - 1);
            isInCol = (u->parent->type == PRECISION_COL);
            isInRow = (u->parent->type == PRECISION_ROW);
        }
        noInner = (u->inner.size() <= 1);
        
        
        ofPoint tl = bo.getTopLeft();
        ofPoint tr = bo.getTopRight();
        ofPoint br = bo.getBottomRight();
        ofPoint bl = bo.getBottomLeft();
        
        float w = bo.width;
        float h = bo.height;
        
        /*-- offset by number of overlapping areas --*/
        
        if (isLast && isInCol) {
            int i = u->getLastAndInColCount(0);
            bl.y -= (padd * 2) * i;
            br.y -= (padd * 2) * i;
        }
        if (isFirst && isInCol) {
            int i = u->getFirstAndInColCount(0);
            tl.y += (padd * 2) * i;
            tr.y += (padd * 2) * i;
        }
        
        b.set( bl.x, bl.y - padd, w, padd * 2 );
        t.set( tl.x, tl.y - padd, w, padd * 2 );
        
        l.set( tl.x - padd, tl.y, padd * 2, h );
        r.set( tr.x - padd, tr.y, padd * 2, h );
        
        /*-- offset corners --*/
        
        b.x -= padd;
        b.width += padd * 2;
        t.x -= padd;
        t.width += padd * 2;
        l.y -= padd;
        l.height += padd * 2;
        r.y -= padd;
        r.height += padd * 2;
        
        
    }
};

class ofxPrecisionUi {
public:
    
    int mode;
    
    ofxPrecisionGrid * bin;
    int binChoice;
    
    ofxPrecisionGrid * unit;
    ofxPrecisionGridStyle style;
    vector<ofxPrecisionAction> actions;
    vector<ofxPrecisionGrid *> resizing;
    map<string, ofxPrecisionAreas> areas;
    vector<int> currLoc;
    string currName;
    
    ofPoint lastDragged;
    
    void up() {
        
        ofxPrecisionGrid * u = unit->hash[currName];
        
        if (u) {
            float d = 99999;
            if (u->inner.size() != 0) {
                ofVec3f p1 = u->bounds.getCenter();
                for (auto & uu : u->inner) {
                    ofVec3f p2 = uu->bounds.getCenter();
                    int dd = p1.distance( p2 );
                    if (dd < d) {
                        select(uu);
                        d = dd;
                    }
                }
            }
        } else {
            ofLog() << "DOESNT EXIST" << currName;
        }
    }
    void down() {
        
        ofxPrecisionGrid * u = unit->hash[currName];
        
        if (u && u->parent) {
            if (u->parent) select(u->parent);
        }
    }
    void prev() {
        
        ofxPrecisionGrid * u = unit->hash[currName];
        
        if (u && u->parent) {
            
            int i = u->getIndex() - 1;
            if (i < 0) i = u->parent->inner.size() - 1;
            select( u->parent->inner[i] );
        }
    }
    void next() {
        
        ofxPrecisionGrid * u = unit->hash[currName];
        
        if (u && u->parent) {
            
            int i = u->getIndex() + 1;
            if (i >= u->parent->inner.size()) i = 0;
            select( u->parent->inner[i] );
        }
    }
    
    void select(ofxPrecisionGrid * u) {
        
        currName = u->name;
        currLoc = u->location;
//        ofLog() << "Select" << currName;
//        ofLog() << areas[currName].t.width << areas[currName].t.height;
    }
    
    ofxPrecisionUi(ofxPrecisionGrid * u) {
        unit = u;
        mode = PRECISION_M_RESIZE;
        
        binChoice = -1;
        bin = new ofxPrecisionGrid(0, NULL);
        bin->set(20,20,20,80);
        bin->add(0);
        bin->add(0);
        bin->add(0);
        bin->add(0);
        
        for (auto & u : unit->global) {
            ofxPrecisionAreas a( u );
            areas[u->name] = a;
        }
        
        ofAddListener(unit->added, this, &ofxPrecisionUi::onAdded);
        ofAddListener(unit->amended, this, &ofxPrecisionUi::onAmended);
    }
    
    void onAdded(string & n) {
//        ofLog() << "Added" << n;
        select(unit->hash[n]);
    }
    void onAmended(string & n) {
//        ofLog() << "Amended" << n << areas[n].t.width << areas[n].t.height;
        ofxPrecisionGrid * u = unit->hash[n];
        ofxPrecisionAreas a( u );
        areas[n] = a;
    }
    
    void draw() {
        
        bin->draw();
        
        
        
        /*-- draw selected --*/
        
        ofSetColor(255,255,255,150);
        ofSetLineWidth(4);
        ofNoFill();
        ofxPrecisionGrid * curr = unit->get(currLoc);
        ofxPrecisionAreas arr = areas[currName];
        ofDrawRectangle(curr->bounds);
        ofSetLineWidth(1);
        
//        ofSetColor(255,255,0, 150);
//        ofDrawRectangle(arr.t);
//        ofDrawRectangle(arr.r);
//        ofDrawRectangle(arr.b);
//        ofDrawRectangle(arr.l);
        
//        for (auto & un : resizing) {
//
//            ofSetColor(255,0,0);
//            ofNoFill();
//            ofDrawRectangle(un->bounds);
//        }
        
        for (auto & act : actions) {
            
            ofxPrecisionGrid * u = act.unit;
            int d = u->depth;
            
            ofPushMatrix();
            
            
            ofSetColor(style.hoverColor);
            ofNoFill();
            
            ofPoint tl = u->bounds.getTopLeft();
            ofPoint tr = u->bounds.getTopRight();
            ofPoint br = u->bounds.getBottomRight();
            ofPoint bl = u->bounds.getBottomLeft();
            
            /*-- draw resizing --*/
            
            if (act.type == PRECISION_W) {
                ofDrawLine(tr,br);
            }
            if (act.type == PRECISION_H) {
                ofDrawLine(bl,br);
            }
            
            ofSetColor(0, 255, 0);
            
            /*-- draw dropping --*/
            
            bool isInside = (act.type == PRECISION_IN);
            bool isEdge = (act.type == PRECISION_DB || act.type == PRECISION_DT || act.type == PRECISION_DR || act.type == PRECISION_DL );
            
            if (isInside) ofDrawRectangle( u->bounds );
            if (isEdge && u->parent) ofDrawRectangle(u->parent->bounds);
                
            ofSetColor(255);
            
            ofSetCircleResolution(32);
            ofSetLineWidth(4);
            
            if (isInside) ofDrawCircle( u->bounds.getCenter(), 8 );
            if (act.type == PRECISION_DB) ofDrawLine(bl,br);
            if (act.type == PRECISION_DT) ofDrawLine(tl,tr);
            if (act.type == PRECISION_DR) ofDrawLine(tr,br);
            if (act.type == PRECISION_DL) ofDrawLine(tl,bl);
            
            
            ofSetLineWidth(1);
            
            ofPopMatrix();
        }
        
        
        
    }
    
    void released(int x, int y) {
        
        if (binChoice != -1) {
            
            ofLog() << "Bin Choice " << binChoice;
            
            for (auto & a : actions) {
                ofxPrecisionGrid * p = a.unit->parent;
                
                int bef = a.unit->getIndex();
                int aft = a.unit->getIndex() + 1;
                
                if (a.type == PRECISION_DB) {
                    ofLog() << "+DB";
                    p->add(binChoice, aft);
                    binChoice = -1;
                    return;
                }
                if (a.type == PRECISION_DR) {
                    ofLog() << "+DR";
                    p->add(binChoice, aft);
                    binChoice = -1;
                    return;
                }
                if (a.type == PRECISION_DL) {
                    ofLog() << "+DL";
                    p->add(binChoice, bef);
                    binChoice = -1;
                    return;
                }
                if (a.type == PRECISION_DT) {
                    ofLog() << "+DT";
                    p->add(binChoice, bef);
                    binChoice = -1;
                    return;
                }
                if (a.type == PRECISION_IN) {
                    ofLog() << "+IN";
                    a.unit->add(binChoice);
                    binChoice = -1;
                    return;
                }
            }
        }
        actions.clear();
        binChoice = -1;
        moved(x, y);
    }
    void dragged(int x, int y) {
        
        if (binChoice != -1) {
            
            /*-- reuse moved logic --*/
            
            moved(x, y);
            return;
        }
        
        bool isShift = ofGetKeyPressed(OF_KEY_SHIFT);
        bool isAlt = ofGetKeyPressed(OF_KEY_ALT);
        
        if (unit->hasError() ) {
            x = lastDragged.x;
            y = lastDragged.y;
        }
        
        
        for (auto & a : actions) {
            
            
            if (!a.unit->parent) {
                
                ofRectangle rr = a.oBounds;
                float cx = a.oPress.x - x;
                float cy = a.oPress.y - y;
                float cw = (a.type == PRECISION_W) ? rr.width - cx : a.unit->bounds.width;
                float ch = (a.type == PRECISION_H) ? rr.height - cy : a.unit->bounds.height;
                
                a.unit->set( rr.x, rr.y, cw, ch);
                
                
            } else {
            
                float pW = a.unit->parent->bounds.width; // parent width
                float pH = a.unit->parent->bounds.height; // parent height
                float offX = (a.oPress.x - x) / pW; // X multiple
                float offY = (a.oPress.y - y) / pH; // Y multiple
                int idx = a.unit->getIndex();
                
                
                for (int i = 0; i < a.unit->parent->inner.size(); i++) {
                    ofxPrecisionGrid * sib = a.unit->parent->inner[i];
                    float w = a.mWidths[i];
                    float h = a.mHeights[i];
                    
                    if (!isShift && !isAlt) {
                        if (a.type == PRECISION_W) {
                            sib->mW = w;
                            if (i == idx) sib->mW = w - offX;
                            if (i == idx + 1) sib->mW = w + offX;
                        }
                        
                        if (a.type == PRECISION_H) {
                            sib->mH = h;
                            if (i == idx) sib->mH = h - offY;
                            if (i == idx + 1) sib->mH = h + offY;
                        }
                        
                    } // !shift + !alt
                    
                    if (isShift && isAlt) {
                        
                        if (a.type == PRECISION_W) {
                            sib->mW = w;
                            if (i == idx) sib->mW = w - offX;
                            if (i > idx) sib->mW = w + offX / (float)( a.mWidths.size() - idx - 1 );
                        }
                        
                        if (a.type == PRECISION_H) {
                            sib->mH = h;
                            if (i == idx) sib->mH = h - offY;
                            if (i > idx) sib->mH = h + offY / (float)( a.mHeights.size() - idx - 1 );
                        }
                    } // shift + alt
                    
                    if (isShift && !isAlt) {
                        if (a.type == PRECISION_W) {
                            sib->mW = w;
                            if (i < idx + 1) sib->mW = w - (offX / (float)( idx + 1) );
                            if (i == idx + 1) sib->mW = w + offX;
                        }
                        
                        if (a.type == PRECISION_H) {
                            sib->mH = h;
                            if (i < idx + 1) sib->mH = h - (offY / (float)( idx + 1) );
                            if (i == idx + 1) sib->mH = h + offY;
                        }
                    } // shift + !alt
                    
                } // for each sibling
                
            } // if
            
            if (a.unit->parent) a.unit->parent->amend();
            if (!a.unit->parent) a.unit->amend();
            
            
        } // for each action
        
        if (!unit->hasError())  {
            lastDragged.x = x;
            lastDragged.y = y;
        } else {
            unit->errors.clear();
            dragged(lastDragged.x, lastDragged.y);
        }
        
        
    }
    
    void pressed(int x, int y) {
        
        for (int i = 0; i < bin->global.size(); i++) {
            if (bin->global[i]->bounds.inside(x,y)) {
                binChoice = i;
                return;
            }
        }
        
        for (auto & u : unit->global) {
            if (u->bounds.inside(x, y)) {
                currName = u->name;
                currLoc = u->location;
            }
        }
        
        for (auto & a : actions) {
            a.oPress.x = x;
            a.oPress.y = y;
        }
    }
    
    
    void moved(int x, int y) {
        
        
        actions.clear();
        float padd = 5;
        float rszo = unit->style.resizeOffset;
        
        resizing.clear();

        for (auto & g : unit->global) {
            
            /*-- offset by depth --*/
            
            int d = g->depth;
            ofPoint p( x + (d * -10) , y + (d * -10) );
            
            vector<float> mWidths = g->getWidths();
            vector<float> mHeights = g->getHeights();
            
            ofRectangle b(g->bounds);
            
            /*-- Offset bounds by resizing offset --*/
            
            if (mode == PRECISION_M_RESIZE) {
            
                b.x += rszo * d;
                b.y += rszo * d;
                
            }
            
            string n = g->name;
            
            bool isLast = areas[n].isLast;
            bool isFirst = areas[n].isFirst;
            bool isInCol = areas[n].isInCol;
            bool isInRow = areas[n].isInRow;
            bool noInner = g->inner.size() == 0;
            
            bool inR = areas[n].r.inside(x, y);
            bool inB = areas[n].b.inside(x, y);
            bool inL = areas[n].l.inside(x, y);
            bool inT = areas[n].t.inside(x, y);
            
            bool inH = (y > b.y - padd && y < b.y + b.height + padd);
            bool inW = (x > b.x - padd && x < b.x + b.width + padd);
            
            ofRectangle intra;
            intra.x = areas[n].l.getBottomRight().x;
            intra.y = areas[n].t.getBottomLeft().y;
            intra.width = areas[n].r.getTopLeft().x - intra.x;
            intra.height = areas[n].b.getTopRight().y - intra.y;
            
            /*-- dragging and dropping --*/
            
            if (binChoice != -1) {
            
                /*-- drop area --*/
                
                if (intra.inside(x, y) && noInner) {
                    ofxPrecisionAction a(PRECISION_IN, g, p, mWidths, mHeights);
                    actions.push_back(a);
                }
            
                
                /*-- drop right --*/
                
                if (inR && inH && isInRow) {
                    ofxPrecisionAction a(PRECISION_DR, g, p, mWidths, mHeights);
                    actions.push_back(a);
                }

                /*-- drop left --*/

                if (inL && inH && isInRow) {
                    ofxPrecisionAction a(PRECISION_DL, g, p, mWidths, mHeights);
                    actions.push_back(a);
                }
            
                /*-- drop bottom --*/
                
                if (inB && inW && isInCol) {
                    ofxPrecisionAction a(PRECISION_DB, g, p, mWidths, mHeights);
                    actions.push_back(a);
                }

                /*-- drop top --*/

                if (inT && inW && isInCol && isFirst) {
                    ofxPrecisionAction a(PRECISION_DT, g, p, mWidths, mHeights);
                    actions.push_back(a);
                }
            
                
            } else {

                /*-- resizing --*/


                /*-- height drag --*/


                if (inB && inW && !isLast && !isInRow) {
                    vector<float> mWidths = g->getWidths();
                    vector<float> mHeights = g->getHeights();
                    ofxPrecisionAction a(PRECISION_H, g, p, mWidths, mHeights);
                    actions.push_back(a);
                    resizing.push_back(a.unit);
                }

                /*-- width drag --*/

                if (inR && inH && !isLast && !isInCol) {
                    ofxPrecisionAction a(PRECISION_W, g, p, mWidths, mHeights);
                    actions.push_back(a);
                    resizing.push_back(a.unit);
                }

            }
        }
    }
    
    
};
