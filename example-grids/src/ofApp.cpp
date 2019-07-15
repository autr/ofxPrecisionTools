#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(120);
//    TIME_SAMPLE_SET_FRAMERATE(120.0f); //specify a target framerate
    ofEnableAlphaBlending();
    ofLog::setAutoSpace(true);
    
    count = 0;
    depth = 2;
    maxIter = 6;
    sel = {};
    
    cursor = new ofxPrecisionCursor();
    grid = new ofxPrecisionGrid();
    ofRectangle r(200,200,ofGetWidth() - 400, ofGetHeight() - 400);
    grid->set(r);
    undo = new ofxPrecisionUndo(grid);
    generate(grid);
    
    iso = false;
    
    bin = new ofxPrecisionGrid();
    bin->set(20,20,20,80);
    bin->add(0);
    bin->add(0);
    bin->add(0);
    bin->add(0);
    
    
    
    ui = new ofxPrecisionUi(grid, bin);
    
    ofBackground(0);
    
    scroll.outer.set( 200, 200, 400, 400);
    scroll.inner.set( 200, 200, 400, 1200);
    
}

void ofApp::generate(ofxPrecisionGrid * iter) {
    
    ofxPrecisionGrid * a = &grid->add(0);
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
    json = grid->json();
}

//--------------------------------------------------------------
void ofApp::update(){
//    TS_START("update");
//    undo->update();
    
//    TS_STOP("update");
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


    float x = grid->bounds.x;
    float y = grid->bounds.y;
    float width = grid->bounds.width;
    float height = grid->bounds.height;

////    ofNoFill();
////    ofSetLineWidth(1);
////
////    for (int i = 0; i <= ofGetWidth() / (col / cm) ; i ++) {
////
////        float ix = ( (col/cm) * i) + fmod(x,col);
////        ofPoint a( ix, 0 );
////        ofPoint b( ix, ofGetHeight() );
////
////        ofSetColor(255,255,255, 20);
////        if (i%2 == 0) ofSetColor(255,255,255, 40);
////        ofDrawLine(a, b);
////    }
////    for (int i = 0; i <= ofGetHeight() / (row / rm); i ++) {
////
////        float iy = ( (row/rm) * i) + fmod(y,row);
////        ofPoint a( 0, iy );
////        ofPoint b( ofGetWidth(), iy );
////
////        ofSetColor(255,255,255, 20);
////        if (i%4 == 0) ofSetColor(255,255,255, 40);
////        ofDrawLine(a, b);
////    }
//
////    ofFill();
////    ofSetColor(255,255,255, 150);
////    for (int xx = 0; xx <= ofGetWidth() / col; xx ++) {
////
////        for (int yy = 0; yy <= ofGetHeight() / row; yy ++) {
////
////            float ix = ( col * xx) + fmod(x,col);
////            float iy = ( row * yy) + fmod(y,row);
////            if (!grid->bounds.inside(ix,iy)) ofDrawCircle(ix, iy, 1);
////        }
////    }
////    ofFill();
////    ofSetColor(255,255,255, 150);
////    for (int xx = 0; xx <= ofGetWidth() / col; xx ++) {
////
////        for (int yy = 0; yy <= ofGetHeight() / col; yy ++) {
////
////            float ix = ( col * xx) + fmod(x,col);
////            float iy = ( col * yy) + fmod(y,col);
////            if (!grid->bounds.inside(ix,iy)) ofDrawCircle(ix, iy, 1);
////        }
////    }
//
    /*-- draw grids and ui --*/

    if (iso) {
        ofPushMatrix();
        ofTranslate(w, h);
        ofRotateX( ofMap( ofGetMouseY(), 0, ofGetHeight(), 100, -100) );
        ofRotateY( ofMap( ofGetMouseX(), 0, ofGetHeight(), -100, 100) );
        ofTranslate(-w, -h);
    }

    grid->draw(iso);
    ui->draw(iso);

    if (iso) ofPopMatrix();
    
    
    cursor->draw( ofGetMouseX(), ofGetMouseY() );
    
    
    string m = "CURRENTLY_NOWT";
    if ( ui->is(CURRENTLY_NOWT) ) m = "CURRENTLY_NOWT";
    if ( ui->is(CURRENTLY_SELECTING) ) m = "CURRENTLY_SELECTING";
    if ( ui->is(CURRENTLY_ADDING) ) m = "CURRENTLY_ADDING";
    if ( ui->is(CURRENTLY_RESIZING) ) m = "CURRENTLY_RESIZING";
    if ( ui->is(CURRENTLY_MOVING) ) m = "CURRENTLY_MOVING";
    if ( ui->is(CURRENTLY_HOVERING) ) m = "CURRENTLY_HOVERING";
    ofDrawBitmapStringHighlight( m, 20, 280);
    
    if (ui->current) {
    
        ofDrawBitmapStringHighlight( "Location:" , 20, 300 );
        ofDrawBitmapStringHighlight( ui->current->getLocationString() , 20, 320 );
        ofDrawBitmapStringHighlight( "Depth:" , 20, 340 );
        ofDrawBitmapStringHighlight( ofToString( ui->current->depth() ) , 20, 360 );
        ofDrawBitmapStringHighlight( "Global Index:" , 20, 380 );
        
        string gl = "";
        auto it = find(ui->unit->global.begin(), ui->unit->global.end(), ui->current);
        if (it != ui->unit->global.end()) {
            gl = "Not found";
        } else {
            gl = ofToString( std::distance(ui->unit->global.begin(), it) );
        }
        ofDrawBitmapStringHighlight( gl , 20, 400 );
        
        
    }
    
    scroll.update();
    
    ofNoFill();
    ofSetColor(255,0,0);
    ofPushMatrix();
//    ofTranslate(100,100);
    ofDrawRectangle( scroll.outer );
    ofDrawRectangle( scroll.inner );
    
    ofFill();
//    ofDrawRectangle( scroll.getScrollBar() );
    
    ofPopMatrix();
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == OF_KEY_UP) ui->up();
    if (key == OF_KEY_DOWN) ui->down();
    if ( key == OF_KEY_LEFT ) ui->prev();
    if ( key == OF_KEY_RIGHT ) ui->next();
    
    if (key == ' ') {
//        ofLog() << grid->json();
        
        iso = !iso;
    }
    if (key == 'z') {
        
        grid->load(json);
        
    }
    if (key == 'r') cursor->random();
    
    if (key == 'x') {
        undo->clear();
    }
    if (key == 'a') {
        ofJson j = grid->json();
        grid->load(j);
    }
    if (key == 'f' ) {
        if (ui->current) ui->current->fixed = !ui->current->fixed;
    }
    
    ui->keypress(key);
    undo->keypress(key);
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    ui->keyrelease(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
//    TS_START("moved");
    ui->moved(x, y);
//    TS_STOP("moved");
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    scroll.dragged(x, y);
//    TS_START("dragged");
    ui->dragged(x, y);
//    TS_STOP("dragged");
}

void ofApp::mouseScrolled( ofMouseEventArgs& e) {
    scroll.scrolled( e );
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    scroll.pressed(x, y);
    ui->pressed(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    scroll.released(x, y);
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
