#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qtreewidget.h"
#include "ui_mainwindow.h"
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
public:
    MainWindow *pMainWindow;
};

class cInterface : public QObject, QNetworkInterface {
public:
    cTester * pParent;
    QTreeWidget * pTreeWidget;
    cInterface(const QNetworkInterface& i, cTester* par);
    void getParams();

    QTreeWidgetItem *pTopItem;
    static QList<cInterface *>  interfaces;
    QList<QNetworkAddressEntry> addresses;
    QList<QTreeWidgetItem *>    addressItems;
};

#endif // MAINWINDOW_H
