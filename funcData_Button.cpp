//
// Created by 10036 on 2023/10/17.
//
#include "funcData_Button.h"

/*
F = function name

v = void

i = int
pi = int*

c = char
uc = unsigned char = byte
pc = char*
puc = unsigned char* = byte*
str = const char*

d = double
pd = double*

*/



static QString printJson(const QJsonObject &object)
{
	QJsonDocument document(object);
	return {document.toJson(QJsonDocument::Indented)};
}

static QString printJson(const QJsonValueRef &ref)
{
	QJsonDocument document(ref.toObject());
	return {document.toJson(QJsonDocument::Indented)};
}

static QString JsonToStr(const QVariantMap &map)
{
	QJsonObject object = QJsonObject::fromVariantMap(map);
	QJsonDocument document(object);
	return {document.toJson(QJsonDocument::Compact)};
}

static QVariantMap StrToJson(const QString &str)
{
	QJsonDocument document = QJsonDocument::fromJson(str.toUtf8());
	if (document.isNull() || !document.isObject())
	{
		return {};
	}
	return document.object().toVariantMap();
}

static QString JsonArrayToStr(const QVariantList &list)
{
	QJsonArray array = QJsonArray::fromVariantList(list);
	QJsonDocument document(array);
	return {document.toJson(QJsonDocument::Compact)};
}

static QVariantList StrToJsonArray(const QString &str)
{
	QJsonDocument document = QJsonDocument::fromJson(str.toUtf8());
	if (document.isNull() || !document.isArray())
	{
		return {};
	}
	return document.array().toVariantList();
}


#define BUF_SIZE 1024

static char *tran(const QVariant &var)
{
	char *arg{};
	if (var.type() == QVariant::List)
	{
		int list_size = var.toList().size();
		if (list_size > 0)
		{
			arg = new char[var.toList()[0].toInt()];
			if (list_size > 1)
			{
				strcpy(arg, var.toList()[1].toString().toStdString().c_str());
			}
		}
		else
		{
			arg = new char[BUF_SIZE];
		}
	}
	else if (var.type() == QVariant::Map)
	{
		arg = new char[BUF_SIZE];
		strcpy(arg, JsonToStr(var.toMap()).toStdString().c_str());
	}
	else
	{
		arg = new char[BUF_SIZE];
		strcpy(arg, var.toString().toStdString().c_str());
	}
	return arg;
}


/// 返回QStringList，在func中进行返回值的拼接
using func_type = QStringList (*)(void *func_ptr, const QVariantList &args);

static QStringList i_F_i_i_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i = int (*)(int, int);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();

	int retCode = i_F_i_i(func_ptr)(arg1, arg2);
	return {QString::number(retCode)};
}

static QStringList v_F_i_pc_(void *func_ptr, const QVariantList &args)
{
	using v_F_i_pc = void (*)(int, char *);
	int arg1 = args[0].toInt();
	char *arg2 = tran(args[1]);

	(v_F_i_pc(func_ptr))(arg1, arg2);
	QString tmp1{arg2};
	delete[] arg2;

	return {tmp1};
}

static QStringList v_F_i_str_(void *func_ptr, const QVariantList &args)
{
	using v_F_i_str = void (*)(int, const char *);
	int arg1 = args[0].toInt();
	char *arg2 = tran(args[1]);

	(v_F_i_str(func_ptr))(arg1, arg2);
	delete[] arg2;

	return {};
}

static QStringList i_F_i_pc_i_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_pc_i = int (*)(int, char *, int);
	int arg1 = args[0].toInt();
	char *arg2 = tran(args[1]);
	int arg3 = args[2].toInt();

	int retCode = i_F_i_pc_i(func_ptr)(arg1, arg2, arg3);
	QString tmp1{arg2};
	delete[] arg2;

	return {QString::number(retCode), tmp1};
}

