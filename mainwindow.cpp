#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <map>
#include <string>
#include <vector>
#include <QPlainTextEdit>
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
            ui->input->clear();
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
                MyButton* button = qobject_cast<MyButton*>(ui->gridLayout->itemAt(i-1)->widget());

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
        return QVariantMap();
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    if (document.isNull() || !document.isObject()) {
        return QVariantMap();
    }

    return document.object().toVariantMap();
}
#include <QJsonDocument>
#include <QJsonObject>

QString printJson(const QVariantMap& map)
{
    QJsonObject object = QJsonObject::fromVariantMap(map);
    QJsonDocument document(object);
    return QString(document.toJson(QJsonDocument::Indented));
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
        funcList.push_back(std::make_tuple(funcDef[0], funcDef[1], value.toString()));
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


        MyButton *newPB = new MyButton(get_funcName(funcList[i]), this);

        int textWidth = newPB->getTextWidth();
        newPB->setMaximum_Width_Height(textWidth + 20, 21);

        ui->gridLayout->addWidget(newPB, 0, i);


        QObject::connect(newPB, &MyButton::leftClicked, [&](QString funcName){
            size_t index = funcMap[funcName];
            QStringList msgList = callFunc(get_funcDef(funcList[index]), funcPtr[index], get_funcArgs(funcList[index]).split(','));

            QString msg = "Call " + funcName;
            for (auto resp : msgList)
            {
                msg += " msgList=" + resp;
            }
            ui->result->append(msg);
        });
        QObject::connect(newPB, &MyButton::rightClicked, [&](QString funcName){
            size_t index = funcMap[funcName];
            ui->input->appendPlainText(get_funcArgs(funcList[index]));
        });
    }
}

#include <QFileDialog>
#include <QMessageBox>
void MainWindow::on_LibrartSeleter_clicked()
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

void MainWindow::on_ConfigSeleter_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("open a Config."),
        "./",
        tr("Config(*.ini);;All files(*.*)"));

    if (fileName.isEmpty())
    {
        QMessageBox::warning(this, "Warning!", "Failed to open the Config!");
    }
    else
    {
        ui->ConfigPath->setText(fileName);
    }
}


void MainWindow::on_pB_Test1_clicked()
{
    ui->result->append("1234556");

//    QStringList def = funcList[0].split('/');
//    QStringList args = argsList[0].split(',');

//    DynamicLib lib("./libTestLiber.dll");
//    void* func = lib.loadFunc<void*>(def[1].toStdString().c_str());
//    callFunc(def[0], func, args);
}


void MainWindow::on_pB_Test2_clicked()
{
    DynamicLib lib("./libTestLiber.dll");
    i_F_i_i func = lib.loadFunc<i_F_i_i>("testFunc1");
    if (func)
        func(2, 3);
    else
        ui->input->appendPlainText(lib.getErrorMsg().c_str());
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
    for (auto &config : configs)
    {
        ui->result->append("config:");
        ui->result->append(config.toString());
    }
}


