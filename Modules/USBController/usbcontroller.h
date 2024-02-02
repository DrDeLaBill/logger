#ifndef USBWATCHER_H
#define USBWATCHER_H


#include <QThread>
#include <QObject>
#include <QString>

#include <libusb.h>

#include <memory>


class USBWorker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString &parameter);

signals:
    void resultReady(const QString &result);

};


class USBController : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    USBController();
    ~USBController();

    void search();

public slots:
    void handleResults(const QString &parameter);

signals:
    void operate(const QString &parameter);

};


#endif // USBWATCHER_H
