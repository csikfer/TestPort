
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "proc_fs.h"
#include "cnmcli.h"
#include "cping.h"

static QString IfaceType2str(QNetworkInterface::InterfaceType _t)
{
    switch (_t) {
        case QNetworkInterface::Unknown:   return QObject::tr("Ismeretlen");
        case QNetworkInterface::Loopback:  return QObject::tr("loopback");
        case QNetworkInterface::Virtual:   return QObject::tr("virtual");
        case QNetworkInterface::Ethernet:  return QObject::tr("Ethernet");
        case QNetworkInterface::Wifi:      return QObject::tr("Wi-Fi");
        case QNetworkInterface::CanBus:    return QObject::tr("CanBus");
        case QNetworkInterface::Fddi:      return QObject::tr("Fddi");
        case QNetworkInterface::Ppp:       return QObject::tr("Ppp");
        case QNetworkInterface::Slip:      return QObject::tr("Slip");
        case QNetworkInterface::Phonet:    return QObject::tr("Phonet");
        case QNetworkInterface::Ieee802154:return QObject::tr("IEEE 802.15.4");
        case QNetworkInterface::SixLoWPAN: return QObject::tr("LoWPAN");
        case QNetworkInterface::Ieee80216: return QObject::tr("WiMAX");
        case QNetworkInterface::Ieee1394:  return QObject::tr("IEEE 1394 (FireWire)");
    }
    return QString("?");
}

static QString& strAppend(QString& s, const QString& a, const QString& sep = QString(", ")) {
    if (!s.isEmpty()) s += sep;
    s += a;
    return s;
}

static QString IfaceState2str(QNetworkInterface::InterfaceFlags _f)
{
    QFlags f(_f);
    QString r;
    if (f.testFlag(QNetworkInterface::IsUp))                r += QObject::tr("up");
    if (f.testFlag(QNetworkInterface::IsRunning))           strAppend(r, QObject::tr("running"));
    if (f.testFlag(QNetworkInterface::CanBroadcast))        strAppend(r, QObject::tr("broadcast mode"));
    if (f.testFlag(QNetworkInterface::IsLoopBack))          strAppend(r, QObject::tr("loopback"));
    if (f.testFlag(QNetworkInterface::IsPointToPoint))      strAppend(r, QObject::tr("point-to-point"));
    if (f.testFlag(QNetworkInterface::CanMulticast))        strAppend(r, QObject::tr("multicasting"));
    return r;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusbar->showMessage(tr("Inicializálás"));
    ui->treeWidget->setColumnCount(3);
    pTester = nullptr;
    pThread = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTester()
{
    pTester = new cTester(this);
    pThread = new QThread(this);
    pTester->moveToThread(pThread);
    connect(pThread, SIGNAL(started()), pTester, SLOT(process()));
    connect(pTester, SIGNAL(ready()),   pThread, SLOT(quit()));
    connect(pTester, SIGNAL(ready()),   pTester, SLOT(deleteLater()));
    connect(pThread, SIGNAL(finished()),pThread, SLOT(deleteLater()));
    emit pThread->start();
}

void MainWindow::onSlot(const QStringList& r, QObject* pSender)
{
    QStringList row = r;
    QTreeWidgetItem *pTreeItem;
    if (pSender->inherits("cPing")) {
        cPing *pCPing = qobject_cast<cPing *>(pSender);
        pTreeItem = pCPing->pTreeItem;
        row.push_front("PING");
    }
    else {
        return;
    }
    pTreeItem->addChild(new QTreeWidgetItem(row));
    delete pSender;
}

void MainWindow::onError(QProcess::ProcessError e)
{
    QObject *pSender = sender();
    QStringList row;
    row << "Error" << QString::number((int)e);
    onSlot(row, pSender);
}

void MainWindow::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QObject *pSender = sender();
    QStringList row;
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            row << "On";
        }
        else {
            row << "No";
        }
    }
    else {
        row << "Crash";
    }
    onSlot(row, pSender);
}




cTester::cTester(MainWindow *_mw)
    : QObject()
    , pMainWindow(_mw)
{
    ;
}

cTabFromProc routerTable;

