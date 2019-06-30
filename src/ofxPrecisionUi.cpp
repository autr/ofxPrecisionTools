
#include "ofxPrecisionUi.h"


ofxPrecisionUi::ofxPrecisionUi(ofxPrecisionGrid * u) {
    unit = u;
    mode = PRECISION_M_RESIZE;
    
    binChoice = -1;
    bin = new ofxPrecisionGrid();
    bin->set(20,20,20,80);
    bin->add(0);
    bin->add(0);
    bin->add(0);
    bin->add(0);
    
    for (auto & u : unit->global) {
        ofxPrecisionAreas a( u );
        areas[u] = a;
    }
    
    ofAddListener(unit->added, this, &ofxPrecisionUi::onAdded);
    ofAddListener(unit->amended, this, &ofxPrecisionUi::onAmended);
}

void ofxPrecisionUi::keypress(int k) {
    
    int i = k - 48;
    if (i == PRECISION_M_NONE) mode = PRECISION_M_NONE;
    if (i == PRECISION_M_RESIZE) mode = PRECISION_M_RESIZE;
    if (i == PRECISION_M_MOVE) mode = PRECISION_M_MOVE;
    if (i == PRECISION_M_MARGIN) mode = PRECISION_M_MARGIN;
    
    if (k == OF_KEY_BACKSPACE) {
        actions.clear();
        current->removeReferences();
        unit->amend();
        delete current;
        current = nullptr;
    }
}
void ofxPrecisionUi::keyrelease(int k) {
    
}

void ofxPrecisionUi::up() {
    
    ofxPrecisionGrid * u = current;
    
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
    }
}
void ofxPrecisionUi::down() {
    
    ofxPrecisionGrid * u = current;
    
    if (u && u->parent) {
        if (u->parent) select(u->parent);
    }
}
void ofxPrecisionUi::prev() {
    
    ofxPrecisionGrid * u = current;
    
    if (u && u->parent) {
        
        int i = u->getIndex() - 1;
        if (i < 0) i = u->parent->inner.size() - 1;
        select( u->parent->inner[i] );
    }
}
void ofxPrecisionUi::next() {
    
    ofxPrecisionGrid * u = current;
    
    if (u && u->parent) {
        
        int i = u->getIndex() + 1;
        if (i >= u->parent->inner.size()) i = 0;
        select( u->parent->inner[i] );
    }
}

void ofxPrecisionUi::select(ofxPrecisionGrid * u) {
    current = u;
}

void ofxPrecisionUi::onAdded(ofxPrecisionEvent & e) {
    
    select(e.u);
}
void ofxPrecisionUi::onAmended(ofxPrecisionEvent & e) {
    
    ofxPrecisionAreas a( e.u );
    areas[e.u] = a;
    
}

void ofxPrecisionUi::draw() {
    
    bin->draw();
    
    //        ofSetColor(255,255,0, 150);
    //        ofDrawRectangle(arr.t);
    //        ofDrawRectangle(arr.r);
    //        ofDrawRectangle(arr.b);
    //        ofDrawRectangle(arr.l);
    
    /*-- draw selected --*/
    
    ofSetColor(255,255,255,60);
    ofFill();
    
    if (current) {
        ofxPrecisionGrid * curr = current;
        ofDrawRectangle(curr->bounds);
        ofNoFill();
    }
    
    
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
        
        
        bool isResize = (act.type == PRECISION_W || act.type == PRECISION_H );
        
        if (isResize && u->parent) ofDrawRectangle(u->parent->bounds);
        if (isResize && !u->parent) ofDrawRectangle(u->bounds);
        
        if (act.type == PRECISION_W) ofDrawLine(tr,br);
        if (act.type == PRECISION_H) ofDrawLine(bl,br);
        
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
    
    
    for (int i = 1; i < 5; i++) {
        string t = "None";
        if (i == PRECISION_M_RESIZE) t = "Resize";
        if (i == PRECISION_M_MOVE) t = "Move";
        if (i == PRECISION_M_MARGIN) t = "Margin";
        
        ofSetColor(0,255,255);
        if ( mode == i ) ofSetColor( 255 );
        ofDrawBitmapString( t , 20, 120 + (20 * i) );
    }
    
    
    
}

