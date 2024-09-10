#ifndef CNMCLI_H
#define CNMCLI_H

#include <QProcess>

class cNmcli : public QProcess
{
public:
    cNmcli(QObject *_p = nullptr);
    QMap<QString, QStringList> getAndWait(const QString& _devName, const QString& _fieldName);
private:
    static const QString cmd;
};

#endif // CNMCLI_H