static QStringList i_F_pc_pc_pc_i_i_(void *func_ptr, const QVariantList &args)
{
	using i_F_pc_pc_pc_i_i = int (*)(char *, char *, char *, int, int);
	char *arg1 = tran(args[0]);
	char *arg2 = tran(args[1]);
	char *arg3 = tran(args[2]);
	int arg4 = args[3].toInt();
	int arg5 = args[4].toInt();

	int retCode = i_F_pc_pc_pc_i_i(func_ptr)(arg1, arg2, arg3, arg4, arg5);

	QString tmp1{arg1};
	delete[] arg1;
	QString tmp2{arg2};
	delete[] arg2;
	QString tmp3{arg3};
	delete[] arg3;

	return {QString::number(retCode), tmp1, tmp2, tmp3};
}

static QStringList i_F_i_str_i_str_str_i_str_str_str_str_i_str_i_pc_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_str_i_str_str_i_str_str_str_str_i_str_i_pc_pc = int (*)(int, const char *, int, const char *, const char *, int, const char *, const char *, const char *, const char *, int,
																		const char *, int, char *, char *);

	int arg1 = args[0].toInt();
	const char *arg2 = tran(args[1]);
	int arg3 = args[2].toInt();
	const char *arg4 = tran(args[3]);
	const char *arg5 = tran(args[4]);
	int arg6 = args[5].toInt();
	const char *arg7 = tran(args[6]);
	const char *arg8 = tran(args[7]);
	const char *arg9 = tran(args[8]);
	const char *arg10 = tran(args[9]);
	int arg11 = args[10].toInt();
	const char *arg12 = tran(args[11]);
	int arg13 = args[12].toInt();
	char *arg14 = tran(args[13]);
	char *arg15 = tran(args[14]);
	int retCode = (i_F_i_str_i_str_str_i_str_str_str_str_i_str_i_pc_pc(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15);

	delete[] arg2;
	delete[] arg4;
	delete[] arg5;
	delete[] arg7;
	delete[] arg8;
	delete[] arg9;
	delete[] arg10;
	delete[] arg12;
	QString tmp1{arg14};
	delete[] arg14;
	QString tmp2{arg15};
	delete[] arg15;
	return {QString::number(retCode), tmp1, tmp2};
}

static QStringList i_F_i_i_i_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i_i = int (*)(int, int, int);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();
	int arg3 = args[2].toInt();
	int retCode = (i_F_i_i_i(func_ptr))(arg1, arg2, arg3);

	return {QString::number(retCode)};
}

static QStringList i_F_i_str_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_str = int (*)(int, const char *);

	int arg1 = args[0].toInt();
	const char *arg2 = tran(args[1]);
	int retCode = (i_F_i_str(func_ptr))(arg1, arg2);

	delete[] arg2;
	return {QString::number(retCode)};
}

static QStringList i_F_i_str_str_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_str_str = int (*)(int, const char *, const char *);

	int arg1 = args[0].toInt();
	const char *arg2 = tran(args[1]);
	const char *arg3 = tran(args[2]);
	int retCode = (i_F_i_str_str(func_ptr))(arg1, arg2, arg3);

	delete[] arg2;
	delete[] arg3;
	return {QString::number(retCode)};
}

static QStringList i_F_i_i_i_i_str_str_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i_i_i_str_str = int (*)(int, int, int, int, const char *, const char *);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();
	int arg3 = args[2].toInt();
	int arg4 = args[3].toInt();
	const char *arg5 = tran(args[4]);
	const char *arg6 = tran(args[5]);
	int retCode = (i_F_i_i_i_i_str_str(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6);

	delete[] arg5;
	delete[] arg6;
	return {QString::number(retCode)};
}

static QStringList i_F_i_str_str_i_pc_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_str_str_i_pc_pc = int (*)(int, const char *, const char *, int, char *, char *);

	int arg1 = args[0].toInt();
	const char *arg2 = tran(args[1]);
	const char *arg3 = tran(args[2]);
	int arg4 = args[3].toInt();
	char *arg5 = tran(args[4]);
	char *arg6 = tran(args[5]);
	int retCode = (i_F_i_str_str_i_pc_pc(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6);

	delete[] arg2;
	delete[] arg3;
	QString tmp1{arg5};
	delete[] arg5;
	QString tmp2{arg6};
	delete[] arg6;
	return {QString::number(retCode), tmp1, tmp2};
}

