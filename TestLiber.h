#ifndef TESTLIBER_H
#define TESTLIBER_H


class TestLiber
{
public:
    TestLiber();
};

extern "C"
{

int testFunc1(int a1, int a2);

int testFunc2(int a1, char* a2, int a3);

void testFunc3(int a1, char* a2);

}

#endif // TESTLIBER_H
