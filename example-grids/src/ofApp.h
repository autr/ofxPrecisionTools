#pragma once

#include "ofMain.h"
#include "ofxKitGrid.h"
#include "ofxKitUi.h"
#include "ofxKitUndo.h"
#include "ofxKitCursor.h"
#include "ofxTimeMeasurements.h"
#include "ofxKitLogger.h"

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
        void mouseScrolled( ofMouseEventArgs& e );
        void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void draw(ofxKitGrid * g, int x, int y);
        ofxKitGrid * grid;
        ofxKitUi * ui;
        ofxKitUndo * undo;
        ofxKitCursor * cursor;
        void generate(ofxKitGrid * iter);
        int depth;
        int maxIter;
        int count;
        bool iso;
        string name;
        vector<int> sel;
        ofJson json;
        int mx, my;
//        ofxScrollBox scroll;
        ofxKitGrid * bin;
    
    ofxKitLogger logger;
    
        
};
