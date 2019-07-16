
#include "ofxKitUi.h"


ofxKitUi::ofxKitUi(ofxKitGrid * u, ofxKitGrid * b) {
    unit = u;
    mode = PRECISION_M_RESIZE;
    isCurrently = CURRENTLY_NOWT;
    
    binChoice = -1;
    bin = b;
    
    for (auto & u : unit->global) {
        ofxKitAreas a( u );
        areas[u] = a;
    }
    
    ofAddListener(unit->event, this, &ofxKitUi::onAction);
    
    offsetScroll();
}

void ofxKitUi::keypress(int k) {
    
    int i = k - 48;
    if (i == PRECISION_M_NONE) mode = PRECISION_M_NONE;
    if (i == PRECISION_M_RESIZE) mode = PRECISION_M_RESIZE;
    if (i == PRECISION_M_MOVE) mode = PRECISION_M_MOVE;
    if (i == PRECISION_M_PADDING) mode = PRECISION_M_PADDING;
    
    if (k == OF_KEY_BACKSPACE) {
        if (current != unit) {
            actions.clear();
            current->remove();
            current = nullptr;
            unit->amend();
        }
    }
}
void ofxKitUi::keyrelease(int k) {
    
}

void ofxKitUi::up() {
    
    ofxKitGrid * u = current;
    
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
void ofxKitUi::down() {
    
    ofxKitGrid * u = current;
    
    if (u && u->parent) {
        if (u->parent) select(u->parent);
    }
}
void ofxKitUi::prev() {
    
    ofxKitGrid * u = current;
    
    if (u && u->parent) {
        
        int i = u->getIndex() - 1;
        if (i < 0) i = u->parent->inner.size() - 1;
        select( u->parent->inner[i] );
    }
}
void ofxKitUi::next() {
    
    ofxKitGrid * u = current;
    
    if (u && u->parent) {
        
        int i = u->getIndex() + 1;
        if (i >= u->parent->inner.size()) i = 0;
        select( u->parent->inner[i] );
    }
}

void ofxKitUi::select(ofxKitGrid * u) {
    current = u;
}

void ofxKitUi::drawAreas() {
    for( auto & a : areas) {
        ofSetColor(255,0,0,150);
        ofNoFill();
        ofDrawRectangle(a.second.t);
        ofDrawRectangle(a.second.b);
        ofDrawRectangle(a.second.r);
        ofDrawRectangle(a.second.l);
    }
}

void ofxKitUi::onAction(ofxKitEvent & e) {
    
    if (e.type == "clear" ) {
        areas.clear();
    }
    if (e.type == "loaded" ) {
        current = nullptr;
    }
    
    if (e.type == "added") select(e.u);
    
    if (e.type == "amended") {
        ofxKitAreas a( e.u );
        areas[e.u] = a;
    }
    if (e.type == "removed") {
        areas.erase(e.u);
    }
    if (e.type == "setscroll") {
        offsetScroll();
    }
    
}

void ofxKitUi::offsetScroll() {
    
    vector<ofxKitGrid *> list;
    int count = unit->getScrollingGrids(list);
    float x = unit->bounds.x;
    float w = unit->bounds.width;
    float m = 20;
    float i = 0;
    for (auto & ch : list) {
        float o = w - (ch->bounds.x - x) + m;
        i += o;
        ch->sideOffset = i;
    }
    unit->amend();
}

void ofxKitUi::draw(bool iso) {
    
    bool modeMove = isMode( PRECISION_M_MOVE );
    bool modeResize = isMode( PRECISION_M_RESIZE );
    
    bool isSelecting = is(CURRENTLY_SELECTING);
    bool isResizing = is(CURRENTLY_RESIZING);
    bool isMoving = is(CURRENTLY_MOVING);
    
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
        if (iso) {
        ofPushMatrix();
        int d = current->depth();
        ofTranslate(0, 0, d * 80 );
        }
        ofxKitGrid * curr = current;
        ofDrawRectangle(curr->bounds);
        ofNoFill();
        if (iso) ofPopMatrix();
        if (current->fixed) {
            ofDrawBitmapStringHighlight("Fixed", current->bounds.getCenter());
        }
        if (current->scroll) {
            ofDrawBitmapStringHighlight("Scroll", current->bounds.getCenter());
        }
    }
    
    /*-- actions --*/
    
    if (isMoving || modeResize ) {
    
    
        for (auto & act : actions) {
            
            ofxKitGrid * u = act.unit;
            int d = u->depth();
            
            ofPushMatrix();
            
            
            ofSetColor(style.hoverColor);
            ofNoFill();
            
            ofPoint tl = u->bounds.getTopLeft();
            ofPoint tr = u->bounds.getTopRight();
            ofPoint br = u->bounds.getBottomRight();
            ofPoint bl = u->bounds.getBottomLeft();
            
            
            bool isResize = (act.type == PRECISION_RR || act.type == PRECISION_RB );
            
            if (isResize && u->parent) ofDrawRectangle(u->parent->bounds);
            if (isResize && !u->parent) ofDrawRectangle(u->bounds);
            
            if (act.type == PRECISION_RR) ofDrawLine(tr,br);
            if (act.type == PRECISION_RB) ofDrawLine(bl,br);
            if (act.type == PRECISION_RT) ofDrawLine(tl,tr);
            if (act.type == PRECISION_RL) ofDrawLine(tl,bl);
            
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
    
    /*-- bin --*/
    
    for (int i = 1; i < 4; i++) {
        string t = "None";
        if (i == PRECISION_M_RESIZE) t = "Resize / Move";
        if (i == PRECISION_M_MOVE) t = "Resize / Move";
        if (i == PRECISION_M_PADDING) t = "Padding";
        
        ofSetColor(0,255,255);
        if ( mode == i ) ofSetColor( 255 );
        ofDrawBitmapString( t , 20, 120 + (20 * i) );
    }
    
    
    ofNoFill();
//
//    for ( auto & u : selected ) {
//        ofSetColor(255,0,0);
//        ofDrawRectangle(u->bounds);
//    }
//
    
    /*-- select area --*/
    
    ofSetColor(255,255,255,100);
    if ( is(CURRENTLY_SELECTING) )ofDrawRectangle(dragArea);
    
    /*-- select area --*/
    
    if ( is(CURRENTLY_MOVING) ) {
        ofNoFill();
        ofSetColor(current->style.defaultColor, 100);
        ofRectangle r = current->bounds;
        r.x = draggedOrigin.x;
        r.y = draggedOrigin.y;
        ofDrawRectangle( r );
    }
    
    
    
}

ofxKitGrid * ofxKitUi::deepest(int x, int y) {
    
    ofxKitGrid * o = nullptr;
    int d = -1;
    for (auto & u : unit->global) {
        
        /*-- find deepest --*/
        
        if (u->bounds.inside(x, y) && u->depth() > d) {
            o = u;
        }
    }
    
    return o;
}


void ofxKitUi::setStatus(int i) {
    isCurrently = i;
}

bool ofxKitUi::is(int i) {
    return isCurrently == i;
}
bool ofxKitUi::isMode(int i) {
    return mode == i;
}
void ofxKitUi::selectFromRect(ofRectangle r) {
    
    selected.clear();
    for (auto & u : unit->global) {
        if (u->bounds.intersects( r ) && u->inner.size() == 0) selected.push_back( u );
    }
    
    current = findCommonParent( selected );
}

void ofxKitUi::logAction( ofxKitAction & a ) {
    
    int t = a.type;
    string m = "";
    if (t == PRECISION_RR) m = "PRECISION_RR";
    if (t == PRECISION_RB) m = "PRECISION_RB";
    if (t == PRECISION_RL) m = "PRECISION_RL";
    if (t == PRECISION_RT) m = "PRECISION_RT";
    
    if (t == PRECISION_IN) m = "PRECISION_IN";
    if (t == PRECISION_PL) m = "PRECISION_PL";
    if (t == PRECISION_PT) m = "PRECISION_PT";
    if (t == PRECISION_PR) m = "PRECISION_PR";
    if (t == PRECISION_PB) m = "PRECISION_PB";
    if (t == PRECISION_DT) m = "PRECISION_DT";
    if (t == PRECISION_DR) m = "PRECISION_DR";
    if (t == PRECISION_DB) m = "PRECISION_DB";
    if (t == PRECISION_DL) m = "PRECISION_DL";
    if (t == PRECISION_DH) m = "PRECISION_DH";
    if (t == PRECISION_DV) m = "PRECISION_DV";
    ofLogVerbose("[ofxKitUi]") << "Pressed:" << m << a.unit->name;
}

void ofxKitUi::pressed(int x, int y) {
    
    /*-- dropper --*/
    
    for (int i = 0; i < bin->inner.size(); i++) {
        if (bin->inner[i]->bounds.inside(x,y)) {
            binChoice = i;
            setStatus(CURRENTLY_ADDING);
            return;
        }
    }
    
    /*-- actions --*/
    
    bool modeMove = isMode( PRECISION_M_MOVE );
    bool modeResize = isMode( PRECISION_M_RESIZE );
    bool hasCurr = (current);
    bool isInsideCurrent = (current) ? current->bounds.inside(x,y) : false;
    bool hasResizeActions = false;
    
    for (auto & a : actions) {
        
        logAction( a );
        if (a.type == PRECISION_RR || a.type == PRECISION_RB || a.type == PRECISION_RR || a.type == PRECISION_RL ) hasResizeActions = true;
    }
    
    if (modeMove || modeResize) {
        if (isInsideCurrent && !hasResizeActions) {
            setStatus( CURRENTLY_MOVING );
        } else if (hasResizeActions) {
            setStatus( CURRENTLY_RESIZING );
        } else {
            setStatus( CURRENTLY_SELECTING );
            
        }
    } else {
        
        if (isInsideCurrent) setStatus( CURRENTLY_SELECTING );
    }
    
    
    
    /*-- reset all actions origin --*/
    
    for (auto & a : actions) {
        a.oPress.x = x;
        a.oPress.y = y;
    }
    
    if (current) pressOriginBounds = current->bounds;
    pressOrigin.x = x;
    pressOrigin.y = y;
    draggedOrigin = pressOrigin;
    dragArea.set( x, y, 0, 0);
}


void ofxKitUi::dragged(int x, int y) {
    
    /*-- dropper --*/
    
    if (is(CURRENTLY_ADDING)) {
        
        actions.clear();
        findDropPoints(x, y);
        
        return;
    }
    
    /*-- actions --*/
    
    bool isSelecting = is(CURRENTLY_SELECTING);
    bool isResizing = is(CURRENTLY_RESIZING);
    bool isMoving = is(CURRENTLY_MOVING);
    
    if ( isSelecting ) {
        
        /*-- dragbox --*/
        
        dragArea.x = pressOrigin.x;
        dragArea.y = pressOrigin.y;
        dragArea.width = x - pressOrigin.x;
        dragArea.height = y - pressOrigin.y;
        
        selectFromRect( dragArea );
        
    } else if ( isResizing ) {
        
        /*-- resizing --*/
        
        dragToResize( x, y );
        
    } else if  ( isMoving ) {
        
        /*-- dropping --*/
        
        actions.clear();
        
        bool isGhost = current->ghost;
        
        ofRectangle r = pressOriginBounds;
        float ox = pressOrigin.x - x;
        float oy = pressOrigin.y - y;
        draggedOrigin.x = r.x - ox;
        draggedOrigin.y = r.y - oy;
        
        if (isGhost) {
            current->set( draggedOrigin.x, draggedOrigin.y, r.width, r.height );
            current->amend();
        }
        
        findDropPoints(x, y);
    }
    
    
    
}
void ofxKitUi::released(int x, int y) {
    
    /*-- dropper --*/
    
    if (is(CURRENTLY_ADDING)) {
        
        addFromBin( x, y );
        binChoice = -1;
        return;
    }

    /*-- actions --*/
    
    ofRectangle r(x - 2, y -2, 4, 4);
    bool sameOrigin = r.inside(x, y);
    bool modeMove = isMode( PRECISION_M_MOVE );
    bool modeResize = isMode( PRECISION_M_RESIZE );
    bool isSelecting = is( CURRENTLY_SELECTING );
    bool isMoving = is( CURRENTLY_MOVING );
    bool isResizing = is( CURRENTLY_RESIZING );
    
    /*-- move or click to select --*/
    
    bool wasMoved = (isMoving) ? moveToPosition( x, y) : false;
    bool isGhost = (current) ? current->ghost : false;
    if (!wasMoved && !isGhost) {
        clickToSelect(x , y);
    }
    
    actions.clear();
    setStatus( CURRENTLY_NOWT );
    
}

void ofxKitUi::moved(int x, int y) {
    
    
    bool modeMove = isMode(PRECISION_M_MOVE);
    bool modeResize = isMode( PRECISION_M_RESIZE );
    bool modePadding = isMode( PRECISION_M_PADDING );
    
    setStatus( CURRENTLY_NOWT );
    
    if ( modeResize || modeResize ) {
        
        actions.clear();
        findResizePoints( x, y );
        findDropPoints( x, y );
        
    }
    if (actions.size() > 0) setStatus( CURRENTLY_HOVERING );

}


void ofxKitUi::clickToSelect( int x, int y ) {
    
    ofPoint p = pressLast;
    float t = ofGetElapsedTimef();
    
    
    if (ofGetKeyPressed(OF_KEY_SHIFT) && current) {
        
        /*-- select shared parent element --*/
        
        ofxKitGrid * pressed = deepest(x , y);
        
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
            
            selectFromRect( dragArea );
            pressCounter = 0;
        }
        pressTimestamp = t;
    }
    
    
    pressLast.x = x;
    pressLast.y = y;
}


void ofxKitUi::addFromBin( int x, int y ) {
    
    /*-- drag and drop --*/
    
    
    for (auto & a : actions) {
        ofxKitGrid * p = a.unit->parent;
        
        int bef = a.unit->getIndex();
        int aft = a.unit->getIndex() + 1;
        
        if (a.type == PRECISION_DB) {
            ofLogVerbose("[ofxKitUi]") << "Dropping: bottom" << a.unit->name;
            p->add(binChoice, aft);
            binChoice = -1;
            return;
        } else if (a.type == PRECISION_DR) {
            ofLogVerbose("[ofxKitUi]") << "Dropping: right" << a.unit->name;
            p->add(binChoice, aft);
            binChoice = -1;
            return;
        } else if (a.type == PRECISION_DL) {
            ofLogVerbose("[ofxKitUi]") << "Dropping: left" << a.unit->name;
            p->add(binChoice, bef);
            binChoice = -1;
            return;
        } else if (a.type == PRECISION_DT) {
            ofLogVerbose("[ofxKitUi]") << "Dropping: top" << a.unit->name;
            p->add(binChoice, bef);
            binChoice = -1;
            return;
        } else if (a.type == PRECISION_IN) {
            ofLogVerbose("[ofxKitUi]") << "Dropping: inside" << a.unit->name;
            a.unit->add(binChoice);
            binChoice = -1;
            return;
        }
    }
    
    ofLogVerbose("[ofxKitUi]") << "Dropping: ghost";
    
//    ofxKitGrid * ch = new ofxKitGrid(binChoice);
//    ch->mH = 1; // default multiH
//    ch->mW = 1; // default multiWidth
//    ch->parent = nullptr;
    
    
//    ofxKitGrid * ch = &unit->add(binChoice);//new ofxKitGrid(binChoice);
//    ch->mH = 1;
//    ch->mW = 1;
////    if (ch->parent) ch->remove( ch->parent->inner, ch ); // remove from old parent
//    ch->set( x, y, 100, 100 );
//    ch->ghost = true;
//    ch->amend();
//    unit->amend();
//
//    ofxKitEvent e("added", ch);
//    ofNotifyEvent(unit->event, e);
    
}


bool ofxKitUi::moveToPosition( int x, int y ) {
    
    if ( !current ) return false;
    
    ofPoint p = pressLast;
    float t = ofGetElapsedTimef();
    bool wasMoved = false;
    
    /*-- moving drag and drop --*/
    
    for (auto & a : actions) {
        
        if (unit->name != current->name) {
            
            
            int bef = a.unit->getIndex();
            int aft = a.unit->getIndex() + 1;
            
            bool isDropping = (a.type == PRECISION_DB || a.type == PRECISION_DR || a.type == PRECISION_DL || a.type == PRECISION_DT || a.type == PRECISION_IN );
            
            if (isDropping) {
                
                
                bool isInside = (a.type == PRECISION_IN);
                bool isAfter = (a.type == PRECISION_DB || a.type == PRECISION_DR);
                bool isBefore = (a.type == PRECISION_DL || a.type == PRECISION_DT);
                int pos = (isAfter) ? a.unit->getIndex() + 1 : a.unit->getIndex();
                ofxKitGrid * target = (isInside) ? a.unit : a.unit->parent;
                
                actions.clear();
                
                wasMoved = target->move(current, pos);
                if (wasMoved) unit->amend();
            }
            
        }
    }
    
    return wasMoved;
}

void ofxKitUi::dragToResize(int x, int y) {
    
    bool isShift = ofGetKeyPressed(OF_KEY_SHIFT);
    bool isAlt = ofGetKeyPressed(OF_KEY_ALT);
    
    for (auto & a : actions) {
        
        bool isGhost = a.unit->ghost;
        
        if (!a.unit->parent || isGhost) {
            
            /*-- resize container --*/
            
            ofRectangle rr = a.oBounds;
            float ox = a.oPress.x - x;
            float oy = a.oPress.y - y;
            float cx = rr.x;
            float cy = rr.y;
            float cw =  a.unit->bounds.width;
            float ch =  a.unit->bounds.height;
            if (a.type == PRECISION_RR) {
                cw = rr.width - ox;
            }
            if (a.type == PRECISION_RB) {
                ch = rr.height - oy;
            }
            if (a.type == PRECISION_RL) {
                cx = rr.x - ox;
                cw = rr.width + ox;
            }
            if (a.type == PRECISION_RT) {
                cy = rr.y - oy;
                ch = rr.height + oy;
            }
            
            a.unit->set( cx, cy, cw, ch);
            a.unit->amend();
            setStatus( CURRENTLY_RESIZING );
            
            
        } else {
            
            /*-- resize element --*/
            
            float pW = a.unit->parent->bounds.width; // parent width
            float pH = a.unit->parent->bounds.height; // parent height
            float offX = (a.oPress.x - x) / pW; // X multiple
            float offY = (a.oPress.y - y) / pH; // Y multiple
            int idx = a.unit->getIndex();
            
            offY *= std::accumulate(a.mHeights.begin(), a.mHeights.end(), 0.0);
            offX *= std::accumulate(a.mWidths.begin(), a.mWidths.end(), 0.0);
            
            
            for (int i = 0; i < a.unit->parent->inner.size(); i++) {
                ofxKitGrid * sib = a.unit->parent->inner[i];
                float w = a.mWidths[i];
                float h = a.mHeights[i];
                bool isResize = (a.type == PRECISION_RR || a.type == PRECISION_RB );
                if (isResize) setStatus( CURRENTLY_RESIZING );
                
                
                
                if (isShift && isAlt) {
                    
                    /*-- resize: shift + alt --*/
                    
                    if (a.type == PRECISION_RR) {
                        sib->mW = w;
                        if (i == idx) sib->mW = w - offX;
                        if (i > idx) sib->mW = w + offX / (float)( a.mWidths.size() - idx - 1 );
                    }
                    
                    if (a.type == PRECISION_RB) {
                        sib->mH = h;
                        if (i == idx) sib->mH = h - offY;
                        if (i > idx) sib->mH = h + offY / (float)( a.mHeights.size() - idx - 1 );
                    }
                } else if (isShift && !isAlt) {
                    
                    /*-- resize: shift --*/
                    
                    if (a.type == PRECISION_RR) {
                        sib->mW = w;
                        if (i < idx + 1) sib->mW = w - (offX / (float)( idx + 1) );
                        if (i == idx + 1) sib->mW = w + offX;
                    }
                    
                    if (a.type == PRECISION_RB) {
                        sib->mH = h;
                        if (i < idx + 1) sib->mH = h - (offY / (float)( idx + 1) );
                        if (i == idx + 1) sib->mH = h + offY;
                    }
                } else {
                    
                    /*-- resize --*/
                    
                    if (a.type == PRECISION_RR) {
                        sib->mW = w;
                        if (i == idx) sib->mW = w - offX;
                        if (i == idx + 1) sib->mW = w + offX;
                    }
                    
                    if (a.type == PRECISION_RB) {
                        sib->mH = h;
                        if (i == idx) sib->mH = h - offY;
                        if (i == idx + 1) sib->mH = h + offY;
                    }
                    
                }
                
                if (i == idx) {
                    
                    ofRectangle rr = a.oBounds;
                    float ox = a.oPress.x - x;
                    float oy = a.oPress.y - y;
                    float cx = rr.x;
                    float cy = rr.y;
                    float cw =  a.unit->bounds.width;
                    float ch =  a.unit->bounds.height;
                    if (a.type == PRECISION_RR) {
                        cw = rr.width - ox;
                    }
                    if (a.type == PRECISION_RB) {
                        ch = rr.height - oy;
                    }
                    a.unit->set( cx, cy, cw, ch);
                }
                
                
            } // for each sibling
            
        } // if
        
        if (a.unit->parent) a.unit->parent->amend();
        if (!a.unit->parent) a.unit->amend();
        
        
    } // for each action
    
}


void ofxKitUi::clear() {
    
    actions.clear();
    areas.clear();
    delete current;
    delete unit;
    current = nullptr;
    unit = nullptr;
}


void ofxKitUi::findDropPoints( int x, int y ) {
    
    
    ofPoint p(x, y);
    float padd = 5;
    bool isFound = false;
    
    
    for (auto & g : unit->global) {
        
        ofRectangle b = g->bounds;
        
        bool isLast = areas[g].isLast;
        bool isFirst = areas[g].isFirst;
        
        bool isInCol = areas[g].isInCol;
        bool isInRow = areas[g].isInRow;
        bool noInner = g->inner.size() == 0;
        
        bool inR = areas[g].r.inside(x, y);
        bool inB = areas[g].b.inside(x, y);
        bool inL = areas[g].l.inside(x, y);
        bool inT = areas[g].t.inside(x, y);
        
        bool inH = (y > b.y - padd && y < b.y + b.height + padd);
        bool inW = (x > b.x - padd && x < b.x + b.width + padd);
        
        /*-- rectangle exluding edges --*/
        
        ofRectangle intra;
        intra.x = areas[g].l.getBottomRight().x;
        intra.y = areas[g].t.getBottomLeft().y;
        intra.width = areas[g].r.getTopLeft().x - intra.x;
        intra.height = areas[g].b.getTopRight().y - intra.y;
        
        bool dropInside = intra.inside(x, y) && noInner;
        bool dropRight = inR && inH && isInRow;
        bool dropLeft = inL && inH && isInRow;
        bool dropBottom = inB && inW && isInCol;
        bool dropTop = inT && inW && isInCol && isFirst;
        
        vector<float> mWidths;
        vector<float> mHeights;
        
        /*-- check if its a valid drop area --*/
        
        bool invalid = (current) ? current->isInvalid( g ) : false;
        
        if (!invalid) {
            
            if (dropInside || dropRight || dropLeft || dropBottom || dropTop) {
                
                mWidths = g->getWidths();
                mHeights = g->getHeights();
                isFound = true;
            }
            
            /*-- drop area --*/
            
            if (dropInside) {
                ofxKitAction a(PRECISION_IN, g, p, mWidths, mHeights);
                actions.push_back(a);
            }
            
            
            /*-- drop right --*/
            
            if (dropRight) {
                ofxKitAction a(PRECISION_DR, g, p, mWidths, mHeights);
                actions.push_back(a);
            }
            
            /*-- drop left --*/
            
            if (dropLeft) {
                ofxKitAction a(PRECISION_DL, g, p, mWidths, mHeights);
                actions.push_back(a);
            }
            
            /*-- drop bottom --*/
            
            if (dropBottom) {
                ofxKitAction a(PRECISION_DB, g, p, mWidths, mHeights);
                actions.push_back(a);
            }
            
            /*-- drop top --*/
            
            if (dropTop) {
                ofxKitAction a(PRECISION_DT, g, p, mWidths, mHeights);
                actions.push_back(a);
            }
        }
        
    }
    
    return isFound;
}

void ofxKitUi::findResizePoints( int x, int y ) {
    
    
    ofPoint p(x, y);
    float padd = 5;
    bool isFound = false;
    
    for (auto & g : unit->global) {
    
            
        ofRectangle b = g->bounds;
        
        bool inR = areas[g].r.inside(x, y);
        bool inB = areas[g].b.inside(x, y);
        bool inL = areas[g].l.inside(x, y);
        bool inT = areas[g].t.inside(x, y);
        
        bool isLast = areas[g].isLast;
        bool isFirst = areas[g].isFirst;
        
        bool isInCol = areas[g].isInCol;
        bool isInRow = areas[g].isInRow;
        bool noInner = g->inner.size() == 0;
        
        bool inH = (y > b.y - padd && y < b.y + b.height + padd);
        bool inW = (x > b.x - padd && x < b.x + b.width + padd);
        
        bool resizeHeight = inB && inW && !isLast && !isInRow;
        bool resizeWidth = inR && inH && !isLast && !isInCol;
        
        bool isGhost = g->ghost || (!g->parent);
        
        bool resizeGhostLeft = inL && inH && isGhost;
        bool resizeGhostTop = inT && inW && isGhost;
        bool resizeGhostRight = inR && inH && isGhost;
        bool resizeGhostBottom = inB && inW && isGhost;
        
        
        
        vector<float> mWidths;
        vector<float> mHeights;
        
        if (resizeHeight || resizeWidth || resizeGhostLeft || resizeGhostTop) {
            
            mWidths = g->getWidths();
            mHeights = g->getHeights();
            isFound = true;
        }
        
        /*-- ghosts --*/
        
        if (resizeGhostLeft) {
            ofxKitAction a(PRECISION_RL, g, p, mWidths, mHeights);
            actions.push_back(a);
        }
        if (resizeGhostTop) {
            ofxKitAction a(PRECISION_RL, g, p, mWidths, mHeights);
            actions.push_back(a);
        }
        
        /*-- height drag --*/
        
        if (resizeHeight || resizeGhostBottom) {
            ofxKitAction a(PRECISION_RB, g, p, mWidths, mHeights);
            actions.push_back(a);
        }
        
        /*-- width drag --*/
        
        if (resizeWidth || resizeGhostRight) {
            ofxKitAction a(PRECISION_RR, g, p, mWidths, mHeights);
            actions.push_back(a);
        }
        
    }
    return isFound;
}


ofxKitGrid * ofxKitUi::findCommonParent( vector<ofxKitGrid *> list ) {
    
    if (list.size() <= 0) return nullptr;
    bool found = false;
    int i = 0;
    vector<int> loc;
    while( !found ) {
        
        bool cantGoDeeper = false;
        bool numSet = false;
        int num;
        found = false;
        for( auto & el : list) {
            
            if ( i < el->location.size() ) {
                int p = el->location[i];
                if (!numSet) {
                    num = el->location[i];
                    numSet = true;
                }
                if (numSet && num != el->location[i] ) {
                    found = true;
                }
            } else {
                found = true;
            }
        }
        
        if (!found) {
            loc.push_back(num);
            i += 1;
        }
        
        
        
    }
    return unit->get( loc );
}
