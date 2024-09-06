#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusbar->showMessage(tr("Inicializálás"));
    ui->treeWidget->setColumnCount(2);
    cTester *tester = new cTester(this);

    tester->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}


cTester::cTester(MainWindow *_mw) : QThread(), pMainWindow(_mw) {
    ;
}

void cTester::run()
{
    pMainWindow->ui->statusbar->showMessage(tr("Ethernet inrfész lista"));
    QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for (int ix = 0; ix < ifs.size(); ++ix) {
        const QNetworkInterface& i = ifs.at(ix);
        if (i.type() != QNetworkInterface::Ethernet) {
            ifs. removeAt(ix);
            --ix;
        }
        QStringList row;
        row << i.humanReadableName();
        QString stat = (i.flags() & QNetworkInterface::IsUp) ? "Up" : "Dowm";
        if (i.flags() & QNetworkInterface::IsRunning) stat += " Runing";
        row << stat;

        pMainWindow->ui->treeWidget->insertTopLevelItem(ix, new QTreeWidgetItem(row));
    }
    ;
}
