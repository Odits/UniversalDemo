#ifndef FUNCPTR_H
#define FUNCPTR_H

/*
F = function name

v = void

i = int
pi = int*

c = char
pc = char*
str = const char*

d = double
pd = double*

*/

typedef int (*i_F_i_i)(int, int);

typedef int (*i_F_i_pc_i)(int, char*, int);

typedef void (*v_F_i_pc)(int, char*);

typedef void (*v_F_i_str)(int, const char*);

typedef int (*i_F_pc_pc_pc_i_i)(char*, char*, char*, int, int);


#define BUF_SIZE 1024

static int i_F_i_i_(void *func, int a1, int a2)
{
    return i_F_i_i(func)(a1, a2);
}

static int i_F_i_pc_i_(void *func, int a1, char *a2, int a3)
{
    return i_F_i_pc_i(func)(a1, a2, a3);
}

static void v_F_i_pc_(void *func, int a1, char *a2)
{
	return v_F_i_pc(func)(a1, a2);
}

static void v_F_i_str_(void *func, int a1, const char *a2)
{
	return v_F_i_str(func)(a1, a2);
}

static int i_F_pc_pc_pc_i_i_(void *func, char *a1, char *a2, char *a3, int a4, int a5)
{
	return i_F_pc_pc_pc_i_i(func)(a1, a2, a3, a4, a5);
}

#include <iostream>
#include <string>
#include <QString>
#include <QStringList>


static QStringList callFunc(const QString &type, void* X, const QStringList &args = {})
{
    QStringList msgList;
    if (type == "i_F_i_i")
    {
        int retCode = i_F_i_i_(X, args[0].toInt(), args[1].toInt());
        msgList.push_back(QString::number(retCode));
    }
    else if (type == "i_F_i_pc_i")
    {
        char buf[BUF_SIZE]{};
        strcpy(buf, args[1].toStdString().c_str());
        int retCode = i_F_i_pc_i_(X, args[0].toInt(), buf, args[2].toInt());
        msgList.push_back(QString::number(retCode));
        msgList.push_back(buf);
    }
    else if (type == "v_F_i_pc")
    {
        char buf[BUF_SIZE]{};
        strcpy(buf, args[1].toStdString().c_str());
        v_F_i_pc_(X, args[0].toInt(), buf);
        msgList.push_back(buf);
    }
    else if (type == "v_F_i_str")
    {
        v_F_i_str_(X, args[0].toInt(), args[1].toStdString().c_str());
    }
	else if (type == "i_F_pc_pc_pc_i_i")
	{
		char a1[BUF_SIZE]{}, a2[BUF_SIZE]{}, a3[BUF_SIZE]{};
		strcpy(a1, args[0].toStdString().c_str());
		strcpy(a2, args[1].toStdString().c_str());
		strcpy(a3, args[3].toStdString().c_str());

		int retCode = i_F_pc_pc_pc_i_i_(X, a1, a2, a3, args[3].toInt(), args[4].toInt());

		msgList.push_back(QString::number(retCode));
		msgList.push_back(a1);
		msgList.push_back(a2);
		msgList.push_back(a3);
	}
    else
    {
        // 处理未知类型或错误情况
		std::cerr << "error type: " << type.toStdString() << std::endl;
        throw std::runtime_error("Unsupported type");
    }
    return msgList;
}

template <typename T>
T trans(const std::string &type, void* X) {
    if (type == "int")
    {
        return *static_cast<int*>(X);
    }
    else if (type == "double")
    {
        return *static_cast<double*>(X);
    }
    else if (type == "float")
    {
        return *static_cast<float*>(X);
    }
    else if (type == "char")
    {
        return *static_cast<char*>(X);
    }
    else if (type == "string")
    {
        return *static_cast<std::string*>(X);
    }
    else
    {
        // 处理未知类型或错误情况
        throw std::runtime_error("Unsupported type");
    }
}




#endif // FUNCPTR_H
