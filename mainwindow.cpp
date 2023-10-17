#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <map>
#include <utility>
#include <vector>

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QTableWidget>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QListWidget>

//#include "FuncPtr.h"
#include "MyButton.h"

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
	flowLayout = new FlowLayout();
	ui->funcButton_gLayout->addLayout(flowLayout, 0, 0);

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


QJsonObject readJsonFile(const QString &filePath)
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

	QJsonObject configs = readJsonFile(ui->ConfigPath->text());
	if (configs.empty())
	{
		ui->result->setText("配置文件错误");
		return;
	}
	ui->pB_LOAD->hide();

//	ui->result->append(printJson(configs));
	for (const auto &key: configs.keys())
	{
//		ui->result->append(key + ":" + printJson(configs[key]));

		auto *newPB = new MyButton(key, configs[key], this);
		newPB->autoResize();
		if (!newPB->func_loadLib(lib))
		{
			QString errMsg = "load function " + newPB->text() + " fail. errMsg=" + lib->getErrorMsg().c_str();
			ui->result->append(errMsg);
			continue;
		}

//		ui->funcButton_gLayout->addWidget(newPB, 0, i++);
		flowLayout->addWidget(newPB);

		QObject::connect(newPB, &MyButton::leftClicked, [&](func_Data *func) {
			QString msg = "Call " + func->getName();
			try
			{
				func->loadArgs(ui->param_inputTable);
				auto msgList = func->call();
				for (const auto &resp: msgList)
				{
					msg += " msgList=" + resp;
				}
			} catch (std::exception& e)
			{
				msg += " fail";
				msg += e.what();
			}
			ui->result->append(msg);
		});
		QObject::connect(newPB, &MyButton::rightClicked, [&](const func_Data *func) {
			func->display2table(ui->param_inputTable);
		});
	}

}


void MainWindow::on_Close_triggered()
{
	ui->param_inputTable->clear();
	ui->result->clear();
	if (lib)
	{
		delete lib;
		lib = nullptr;
	}

	for (int i{ui->funcButton_gLayout->count()}; i > 1; i--)
	{
		auto *button = qobject_cast<MyButton *>(ui->funcButton_gLayout->itemAt(i - 1)->widget());

		ui->funcButton_gLayout->removeWidget(button);
		button->hide();
		delete button;
	}

	ui->pB_LOAD->show();
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

}

void MainWindow::on_pB_Test2_clicked()
{

}

void MainWindow::on_pB_Test3_clicked()
{

}