void cTester::process()
{
    pLoop = new QEventLoop(this);
    connect(thread(), SIGNAL(finished()), pLoop, SLOT(quit()));
    DNSIsKnown = false;
    pMainWindow->ui->statusbar->showMessage(tr("router tábla"));
    if (!routerTable.load("/proc/net/route")) {
        pMainWindow->ui->statusbar->showMessage(tr("A router tábla beolvasása sikertelen"));
        return;
    }
    pMainWindow->ui->statusbar->showMessage(tr("Inrfész lista"));
    QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for (int ix = 0; ix < ifs.size(); ++ix) {
        cInterface iface(ifs.at(ix), this);
        iface.getParams();
    }
    ;
    pMainWindow->ui->statusbar->showMessage(tr("Kész"));

    pLoop->exec();
}

QList<cInterface *>   cInterface::interfaces;

cInterface::cInterface(const QNetworkInterface& i, cTester* par)
    : QObject(par)
    , QNetworkInterface(i)
{
    pParent = par;
    pTreeWidget = pParent->pMainWindow->ui->treeWidget;
    interfaces << this;
}


void cInterface::getParams() {
    QStringList row;
    QString ifname = humanReadableName();
    row << ifname << IfaceType2str(type()) << IfaceState2str(flags());
    pTopItem = new QTreeWidgetItem(row);
    pTreeWidget->addTopLevelItem(pTopItem);

    row.clear();
    row << "MAC" << hardwareAddress();
    pTopItem->addChild(new QTreeWidgetItem(row));

    row.clear();
    row << "MTU" << QString("%1").arg(maximumTransmissionUnit()) << "Byte";
    pTopItem->addChild(new QTreeWidgetItem(row));

    QList<QNetworkAddressEntry> addresses = addressEntries();
    foreach (auto ip, addresses) {
        QHostAddress a = ip.ip();
        enum QAbstractSocket::NetworkLayerProtocol p = a.protocol();
        QString n;
        switch (p) {
        case QAbstractSocket::IPv4Protocol: n = "IPV4"; break;
        case QAbstractSocket::IPv6Protocol: n = "IPV6"; break;
        default:                            n = "IP ?"; break;
        }

        row.clear();
        row << n << ip.ip().toString() << QString("%1 (%2)").arg(ip.netmask().toString()).arg(ip.prefixLength());
        addresses << ip;
        addressItems << new QTreeWidgetItem(row);
        pTopItem->addChild(addressItems.last());

        if (!ip.broadcast().isNull()) {
            row.clear();
            row << "Broadcast" << ip.broadcast().toString();
            addressItems.last()->addChild(new QTreeWidgetItem(row));
        }

    }
    // Router table
    static QString _Iface       = "Iface";
    int routerTableSize = routerTable.value(_Iface).size();
    if (routerTableSize) {
        QTreeWidgetItem *pRouteItem = nullptr;
        row.clear();
        row << "Route";
        static QString _Destination = "Destination";
        static QString _Gateway     = "Gateway";
        static QString _Metric      = "Metric";
        static QString _Mask        = "Mask";
        for (int i = 0; i < routerTableSize; ++i) {
            if (routerTable[_Iface][i] == ifname) {
                if (pRouteItem == nullptr) {
                    pRouteItem = new QTreeWidgetItem(row);
                    pTopItem->addChild(pRouteItem);
                }
                row.clear();
                row << hex2ip(routerTable[_Gateway][i])
                    << hex2ipm(routerTable[_Destination][i], routerTable[_Mask][i])
                    << routerTable[_Metric][i];
                pRouteItem->addChild(new QTreeWidgetItem(row));
            }
        }
    }
    // DNS
    cNmcli nmcli;
    static const QString _sIp4_dns = "IP4.DNS";
    QMap<QString, QStringList> mapIp4Dns = nmcli.getAndWait(ifname, _sIp4_dns);
    QStringList nameservers = mapIp4Dns[_sIp4_dns];
    if (!nameservers.isEmpty()) {
        pParent->DNSIsKnown = true;
        row.clear();
        row << "DNS";
        QTreeWidgetItem *pDnsItem = new QTreeWidgetItem(row);
        pTopItem->addChild(pDnsItem);
        for (QString& a: nameservers) {
            row.clear();
            row << a;
            QTreeWidgetItem *ptwi = new QTreeWidgetItem(row);
            pDnsItem->addChild(ptwi);
            new cPing(ptwi, pParent->pMainWindow, pParent);
        }
    }

}
