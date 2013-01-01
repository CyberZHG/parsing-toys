#include "string_utils.h"
#include <algorithm>

string stringJoin(const vector<string>& strings, const string& separator) {
    string result;
    for (size_t i = 0; i < strings.size(); i++) {
        if (i > 0) {
            result += separator;
        }
        result += strings[i];
    }
    return result;
}

bool operator<(const vector<string>& a, const vector<string>& b) {
    for (size_t i = 0; i < min(a.size(), b.size()); i++) {
        if (a[i] < b[i]) {
            return true;
        }
        if (a[i] > b[i]) {
            return false;
        }
    }
    return a.size() < b.size();
}

bool operator==(const vector<string>& a, const vector<string>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}
