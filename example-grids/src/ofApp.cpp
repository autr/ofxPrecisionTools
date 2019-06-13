#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(120);
    
    ofLog::setAutoSpace(true);
    
    count = 0;
    depth = 2;
    maxIter = 6;
    sel = {};
    
    grid = new ofxPrecisionRow((int)ofRandom(0, 1.9), NULL);
    ofRectangle r(200,200,ofGetWidth() - 400, ofGetHeight() - 400);
    grid->set(r);
    generate(grid);
    
    iso = true;
    
//    grid->set(r);
    
}

void ofApp::generate(ofxPrecisionRow * iter) {
    
    ofxPrecisionRow * a = &grid->add(0);
//    grid->add(0);
    
    a->add(0);
    a->add(0);
    a->add(0);
    a->add(0);
    a->add(0);
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
    
    grid->draw(0, iso);
    
    ofSetColor(255);
    ofDrawRectangle(grid->get(sel)->bounds);
    
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
    
    if (key == 'q') maxIter -= 1;
    if (key == 'w') maxIter += 1;
    if (key == 'a') depth -= 1;
    if (key == 's') depth += 1;
    
    if (key == ' ') {
        iso = !iso;
    }
    if (key == 'z') {
        delete grid;
        count = 0;
        
        grid = new ofxPrecisionRow((int)ofRandom(0, 1.9), NULL);
        ofRectangle r(200,200,ofGetWidth() - 400, ofGetHeight() - 400);
        grid->set(r);
        generate(grid);
        
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
