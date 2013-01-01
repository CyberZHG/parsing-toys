#include "string_utils.h"

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
