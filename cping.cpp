#include "cping.h"

cPing::cPing(QTreeWidgetItem *_i, MainWindow *pMainWindow, QObject *_par)
    : QProcess(_par)
{
    pTreeItem = _i;
    connect(this, SIGNAL(errorOccurred(QProcess::ProcessError)), pMainWindow, SLOT(onError(QProcess::ProcessError)));
    connect(this, SIGNAL(finished(int,QProcess::ExitStatus)), pMainWindow, SLOT(onFinished(int,QProcess::ExitStatus)));
    QStringList args;
    args << "-c" << "2" << pTreeItem->text(0);
    start("ping", args);
}