static QStringList i_F_i_(void *func_ptr, const QVariantList &args)
{
	using i_F_i = int (*)(int);

	int arg1 = args[0].toInt();
	int retCode = (i_F_i(func_ptr))(arg1);

	return {QString::number(retCode)};
}

static QStringList i_F_i_str_str_i_str_str_i_pc_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_str_str_i_str_str_i_pc_pc = int (*)(int, const char *, const char *, int, const char *, const char *, int, char *, char *);

	int arg1 = args[0].toInt();
	const char *arg2 = tran(args[1]);
	const char *arg3 = tran(args[2]);
	int arg4 = args[3].toInt();
	const char *arg5 = tran(args[4]);
	const char *arg6 = tran(args[5]);
	int arg7 = args[6].toInt();
	char *arg8 = tran(args[7]);
	char *arg9 = tran(args[8]);
	int retCode = (i_F_i_str_str_i_str_str_i_pc_pc(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);

	delete[] arg2;
	delete[] arg3;
	delete[] arg5;
	delete[] arg6;
	QString tmp1{arg8};
	delete[] arg8;
	QString tmp2{arg9};
	delete[] arg9;
	return {QString::number(retCode), tmp1, tmp2};
}

static QStringList i_F_i_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_pc = int (*)(int, char *);

	int arg1 = args[0].toInt();
	char *arg2 = tran(args[1]);
	int retCode = (i_F_i_pc(func_ptr))(arg1, arg2);

	QString tmp1{arg2};
	delete[] arg2;
	return {QString::number(retCode), tmp1};
}

static QStringList i_F_i_pi_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_pi = int (*)(int, int *);

	int arg1 = args[0].toInt();
	int *arg2 = new int(args[1].toInt());
	int retCode = (i_F_i_pi(func_ptr))(arg1, arg2);

	QString tmp1{QString::number(*arg2)};
	delete arg2;
	return {QString::number(retCode), tmp1};
}

static QStringList i_F_i_i_str_pc_pc_pc_pc_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i_str_pc_pc_pc_pc_pc = int (*)(int, int, const char *, char *, char *, char *, char *, char *);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();
	const char *arg3 = tran(args[2]);
	char *arg4 = tran(args[3]);
	char *arg5 = tran(args[4]);
	char *arg6 = tran(args[5]);
	char *arg7 = tran(args[6]);
	char *arg8 = tran(args[7]);
	int retCode = (i_F_i_i_str_pc_pc_pc_pc_pc(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);

	delete[] arg3;
	QString tmp1{arg4};
	delete[] arg4;
	QString tmp2{arg5};
	delete[] arg5;
	QString tmp3{arg6};
	delete[] arg6;
	QString tmp4{arg7};
	delete[] arg7;
	QString tmp5{arg8};
	delete[] arg8;
	return {QString::number(retCode), tmp1, tmp2, tmp3, tmp4, tmp5};
}

