#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "funcData_Button.h"

#include <vector>

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

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
	ui->splitter->setStretchFactor(0, 40);
	ui->splitter->setStretchFactor(1, 60);

	flowLayout = new FlowLayout();
	ui->main_vLayout->insertLayout(1, flowLayout);
	ui->main_vLayout->setStretch(1, 1);

	ui->oeUTF8->setChecked(true);
	ui->reUTF8->setChecked(true);
}

void MainWindow::setLibrary(const QString& path)
{
	ui->LibraryPath->setText(path);
}

void MainWindow::setConfig(const QString& path)
{
	ui->ConfigPath->setText(path);
}

MainWindow::~MainWindow()
{
	delete ui;
	if (lib)
	{
		delete lib;
		lib = nullptr;
	}

	for (int i{flowLayout->count()}; i > 0; i--)
	{
		auto *widget = flowLayout->itemAt(0)->widget();

		flowLayout->removeWidget(widget);
		widget->hide();
		delete widget;
	}
}

void MainWindow::on_oeUTF8_triggered()
{
	ui->oeGBK->setChecked(false);
}

void MainWindow::on_oeGBK_triggered()
{
	ui->oeUTF8->setChecked(false);
}

void MainWindow::on_reUTF8_triggered()
{
	ui->reGBK->setChecked(false);
}

void MainWindow::on_reGBK_triggered()
{
	ui->reUTF8->setChecked(false);
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

void removeComment(QByteArray &str)
{
	QByteArray out;

	bool line_comment{false};
	bool block_comment{false};
	for (int i = 0; i < str.size(); i++)    // 先判断是否在注释中，尽量早点退出注释，负责再判断注释
	{
		if (block_comment)
		{
			if (str[i] == '*' && str[i + 1] == '/')
			{
				block_comment = false;
				i++;
			}
			continue;    // 在注释中，跳过
		}
		if (line_comment)
		{
			if (str[i] == '\n')
			{
				line_comment = false;
			}
			continue;    // 在注释中，跳过
		}

		if (str[i] == '/' && str[i + 1] == '/')
		{
			line_comment = true;
			i++;
			continue;
		}
		if (str[i] == '/' && str[i + 1] == '*')
		{
			block_comment = true;
			i++;
			continue;
		}

		if (str[i] != '\n' && str[i] != '\t')
			out.append(str[i]);
	}
	str.clear();
	str = out;
}

QList<QByteArray> getExternFuncs(const QByteArray &str)
{
	int start{}, end{};

	for (int i = str.indexOf("extern") + 6; i < str.size(); i++)
	{
		if (start == 0)
		{
			if (str[i] == ' ')
			{
				continue;
			}
			else if (str[i] == '"' && str[i + 1] == 'C' && str[i + 2] == '"')
			{
				start = i + 3;
			}
			else
				return {};        // error
		}
		else
		{
			static int inside{0};
			if (str[i] == '{')
			{
				if (inside == 0)
					start = i + 1;
				else
					inside++;
			}
			else if (str[i] == '}')
			{
				if (inside > 0)
					inside--;
				else
					end = i - 1;
			}
		}
	}
	return str.mid(start, end - start).split(';');
}

QJsonObject header2Json(const QString &filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return {};
	}

	QByteArray headStr = file.readAll();
	file.close();
	removeComment(headStr);
	auto funcs = getExternFuncs(headStr);

	QJsonObject jsonObject;
	QJsonObject innerObject;
	innerObject["args"] = QJsonArray(); // 创建 {"args": []}

	// 使用 keyList 作为键
	for (const QByteArray &func : funcs)
	{
		jsonObject[func] = innerObject;
	}

	QJsonDocument document(jsonObject);
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

void MainWindow::on_SelectLibrary_triggered()
{
	on_LibrarySelector_clicked();
}


void MainWindow::on_ConfigSelector_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(
			this,
			tr("open a Config."),
			"./",
			tr("Config(*.json);;Header(*.h);;All files(*.*)"));

	if (fileName.isEmpty())
	{
		QMessageBox::warning(this, "Warning!", "Failed to open the Config!");
	}
	else
	{
		ui->ConfigPath->setText(fileName);
	}
}

void MainWindow::on_SelectConfig_triggered()
{
	on_ConfigSelector_clicked();
}


