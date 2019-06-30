
#pragma once
#include "ofMain.h"
#include "ofxPrecisionGrid.h"

/*-- struct types --*/

#include "ofxPrecisionAction.h"
#include "ofxPrecisionAreas.h"

/*-- modes --*/

#define PRECISION_M_NONE 1
#define PRECISION_M_RESIZE 2
#define PRECISION_M_MOVE 3
#define PRECISION_M_MARGIN 4

class ofxPrecisionUi {
public:
    
    /*-- mode --*/

    int mode = 1;
    
    /*-- drag + drop --*/

    bool isMoving = false;
    ofxPrecisionGrid * bin;
    int binChoice;
    
    /*-- setup --*/
    
    ofxPrecisionGrid * unit;
    ofxPrecisionGridStyle style;
    vector<ofxPrecisionAction> actions;
    map<ofxPrecisionGrid *, ofxPrecisionAreas> areas;
    
    
    ofxPrecisionUi(ofxPrecisionGrid * u);
    
    
    /*-- current selected --*/

    ofxPrecisionGrid * current = nullptr;

    void up();
    void down();
    void prev();
    void next();
    void select(ofxPrecisionGrid * u);
    
    /*-- key interactions --*/

    void keypress(int k);
    void keyrelease(int k);

    /*-- mouse interactions --*/

    float pressTimestamp = 0;
    float pressTimeLimit = 1;
    int pressCounter = 0;
    ofPoint pressLast;
    vector<int> pressLastLoc;
    ofPoint lastDragged;

    void pressed(int x, int y);
    void released(int x, int y);
    void dragged(int x, int y);
    void moved(int x, int y);
    
    void pressed_resize( int x, int y);
    void pressed_move( int x, int y);
    void pressed_margin( int x, int y);
    
    void released_drop( int x, int y);
    void released_select( int x, int y);
    void released_move( int x, int y);

    /*-- callback events --*/

    void onAdded(ofxPrecisionEvent & e);
    void onAmended(ofxPrecisionEvent & e);

    /*-- draw --*/
    
    void draw();

    /*-- util --*/
    
    ofxPrecisionGrid * deepest(int x, int y);
    
    
    void clear();
    

};
