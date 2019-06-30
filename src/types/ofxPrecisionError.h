#pragma once

struct ofxPrecisionError {
    string msg;
    string name;
    ofxPrecisionError(string m, string n) {
        msg = m;
        name = n;
    }
};