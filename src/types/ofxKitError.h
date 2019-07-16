#pragma once

struct ofxKitError {
    string msg;
    string name;
    ofxKitError(string m, string n) {
        msg = m;
        name = n;
    }
};