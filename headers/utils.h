#ifndef CST_UTILS_H
#define CST_UTILS_H

#include <string>

#define QUIT(errno) exit(errno)

bool starts_with(const std::string& s1, const std::string& s2);

#endif