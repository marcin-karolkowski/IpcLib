#include "String.hpp"

namespace Utils::String
{

    std::vector<std::string> StringSplit(const std::string& str, const std::string& delimiter)
    {
        std::vector<std::string> ret;
        auto strCopy = str;

        while(true)
        {
            auto delimiterPos = strCopy.find(delimiter);
            if(delimiterPos==std::string::npos)
            {
                ret.emplace_back(strCopy);
                break;
            }
            ret.emplace_back(strCopy.substr(0, delimiterPos));
            strCopy.erase(strCopy.begin(), strCopy.begin()+delimiterPos + delimiter.size());
        }
        return ret;
    }
    std::string StringJoin(const std::vector<std::string>& vec, const std::string& join)
    {
        std::string ret;
        auto vecSize = vec.size();
        for(uint32_t i=0; i<vecSize; i++)
        {
            ret+=vec[i];
            if(i<vecSize-1)
                ret+=join;
        }
        return ret;
    }
}