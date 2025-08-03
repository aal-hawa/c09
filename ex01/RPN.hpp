#ifndef RPN_HPP
#define RPN_HPP

#include <stack>
#include <iostream>
#include <sstream>
#include <cstdlib>

class RPN
{
    public:
        static int evaluate(const std::string& expression);

};

#endif