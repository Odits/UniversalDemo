#include "TestLiber.h"
#include <iostream>
#include <string>
#include <cstring>

TestLiber::TestLiber()
{

}



int testFunc1(int a1, int a2)
{
    std::cout << "Enter " << __func__ << std::endl;

    std::cout << "a1=" << a1 << ", a2=" << a2 << std::endl;

    return a1 + a2;
}

int testFunc2(int a1, char* a2, int a3)
{
    std::cout << "Enter " << __func__ << std::endl;

    std::cout << "a1=" << a1 << ", a2=" << a2 << ", a3=" << a3 << std::endl;

    std::string str = std::to_string(a1) + "+" + std::to_string(a3);
    strncpy(a2, str.c_str(), str.length());
    return 0;
}

void testFunc3(int a1, char* a2)
{
    std::cout << "Enter " << __func__ << std::endl;

    std::cout << "a1=" << a1 << ", a2=" << a2 << std::endl;
}
