#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <map>
#include <utility>
#include <vector>

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QSettings>
#include <QDebug>
#include <QTableWidget>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>

#include "FuncPtr.h"
#include "DynamicLib.h"
#include "MyButton.h"

std::vector<std::tuple<QString, QString, QString>> funcList;
DynamicLib *lib;
std::vector<void *> funcPtr;
std::map<QString, size_t> funcMap;

#define get_funcDef(func) \
    std::get<0>(func)

#define get_funcName(func) \
    std::get<1>(func)

#define get_funcArgs(func) \
    std::get<2>(func)

/*
	arg——实参（Argument）
	param——形参（Parameter）

	"int jsonTest(char *args, char *resmsg, char *errmsg, int readTimeout, int writeTimeout);": {
		"args": [{}, [1024], [128], 15, 15]
	}
	func_declare: int jsonTest(char *args, char *resmsg, char *errmsg, int readTimeout, int writeTimeout);
	func_name: jsonTest
	func_paramList: [char *args, char *resmsg, char *errmsg, int readTimeout, int writeTimeout]
	func_argList: [{}, [1024], [128], 15, 15]
	func_typeRef: i_F_str_pc_pc_i_i
 */

MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QObject::connect(ui->Close, &QAction::triggered, [&]() {
		ui->param_inputTable->clear();
		ui->result->clear();
		if (lib)
		{
			delete lib;
			lib = nullptr;
		}
		funcList.clear();
		funcPtr.clear();
		funcMap.clear();

		for (int i{ui->gridLayout->count()}; i > 1; i--)
		{
			auto *button = qobject_cast<MyButton *>(ui->gridLayout->itemAt(i - 1)->widget());

			ui->gridLayout->removeWidget(button);
			button->hide();
			delete button;
		}

		ui->pB_LOAD->show();
	});

}

MainWindow::~MainWindow()
{
	delete ui;
	if (lib)
	{
		delete lib;
		lib = nullptr;
	}
}

QVariantMap readJsonFile(const QString &filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return {};
	}

	QByteArray jsonData = file.readAll();
	file.close();

	QJsonDocument document = QJsonDocument::fromJson(jsonData);
	if (document.isNull() || !document.isObject())
	{
		return {};
	}

	return document.object().toVariantMap();
}

QJsonObject readJsonFile1(const QString &filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return {};
	}

	QByteArray jsonData = file.readAll();
	file.close();

	QJsonDocument document = QJsonDocument::fromJson(jsonData);
	if (document.isNull() || !document.isObject())
	{
		return {};
	}

	return document.object();
}

QString printJson(const QVariantMap &map)
{
	QJsonObject object = QJsonObject::fromVariantMap(map);
	QJsonDocument document(object);
	return {document.toJson(QJsonDocument::Indented)};
}

QString printJson(const QJsonObject &object)
{
	QJsonDocument document(object);
	return {document.toJson(QJsonDocument::Indented)};
}

QString printJson(const QJsonValueRef &ref)
{
	QJsonDocument document(ref.toObject());
	return {document.toJson(QJsonDocument::Indented)};
}

static void func_display2table(QTableWidget *table, QTableWidgetItem *func_name, const QStringList &func_params, const QStringList &func_args)
{
	table->clear();

	table->setRowCount(func_params.size());
	table->setColumnCount(1);

	table->setHorizontalHeaderItem(0, func_name);
	table->setVerticalHeaderLabels(func_params);

	for (int i{}; i < func_args.size(); i++)
		table->setItem(i, 0, new QTableWidgetItem(func_args[i]));
}


void MainWindow::on_LibrarySelector_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(
			this,
			tr("open a Library."),
			"./",
			tr("Library(*.so *.dll);;All files(*.*)"));

	if (fileName.isEmpty())
	{
		QMessageBox::warning(this, "Warning!", "Failed to open the Library!");
	}
	else
	{
		ui->LibraryPath->setText(fileName);
	}
}

void MainWindow::on_ConfigSelector_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(
			this,
			tr("open a Config."),
			"./",
			tr("Config(*.json);;All files(*.*)"));

	if (fileName.isEmpty())
	{
		QMessageBox::warning(this, "Warning!", "Failed to open the Config!");
	}
	else
	{
		ui->ConfigPath->setText(fileName);
	}
}

