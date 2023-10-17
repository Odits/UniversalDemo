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
	explicit funcData_Button(QWidget *parent = nullptr) : QPushButton(parent) {}

	explicit funcData_Button(const QString &text, QWidget *parent = nullptr) : QPushButton(text, parent) {}

	explicit funcData_Button(const QString &func_declare, const QJsonValue &func_argsList, QWidget *parent = nullptr)
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

	~funcData_Button() override
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


#endif // FUNCDATA_BUTTON_H
