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


#define BUF_SIZE 1024

int _i_F_i_i(void *func, int a1, int a2)
{
    i_F_i_i Y = (i_F_i_i)func;
    return Y(a1, a2);
}

int _i_F_i_pc_i(void *func, int a1, char *a2, int a3)
{
    i_F_i_pc_i Y = (i_F_i_pc_i)func;
    return Y(a1, a2, a3);
}

void _v_F_i_pc(void *func, int a1, char *a2)
{
    v_F_i_pc Y = (v_F_i_pc)func;
    Y(a1, a2);
}

void _v_F_i_str(void *func, int a1, const char *a2)
{
    v_F_i_str Y = (v_F_i_str)func;
    Y(a1, a2);
}


#include <iostream>
#include <string>
#include <QString>
#include <QStringList>


QStringList callFunc(const QString &type, void* X, const QStringList &args = {})
{
    QStringList msgList;
    if (type == "i_F_i_i")
    {
        int retCode = _i_F_i_i(X, args[0].toInt(), args[1].toInt());
        msgList.push_back(QString::number(retCode));
    }
    else if (type == "i_F_i_pc_i")
    {
        char buf[BUF_SIZE]{};
        strcpy(buf, args[1].toStdString().c_str());
        int retCode = _i_F_i_pc_i(X, args[0].toInt(), buf, args[2].toInt());
        msgList.push_back(QString::number(retCode));
        msgList.push_back(buf);
    }
    else if (type == "v_F_i_pc")
    {
        char buf[BUF_SIZE]{};
        strcpy(buf, args[1].toStdString().c_str());
        _v_F_i_pc(X, args[0].toInt(), buf);
        msgList.push_back(buf);
    }
    else if (type == "v_F_i_str")
    {
        _v_F_i_str(X, args[0].toInt(), args[1].toStdString().c_str());
    }
    else
    {
        // 处理未知类型或错误情况
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