void MainWindow::on_pB_LOAD_clicked()
{
	lib = new DynamicLib(ui->LibraryPath->text().toStdString());

	QJsonObject configs = readJsonFile1(ui->ConfigPath->text());
	if (configs.empty())
	{
		ui->result->setText("配置文件错误");
		return;
	}
	ui->pB_LOAD->hide();

//	ui->result->append(printJson(configs));
	for (const auto &key: configs.keys())
	{
		ui->result->append(key + ":" + printJson(configs[key]));
		auto *func = new func_Data(key, configs[key]);

		auto *newPB = new MyButton(func->getName(), this);
		newPB->autoResize();
		ui->gridLayout->addWidget(newPB);

		QObject::connect(newPB, &MyButton::leftClicked, [&](const QString &funcName) {
//			size_t index = funcMap[funcName];
//			QStringList msgList = callFunc(get_funcDef(funcList[index]), funcPtr[index], get_funcArgs(funcList[index]).split(','));
//
//			QString msg = "Call " + funcName;
//			for (const auto &resp: msgList)
//			{
//				msg += " msgList=" + resp;
//			}
//			ui->result->append(msg);
		});
		QObject::connect(newPB, &MyButton::rightClicked, [&](const QString &funcName) {
//			size_t index = funcMap[funcName];
//			ui->input->appendPlainText(get_funcArgs(funcList[index]));

//			func_display2table(ui->param_inputTable, new QTableWidgetItem(funcName), get_funcArgs(funcList[index]).split(','), {});

			func->display2table(ui->param_inputTable);
		});


	}




	for (int i{}; i < funcList.size(); i++)
	{
		void *func = lib->loadFunc<void *>(get_funcName(funcList[i]).toStdString().c_str());
		funcPtr.push_back(func);
		if (!func)
		{
			QString errMsg = "load function " + get_funcName(funcList[i]) + " fail. errMsg=" + lib->getErrorMsg().c_str();
			ui->result->append(errMsg);
			continue;
		}
		funcMap.insert(std::pair<QString, size_t>(get_funcName(funcList[i]), i));

		auto *newPB = new MyButton(get_funcName(funcList[i]), this);
		newPB->autoResize();
		ui->gridLayout->addWidget(newPB);

		QObject::connect(newPB, &MyButton::leftClicked, [&](const QString &funcName) {
			size_t index = funcMap[funcName];
			QStringList msgList = callFunc(get_funcDef(funcList[index]), funcPtr[index], get_funcArgs(funcList[index]).split(','));

			QString msg = "Call " + funcName;
			for (const auto &resp: msgList)
			{
				msg += " msgList=" + resp;
			}
			ui->result->append(msg);
		});
		QObject::connect(newPB, &MyButton::rightClicked, [&](const QString &funcName) {
			size_t index = funcMap[funcName];
			func_display2table(ui->param_inputTable, new QTableWidgetItem(funcName), get_funcArgs(funcList[index]).split(','), {});

//			ui->input->appendPlainText(get_funcArgs(funcList[index]));
		});
	}
}

std::string formatJson(wchar_t *json)
{
	int indent = 0;

	std::string formattedJson;

	bool inQuote = false;

	for (int i = 0; json[i]; i++)
	{
		if (json[i] == '"')
		{
			formattedJson += json[i];
			inQuote = !inQuote;
			continue;
		}
		if (inQuote)                // ""内的内容不做处理
		{
			formattedJson += json[i];
			continue;
		}


		if (json[i] == ' ' || json[i] == '\n' || json[i] == '\r' || json[i] == '\t')
		{
			continue;
		}
		else if (json[i] == '{' || json[i] == '[')
		{
			if (json[i + 1] == '}')
			{
				formattedJson += "{}";
				i++;
				continue;
			}
			formattedJson += json[i];
			formattedJson += "\r\n";
			indent++;
			for (int j = 0; j < indent; j++)
			{
				formattedJson += "\t";
			}
		}
		else if (json[i] == '}' || json[i] == ']')
		{
			formattedJson += "\r\n";
			indent--;
			for (int j = 0; j < indent; j++)
			{
				formattedJson += "\t";
			}
			formattedJson += json[i];
		}
		else if (json[i] == ',')
		{
			formattedJson += json[i];
			formattedJson += "\r\n";
			for (int j = 0; j < indent; j++)
			{
				formattedJson += "\t";
			}
		}
		else
		{
			formattedJson += json[i];
		}
	}

	return formattedJson;
}


void MainWindow::on_pB_Test1_clicked()
{
	QJsonObject configs = readJsonFile1(ui->ConfigPath->text());
	if (configs.empty())
	{
		ui->result->setText("配置文件错误");
		return;
	}
//	ui->result->append(printJson(configs));
	for (const auto &key: configs.keys())
	{
//		ui->result->append(key + ":" + printJson(configs[key]));
	}

	for (QJsonObject::const_iterator iter = configs.begin(); iter != configs.end(); ++iter)
	{
		ui->result->append(iter.key() + ": " + printJson(iter.value().toObject()));
	}

}

void MainWindow::on_pB_Test2_clicked()
{
	QStringList func_params;
	func_params << "param 1" << "param 2" << "param 3" << "param 4";

	QStringList func_args;
	func_args << "arg 1" << "arg 2" << "arg 3" << "arg 4";

	func_display2table(ui->param_inputTable, new QTableWidgetItem("func_name"), func_params, func_args);
}

void MainWindow::on_pB_Test3_clicked()
{
	lib = new DynamicLib(ui->LibraryPath->text().toStdString());

	QVariantMap configs = readJsonFile(ui->ConfigPath->text());
	if (configs.empty())
	{
		ui->result->setText("配置文件错误");
		return;
	}
	ui->result->append(printJson(configs));
	for (const auto &key: configs.keys())
	{
		ui->result->append(key + ":" + printJson(configs[key].toJsonObject().toVariantMap()));
	}

}


