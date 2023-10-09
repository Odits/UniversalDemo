#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <map>
#include <vector>
#include <QTextStream>
#include <QSettings>
#include <QDebug>

#include "FuncPtr.h"
#include "DynamicLib.h"
#include "MyButton.h"

std::vector<std::tuple<QString, QString, QString>> funcList;
DynamicLib *lib;
std::vector<void*> funcPtr;
std::map<QString, size_t> funcMap;

#define get_funcDef(func) \
    std::get<0>(func)

#define get_funcName(func) \
    std::get<1>(func)

#define get_funcArgs(func) \
    std::get<2>(func)


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->Close, &QAction::triggered, [&](){
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
                auto* button = qobject_cast<MyButton*>(ui->gridLayout->itemAt(i-1)->widget());

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
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>

QVariantMap readJsonFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    if (document.isNull() || !document.isObject()) {
        return {};
    }

    return document.object().toVariantMap();
}

QJsonObject readJsonFile1(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return {};
	}

	QByteArray jsonData = file.readAll();
	file.close();

	QJsonDocument document = QJsonDocument::fromJson(jsonData);
	if (document.isNull() || !document.isObject()) {
		return {};
	}

	return document.object();
}

QString printJson(const QVariantMap& map)
{
    QJsonObject object = QJsonObject::fromVariantMap(map);
    QJsonDocument document(object);
    return {document.toJson(QJsonDocument::Indented)};
}

QString printJson(const QJsonObject& object)
{
	QJsonDocument document(object);
	return {document.toJson(QJsonDocument::Indented)};
}

QString printJson(const QJsonValueRef& ref)
{
	QJsonDocument document(ref.toObject());
	return {document.toJson(QJsonDocument::Indented)};
}

void MainWindow::on_pB_LOAD_clicked()
{
    lib = new DynamicLib(ui->LibraryPath->text().toStdString());

//    QVariantMap configs = readJsonFile(ui->ConfigPath->text());
//    if (configs.empty())
//    {
//        ui->result->setText("配置文件错误");
//        return;
//    }
//    for (auto &config : configs)
//    {

//    }

    QSettings settings(ui->ConfigPath->text(), QSettings::IniFormat);

    QStringList keys = settings.allKeys();
    for(const QString &key : keys)
    {
        QVariant value = settings.value(key);
        ui->result->append(key+":"+value.toString());
        auto funcDef = key.split('/');
        funcList.emplace_back(funcDef[0], funcDef[1], value.toString());
    }


    ui->pB_LOAD->hide();

    for (int i{}; i < funcList.size(); i++)
    {
        void* func = lib->loadFunc<void*>(get_funcName(funcList[i]).toStdString().c_str());
        funcPtr.push_back(func);
        if(!func)
        {
            QString errMsg = "load function " + get_funcName(funcList[i]) + " fail. errMsg=" + lib->getErrorMsg().c_str();
            ui->result->append(errMsg);
            continue;
        }

        funcMap.insert(std::pair<QString, size_t>(get_funcName(funcList[i]), i));


        auto *newPB = new MyButton(get_funcName(funcList[i]), this);

        int textWidth = newPB->getTextWidth();
        newPB->setMaximum_Width_Height(textWidth + 20, 21);

        ui->gridLayout->addWidget(newPB, 0, i);


        QObject::connect(newPB, &MyButton::leftClicked, [&](const QString& funcName){
            size_t index = funcMap[funcName];
            QStringList msgList = callFunc(get_funcDef(funcList[index]), funcPtr[index], get_funcArgs(funcList[index]).split(','));

            QString msg = "Call " + funcName;
            for (const auto& resp : msgList)
            {
                msg += " msgList=" + resp;
            }
            ui->result->append(msg);
        });
        QObject::connect(newPB, &MyButton::rightClicked, [&](const QString& funcName){
            size_t index = funcMap[funcName];
//            ui->input->appendPlainText(get_funcArgs(funcList[index]));
        });
    }
}

#include <QFileDialog>
#include <QMessageBox>
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


std::string formatJson(wchar_t *json)
{
	int indent = 0;

	std::string formattedJson;

	bool inQuote = false;

	for (int i = 0; json[i]; i++) {
		if (json[i] == '"')
		{
			formattedJson += json[i];
			inQuote = !inQuote;
			continue;
		}
		if (inQuote)				// ""内的内容不做处理
		{
			formattedJson += json[i];
			continue;
		}


		if (json[i] == ' ' || json[i] == '\n' || json[i] == '\r' || json[i] == '\t') {
			continue;
		}
		else if (json[i] == '{' || json[i] == '[') {
			if (json[i + 1] == '}')
			{
				formattedJson += "{}";
				i++;
				continue;
			}
			formattedJson += json[i];
			formattedJson += "\r\n";
			indent++;
			for (int j = 0; j < indent; j++) {
				formattedJson += "\t";
			}
		}
		else if (json[i] == '}' || json[i] == ']') {
			formattedJson += "\r\n";
			indent--;
			for (int j = 0; j < indent; j++) {
				formattedJson += "\t";
			}
			formattedJson += json[i];
		}
		else if (json[i] == ',') {
			formattedJson += json[i];
			formattedJson += "\r\n";
			for (int j = 0; j < indent; j++) {
				formattedJson += "\t";
			}
		}
		else {
			formattedJson += json[i];
		}
	}

	return formattedJson;
}


void JSON_parsing(const QString &func_declare, const QJsonValueRef &params)
{


}


void MainWindow::on_pB_Test1_clicked()
{
	QJsonObject configs = readJsonFile1(ui->ConfigPath->text());
	if (configs.empty())
	{
		ui->result->setText("配置文件错误");
		return;
	}
	ui->result->append(printJson(configs));
	for (const auto& key : configs.keys())
	{
		ui->result->append(key + ":" + printJson(configs[key]));
	}
	for (auto config : configs)
	{

	}


}

void test(QTableWidget *func)
{
    func->clear();

    func->setRowCount(5);
    func->setColumnCount(1);

    QTableWidgetItem *func_name = new QTableWidgetItem("func_name");
    func->setHorizontalHeaderItem(0, func_name);

    QStringList func_params;
    func_params << "args 1" << "args 2" << "args 3" << "args 4";

    func->setVerticalHeaderLabels(func_params);

    return;
}


void MainWindow::on_pB_Test2_clicked()
{
    test(ui->param_inputTable);
}

void MainWindow::on_pB_Test3_clicked()
{
    lib = new DynamicLib(ui->LibraryPath->text().toStdString());

	//
    QVariantMap configs = readJsonFile(ui->ConfigPath->text());
    if (configs.empty())
    {
        ui->result->setText("配置文件错误");
        return;
    }
    ui->result->append(printJson(configs));
	for (const auto& key : configs.keys())
	{
		ui->result->append(key + ":" + printJson(configs[key].toJsonObject().toVariantMap()));
	}

}


