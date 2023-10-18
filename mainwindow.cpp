#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "funcData_Button.h"

#include <vector>

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

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
	ui->splitter->setStretchFactor(0, 30);
	ui->splitter->setStretchFactor(1, 70);

	flowLayout = new FlowLayout();
	ui->main_vLayout->insertLayout(1, flowLayout);
	ui->main_vLayout->setStretch(1, 1);
}

MainWindow::~MainWindow()
{
	delete ui;
	if (lib)
	{
		delete lib;
		lib = nullptr;
	}

	for (int i{flowLayout->count()}; i > 0 ; i--)
	{
		auto *widget = flowLayout->itemAt(0)->widget();

		flowLayout->removeWidget(widget);
		widget->hide();
		delete widget;
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

		auto *newPB = new funcData_Button(key, configs[key], this);
		newPB->autoResize();
		if (!newPB->func_loadLib(lib))
		{
			QString errMsg = "load function " + newPB->text() + " fail. errMsg=" + lib->getErrorMsg().c_str();
			ui->result->append(errMsg);
			continue;
		}
		flowLayout->addWidget(newPB);

		QObject::connect(newPB, &funcData_Button::leftClicked, [&](func_Data *func) {
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
		QObject::connect(newPB, &funcData_Button::rightClicked, [&](const func_Data *func) {
			func->display2table(ui->param_inputTable);
		});
	}

}

void MainWindow::on_Close_triggered()
{
	ui->param_inputTable->clear();
	ui->param_inputTable->setRowCount(0);
	ui->param_inputTable->setColumnCount(0);

	ui->result->clear();
	if (lib)
	{
		delete lib;
		lib = nullptr;
	}

	for (int i{flowLayout->count()}; i > 0 ; i--)
	{
		auto *button = qobject_cast<funcData_Button *>(flowLayout->itemAt(0)->widget());

		flowLayout->removeWidget(button);
		button->hide();
		delete button;
	}

	ui->pB_LOAD->show();
}

void MainWindow::on_pB_Test1_clicked()
{
	static int var{1};
	auto *newPB = new funcData_Button(QString::number(var++), this);
	newPB->autoResize();
	flowLayout->addWidget(newPB);
	qDebug() << "add " << newPB;
}

void MainWindow::on_pB_Test2_clicked()
{
	for (int i{flowLayout->count()}; i > 0 ; i--)
	{
		auto *widget = flowLayout->itemAt(0)->widget();

		qDebug() << "remove " << widget;
		flowLayout->removeWidget(widget);
		widget->hide();
		delete widget;
	}
}

void MainWindow::on_pB_Test3_clicked()
{

}
