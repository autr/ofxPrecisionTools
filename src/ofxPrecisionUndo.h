#pragma once

#include "ofMain.h"
#include "ofxPrecisionGrid.h"
#include "ofxPrecisionEvent.h"

class ofxPrecisionUndo {
public:
    ofxPrecisionGrid * unit;
    float stamp;
    float tLimit;
    int aLimit;
    bool active;
    bool restoring;
    int count;
    vector<ofJson> store;
    int index;
    
    ofxPrecisionUndo( ofxPrecisionGrid * u ) {
        unit = u;
        tLimit = 0.2;
        aLimit = 20;
        count = 0;
        active = false;
        restoring = false;
        index = 0;
        ofAddListener(unit->event, this, &ofxPrecisionUndo::onAction);
    }
    void onAction( ofxPrecisionEvent & e) {
        active = true;
        stamp = ofGetElapsedTimef();
        count += 1;
    }
    void clear() {
        ofLog() << "Clear A" << store.size();
        index = 0;
        store.clear();
        ofLog() << "Clear B" << store.size();
    }
    void update() {
        
        
        if (ofGetElapsedTimef() - stamp > tLimit && active) {
            
            if (!restoring) {
                
                int s = store.size();
                if (index < s - 1) store.erase(store.begin() + index + 1, store.end());
                ofJson j = unit->json();
                store.push_back( j );
                while (store.size() > aLimit) {
                    store.erase( store.begin() );
                    index -= 1;
                }
                index = store.size() - 1;
                
            }
            restoring = false;
            active = false;
            count = 0;
        }
    }
    void restore() {
        if (store.size() <= 0) return;
        restoring = true;
        unit->load( store[index] );
    }
    void undo() {
        
        if (index > 0) index -= 1;
//        ofLog() << "UNDO" << index << store.size();
        restore();
    }
    void redo() {
        
        if (index < store.size() - 1) index += 1;
//        ofLog() << "REDO" << index << store.size();
        restore();
    }
    void keypress(int k) {
        
        bool cmd = ofGetKeyPressed( OF_KEY_COMMAND );
        bool shft = ofGetKeyPressed( OF_KEY_SHIFT );
        
        if (!shft && cmd && k == 'z') {
            undo();
        }
        if (shft && cmd && k == 'z') {
            redo();
        }
    }
};
