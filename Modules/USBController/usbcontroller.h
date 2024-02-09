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


#define USB_SEARCH_HANDLER             (QString("SEARCH_HANDLER"))
#define USB_LOAD_RECORD_HANDLER        (QString("REPORT_HANDLER"))
#define USB_GET_CHARACTERISTIC_HANDLER (QString("GET_CHARACTERISITIC_HANDLER"))


class USBWorker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString& parameter);

signals:
    void resultReady(const QString& result);

private:
    static constexpr char TAG[] = "USBW";

    using handler_v = std::variant<
        USBSearchHandler,
        USBLoadRecordHandler,
        USBCGetCharacteristicHandler
    >;
    using handler_t = std::unordered_map<
        QString,
        handler_v,
        USBHandlerHash,
        USBHandlerEqual
    >;

    handler_t handlers = {
        {USB_SEARCH_HANDLER, USBSearchHandler{}},
        {USB_LOAD_RECORD_HANDLER, USBLoadRecordHandler{}}, // TODO: load record handler
        {USB_GET_CHARACTERISTIC_HANDLER, USBCGetCharacteristicHandler{}}
    };

};


class USBController : public QObject
{
    Q_OBJECT
    QThread workerThread;

private:
    static constexpr char TAG[] = "USBC";

    QString curParameter;

public:
    USBController();
    ~USBController();

    void proccess(const QString& parameter = USB_SEARCH_HANDLER);

public slots:
    void handleResults(const QString& parameter);

signals:
    void operate(const QString& parameter);

};


#endif // USBWATCHER_H