ofxPrecisionGrid * ofxPrecisionUi::deepest(int x, int y) {
    
    ofxPrecisionGrid * o = nullptr;
    int d = -1;
    for (auto & u : unit->global) {
        
        /*-- find deepest --*/
        
        if (u->bounds.inside(x, y) && u->depth > d) {
            o = u;
        }
    }
    
    return o;
}

void ofxPrecisionUi::released(int x, int y) {
    
    if (binChoice >= 0) {
        
        released_drop( x, y );
        binChoice = -1;
        return;
    }
    
//    if (mode == PRECISION_M_MOVE) {
//        released_move( x, y);
//    }
    
    released_select( x, y );
    
    actions.clear();
    moved(x, y);
}

void ofxPrecisionUi::released_drop( int x, int y ) {
    
    /*-- drag and drop --*/
    
    
    for (auto & a : actions) {
        ofxPrecisionGrid * p = a.unit->parent;
        
        int bef = a.unit->getIndex();
        int aft = a.unit->getIndex() + 1;
        
        if (a.type == PRECISION_DB) {
            ofLog() << "+B" << binChoice;
            p->add(binChoice, aft);
            binChoice = -1;
            return;
        }
        if (a.type == PRECISION_DR) {
            ofLog() << "+R" << binChoice;
            p->add(binChoice, aft);
            binChoice = -1;
            return;
        }
        if (a.type == PRECISION_DL) {
            ofLog() << "+L" << binChoice;
            p->add(binChoice, bef);
            binChoice = -1;
            return;
        }
        if (a.type == PRECISION_DT) {
            ofLog() << "+T" << binChoice;
            p->add(binChoice, bef);
            binChoice = -1;
            return;
        }
        if (a.type == PRECISION_IN) {
            ofLog() << "+In" << binChoice;
            a.unit->add(binChoice);
            binChoice = -1;
            return;
        }
    }
    
}

void ofxPrecisionUi::released_select( int x, int y ) {
    
    ofPoint p = pressLast;
    float t = ofGetElapsedTimef();
    
    
    
    /*-- drag and drop --*/
    
    if (ofGetKeyPressed(OF_KEY_SHIFT)) {
        
        /*-- select shared parent element --*/
        
        ofxPrecisionGrid * pressed = deepest(x , y);
        
        if (pressed) {
            vector<int> newLoc;
            vector<int> currLoc = current->location;
            for (int i = 0; i < pressed->location.size(); i++) {
                if ( pressed->location[i] == currLoc[i] ) {
                    newLoc.push_back(pressed->location[i]);
                } else {
                    break;
                }
            }
            
            current = unit->get( newLoc );
        } else {
            
        }
        
        
        pressCounter = 0;
    } else {
        
        
        ofRectangle r(p.x - 2, p.y -2, 4, 4);
        bool isSamePos = r.inside(x, y);
        bool isInLimit = (t - pressTimestamp < pressTimeLimit);
        bool isRoot = (current == unit);
        
        if (isInLimit && isSamePos && !isRoot) {
            
            /*-- select parent for each click --*/
            
            pressCounter += 1;
            down();
            
        } else {
            
            /*-- select element --*/
            
            ofxPrecisionGrid * pressed = deepest(x , y);
            
            if (pressed) {
                current = pressed;
            } else {
                current = nullptr;
            }
            
            pressCounter = 0;
        }
        pressTimestamp = t;
    }
    
    
    pressLast.x = x;
    pressLast.y = y;
}