static QStringList i_F_i_i_i_i_i_i_str_str_i_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i_i_i_i_i_str_str_i_pc = int (*)(int, int, int, int, int, int, const char *, const char *, int, char *);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();
	int arg3 = args[2].toInt();
	int arg4 = args[3].toInt();
	int arg5 = args[4].toInt();
	int arg6 = args[5].toInt();
	const char *arg7 = tran(args[6]);
	const char *arg8 = tran(args[7]);
	int arg9 = args[8].toInt();
	char *arg10 = tran(args[9]);
	int retCode = (i_F_i_i_i_i_i_i_str_str_i_pc(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

	delete[] arg7;
	delete[] arg8;
	QString tmp1{arg10};
	delete[] arg10;
	return {QString::number(retCode), tmp1};
}

static QStringList i_F_i_i_i_i_i_str_str_i_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i_i_i_i_str_str_i_pc = int (*)(int, int, int, int, int, const char *, const char *, int, char *);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();
	int arg3 = args[2].toInt();
	int arg4 = args[3].toInt();
	int arg5 = args[4].toInt();
	const char *arg6 = tran(args[5]);
	const char *arg7 = tran(args[6]);
	int arg8 = args[7].toInt();
	char *arg9 = tran(args[8]);
	int retCode = (i_F_i_i_i_i_i_str_str_i_pc(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);

	delete[] arg6;
	delete[] arg7;
	QString tmp1{arg9};
	delete[] arg9;
	return {QString::number(retCode), tmp1};
}

static QStringList i_F_i_str_str_i_str_i_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_str_str_i_str_i = int (*)(int, const char *, const char *, int, const char *, int);

	int arg1 = args[0].toInt();
	const char *arg2 = tran(args[1]);
	const char *arg3 = tran(args[2]);
	int arg4 = args[3].toInt();
	const char *arg5 = tran(args[4]);
	int arg6 = args[5].toInt();
	int retCode = (i_F_i_str_str_i_str_i(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6);

	delete[] arg2;
	delete[] arg3;
	delete[] arg5;
	return {QString::number(retCode)};
}

static QStringList i_F_i_str_str_str_str_str_str_i_i_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_str_str_str_str_str_str_i_i_pc = int (*)(int, const char *, const char *, const char *, const char *, const char *, const char *, int, int, char *);

	int arg1 = args[0].toInt();
	const char *arg2 = tran(args[1]);
	const char *arg3 = tran(args[2]);
	const char *arg4 = tran(args[3]);
	const char *arg5 = tran(args[4]);
	const char *arg6 = tran(args[5]);
	const char *arg7 = tran(args[6]);
	int arg8 = args[7].toInt();
	int arg9 = args[8].toInt();
	char *arg10 = tran(args[9]);
	int retCode = (i_F_i_str_str_str_str_str_str_i_i_pc(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

	delete[] arg2;
	delete[] arg3;
	delete[] arg4;
	delete[] arg5;
	delete[] arg6;
	delete[] arg7;
	QString tmp1{arg10};
	delete[] arg10;
	return {QString::number(retCode), tmp1};
}

static QStringList i_F_i_i_i_str_str_pi_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i_i_str_str_pi = int (*)(int, int, int, const char *, const char *, int *);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();
	int arg3 = args[2].toInt();
	const char *arg4 = tran(args[3]);
	const char *arg5 = tran(args[4]);
	int *arg6 = new int(args[5].toInt());
	int retCode = (i_F_i_i_i_str_str_pi(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6);

	delete[] arg4;
	delete[] arg5;
	QString tmp1{QString::number(*arg6)};
	delete arg6;
	return {QString::number(retCode), tmp1};
}

static QStringList i_F_i_i_i_str_str_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i_i_str_str = int (*)(int, int, int, const char *, const char *);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();
	int arg3 = args[2].toInt();
	const char *arg4 = tran(args[3]);
	const char *arg5 = tran(args[4]);
	int retCode = (i_F_i_i_i_str_str(func_ptr))(arg1, arg2, arg3, arg4, arg5);

	delete[] arg4;
	delete[] arg5;
	return {QString::number(retCode)};
}

static QStringList i_F_i_i_ustr_i_i_str_str_str_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i_ustr_i_i_str_str_str_pc = int (*)(int, int, const unsigned char *, int, int, const char *, const char *, const char *, char *);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();
	const unsigned char *arg3 = reinterpret_cast<unsigned char *>(args[2].toByteArray().data());
	int arg4 = args[3].toInt();
	int arg5 = args[4].toInt();
	const char *arg6 = tran(args[5]);
	const char *arg7 = tran(args[6]);
	const char *arg8 = tran(args[7]);
	char *arg9 = tran(args[8]);
	int retCode = (i_F_i_i_ustr_i_i_str_str_str_pc(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);

	delete[] arg3;
	delete[] arg6;
	delete[] arg7;
	delete[] arg8;
	QString tmp1{arg9};
	delete[] arg9;
	return {QString::number(retCode), tmp1};
}

static QStringList i_F_i_pi_pc_pi_pc_pi_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_pi_pc_pi_pc_pi = int (*)(int, int *, char *, int *, char *, int *);

	int arg1 = args[0].toInt();
	int *arg2 = new int(args[1].toInt());
	char *arg3 = tran(args[2]);
	int *arg4 = new int(args[3].toInt());
	char *arg5 = tran(args[4]);
	int *arg6 = new int(args[5].toInt());
	int retCode = (i_F_i_pi_pc_pi_pc_pi(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6);

	QString tmp1{QString::number(*arg2)};
	delete arg2;
	QString tmp2{arg3};
	delete[] arg3;
	QString tmp3{QString::number(*arg4)};
	delete arg4;
	QString tmp4{arg5};
	delete[] arg5;
	QString tmp5{QString::number(*arg6)};
	delete arg6;
	return {QString::number(retCode), tmp1, tmp2, tmp3, tmp4, tmp5};
}

static QStringList i_F_i_i_i_i_str_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i_i_i_str_pc = int (*)(int, int, int, int, const char *, char *);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();
	int arg3 = args[2].toInt();
	int arg4 = args[3].toInt();
	const char *arg5 = tran(args[4]);
	char *arg6 = tran(args[5]);
	int retCode = (i_F_i_i_i_i_str_pc(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6);

	delete[] arg5;
	QString tmp1{arg6};
	delete[] arg6;
	return {QString::number(retCode), tmp1};
}

static QStringList i_F_i_ustr_i_i_str_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_ustr_i_i_str = int (*)(int, const unsigned char *, int, int, const char *);

	int arg1 = args[0].toInt();
	const unsigned char *arg2 = reinterpret_cast<unsigned char *>(args[1].toByteArray().data());
	int arg3 = args[2].toInt();
	int arg4 = args[3].toInt();
	const char *arg5 = tran(args[4]);
	int retCode = (i_F_i_ustr_i_i_str(func_ptr))(arg1, arg2, arg3, arg4, arg5);

	delete[] arg2;
	delete[] arg5;
	return {QString::number(retCode)};
}

