#ifndef PARSINGTOYS_STRING_UTILS_H
#define PARSINGTOYS_STRING_UTILS_H

#include <vector>
#include <string>
using namespace std;

string stringJoin(const vector<string>& strings, const string& separator);
bool operator<(const vector<string>& a, const vector<string>& b);
bool operator==(const vector<string>& a, const vector<string>& b);
string toSubscript(size_t number);

#endif //PARSINGTOYS_STRING_UTILS_H
