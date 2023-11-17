#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTextBrowser>
#include <QMenu>
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

	void setLibrary(const QString &);
	void setConfig(const QString &);

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

	static void showContextMenu(const QPoint &pos, QTextBrowser *textBrowser)
	{
		QMenu menu;
		// 添加默认的上下文菜单动作
		menu.addActions(textBrowser->createStandardContextMenu()->actions());

		// 添加自定义的清空按钮
		auto *clearAction = new QAction("清空", &menu);
		connect(clearAction, &QAction::triggered, [textBrowser]() {
			textBrowser->clear();
		});
		menu.addAction(clearAction);

		menu.exec(textBrowser->mapToGlobal(pos));
	}

private:
	Ui::MainWindow *ui;
	DynamicLib *lib{};
	FlowLayout *flowLayout;
};

#endif // MAINWINDOW_H
