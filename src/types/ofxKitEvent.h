#pragma once

struct ofxKitEvent {
    ofxKitGrid * u;
    string type;
    ofxKitEvent( string t, ofxKitGrid * uu ) {
        type = t;
        u = uu;
    }
};