void ofxPrecisionUi::released_move( int x, int y ) {
    
    ofPoint p = pressLast;
    float t = ofGetElapsedTimef();
    
    for (auto & a : actions) {
        
        if (unit->name != current->name) {
            
            ofxPrecisionGrid * p = a.unit->parent;
            
            int bef = a.unit->getIndex();
            int aft = a.unit->getIndex() + 1;
            
            bool isDropping = (a.type == PRECISION_DB || a.type == PRECISION_DR || a.type == PRECISION_DL || a.type == PRECISION_DT || a.type == PRECISION_IN );
            
            if (isDropping) {
                
                ofLog() << "clearing";
                actions.clear();
                current->removeReferences();
                unit->amend();
            }
            
            if (current != p) {
                
                if (a.type == PRECISION_DB || a.type == PRECISION_DR) {
                    ofLog() << "~R / ~B";
                    p->add(current, aft);
                    return;
                }
                if (a.type == PRECISION_DL || a.type == PRECISION_DT) {
                    ofLog() << "~T / ~ L";
                    p->add(current, bef);
                    unit->amend();
                    return;
                }
                
            }
            
            if (current != a.unit) {
                if (a.type == PRECISION_IN) {
                    ofLog() << "~In";
                    a.unit->add(current);
                    unit->amend();
                    return;
                }
            }
        }
    }
    
    isMoving = false;
    
}

void ofxPrecisionUi::dragged(int x, int y) {
    
    if (binChoice != -1 || isMoving) {
        
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

void ofxPrecisionUi::pressed(int x, int y) {
    
    /*-- drag drop ui --*/
    
    for (int i = 0; i < bin->inner.size(); i++) {
        if (bin->inner[i]->bounds.inside(x,y)) {
            binChoice = i;
            return;
        }
    }
    
    /*-- select current --*/
    
    // TODO: moving
//    if (current) {
//        if (current->bounds.inside(x,y)) isMoving = true;
//    }
    
    
    
    /*-- reset all actions origin --*/
    
    for (auto & a : actions) {
        a.oPress.x = x;
        a.oPress.y = y;
    }
}

void ofxPrecisionUi::clear() {
    
    actions.clear();
    areas.clear();
    delete current;
    delete unit;
    current = nullptr;
    unit = nullptr;
}

void ofxPrecisionUi::moved(int x, int y) {
    
    
    actions.clear();
    float padd = 5;
    float rszo = unit->style.resizeOffset;
    

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
        
        ofxPrecisionGrid * n = g;
        
        bool isLast = areas[n].isLast;
        bool isFirst = areas[n].isFirst;
        bool isInCol = areas[n].isInCol;
        bool isInRow = areas[n].isInRow;
        bool noInner = g->inner.size() == 0;
        bool dragDrop = ( binChoice != -1 );
        
        
        bool inR = areas[n].r.inside(x, y);
        bool inB = areas[n].b.inside(x, y);
        bool inL = areas[n].l.inside(x, y);
        bool inT = areas[n].t.inside(x, y);
        
        bool inH = (y > b.y - padd && y < b.y + b.height + padd);
        bool inW = (x > b.x - padd && x < b.x + b.width + padd);
        
        
        /*-- rectangle exluding edges --*/
        
        ofRectangle intra;
        intra.x = areas[n].l.getBottomRight().x;
        intra.y = areas[n].t.getBottomLeft().y;
        intra.width = areas[n].r.getTopLeft().x - intra.x;
        intra.height = areas[n].b.getTopRight().y - intra.y;
        
        /*-- dragging and dropping --*/
        
        if (dragDrop || isMoving) {
            
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


            /*-- height drag --*/


            if (inB && inW && !isLast && !isInRow) {
                vector<float> mWidths = g->getWidths();
                vector<float> mHeights = g->getHeights();
                ofxPrecisionAction a(PRECISION_H, g, p, mWidths, mHeights);
                actions.push_back(a);
            }

            /*-- width drag --*/

            if (inR && inH && !isLast && !isInCol) {
                ofxPrecisionAction a(PRECISION_W, g, p, mWidths, mHeights);
                actions.push_back(a);
            }

        }
    }
}
