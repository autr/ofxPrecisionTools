#pragma once

struct ofxPrecisionEvent {
    ofxPrecisionGrid * u;
    string type;
    ofxPrecisionEvent( string t, ofxPrecisionGrid * uu ) {
        type = t;
        u = uu;
    }
};
