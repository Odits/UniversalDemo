#ifndef TESTLIBRARY_H
#define TESTLIBRARY_H

using byte = unsigned char;

extern "C"
{

int testFunc1(int a1, int a2);

int testFunc2(int a1, char* a2, int a3);

void testFunc3(int a1, char* a2);

int jsonTest(char *args, char *resmsg, char *errmsg, int readTimeout, int writeTimeout);

void gbkTest(int a1, const char *gbk);

int fileTest(int a1, const byte *a2, int a3, int a4, const char *a5);
}

#endif // TESTLIBRARY_H