static QStringList i_F_i_ustr_i_i_i_i_i_i_i_pc_pi_pc_pi_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_ustr_i_i_i_i_i_i_i_pc_pi_pc_pi = int (*)(int, const unsigned char *, int, int, int, int, int, int, int, char *, int *, char *, int *);

	int arg1 = args[0].toInt();
	const unsigned char *arg2 = reinterpret_cast<unsigned char *>(args[1].toByteArray().data());
	int arg3 = args[2].toInt();
	int arg4 = args[3].toInt();
	int arg5 = args[4].toInt();
	int arg6 = args[5].toInt();
	int arg7 = args[6].toInt();
	int arg8 = args[7].toInt();
	int arg9 = args[8].toInt();
	char *arg10 = tran(args[9]);
	int *arg11 = new int(args[10].toInt());
	char *arg12 = tran(args[11]);
	int *arg13 = new int(args[12].toInt());
	int retCode = (i_F_i_ustr_i_i_i_i_i_i_i_pc_pi_pc_pi(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);

	delete[] arg2;
	QString tmp1{arg10};
	delete[] arg10;
	QString tmp2{QString::number(*arg11)};
	delete arg11;
	QString tmp3{arg12};
	delete[] arg12;
	QString tmp4{QString::number(*arg13)};
	delete arg13;
	return {QString::number(retCode), tmp1, tmp2, tmp3, tmp4};
}

