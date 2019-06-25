#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(120);
    ofEnableAlphaBlending();
    ofLog::setAutoSpace(true);
    
    count = 0;
    depth = 2;
    maxIter = 6;
    sel = {};
    
    grid = new ofxPrecisionGrid(0, NULL);
    ofRectangle r(200,200,ofGetWidth() - 400, ofGetHeight() - 400);
    grid->set(r);
    generate(grid);
    
    iso = true;
    
//    grid->set(r);
    
}

void ofApp::generate(ofxPrecisionGrid * iter) {
//    grid->add(0);
//    grid->add(0);
//    grid->add(0);
    ofxPrecisionGrid * a = &grid->add(0);
//    grid->add(0);
    
    a->add(0);
    a->add(0);
    a->add(0);
    a->add(0);
    a->add(0);
    ofxPrecisionGrid * aa = &grid->add(1);
    aa->add(0);
    ofxPrecisionGrid * aaa = &aa->add(0);
    aaa->add(0);
    aaa->add(0);
    aa->add(0);
    grid->add(0);
//
//    a->add(1);
//    ofxPrecisionRow * b = &a->add(1);
//    a->add(1);
//
//    b->add(1);
//    ofxPrecisionRow * c = &b->add(1);
//    b->add(1);
    
    
//    if (count > depth) return;
//
////    if ((int)ofRandom(0, 3) <= 1) return;
//
//    count += 1;
//
//    for (int i = 0; i < ofRandom(1, maxIter); i++) {
//        int t = (int)ofRandom(0, 1.9);
////        t = 0;
//        ofxPrecisionRow * n = &iter->add( t );
//        generate(n);
//    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

void ofApp::draw(ofxPrecisionGrid * g, int x, int y) {
    
    ofFill();
    ofSetColor(grid->style.hoverColor);
    
    ofRectangle r(g->bounds);
    r.y = y;
    r.height = 10;
    ofDrawRectangle(r);
    for (auto & u : g->inner) draw(u, x, y + 20);
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(255);
    ofDrawBitmapString("maxIter " + ofToString(maxIter), 20, 20);
    ofDrawBitmapString("depth " + ofToString(depth), 20, 40);
    ofBackground(0);
    int w = ofGetWidth()/2;
    int h = ofGetHeight()/2;
    
    if (iso) {
        ofPushMatrix();
        ofTranslate(w, h);
        ofRotateX( ofMap( ofGetMouseY(), 0, ofGetHeight(), 100, -100) );
        ofRotateY( ofMap( ofGetMouseX(), 0, ofGetHeight(), -100, 100) );
        ofTranslate(-w, -h);
    }
    
    grid->draw(iso);
    int x = 0;
    int y = 20;
    draw(grid, x, y);
    
//    ofSetColor(255);
//    ofDrawRectangle(grid->get(sel)->bounds);
    
    if (iso) ofPopMatrix();
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
//    if (key == 'a') row->add(0);
//    if (key == 'z') row.setWidth(0, ofRandom(0, 1));
    
    if (key == OF_KEY_UP) sel.push_back(0);
    if (key == OF_KEY_DOWN && sel.size() > 0) sel.pop_back();
    if ( key == OF_KEY_LEFT ) sel.back() = sel.back() - 1;
    if ( key == OF_KEY_RIGHT ) sel.back() = sel.back() + 1;
    
    if (key == ' ') {
        iso = !iso;
    }
    if (key == 'z') {
        delete grid;
        count = 0;
        
        grid = new ofxPrecisionGrid(0, NULL);
        ofRectangle r(200,200,ofGetWidth() - 400, ofGetHeight() - 400);
        grid->set(r);
        generate(grid);
        
    }
    
    if (key == 'a' || key == 's') {
        int t;
        if (key == 'a') t = PRECISION_COL;
        if (key == 's') t = PRECISION_ROW;
        
        for (auto & g : grid->global) {
            for (auto & a : g->actions) {
                if (a.type == PRECISION_DR || a.type == PRECISION_DB) {
                    g->parent->add(t, g->getIndex() + 1);
                }
                if (a.type == PRECISION_DL || a.type == PRECISION_DT) {
                    g->parent->add(t, g->getIndex());
                }
                if (a.type == PRECISION_IN) {
                    g->add(t);
                }
            }
        }
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    grid->moved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    grid->dragged(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    grid->pressed(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    grid->released(x, y);

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
