#ifndef FUNCDATA_BUTTON_H
#define FUNCDATA_BUTTON_H

#include <QTableWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>
#include <QDebug>
#include <QPushButton>
#include <QMouseEvent>

#include <utility>
#include <string>

#include "DynamicLib.h"

bool check_funcList(const QString& func);

QString getRefType(const QString &type);

class func_Data
{
private:
	QString declare, name, typeRef;
	QStringList paramList;
	QVariantList argsList, fileList;
	void *ptr{};

public:
	func_Data(QString func_declare, const QJsonValue &func_argsList);

	const QString &getDeclare() const
	{
		return declare;
	}

	const QString &getName() const
	{
		return name;
	}

	const QVariantList &getArgsList() const
	{
		return argsList;
	}

	const QVariantList &getfileList() const
	{
		return fileList;
	}

	const QStringList &getParamList() const
	{
		return paramList;
	}

	const QString &getTypeRef() const
	{
		return typeRef;
	}

	void display2table(QTableWidget *table) const;

	static void display2table(const func_Data *func, QTableWidget *table)
	{
		func->display2table(table);
	}

	static QString getTableName(QTableWidget *table)
	{
		return table->horizontalHeaderItem(0)->text();
	}

	void setPtr(void *func_ptr)
	{
		this->ptr = func_ptr;
	}

	QVariantList loadFile() const;

	QString call(bool isGBK = false) const;

	void loadArgs(QTableWidget *table);
};

QT_BEGIN_NAMESPACE
class funcData_Button;

QT_END_NAMESPACE

class funcData_Button : public QPushButton
{
Q_OBJECT
	func_Data *func;
public:
	explicit funcData_Button(QWidget *parent = nullptr) : QPushButton(parent), func(nullptr) {}
	// 这两个构造必须初始化func，不然析构时会崩溃
	explicit funcData_Button(const QString &text, QWidget *parent = nullptr) : QPushButton(text, parent), func(nullptr) {}

	// 这个构造里的init_func已初始化func
	explicit funcData_Button(const QString &func_declare, const QJsonValue &func_argsList, QWidget *parent = nullptr)
			: QPushButton(init_func(func_declare, func_argsList), parent) {}

	~funcData_Button() override
	{
		delete func;
	}

	func_Data *func_data() const
	{
		return func;
	}

	int getTextWidth()
	{
		// 使用 QFontMetrics 计算文本的宽度
		QFontMetrics fontMetrics(this->font());
		return fontMetrics.width(this->text());
	}

	void setMaximum_Width_Height(int width, int height)
	{
		this->setMaximumWidth(width);
		this->setMaximumHeight(height);
	}

	void autoResize()
	{
//        this->setMaximum_Width_Height(this->getTextWidth() + 20, 31);
		this->setSizeIncrement(this->sizeHint());
	}

	static void autoResize(QPushButton *pb)
	{
		QFontMetrics fontMetrics(pb->font());
		int width = fontMetrics.width(pb->text());

		pb->setMaximumWidth(width);
	}

	const QString &init_func(const QString &func_declare, const QJsonValue &func_argsList)
	{
		func = new func_Data(func_declare, func_argsList);
		return func->getName();
	}

	void init_func(func_Data *funcData)
	{
		this->func = funcData;
	}

	bool func_loadLib(DynamicLib *lib)
	{
		QString name = func->getName();
		void *ptr = lib->loadFunc<void *>(name.toStdString().c_str());
		if (ptr)
		{
			func->setPtr(ptr);
			return true;
		}
		else
			return false;
	}

	std::tuple<QString, QJsonObject> getJsonObj()
	{
		QJsonObject args;
		args["args"] = QJsonArray::fromVariantList(func->getArgsList());
		if (!func->getfileList().isEmpty())
			args["file"] = QJsonArray::fromVariantList(func->getfileList());
		return std::make_tuple(func->getDeclare(), args);
	}

