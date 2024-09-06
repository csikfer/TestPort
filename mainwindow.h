#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QtNetwork/QtNetwork>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

// private:
    Ui::MainWindow *ui;
};

class cTester : public QThread {
    Q_OBJECT
public:
    cTester(MainWindow * _mw);
private:
    void run() override;
    MainWindow *pMainWindow;
};


#endif // MAINWINDOW_H