void MainWindow::on_pB_LOAD_clicked()
{
	lib = new DynamicLib(ui->LibraryPath->text().toStdString());

	QJsonObject configs;
	auto configPath = ui->ConfigPath->text();
	if (QFileInfo(configPath).suffix() == "h")
		configs = header2Json(configPath);
	else
		configs = readJsonFile(configPath);
	if (configs.empty())
	{
		ui->result->setText("配置文件错误");
		return;
	}
	ui->pB_LOAD->hide();

//	ui->result->append(printJson(configs));
	for (const auto &key : configs.keys())
	{
//		ui->result->append(key + ":" + printJson(configs[key]));

		auto *newPB = new funcData_Button(key, configs[key], this);
		newPB->autoResize();
		flowLayout->addWidget(newPB);

		if (!check_funcList(newPB->func_data()->getTypeRef()))
		{
			QString errMsg = "没有实现的函数指针类型：" + newPB->func_data()->getTypeRef();
			ui->result->append(errMsg);
			newPB->setDisabled(true);
			continue;
		}
		if (!newPB->func_loadLib(lib))
		{
			QString errMsg = "load function " + newPB->text() + " fail. errMsg=" + lib->getErrorMsg().c_str();
			ui->result->append(errMsg);
			newPB->setDisabled(true);
			continue;
		}

		QObject::connect(newPB, &funcData_Button::leftClicked, [&](func_Data *func) {
			QString msg = "Call " + func->getName();
			func->loadArgs(ui->param_inputTable);
			try
			{
				msg += func->call(ui->oeGBK->isChecked());
			} catch (std::exception &e)
			{
				msg += " fail: ";
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

	for (int i{flowLayout->count()}; i > 0; i--)
	{
		auto *button = qobject_cast<funcData_Button *>(flowLayout->itemAt(0)->widget());

		flowLayout->removeWidget(button);
		button->hide();
		delete button;
	}

	ui->pB_LOAD->show();
}


void MainWindow::on_Save_triggered()
{
	QFileInfo fileInfo{ui->ConfigPath->text()};
	if (fileInfo.suffix() != "json")
	{
		ui->result->append("当前配置文件不是json，请选择其它保存路径");
		return on_SaveAs_triggered();
	}

	QString fileName = fileInfo.fileName();
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return;
	}

	QJsonObject jsonObject;

	for (int i{0}; i < flowLayout->count(); i++)
	{
		auto *button = qobject_cast<funcData_Button *>(flowLayout->itemAt(i)->widget());
		auto buttonObj = button->getJsonObj();
		jsonObject[std::get<0>(buttonObj)] = std::get<1>(buttonObj);
	}

	QByteArray data{QJsonDocument(jsonObject).toJson()};
	file.write(data);
	file.close();

	ui->result->append(fileName + "已保存");
}

void MainWindow::on_SaveAs_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(
			this,
			tr("save as Config."),
			"./",
			tr("Config(*.json);;All files(*.*)"));

	if (fileName.isEmpty())
	{
		QMessageBox::warning(this, "Warning!", "Failed to save this file!");
		return;
	}

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return;
	}

	QJsonObject jsonObject;

	for (int i{0}; i < flowLayout->count(); i++)
	{
		auto *button = qobject_cast<funcData_Button *>(flowLayout->itemAt(i)->widget());
		auto buttonObj = button->getJsonObj();
		jsonObject[std::get<0>(buttonObj)] = std::get<1>(buttonObj);
	}

	QByteArray data{QJsonDocument(jsonObject).toJson()};
	file.write(data);
	file.close();

	ui->ConfigPath->setText(fileName);
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
	for (int i{flowLayout->count()}; i > 0; i--)
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
#if 1
	QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
	auto def_path{tempDir + "/def.cpp"};

	QFile def_file(def_path);
	if (!def_file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		ui->result->append("fail, def_path=" + def_path);
		return;
	}

	QString tmp_map;

	for (int i{0}; i < flowLayout->count(); i++)
	{
		auto *button = qobject_cast<funcData_Button *>(flowLayout->itemAt(i)->widget());
//		if (!button->isEnabled())
		{
			def_file.write(button->getDefParsing().toLocal8Bit());

			auto typeRef = button->func_data()->getTypeRef();
			tmp_map += "{\"" + typeRef + "\", " + typeRef + "_},\n\t";
		}
	}
	def_file.write(tmp_map.toLocal8Bit());
	def_file.close();
	ui->result->append("success, def_path=" + def_path);

#else
	for (int i{0}; i < flowLayout->count(); i++)
	{
		auto *button = qobject_cast<funcData_Button *>(flowLayout->itemAt(i)->widget());
		button->isEnabled();
		ui->result->append(button->func_data()->getTypeRef());
	}

#endif

};


