//
// Created by 10036 on 2023/8/10.
//

#ifndef DYNAMIC_LIB_H
#define DYNAMIC_LIB_H

#include <iostream>
#include <string>
#include <sstream>

#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#define HMODULE void*
#endif

class DynamicLib
{
private:
	HMODULE handle;
	std::stringstream errorMsg;
public:
	explicit DynamicLib(const std::string &libPath)
	{
#ifdef WIN32
		handle = LoadLibrary(libPath.c_str());
#else
		handle = dlopen(libPath.c_str(), RTLD_LAZY);
#endif
		if (!handle)
		{
			errorMsg.str("");
#ifdef WIN32
			errorMsg << "loading library Error: " << GetLastError();
#else
			errorMsg << "loading library Error: " << dlerror();
#endif
			std::cerr << errorMsg.str() << std::endl;
		}
	}

	~DynamicLib()
	{
		if (handle)
#ifdef WIN32
			FreeLibrary(handle);
#else
		dlclose(handle);
#endif
	}

	template<typename T>
	T loadFunc(const char *funcName)
	{
		if (!handle)
		{
			errorMsg.str("");
			errorMsg << "library no loaded.";
			std::cerr << errorMsg.str() << std::endl;
			return nullptr;
		}
#ifdef WIN32
		T func = (T) GetProcAddress(handle, funcName);
#else
		T func = (T) dlsym(handle, funcName);
#endif
		if (!func)
		{
			errorMsg.str("");
#ifdef WIN32
			errorMsg << "Error loading function: " << GetLastError();
#else
			errorMsg << "Error loading function: " << dlerror();
#endif
			std::cerr << errorMsg.str() << std::endl;
			return nullptr;
		}
		return func;
	}

	std::string getErrorMsg()
	{
		return errorMsg.str();
	}
};

#endif	// DYNAMIC_LIB_H
