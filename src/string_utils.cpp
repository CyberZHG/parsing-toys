#include "string_utils.h"
#include <algorithm>

using namespace std;

vector<string> stringSplit(const string& str, const char delimiter, const bool removeEmpty) {
    vector<string> splits;
    size_t last = 0, pos = 0;
    while ((pos = str.find(delimiter, last)) != string::npos) {
        if (const auto sub = str.substr(last, pos - last); !sub.empty() || !removeEmpty) {
            splits.emplace_back(sub);
        }
        last = pos + 1;
    }
    if (const auto sub = str.substr(last); !sub.empty() || !removeEmpty) {
        splits.emplace_back(sub);
    }
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

string utf8CharAt(const string& s, const size_t index) {
    size_t charIndex = 0;
    size_t byteIndex = 0;
    while (byteIndex < s.size()) {
        if (charIndex == index) {
            size_t charLen = 1;
            if (const unsigned char c = s[byteIndex]; (c & 0b11110000) == 0b11110000) {
                charLen = 4;
            } else if ((c & 0b11100000) == 0b11100000) {
                charLen = 3;
            } else if ((c & 0b11000000) == 0b11000000) {
                charLen = 2;
            }
            return s.substr(byteIndex, charLen);
        }
        if (const unsigned char c = s[byteIndex]; (c & 0b11110000) == 0b11110000) {
            byteIndex += 4;
        } else if ((c & 0b11100000) == 0b11100000) {
            byteIndex += 3;
        } else if ((c & 0b11000000) == 0b11000000) {
            byteIndex += 2;
        } else {
            byteIndex++;
        }
        charIndex++;
    }
    return "";
}

string utf8Substring(const string& s, const size_t start, const size_t length) {
    size_t charIndex = 0;
    size_t byteIndex = 0;
    size_t startByte = 0;
    bool foundStart = false;

    while (byteIndex < s.size()) {
        if (charIndex == start) {
            startByte = byteIndex;
            foundStart = true;
        }
        if (foundStart && charIndex == start + length) {
            return s.substr(startByte, byteIndex - startByte);
        }
        if (const unsigned char c = s[byteIndex]; (c & 0b11110000) == 0b11110000) {
            byteIndex += 4;
        } else if ((c & 0b11100000) == 0b11100000) {
            byteIndex += 3;
        } else if ((c & 0b11000000) == 0b11000000) {
            byteIndex += 2;
        } else {
            byteIndex++;
        }
        charIndex++;
    }

    if (foundStart) {
        return s.substr(startByte);
    }
    return "";
}
