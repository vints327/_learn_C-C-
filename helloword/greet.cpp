// greet.cpp
#include <iostream>
#include <string>
#include "greet.h"

void greet(const std::string &name)
{
    std::cout << "Hello, " << name << "!" << std::endl;
}