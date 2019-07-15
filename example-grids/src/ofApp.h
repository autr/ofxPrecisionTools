#pragma once

#include "ofMain.h"
#include "ofxPrecisionGrid.h"
#include "ofxPrecisionUi.h"
#include "ofxPrecisionUndo.h"
#include "ofxPrecisionCursor.h"
//#include "ofxTimeMeasurements.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void draw(ofxPrecisionGrid * g, int x, int y);
        ofxPrecisionGrid * grid;
        ofxPrecisionUi * ui;
        ofxPrecisionUndo * undo;
        ofxPrecisionCursor * cursor;
        void generate(ofxPrecisionGrid * iter);
        int depth;
        int maxIter;
        int count;
        bool iso;
        string name;
        vector<int> sel;
        ofJson json;
        int mx, my;
    
        ofxPrecisionGrid * bin;
    
        
};
