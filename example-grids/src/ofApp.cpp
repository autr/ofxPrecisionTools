#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(120);
    TIME_SAMPLE_SET_FRAMERATE(120.0f); //specify a target framerate
    ofEnableAlphaBlending();
    ofLog::setAutoSpace(true);
    
    count = 0;
    depth = 2;
    maxIter = 6;
    sel = {};
    
    grid = new ofxPrecisionGrid();
    ofRectangle r(200,200,ofGetWidth() - 400, ofGetHeight() - 400);
    grid->set(r);
    generate(grid);
    
    iso = false;
    
    ui = new ofxPrecisionUi(grid);
    
    
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
    ofxPrecisionGrid * aaaa = &aaa->add(1);
    aaaa->add(0);
    aaaa->add(0);
    aaaa->add(0);
    
    
    
    ofxPrecisionGrid * b = &aa->add(0);
    b->add(0);
    ofxPrecisionGrid * bb = &b->add(1);
    bb->add(0);
    bb->add(0);
    bb->add(0);
    b->add(0);
    
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
    
    json = grid->json();
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
    
    bool isPortrait = grid->bounds.width < grid->bounds.height;
    
    float col = grid->bounds.width;
    float row = grid->bounds.height;
    
    if (isPortrait) {
        col /= 12;
        row /= 18;
    } else {
        col /= 18;
        row /= 12;
    }
    
    float min = 8;
    float cm = 1;//(int) ( (int)col / min );
    float rm = 1;//(int) ( (int)row / min );
    
    if (cm < 1) cm = 0;
    if (rm < 1) rm = 0;
    
//    ofLog() << cm << rm;
    
    
    float x = grid->bounds.x;
    float y = grid->bounds.y;
    float width = grid->bounds.width;
    float height = grid->bounds.height;
    
    ofNoFill();
    ofSetLineWidth(1);
    
    for (int i = 0; i <= ofGetWidth() / (col / cm) ; i ++) {

        float ix = ( (col/cm) * i) + fmod(x,col);
        ofPoint a( ix, 0 );
        ofPoint b( ix, ofGetHeight() );
        
        ofSetColor(255,255,255, 20);
        if (i%2 == 0) ofSetColor(255,255,255, 40);
        ofDrawLine(a, b);
    }
    for (int i = 0; i <= ofGetHeight() / (row / rm); i ++) {

        float iy = ( (row/rm) * i) + fmod(y,row);
        ofPoint a( 0, iy );
        ofPoint b( ofGetWidth(), iy );
        
        ofSetColor(255,255,255, 20);
        if (i%4 == 0) ofSetColor(255,255,255, 40);
        ofDrawLine(a, b);
    }
    
//    ofFill();
//    ofSetColor(255,255,255, 150);
//    for (int xx = 0; xx <= ofGetWidth() / col; xx ++) {
//
//        for (int yy = 0; yy <= ofGetHeight() / row; yy ++) {
//
//            float ix = ( col * xx) + fmod(x,col);
//            float iy = ( row * yy) + fmod(y,row);
//            if (!grid->bounds.inside(ix,iy)) ofDrawCircle(ix, iy, 1);
//        }
//    }
//    ofFill();
//    ofSetColor(255,255,255, 150);
//    for (int xx = 0; xx <= ofGetWidth() / col; xx ++) {
//
//        for (int yy = 0; yy <= ofGetHeight() / col; yy ++) {
//
//            float ix = ( col * xx) + fmod(x,col);
//            float iy = ( col * yy) + fmod(y,col);
//            if (!grid->bounds.inside(ix,iy)) ofDrawCircle(ix, iy, 1);
//        }
//    }
    
    /*-- draw grids and ui --*/
    
    if (iso) {
        ofPushMatrix();
        ofTranslate(w, h);
        ofRotateX( ofMap( ofGetMouseY(), 0, ofGetHeight(), 100, -100) );
        ofRotateY( ofMap( ofGetMouseX(), 0, ofGetHeight(), -100, 100) );
        ofTranslate(-w, -h);
    }
    
    grid->draw(iso);
    ui->draw();
    
    if (iso) ofPopMatrix();
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == OF_KEY_UP) ui->up();
    if (key == OF_KEY_DOWN) ui->down();
    if ( key == OF_KEY_LEFT ) ui->prev();
    if ( key == OF_KEY_RIGHT ) ui->next();
    
    if (key == ' ') {
//        ofLog() << grid->json();
        
//        iso = !iso;
    }
    if (key == 'z') {
        
//        ofJson j = grid->json();
        delete ui;
//        delete grid;
        
        grid = new ofxPrecisionGrid();
        grid->load(json);
        ui = new ofxPrecisionUi(grid);
        
    }
    
    if (key == 'x') {
        grid->amend();
    }
    
    if (key == 'a' || key == 's') {
        int t;
        if (key == 'a') t = PRECISION_COL;
        if (key == 's') t = PRECISION_ROW;
        
//        for (auto & g : grid->global) {
//            for (auto & a : g->actions) {
//                if (a.type == PRECISION_DR || a.type == PRECISION_DB) {
//                    g->parent->add(t, g->getIndex() + 1);
//                }
//                if (a.type == PRECISION_DL || a.type == PRECISION_DT) {
//                    g->parent->add(t, g->getIndex());
//                }
//                if (a.type == PRECISION_IN) {
//                    g->add(t);
//                }
//            }
//        }
    }
    
    ui->keypress(key);
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    ui->keyrelease(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    TS_START("moved");
    ui->moved(x, y);
    TS_STOP("moved");
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    TS_START("dragged");
    ui->dragged(x, y);
    TS_STOP("dragged");
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    ui->pressed(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    ui->released(x, y);

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
