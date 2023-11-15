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

#define printHEX(buf, len)                    \
    printf("%d: %s(HEX) = {", __LINE__, #buf); \
    for (int i = 0; i < len; i++)             \
        printf("%02hhX ", buf[i]);            \
    printf("\b}\n");

#include <sstream>
#include <iomanip>

std::string hex2String(const byte *hex, size_t len)
{
	std::ostringstream oss;
	oss << std::hex << std::uppercase << std::setfill('0');
	for (int i = 0; i < len; i++)
		oss << std::setw(2) << static_cast<unsigned int>(hex[i]);
	return oss.str();
}

int fileTest(int a1, const byte *a2, int a3, int a4, const char *a5)
{
	std::cout << "Enter " << __func__ << "\n";
	std::cout << "a1=" << a1 << ", a3=" << a3 << ", a4=" << a4 << ", a5=" << a5 << std::endl;

	printHEX(a2, a3);
	auto str = hex2String(a2, a3);


	size_t fileSize = a4;
	const int BLOCK_SIZE = 1024;
	size_t nBlockCount = fileSize / BLOCK_SIZE;
	if (fileSize % BLOCK_SIZE)
		++nBlockCount;
	int iCurRestPacket = 0;

	for (int i = 0; i < nBlockCount; ++i)
	{
		iCurRestPacket = nBlockCount - i;
		if (i == 0 && nBlockCount != 1)    // 第一个包
			iCurRestPacket = 0;
		else if (i == nBlockCount - 1)    // 最后一个包
			iCurRestPacket = 0x01;

		byte buf[BLOCK_SIZE]{};
		size_t current_size{fileSize - i * BLOCK_SIZE};

		size_t buf_size = current_size > BLOCK_SIZE ? BLOCK_SIZE : current_size;
//		memcpy(buf, data + i * BLOCK_SIZE, buf_size);
		std::cout << "i=" << i << ", buf_size=" << buf_size << std::endl;
	}


	return a1 + a4;
}













