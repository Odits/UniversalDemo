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
pc = char*
str = const char*

d = double
pd = double*

*/

typedef int (*i_F_i_i)(int, int);

static int i_F_i_i_(void *func, int arg1, int arg2)
{
	return i_F_i_i(func)(arg1, arg2);
}

typedef int (*i_F_i_pc_i)(int, char *, int);

static int i_F_i_pc_i_(void *func, int arg1, char *arg2, int arg3)
{
	return i_F_i_pc_i(func)(arg1, arg2, arg3);
}

typedef void (*v_F_i_pc)(int, char *);

static void v_F_i_pc_(void *func, int arg1, char *arg2)
{
	return v_F_i_pc(func)(arg1, arg2);
}

typedef void (*v_F_i_str)(int, const char *);

static void v_F_i_str_(void *func, int arg1, const char *arg2)
{
	return v_F_i_str(func)(arg1, arg2);
}

typedef int (*i_F_pc_pc_pc_i_i)(char *, char *, char *, int, int);

static int i_F_pc_pc_pc_i_i_(void *func, char *arg1, char *arg2, char *arg3, int arg4, int arg5)
{
	return i_F_pc_pc_pc_i_i(func)(arg1, arg2, arg3, arg4, arg5);
}


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

static QStringList callFunc(const QString &type, void *X, const QVariantList &args = {})
{
	QStringList msgList;
	if (type == "i_F_i_i")
	{
		int arg1 = args[0].toInt();
		int arg2 = args[1].toInt();

		int retCode = i_F_i_i_(X, arg1, arg2);
		msgList.push_back(QString::number(retCode));
	}
	else if (type == "i_F_i_pc_i")
	{
		int arg1 = args[0].toInt();
		char *arg2 = tran(args[1]);
		int arg3 = args[2].toInt();

		int retCode = i_F_i_pc_i_(X, arg1, arg2, arg3);
		msgList.push_back(QString::number(retCode));
		msgList.push_back(arg2);
		delete[] arg2;
	}
	else if (type == "v_F_i_pc")
	{
		int arg1 = args[0].toInt();
		char *arg2 = tran(args[1]);

		v_F_i_pc_(X, arg1, arg2);
		msgList.push_back(arg2);
		delete[] arg2;
	}
	else if (type == "v_F_i_str")
	{
		int arg1 = args[0].toInt();
		char *arg2 = tran(args[1]);

		v_F_i_str_(X, arg1, arg2);
		delete[] arg2;
	}
	else if (type == "i_F_pc_pc_pc_i_i")
	{
		char *arg1 = tran(args[0]);
		char *arg2 = tran(args[1]);
		char *arg3 = tran(args[2]);
		int arg4 = args[3].toInt();
		int arg5 = args[4].toInt();

		int retCode = i_F_pc_pc_pc_i_i_(X, arg1, arg2, arg3, arg4, arg5);

		msgList.push_back(QString::number(retCode));
		msgList.push_back(arg1);
		msgList.push_back(arg2);
		msgList.push_back(arg3);
		delete[] arg1;
		delete[] arg2;
		delete[] arg3;
	}
	else
	{
		// 处理未知类型或错误情况
		std::cerr << "error type: " << type.toStdString() << std::endl;
		throw std::runtime_error("Unsupported type");
	}
	return msgList;
}

static QString getTypeRef(const QString &type)
{
	const static std::map<QString, QString> typeRef{
			{"void",       "v"},
			{"int",        "i"},
			{"int*",       "pi"},
			{"char",       "c"},
			{"char*",      "pc"},
			{"constchar*", "str"},
			{"double",     "d"},
			{"double*",    "pd"}
	};
	QString tmp{type};
	tmp.remove(' ');
	if (typeRef.count(tmp))
		return typeRef.at(tmp);
	else
		return "";
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

	for (auto &param: func_paramList)
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
	if (getTableName(table) == name)
	{
		for (int var{}; var < argsList.size(); var++)
		{
			QString str = table->item(var, 0)->text();
			if (argsList[var].type() == QVariant::Map)
				argsList[var] = StrToJson(str);
			else if (argsList[var].type() == QVariant::List)
				argsList[var] = StrToJsonArray(str);
			else
				argsList[var] = str;
		}
	}
}

