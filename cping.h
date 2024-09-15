#ifndef CPING_H
#define CPING_H

#include "mainwindow.h"

class cPing : public QProcess
{
    Q_OBJECT
public:
    cPing(QTreeWidgetItem *_i,MainWindow *mainWindow,  QObject *_par = nullptr);
    QTreeWidgetItem *pTreeItem;
};

#endif // CPING_H
