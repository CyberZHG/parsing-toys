#ifndef PARSING_TOYS_STRING_UTILS_H
#define PARSING_TOYS_STRING_UTILS_H

#include <vector>
#include <string>

std::vector<std::string> segmentGraphemes(const std::string& s);
std::vector<std::string> stringSplit(const std::string& str, char delimiter, bool removeEmpty = false);
std::string stringJoin(const std::vector<std::string>& strings, const std::string& separator);
std::string stringReplace(const std::string& str, char from, const std::string& to);
bool operator<(const std::vector<std::string>& a, const std::vector<std::string>& b);
bool operator==(const std::vector<std::string>& a, const std::vector<std::string>& b);
std::string toSubscript(std::size_t number);
std::size_t utf8Length(const std::string& s);
std::string utf8CharAt(const std::string& s, std::size_t index);
std::string utf8Substring(const std::string& s, std::size_t start, std::size_t length);

#endif //PARSING_TOYS_STRING_UTILS_H
