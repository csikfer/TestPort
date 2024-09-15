#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qtreewidget.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QThread>
#include <QtNetwork/QtNetwork>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class cTester;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

// private:
    Ui::MainWindow *ui;
    cTester * pTester;
    QThread * pThread;
    void initTester();
    void onSlot(const QStringList& r, QObject* pSender);
public slots:
    void onError(QProcess::ProcessError e);
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

class cTester : public QObject {
    Q_OBJECT
public:
    cTester(MainWindow * _mw);
public:
    MainWindow *pMainWindow;
    QEventLoop *pLoop;
    bool    DNSIsKnown;
public slots:
    void process();
signals:
    void ready();
};

class cInterface : public QObject, QNetworkInterface {
public:
    cTester * pParent;
    QThread * pThread;
    QTreeWidget * pTreeWidget;
    cInterface(const QNetworkInterface& i, cTester* par);
    void getParams();

    QTreeWidgetItem *pTopItem;
    static QList<cInterface *>  interfaces;
    QList<QNetworkAddressEntry> addresses;
    QList<QTreeWidgetItem *>    addressItems;
};

#endif // MAINWINDOW_H