static QStringList i_F_i_i_ustr_i_i_i_i_i_i_i_str_pi_pc_pi_pc_pi_pc_pi_pc_pi_pc_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_i_ustr_i_i_i_i_i_i_i_str_pi_pc_pi_pc_pi_pc_pi_pc_pi_pc = int (*)(int, int, const unsigned char *, int, int, int, int, int, int, int, const char *, int *, char *, int *, char *, int *,
																				 char *, int *, char *, int *, char *);

	int arg1 = args[0].toInt();
	int arg2 = args[1].toInt();
	const unsigned char *arg3 = reinterpret_cast<unsigned char *>(args[2].toByteArray().data());
	int arg4 = args[3].toInt();
	int arg5 = args[4].toInt();
	int arg6 = args[5].toInt();
	int arg7 = args[6].toInt();
	int arg8 = args[7].toInt();
	int arg9 = args[8].toInt();
	int arg10 = args[9].toInt();
	const char *arg11 = tran(args[10]);
	int *arg12 = new int(args[11].toInt());
	char *arg13 = tran(args[12]);
	int *arg14 = new int(args[13].toInt());
	char *arg15 = tran(args[14]);
	int *arg16 = new int(args[15].toInt());
	char *arg17 = tran(args[16]);
	int *arg18 = new int(args[17].toInt());
	char *arg19 = tran(args[18]);
	int *arg20 = new int(args[19].toInt());
	char *arg21 = tran(args[20]);
	int retCode = (i_F_i_i_ustr_i_i_i_i_i_i_i_str_pi_pc_pi_pc_pi_pc_pi_pc_pi_pc(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17,
																						   arg18, arg19, arg20, arg21);

	delete[] arg3;
	delete[] arg11;
	QString tmp1{QString::number(*arg12)};
	delete arg12;
	QString tmp2{arg13};
	delete[] arg13;
	QString tmp3{QString::number(*arg14)};
	delete arg14;
	QString tmp4{arg15};
	delete[] arg15;
	QString tmp5{QString::number(*arg16)};
	delete arg16;
	QString tmp6{arg17};
	delete[] arg17;
	QString tmp7{QString::number(*arg18)};
	delete arg18;
	QString tmp8{arg19};
	delete[] arg19;
	QString tmp9{QString::number(*arg20)};
	delete arg20;
	QString tmp10{arg21};
	delete[] arg21;
	return {QString::number(retCode), tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10};
}

static QStringList i_F_i_ustr_i_i_i_i_i_i_i_(void *func_ptr, const QVariantList &args)
{
	using i_F_i_ustr_i_i_i_i_i_i_i = int (*)(int, const unsigned char *, int, int, int, int, int, int, int);

	int arg1 = args[0].toInt();
	const unsigned char *arg2 = reinterpret_cast<unsigned char *>(args[1].toByteArray().data());
	int arg3 = args[2].toInt();
	int arg4 = args[3].toInt();
	int arg5 = args[4].toInt();
	int arg6 = args[5].toInt();
	int arg7 = args[6].toInt();
	int arg8 = args[7].toInt();
	int arg9 = args[8].toInt();
	int retCode = (i_F_i_ustr_i_i_i_i_i_i_i(func_ptr))(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);

	delete[] arg2;
	return {QString::number(retCode)};
}
// 29

