
#pragma once
#include "ofMain.h"
#include "ofxKitGrid.h"

/*-- struct types --*/

#include "ofxKitAction.h"
#include "ofxKitAreas.h"

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

class ofxKitUi {
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
    
    vector<ofxKitGrid *> selected;
    ofRectangle dragArea;
    
    /*-- drag + drop --*/

    bool moveActive = false;
    ofPoint movePos;
    
    ofxKitGrid * bin;
    int binChoice;
    
    /*-- setup --*/
    
    vector<ofxKitGrid * > grids;
    ofxKitGrid * unit;
    ofxKitGridStyle style;
    vector<ofxKitAction> actions;
    map<ofxKitGrid *, ofxKitAreas> areas;
    
    ofxKitUi(ofxKitGrid * u, ofxKitGrid * b);
    
    /*-- current selected --*/

    ofxKitGrid * current = nullptr;
    
    void up();
    void down();
    void prev();
    void next();
    void select(ofxKitGrid * u);
    
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

    void onAction(ofxKitEvent & e);

    /*-- draw --*/
    
    void draw(bool iso);

    /*-- util --*/
    
    ofxKitGrid  * findCommonParent( vector<ofxKitGrid *> list );
    ofxKitGrid * deepest(int x, int y);
    void logAction( ofxKitAction & a );
    void drawAreas();
    void offsetScroll();
    
    
    void clear();
    

};
