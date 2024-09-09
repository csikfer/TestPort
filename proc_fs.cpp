#include "proc_fs.h"
#include <QRegularExpression>

bool cTabFromProc::load(const QString& _fn)
{
    clear();
    QFile f(_fn);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QString line = QString::fromUtf8(f.readLine());

    static const QRegularExpression sep("\\s+");
    QStringList heads = line.split(sep, Qt::KeepEmptyParts);
    while (!f.atEnd()) {
        line = QString::fromUtf8(f.readLine());
        QStringList fields = line.split(sep);
        if (fields.isEmpty()) continue;
        for (int i = 0; i < heads.size(); ++i) {
            QString hn = heads[i];
            QString fl = (fields.size() > i) ? fields[i] : QString();
            (*this)[hn] << fl;
        }
    }
    return true;
}

QString hex2ip(const QString& _h)
{
    int d1 = _h.mid(6,2).toInt(nullptr, 16);
    int d2 = _h.mid(4,2).toInt(nullptr, 16);
    int d3 = _h.mid(2,2).toInt(nullptr, 16);
    int d4 = _h.mid(0,2).toInt(nullptr, 16);
    return QString("%1.%2.%3.%4").arg(d1).arg(d2).arg(d3).arg(d4);
}

QString hex2ipm(const QString& _hi, const QString& _hm) {
    QString r;
    r  = hex2ip(_hi);
    r += QString("/");
    r += hex2ip(_hm);
    return r;
}