static const std::map<QString, func_type> func_map{
		{"i_F_i_i",                                                      i_F_i_i_},
		{"v_F_i_pc",                                                     v_F_i_pc_},
		{"v_F_i_str",                                                    v_F_i_str_},
		{"i_F_i_pc_i",                                                   i_F_i_pc_i_},
		{"i_F_pc_pc_pc_i_i",                                             i_F_pc_pc_pc_i_i_},
		{"i_F_i_str_i_str_str_i_str_str_str_str_i_str_i_pc_pc",          i_F_i_str_i_str_str_i_str_str_str_str_i_str_i_pc_pc_},
		{"i_F_i_i_i",                                                    i_F_i_i_i_},
		{"i_F_i_str",                                                    i_F_i_str_},
		{"i_F_i_str_str",                                                i_F_i_str_str_},
		{"i_F_i_i_i_i_str_str",                                          i_F_i_i_i_i_str_str_},
		{"i_F_i_str_str_i_pc_pc",                                        i_F_i_str_str_i_pc_pc_},
		{"i_F_i",                                                        i_F_i_},
		{"i_F_i_str_str_i_str_str_i_pc_pc",                              i_F_i_str_str_i_str_str_i_pc_pc_},
		{"i_F_i_pc",                                                     i_F_i_pc_},
		{"i_F_i_pi",                                                     i_F_i_pi_},
		{"i_F_i_i_str_pc_pc_pc_pc_pc",                                   i_F_i_i_str_pc_pc_pc_pc_pc_},
		{"i_F_i_i_i_i_i_i_str_str_i_pc",                                 i_F_i_i_i_i_i_i_str_str_i_pc_},
		{"i_F_i_i_i_i_i_str_str_i_pc",                                   i_F_i_i_i_i_i_str_str_i_pc_},
		{"i_F_i_str_str_i_str_i",                                        i_F_i_str_str_i_str_i_},
		{"i_F_i_str_str_str_str_str_str_i_i_pc",                         i_F_i_str_str_str_str_str_str_i_i_pc_},
		{"i_F_i_i_i_str_str_pi",                                         i_F_i_i_i_str_str_pi_},
		{"i_F_i_i_i_str_str",                                            i_F_i_i_i_str_str_},
		{"i_F_i_i_ustr_i_i_str_str_str_pc",                              i_F_i_i_ustr_i_i_str_str_str_pc_},
		{"i_F_i_pi_pc_pi_pc_pi",                                         i_F_i_pi_pc_pi_pc_pi_},
		{"i_F_i_i_i_i_str_pc",                                           i_F_i_i_i_i_str_pc_},
		{"i_F_i_ustr_i_i_str",                                           i_F_i_ustr_i_i_str_},
		{"i_F_i_ustr_i_i_i_i_i_i_i_pc_pi_pc_pi",                         i_F_i_ustr_i_i_i_i_i_i_i_pc_pi_pc_pi_},
		{"i_F_i_i_ustr_i_i_i_i_i_i_i_str_pi_pc_pi_pc_pi_pc_pi_pc_pi_pc", i_F_i_i_ustr_i_i_i_i_i_i_i_str_pi_pc_pi_pc_pi_pc_pi_pc_pi_pc_},
		{"i_F_i_ustr_i_i_i_i_i_i_i",                                     i_F_i_ustr_i_i_i_i_i_i_i_}
};// 29

bool check_funcList(const QString &func)
{
	return func_map.count(func);
}


static QStringList callFunc(const QString &type, void *X, const QVariantList &args = {})
{
	auto func_iter = func_map.find(type);
	if (func_iter == func_map.end())
	{
		// 处理未知类型或错误情况
		std::cerr << "error type: " << type.toStdString() << std::endl;
		throw std::runtime_error("Unsupported type");
	}
	else
	{
		return func_iter->second(X, args);
	}
}

static QString getTypeRef(const QString &type)
{
	const static std::map<QString, QString> typeRef{
			{"void",               "v"},
			{"int",                "i"},
			{"int*",               "pi"},
			{"char",               "c"},
			{"unsignedchar",       "uc"},
			{"byte",               "uc"},
			{"char*",              "pc"},
			{"unsignedchar*",      "puc"},
			{"byte*",              "puc"},
			{"constchar*",         "str"},
			{"constunsignedchar*", "ustr"},
			{"constbyte*",         "ustr"},
			{"double",             "d"},
			{"double*",            "pd"}
	};
	QString tmp{type};
	tmp.remove(' ');
	if (typeRef.count(tmp))
		return typeRef.at(tmp);
	else
		return "";
}

QString getRefType(const QString &type)
{
	const static std::map<QString, QString> refType{
			{"v",    "void"},
			{"i",    "int"},
			{"pi",   "int*"},
			{"c",    "char"},
			{"uc",   "unsigned char"},
			{"pc",   "char*"},
			{"puc",  "unsigned char*"},
			{"str",  "const char*"},
			{"ustr", "const unsigned char*"},
			{"d",    "double"},
			{"pd",   "double*"}
	};
	QString tmp{type};
	tmp.remove(' ');
	if (refType.count(tmp))
		return refType.at(tmp);
	else
		return "";
}

