#include "string_utils.h"
#include <algorithm>

using namespace std;

vector<string> stringSplit(const string& str, const char delimiter) {
    vector<string> splits;
    size_t last = 0, pos = 0;
    while ((pos = str.find(delimiter, last)) != string::npos) {
        splits.push_back(str.substr(last, pos - last));
        last = pos + 1;
    }
    splits.push_back(str.substr(last));
    return splits;
}

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

string stringReplace(const string& str, const char from, const string& to) {
    string result;
    for (const auto& ch : str) {
        if (ch == from) {
            result += to;
        } else {
            result += ch;
        }
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

string toSubscript(size_t number) {
    static const vector<string> SUBSCRIPTS = {"₀", "₁", "₂", "₃", "₄", "₅", "₆", "₇", "₈", "₉"};
    if (number == 0) {
        return SUBSCRIPTS[0];
    }
    string result;
    while (number > 0) {
        const auto& subscript = SUBSCRIPTS[number % 10];
        result.insert(result.begin(), subscript.begin(), subscript.end());
        number /= 10;
    }
    return result;
}

size_t utf8Length(const string& s) {
    size_t length = 0;
    for (const auto c : s) {
        if ((c & 0b11000000) != 0b10000000) {
            ++length;
        }
    }
    return length;
}
