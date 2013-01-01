#ifndef PARSING_TOYS_STRING_UTILS_H
#define PARSING_TOYS_STRING_UTILS_H

#include <vector>
#include <string>

std::string stringJoin(const std::vector<std::string>& strings, const std::string& separator);
bool operator<(const std::vector<std::string>& a, const std::vector<std::string>& b);
bool operator==(const std::vector<std::string>& a, const std::vector<std::string>& b);
std::string toSubscript(std::size_t number);
std::size_t utf8Length(const std::string& s);

#endif //PARSING_TOYS_STRING_UTILS_H
