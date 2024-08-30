#ifndef USBWATCHER_H
#define USBWATCHER_H


#include <memory>
#include <cstdint>
#include <variant>
#include <type_traits>

#include <QThread>
#include <QObject>
#include <QString>

#include "gutils.h"

#include "gprotocol.h"
#include "usbcstatus.h"
#include "comservice.h"
#include "tableworker.h"


enum USBRequestType
{
    USB_REQUEST_NONE = 0,
    USB_REQUEST_LOAD_SETTINGS,
    USB_REQUEST_LOAD_INFO,
    USB_REQUEST_SAVE_SETTINGS
};


class USBWorker : public QObject
{
    Q_OBJECT

public slots:
    void proccess(const USBRequestType type, const QString& port);

signals:
    void resultReady(const USBRequestType type, const USBCStatus status);

private:
    static constexpr char TAG[] = "USBW";

    static TableWorker tableWorker;

};


class USBController : public QObject
{
    Q_OBJECT
    QThread workerThread;

private:
    static constexpr char TAG[] = "USBc";

    USBWorker worker;

    static USBRequestType requestType;

public:
    USBController();
    ~USBController();

    void loadSettings(const QString& port);
    void saveSettings(const QString& port);


public slots:
    void handleResults(const USBRequestType type, const USBCStatus status);

signals:
    void request(const USBRequestType type, const QString& port);
    void responseReady(const USBRequestType type, const USBCStatus status);
    void error(const QString& message);

};


#endif // USBWATCHER_H
