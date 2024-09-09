#ifndef PROC_FS_H
#define PROC_FS_H

#include <QMap>
#include <QFile>

class cTabFromProc : public QMap<QString, QList<QString> > {
public:
    cTabFromProc() : QMap<QString, QList<QString> >() {;}
    bool load(const QString& _fn);
};

QString hex2ip(const QString& _h);

QString hex2ipm(const QString& _hi, const QString& _hm);

#endif // PROC_FS_H
