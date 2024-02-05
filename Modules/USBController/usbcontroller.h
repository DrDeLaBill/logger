#ifndef USBWATCHER_H
#define USBWATCHER_H


#include <memory>
#include <variant>
#include <unordered_map>

#include <QThread>
#include <QObject>
#include <QString>

#include <libusb.h>
#include "usbhandler.h"


#define USB_SEARCH_HANDLER (QString("SEARCH_HANDLER"))
#define USB_REPORT_HANDLER (QString("REPORT_HANDLER"))


class USBWorker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString& parameter);

signals:
    void resultReady(const QString& result);

private:
    using handler_v = std::variant<
        USBSearchHandler,
        USBReportHandler
    >;
    using handler_t = std::unordered_map<
        QString,
        handler_v,
        USBHandlerHash,
        USBHandlerEqual
    >;

    handler_t handlers = {
        {USB_SEARCH_HANDLER, USBSearchHandler{}},
        {USB_REPORT_HANDLER, USBReportHandler{}}
    };

};


class USBController : public QObject
{
    Q_OBJECT
    QThread workerThread;

private:
    static constexpr char TAG[] = "USB";

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
