#ifndef MYBUTTON_H
#define MYBUTTON_H

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

#include "FuncPtr.h"
#include "DynamicLib.h"

static QString JsonToStr(const QVariantMap &map)
{
	QJsonObject object = QJsonObject::fromVariantMap(map);
	QJsonDocument document(object);
	return {document.toJson(QJsonDocument::Compact)};
}

static QString JsonArrayToStr(const QVariantList &list)
{
	QJsonArray array = QJsonArray::fromVariantList(list);
	QJsonDocument document(array);
	return {document.toJson(QJsonDocument::Compact)};
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


class func_Data
{
private:
	QString declare, name, typeRef;
	QStringList argsList, paramList;
	void *ptr{};

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

			out += "_" + getTypeRef(param.left(p+1));
		}

		return out;
	}

public:
	func_Data(QString func_declare, const QJsonValue &func_argsList) : declare(std::move(func_declare))
	{
		typeRef = declare_parsing(declare, name, paramList);

		for (const auto &arg: func_argsList.toObject()["args"].toArray().toVariantList())
		{
			if (arg.type() == QVariant::Map)
				argsList << JsonToStr(arg.toMap());
			else if (arg.type() == QVariant::List)
				argsList << JsonArrayToStr(arg.toList());
			else
				argsList << arg.toString();
		}
	}

	const QString &getDeclare() const
	{
		return declare;
	}

	const QString &getName() const
	{
		return name;
	}

	const QStringList &getArgsList() const
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

	void display2table(QTableWidget *table) const
	{
		table->clear();

		table->setRowCount(paramList.size());
		table->setColumnCount(1);

		table->setHorizontalHeaderItem(0, new QTableWidgetItem(name));
		table->setVerticalHeaderLabels(paramList);

		for (int i{}; i < argsList.size(); i++)
			table->setItem(i, 0, new QTableWidgetItem(argsList[i]));
	}

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

	QStringList call() const
	{
		return callFunc(typeRef, ptr, argsList);
	}

	static QStringList call(const func_Data *func)
	{
		return func->call();
	}

	void loadArgs(QTableWidget *table)
	{
		if (table->rowCount() == 0 || table->columnCount() == 0)
			return;
		if (getTableName(table) == name)
		{
			for (int var{}; var < argsList.size(); var++)
			{
				argsList[var] = table->item(var, 0)->text();
			}
		}
	}

};

QT_BEGIN_NAMESPACE
class MyButton;
QT_END_NAMESPACE

class MyButton : public QPushButton
{
Q_OBJECT
	func_Data *func;
public:
	explicit MyButton(QWidget *parent = nullptr) : QPushButton(parent) {}

	explicit MyButton(const QString &text, QWidget *parent = nullptr) : QPushButton(text, parent) {}

	explicit MyButton(const QString &func_declare, const QJsonValue &func_argsList, QWidget *parent = nullptr)
			: QPushButton(init_func(func_declare, func_argsList), parent) {}

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
        this->setMaximum_Width_Height(this->getTextWidth() + 200, 31);
//		this->setSizeIncrement(this->sizeHint());
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

	~MyButton() override
	{
		delete func;
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
		if (event->button() == Qt::LeftButton)
		{
			emit leftClicked(this->func);
		}
		else
		{
			QPushButton::mousePressEvent(event);
		}
	}
};


#endif // MYBUTTON_H
