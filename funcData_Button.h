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

QString getRefType(const QString &type);

class func_Data
{
private:
	QString declare, name, typeRef;
	QStringList paramList;
	QVariantList argsList;
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

	QStringList call() const;

	static QStringList call(const func_Data *func)
	{
		return func->call();
	}

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
		return std::make_tuple(func->getDeclare(), args);
	}

	QString getDef()
	{
		//static int i_F_i_i_(void *func, int arg1, int arg2)
		//{
		//	using i_F_i_i = int (*)(int, int);
		//	return i_F_i_i(func)(arg1, arg2);
		//}
		QString func_def{"static "};

		QString def{"using "};
		auto typeRef = func->getTypeRef();
		def += typeRef + " = ";

		auto retType = getRefType(typeRef.mid(0, typeRef.indexOf('_')));
		func_def += retType + " " + typeRef + "_(void *func, ";
		def += retType + " (*)(";
		int arg_num{1};
		for (const auto& param : func->getParamList())
		{
			int p = param.indexOf('*');
			if (p == -1)
				p = param.indexOf(' ');

			auto tmp = param.left(p + 1).remove(' ');
			def += tmp + ", ";

			func_def += tmp + " arg" + QString::number(arg_num++) + ", ";
		}
		func_def.replace(func_def.length() - 2, 2, ")");
		def.replace(def.length() - 2, 2, ");");

		func_def += "\n{\n\t" + def + "\n\treturn " + typeRef + "(func)(";
		for (int i{1}; i < arg_num; i++)
			func_def += "arg" + QString::number(i) + ", ";
		func_def.replace(func_def.length() - 2, 2, ");");
		func_def += "\n}\n\n";

		return func_def;
	}

	QString getParsing()
	{
		/*
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
}		 */

		QString parsing{"else if (type == \""};
		auto typeRef = func->getTypeRef();
		parsing += typeRef + "\")\n{\n\t";
		auto retType = typeRef[0];

		auto paramType = typeRef.right(typeRef.indexOf('F') + 1).split('_');
		for (int i{1}; i <= paramType.length(); i++)
		{
			parsing += getRefType(paramType[i]) + " arg" + QString::number(i) + " = ";
		}


		if (retType == 'i')
		{

		}
		else if (retType == 'v')
		{

		}
		else
		{
			std::cerr << "fail, retType is no think yet";
		}


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
