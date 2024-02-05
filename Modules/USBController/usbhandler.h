#ifndef USBHANDLER_H
#define USBHANDLER_H


#include <QString>


struct USBHandlerEqual
{
    bool operator()(const QString& lhs, const QString& rhs) const;
};

struct USBHandlerHash
{
    std::size_t operator()(const QString& key) const;
};

struct USBSearchHandler
{
    void operate() const;
    void handlerResults();
};

struct USBReportHandler
{
    void operate() const;
    void handlerResults();
};


#endif // USBHANDLER_H
