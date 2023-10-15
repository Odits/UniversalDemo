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

    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setFlow(QListView::LeftToRight);
    ui->listWidget->setResizeMode(QListView::Adjust);

	m_iMarginWidth = 5;

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

		for (int i{ui->funcButton_gLayout->count()}; i > 1; i--)
		{
			auto *button = qobject_cast<MyButton *>(ui->funcButton_gLayout->itemAt(i - 1)->widget());

			ui->funcButton_gLayout->removeWidget(button);
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


bool MainWindow::isMouseNearWindowEdge(const QPoint &mousePos, int titleBarHeight)
{
	QRect windowRect = this->geometry().adjusted(-m_iMarginWidth, -m_iMarginWidth + titleBarHeight, m_iMarginWidth, m_iMarginWidth);
	QRect windowRect1 = this->geometry().adjusted(m_iMarginWidth, m_iMarginWidth + titleBarHeight, -m_iMarginWidth, -m_iMarginWidth);

	return windowRect.contains(mousePos) && !windowRect1.contains(mousePos);
}

#ifdef Q_OS_WIN

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	MSG *msg = static_cast<MSG *>(message);

	switch (msg->message)
	{
	case WM_NCHITTEST:
		// 获取鼠标在屏幕上的位置
		POINTS points = MAKEPOINTS(msg->lParam);
		QPoint globalMousePos(points.x, points.y);

		int titleBarHeight = frameGeometry().y() - geometry().y();
		bool isMouseOnEdge = isMouseNearWindowEdge(globalMousePos, titleBarHeight);

		if (isMouseOnEdge)// 鼠标位于窗口边缘附近
		{
			if (msg->wParam == HTBOTTOMRIGHT)// 鼠标左键按下
			{
				static int i{0};
				qDebug() << "MouseOnEdge" << i++;
			}
		}
		break;
	}

	// 调用默认处理
	return false;
}

#else

#include <QCoreApplication>
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	Q_UNUSED(eventType);

	if (eventType == "xcb_generic_event_t")
	{
		xcb_generic_event_t *xEvent = static_cast<xcb_generic_event_t*>(message);
		if (xEvent->response_type == XCB_BUTTON_PRESS)
		{
			xcb_button_press_event_t *buttonPressEvent = reinterpret_cast<xcb_button_press_event_t*>(xEvent);

			// 获取鼠标按钮号码，1表示左键
			if (buttonPressEvent->detail == XCB_BUTTON_INDEX_1)
			{
				// 获取鼠标位置
				QPoint globalMousePos(buttonPressEvent->event_x, buttonPressEvent->event_y);
				int titleBarHeight = frameGeometry().y() - geometry().y();
				bool isMouseOnEdge = isMouseNearWindowEdge(globalMousePos, titleBarHeight);

				if (isMouseOnEdge)
				{
					// 鼠标左键按下，且位于窗口边缘附近
					static int i{0};
					qDebug() << "MouseOnEdge" << i++;
				}
			}
		}
	}

	// 继续处理事件
	return false;
}
#endif


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
	int i{0};
	for (const auto &key: configs.keys())
	{
		ui->result->append(key + ":" + printJson(configs[key]));

		auto *newPB = new MyButton(key, configs[key], this);
		newPB->autoResize();
//		ui->funcButton_gLayout->addWidget(newPB, 0, i++);
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(newPB->sizeHint());
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, newPB);

		QObject::connect(newPB, &MyButton::leftClicked, [&](const func_Data *func) {
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
		QObject::connect(newPB, &MyButton::rightClicked, [&](const func_Data *func) {
//			size_t index = funcMap[funcName];
//			ui->input->appendPlainText(get_funcArgs(funcList[index]));

//			func_display2table(ui->param_inputTable, new QTableWidgetItem(funcName), get_funcArgs(funcList[index]).split(','), {});

			func->display2table(ui->param_inputTable);
		});

	}
//	ui->gridLayout->setAlignment(Qt::AlignLeft);


#if 0
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
	}
#endif
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


