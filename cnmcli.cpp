#include "cnmcli.h"

#include <QRegularExpression>

const QString cNmcli::cmd = "nmcli";

cNmcli::cNmcli(QObject *_p) : QProcess(_p)
{

}

QMap<QString, QStringList> cNmcli::getAndWait(const QString& _devName, const QString& _fieldName)
{
    reset();
    QMap<QString, QStringList> r;
    QStringList args;
    if (!_fieldName.isEmpty()) {
        args << "-f" << _fieldName;
    }
    args << "device" << "show" << _devName;
    start(cmd, args, QIODevice::ReadOnly);
    if (!waitForStarted()) return r;
    if (!waitForFinished()) return r;
    while (!atEnd()) {
        static const QRegularExpression res("([\\w\\.]+):\\s+(.+)");
        static const QRegularExpression rea("([\\w\\.]+)\\[(\\d+)\\]:\\s+(.+)");
        QString line = QString::fromUtf8(readLine());
        QRegularExpressionMatch m;
        m = res.match(line);
        if (m.hasMatch()) {     // Egy skaláris érték
            r[m.captured(1)] = QStringList(m.captured(2));
            continue;
        }
        m = rea.match(line);
        if (m.hasMatch()) {     // Egy tömb elem
            r[m.captured(1)] << m.captured(3);
            continue;
        }
    }
    return r;
}
