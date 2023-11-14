#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include "DynamicLib.h"
#include "flowlayout.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);

	void setLibrary(const QString&);
	void setConfig(const QString&);

	~MainWindow() override;

private slots:

	void on_pB_Test1_clicked();

	void on_pB_Test2_clicked();

	void on_pB_Test3_clicked();

	void on_pB_LOAD_clicked();

	void on_LibrarySelector_clicked();

	void on_ConfigSelector_clicked();

	void on_Close_triggered();

    void on_SelectLibrary_triggered();

    void on_Save_triggered();

    void on_SaveAs_triggered();

    void on_SelectConfig_triggered();

    void on_oeUTF8_triggered();

    void on_oeGBK_triggered();

    void on_reUTF8_triggered();

    void on_reGBK_triggered();

private:
	Ui::MainWindow *ui;
	DynamicLib *lib{};
	FlowLayout *flowLayout;
};

#endif // MAINWINDOW_H
