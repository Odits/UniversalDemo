#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_pB_Test1_clicked();

    void on_pB_Test2_clicked();

    void on_pB_Test3_clicked();

    void on_pB_LOAD_clicked();

    void on_LibrarySelector_clicked();

    void on_ConfigSelector_clicked();

private:
    Ui::MainWindow *ui;
	bool isMouseNearWindowEdge(const QPoint &mousePos, int titleBarHeight);

protected:
	int m_iMarginWidth;
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

};
#endif // MAINWINDOW_H
