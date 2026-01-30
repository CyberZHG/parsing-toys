#include "string_utils.h"
#include <grapheme_break.h>
#include <algorithm>

using namespace std;

vector<string> segmentGraphemes(const string& s) {
    return grapheme_break::segmentGraphemeClusters(s);
}

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
    return grapheme_break::segmentGraphemeClusters(s).size();
}

string utf8CharAt(const string& s, const size_t index) {
    const auto graphemes = grapheme_break::segmentGraphemeClusters(s);
    if (index < graphemes.size()) {
        return graphemes[index];
    }
    return "";
}

string utf8Substring(const string& s, const size_t start, const size_t length) {
    const auto graphemes = grapheme_break::segmentGraphemeClusters(s);
    string result;
    for (size_t i = start; i < min(start + length, graphemes.size()); ++i) {
        result += graphemes[i];
    }
    return result;
}
