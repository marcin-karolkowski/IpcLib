#include <iostream>
#include <string>
#include <unistd.h>
#include <filesystem>
#include "Utils/String.hpp"
#include "Utils/Utils.hpp"

enum class Operation
{
    Unk,
    Sum,
    Diff,
    Div,
    Mult
};

int handle_sum(const std::vector<std::string>& numbers)
{
    double result = 0;
    auto numbersSize = numbers.size();
    for(int i=0; i<numbersSize; i++)
    {
        if(numbers[i].empty())
            continue;

        
        auto v = std::stold(numbers[i]);
        if(i==0)
            result = v;
        else
            result += v;
    }
    std::cout<<result<<std::endl;
    return 0;
}

int handle_diff(const std::vector<std::string>& numbers)
{
    double result = 0;
    auto numbersSize = numbers.size();
    for(int i=0; i<numbersSize; i++)
    {
        if(numbers[i].empty())
            continue;

        
        auto v = std::stold(numbers[i]);
        if(i==0)
            result = v;
        else
            result -= v;
    }
    std::cout<<result<<std::endl;
    return 0;
}

int handle_div(const std::vector<std::string>& numbers)
{
    double result = 0;
    auto numbersSize = numbers.size();
    for(int i=0; i<numbersSize; i++)
    {
        if(numbers[i].empty())
            continue;

        auto v = std::stold(numbers[i]);
        if(i==0)
            result = v;
        else
            result /= v;
    }
    std::cout<<result<<std::endl;
    return 0;
}

int handle_mult(const std::vector<std::string>& numbers)
{
    double result = 0;
    auto numbersSize = numbers.size();
    for(int i=0; i<numbersSize; i++)
    {
        if(numbers[i].empty())
            continue;

        
        auto v = std::stold(numbers[i]);
        if(i==0)
            result = v;
        else
            result *= v;
    }
    std::cout<<result<<std::endl;
    return 0;
}

int main(int argc, char* argv[])
{
    int stdinFd = fileno(stdin);

    std::string input;

    if(isatty(stdinFd))
    {
        std::cerr<<"Supports only redirected stdin stream"<<std::endl;
        return EXIT_FAILURE;
    }

    Operation op;
    for(int i=0; i<argc; i++)
    {
        std::string argStr = std::string(argv[i]);
        if(argStr=="+")
            op = Operation::Sum;
        else if(argStr == "-")
            op = Operation::Diff;
        else if(argStr == "/")
            op = Operation::Div;
        else if(argStr == "*")
            op = Operation::Sum;
        else
            op = Operation::Unk;
    }

    if(op == Operation::Unk)
    {
        std::cerr<<"Unknown operation, implemented are: {+,-,/,*}"<<std::endl;
        return EXIT_FAILURE;
    }
    
    while(!std::cin.eof())
    {
        char in[1024];
        std::cin.read(in, sizeof(in));
        auto dr = std::cin.gcount();
        input += std::string(in, dr);
    }

    auto lines= Utils::String::StringSplit(input, "\n");

    switch(op)
    {
        case Operation::Sum:
            return handle_sum(lines);
        case Operation::Diff:
            return handle_diff(lines);
        case Operation::Div:
            return handle_div(lines);
        case Operation::Mult:
            return handle_mult(lines);
    }
    return 0;
}