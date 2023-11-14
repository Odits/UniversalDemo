#include "TestLibrary.h"
#include <iostream>
#include <string>
#include <cstring>


int testFunc1(int a1, int a2)
{
	std::cout << "Enter " << __func__ << std::endl;

	std::cout << "a1=" << a1 << ", a2=" << a2 << std::endl;

	return a1 + a2;
}

int testFunc2(int a1, char *a2, int a3)
{
	std::cout << "Enter " << __func__ << std::endl;

	std::cout << "a1=" << a1 << ", a2=" << a2 << ", a3=" << a3 << std::endl;

	std::string str = std::to_string(a1) + "+" + std::to_string(a3);
	strncpy(a2, str.c_str(), str.length());
	return 0;
}

void testFunc3(int a1, char *a2)
{
	std::cout << "Enter " << __func__ << std::endl;

	std::cout << "a1=" << a1 << ", a2=" << a2 << std::endl;
}


int jsonTest(char *args, char *resmsg, char *errmsg, int readTimeout, int writeTimeout)
{
	std::cout << "Enter " << __func__ << std::endl;

	std::cout << "args=" << args << std::endl;
	std::cout << "readTimeout=" << readTimeout << ", writeTimeout=" << writeTimeout << std::endl;


	strcpy(resmsg, "success");
	strcpy(errmsg, "no error");

	return readTimeout + writeTimeout;
}


// v_F_i_str_
void gbkTest(int a1, const char *gbk)
{
	std::cout << "Enter " << __func__ << std::endl;

	std::cout << "a1=" << a1 << ", gbk=" << gbk << std::endl;
	std::cout << "const gbk=" << "\xD6\xD0\xCE\xC4gbk\xB2\xE2\xCA\xD4" << std::endl;

	auto fp = fopen("./gbkTest.txt", "a+");
	fwrite(gbk, 1, strlen(gbk), fp);
	fclose(fp);
}
















