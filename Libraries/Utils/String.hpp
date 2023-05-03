#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <string>
#include <vector>


namespace Utils::String
{
    std::vector<std::string> StringSplit(const std::string& str, const std::string& delimiter);
    
    std::string StringJoin(const std::vector<std::string>& vec, const std::string& join);
}

#endif