	QString getDefParsing() const
	{
/*
static QStringList i_F_pc_pc_pc_i_i_(void *func_ptr, const QVariantList &args)
{
	if (args.size() != 5)
		throw std::runtime_error("args.size error");
	using i_F_pc_pc_pc_i_i = int (*)(char *, char *, char *, int, int);			def

	char *arg1 = tran(args[0]);
	char *arg2 = tran(args[1]);
	char *arg3 = tran(args[2]);													parsing
	int arg4 = args[3].toInt();
	int arg5 = args[4].toInt();

	int retCode = i_F_pc_pc_pc_i_i(func_ptr)(arg1, arg2, arg3, arg4, arg5);		call

	QString tmp1{arg1};
	delete[] arg1;
	QString tmp2{arg2};															save&clear
	delete[] arg2;
	QString tmp3{arg3};
	delete[] arg3;

	return {QString::number(retCode), tmp1, tmp2, tmp3};						ret
}
*/
		QString func_def{"static QStringList "};
		auto typeRef = func->getTypeRef();
		func_def += typeRef + "_(void *func_ptr, const QVariantList &args)\n{\n\t";

		QString def{"using "};
		def += typeRef + " = ";

		auto retType = getRefType(QString(typeRef[0]));
		def += retType + " (*)(";

		QString call, ret{"return {"};
		if (typeRef[0] == 'i')
		{
			call = "int retCode = ";
			ret += "QString::number(retCode), ";
		}
		call += "(" + typeRef + "(func_ptr))(";

		QString parsing, saveClear;
		int saveNums{0};
		auto paramType = typeRef.mid(typeRef.indexOf('F') + 2).split('_');
		for (int i{0}; i < paramType.length(); i++)
		{
			auto refType{getRefType(paramType[i])};

			def += refType + ", ";

			parsing += refType + " arg" + QString::number(i+1) + " = ";
			if (paramType[i] == "i")
				parsing += "args[" + QString::number(i) + "].toInt();";
			else if (paramType[i] == "pi")
				parsing += "new int(args[" + QString::number(i) + "].toInt());";
			else if (paramType[i] == "d")
				parsing += "args[" + QString::number(i) + "].toDouble();";
			else if (paramType[i] == "pd")
				parsing += "new double(args[" + QString::number(i) + "].toDouble());";
			else if (paramType[i] == "c")
				parsing += "args[" + QString::number(i) + "].toChar();";
			else if (paramType[i] == "pc" || paramType[i] == "str")
				parsing += "tran(args[" + QString::number(i) + "]);";
			else if (paramType[i] == "uc")
				parsing += "static_cast<unsigned char>(args[" + QString::number(i) + "].toChar());";
			else if (paramType[i] == "puc" || paramType[i] == "ustr")
				parsing += "reinterpret_cast<unsigned char *>(tran(args[" + QString::number(i) + "]));";
			parsing += "\n\t";

			call += "arg" + QString::number(i+1) + ", ";

			if (refType.contains("*"))	// 有指针情况
			{
				if (!refType.contains("const"))
				{
					saveNums++;

					if (paramType[i].contains("u"))
						saveClear += "QString tmp" + QString::number(saveNums) + "{(char*)arg" + QString::number(i + 1) + "};\n\t";
					else if (paramType[i].contains("c"))
						saveClear += "QString tmp" + QString::number(saveNums) + "{arg" + QString::number(i + 1) + "};\n\t";
					else
						saveClear += "QString tmp" + QString::number(saveNums) + "{QString::number(*arg" + QString::number(i + 1) + ")};\n\t";

					ret += "tmp" + QString::number(saveNums) + ", ";
				}

				if (refType.contains("char"))
					saveClear += "delete[] arg" + QString::number(i+1) + ";\n\t";
				else
					saveClear += "delete arg" + QString::number(i+1) + ";\n\t";
			}
		}
		def.replace(def.length() - 2, 2, ");\n\n\t");
		call.replace(call.length() - 2, 2, ");\n\n\t");
		ret.replace(ret.length() - 2, 2, "};\n}\n");

		QString check{"if (args.size() != " + QString::number(paramType.length()) + ")\n\t\t"};
		check += "throw std::runtime_error(\"args.size error\");\n\t";

		return {func_def + check + def + parsing + call + saveClear + ret};
	}

signals:

	void leftClicked(func_Data *);

	void rightClicked(func_Data *);

protected:
	void mousePressEvent(QMouseEvent *event) override
	{
		if (event->button() == Qt::RightButton)
		{
			emit rightClicked(this->func);
		}
		else if (event->button() == Qt::LeftButton)
		{
			emit leftClicked(this->func);
		}
		else
		{
			QPushButton::mousePressEvent(event);
		}
	}
};


#endif // FUNCDATA_BUTTON_H