#include <QTextCodec>

QString utf8ToGbk(const QString &utf8String)
{
	QTextCodec *codec = QTextCodec::codecForName("GB18030");
	if (codec)
	{
		QByteArray byteArray = codec->fromUnicode(utf8String);
		return QString::fromLocal8Bit(byteArray);
	}
	else
	{
		// 处理编解码器未找到的情况
		return {};
	}
}

QString gbkToUtf8(const QString &gbkString)
{
	QTextCodec *codec = QTextCodec::codecForName("GB18030");
	if (codec)
	{
		QByteArray byteArray = codec->fromUnicode(gbkString);
		return QString::fromUtf8(byteArray);
	}
	else
	{
		// 处理编解码器未找到的情况
		return {};
	}
}


static QString declare_parsing(const QString &func_declare, QString &func_name, QStringList &func_paramList)
{
	QString out;

	int spaceIndex = func_declare.indexOf(" ");
	if (spaceIndex != -1)
		out = getTypeRef(func_declare.left(spaceIndex)) + "_F";
	else
		return {};

	int startIndex = func_declare.indexOf("(");
	int endIndex = func_declare.indexOf(")");

	func_name = func_declare.mid(spaceIndex + 1, startIndex - spaceIndex - 1);

	if (startIndex != -1 && endIndex != -1 && startIndex < endIndex)
	{
		QString func_params = func_declare.mid(startIndex + 1, endIndex - startIndex - 1);
		func_paramList = func_params.split(',');
	}
	else
		return {};

	for (auto &param : func_paramList)
	{
		param = param.trimmed();
		int p = param.indexOf('*');
		if (p == -1)
			p = param.indexOf(' ');

		out += "_" + getTypeRef(param.left(p + 1));
	}

	return out;
}

func_Data::func_Data(QString func_declare, const QJsonValue &func_argsList) : declare(std::move(func_declare))
{
	typeRef = declare_parsing(declare, name, paramList);
	argsList = func_argsList.toObject()["args"].toArray().toVariantList();
}

void func_Data::display2table(QTableWidget *table) const
{
	table->clear();

	table->setRowCount(paramList.size());
	table->setColumnCount(1);

	table->setHorizontalHeaderItem(0, new QTableWidgetItem(name));
	table->resizeColumnToContents(0);
	table->setVerticalHeaderLabels(paramList);

	for (int i{}; i < argsList.size(); i++)
	{
		QTableWidgetItem *item;
		if (argsList[i].type() == QVariant::Map)
			item = new QTableWidgetItem(JsonToStr(argsList[i].toMap()));
		else if (argsList[i].type() == QVariant::List)
			item = new QTableWidgetItem(JsonArrayToStr(argsList[i].toList()));
		else
			item = new QTableWidgetItem(argsList[i].toString());
		table->setItem(i, 0, item);
	}
}

QStringList func_Data::call() const
{
	return callFunc(typeRef, ptr, argsList);
}

void func_Data::loadArgs(QTableWidget *table)
{
	if (table->rowCount() == 0 || table->columnCount() == 0)
		return;
	if (getTableName(table) != name)
		return;

	bool replace{false};
	if (argsList.size() == table->rowCount())
	{
		replace = true;
	}
	for (int var{}; var < table->rowCount(); var++)
	{
		QString str = table->item(var, 0)->text();
		auto first = str[0], last = str[str.length() - 1];
		QVariant tmp;
		if (first == '{' && last == '}')
			tmp = StrToJson(str);
		else if (first == '[' && last == ']')
			tmp = StrToJsonArray(str);
		else
			tmp = str;

		if (replace)
			argsList[var] = tmp;
		else
			argsList.append(tmp);
	}
}

