
#pragma once
#include "ofMain.h"
#include "ofxPrecisionGrid.h"

/*-- struct types --*/

#include "ofxPrecisionAction.h"
#include "ofxPrecisionAreas.h"

/*-- modes --*/

#define PRECISION_M_NONE 1
#define PRECISION_M_RESIZE 2
#define PRECISION_M_MOVE 2
#define PRECISION_M_PADDING 3

#define CURRENTLY_NOWT 0
#define CURRENTLY_SELECTING 1
#define CURRENTLY_ADDING 2
#define CURRENTLY_RESIZING 3
#define CURRENTLY_MOVING 4
#define CURRENTLY_HOVERING 5

class ofxPrecisionUi {
public:
    
    /*-- mode --*/

    int mode = 1;
    int isCurrently;
    ofPoint pressOrigin;
    ofRectangle pressOriginBounds;
    ofPoint draggedOrigin;
    
    bool is(int i);
    void setStatus(int i);
    bool isMode(int i);

    
    /*-- selecting --*/
    
    vector<ofxPrecisionGrid *> selected;
    ofRectangle dragArea;
    
    /*-- drag + drop --*/

    bool moveActive = false;
    ofPoint movePos;
    
    ofxPrecisionGrid * bin;
    int binChoice;
    
    /*-- setup --*/
    
    vector<ofxPrecisionGrid * > grids;
    ofxPrecisionGrid * unit;
    ofxPrecisionGridStyle style;
    vector<ofxPrecisionAction> actions;
    map<ofxPrecisionGrid *, ofxPrecisionAreas> areas;
    
    ofxPrecisionUi(ofxPrecisionGrid * u, ofxPrecisionGrid * b);
    
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
    
    /*-- actions --*/
    
    void findDropPoints( int x, int y);
    void findResizePoints( int x, int y);
    void dragToResize(int x, int y);
    void addFromBin( int x, int y);
    void clickToSelect( int x, int y);
    void selectFromRect( ofRectangle r );
    bool moveToPosition( int x, int y);

    /*-- callback events --*/

    void onAction(ofxPrecisionEvent & e);

    /*-- draw --*/
    
    void draw(bool iso);

    /*-- util --*/
    
    ofxPrecisionGrid  * findCommonParent( vector<ofxPrecisionGrid *> list );
    ofxPrecisionGrid * deepest(int x, int y);
    void logAction( ofxPrecisionAction & a );
    void drawAreas();
    void offsetScroll();
    
    
    void clear();
    

};
