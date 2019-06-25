
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
    }
};

class ofxPrecisionUi {
public:
    ofxPrecisionGrid * unit;
    ofxPrecisionDropper(ofxPrecisionGrid * u) {
        unit = u;
    }
    
    void moved(int x, int y) {
        
        
        float padd = 5;//minPadding;
        for (auto & g : unit->global) {
            
            /*-- offset by depth --*/
            
            int d = g->depth;
            ofPoint p( x + (d * -10) , y + (d * -10) );
            
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
    
    
};
