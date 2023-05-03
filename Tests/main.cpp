#include <string>

int main(int argc, char* argv[])
{    
    return std::stol(std::string(argv[1]));
}