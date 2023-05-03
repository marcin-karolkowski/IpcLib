#include "Utils.hpp"

namespace Utils::Utils
{
    std::string ReadTextFile(const std::string& path)
    {
        std::string ret;
        try
        {
            std::ifstream f(path, std::ios_base::in | std::ios_base::binary);   
            std::vector<char> data;
            std::vector<char> block(1024);
            while(f.is_open() && !f.eof())
            {
                f.read(block.data(), block.size());
                auto br = f.gcount();
                data.insert(data.end(), block.begin(), block.begin()+br);
            }

            f.close();
            ret = std::string(data.data(), data.size());            
        }
        catch(std::exception& e)
        {
            std::cout<<e.what()<<std::endl;
        }
        return ret;
    }

    std::vector<std::string> ReadTextFileLines(const std::string& path)
    {
        auto content = ReadTextFile(path);
        auto ret = String::StringSplit(content, "\n");
        return ret;        
    }

    void WriteTextFile(const std::string& path, const std::string& content)
    {
        try
        {
            std::ofstream f(path);
            f.write(content.data(), content.size());
            f.close();
        }
        catch(std::exception& e)
        {

        }
    }
    
    void WriteTextFileLines(const std::string& path, const std::vector<std::string>& lines)
    {
        auto content = String::StringJoin(lines, "\n");
        WriteTextFile(path, content);
    }
}