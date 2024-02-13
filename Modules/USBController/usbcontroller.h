#ifndef USBWATCHER_H
#define USBWATCHER_H


#include <memory>
#include <cstdint>
#include <variant>
#include <unordered_map>

#include <QThread>
#include <QObject>
#include <QString>

#include <libusb.h>
#include "usbhandler.h"
#include "usbcstatus.h"


#define USB_IDLE_HANDLER         (QString("IDLE_HANDLER"))
#define USB_SEARCH_HANDLER       (QString("SEARCH_HANDLER"))
#define USB_LOAD_RECORD_HANDLER  (QString("LOAD_RECORD_HANDLER"))
#define USB_UPGRADE_HANDLER      (QString("UPGRADE_HANDLER"))
#define USB_UPDATE_HANDLER       (QString("UPDATE_HANDLER"))


class USBWorker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString& status);

signals:
    void resultReady(const USBCStatus& status);

private:
    static constexpr char TAG[] = "USBW";

    using handler_v = std::variant<
        USBIdleHandler,
        USBSearchHandler,
        USBLoadRecordHandler,
        USBUpgradeHandler,
        USBUpdateHandler
    >;
    using handler_t = std::unordered_map<
        QString,
        handler_v,
        USBHandlerHash,
        USBHandlerEqual
    >;

    handler_t handlers = {
        {USB_IDLE_HANDLER,        USBIdleHandler{}},
        {USB_SEARCH_HANDLER,      USBSearchHandler{}}, // TODO: like set_table -> set_tuple in FSM
        {USB_LOAD_RECORD_HANDLER, USBLoadRecordHandler{}},
        {USB_UPGRADE_HANDLER,     USBUpgradeHandler{}},
        {USB_UPDATE_HANDLER,      USBUpdateHandler{}}
    };

};


class USBController : public QObject
{
    Q_OBJECT
    QThread workerThread;

private:
    static constexpr char TAG[] = "USBC";

public:
    USBController();
    ~USBController();

    void proccess(const QString& status = USB_IDLE_HANDLER);

    // TODO: is this needed?
    // Load data from the device
    void update();
    // Load data to the device
    void upgrade();

public slots:
    void handleResults(const USBCStatus& status);

signals:
    void operate(const QString& status);

};


#endif // USBWATCHER_H
