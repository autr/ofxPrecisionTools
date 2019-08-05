#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(30);
    ofBackground(0, 0, 0, 0);
    ofLog::setAutoSpace(true);
    ofEnableAlphaBlending();
    ofSetLoggerChannel(ofxKitLogger::getLogger());
    
    
    ofxKitCursor::set("pointingHand");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    win.draw();
    
    
    if (ofRandom(0, 100) < 10) ofLog() << ofRandom(0,999);
    
    ofxKitLogger::getLogger()->draw( 40, 60, ofGetWidth()/2, 400 );
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == OF_KEY_RIGHT) {
        win.s.osStyle += 1;
        if (win.s.osStyle > 3) win.s.osStyle = 1;
    }
    if (key == OF_KEY_LEFT) {
        win.s.osStyle -= 1;
        if (win.s.osStyle < 1) win.s.osStyle = 3;
    }
    input += key;
    if (key == OF_KEY_RETURN) {
        ofLog() << input;
        input = "";
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    win.moved( x, y );
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    win.dragged( x, y, button );
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    win.pressed( x, y, button );
//    ofSetWindowPosition(0, 0);
//    ofSetWindowShape( ofGetScreenWidth(), ofGetScreenHeight());
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

    win.released( x, y, button );
    
    
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
