#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H


#include <string>
#include <memory>
#include <exception>

#include <QString>


namespace exceptions
{
    /* Exception groups start */
    struct ExceptionGroupBase
    {
        const QString message;
        ExceptionGroupBase(const QString& message): message(message) {}
    };

    struct InternallExceptionGroup: public ExceptionGroupBase
    {
        InternallExceptionGroup(): ExceptionGroupBase("INTERNAL_ERROR") {}
    };
    struct USBExceptionGroup: public ExceptionGroupBase
    {
        USBExceptionGroup(): ExceptionGroupBase("USB_ERROR") {}
    };
    /* Exception groups end */

    /* Exceptions start */
    class ExceptionBase: public std::exception
    {
    protected:
        const std::unique_ptr<ExceptionGroupBase> group;
        const std::string message;

    public:
        ExceptionBase(const std::string& message, const ExceptionGroupBase& group):
            group(std::make_unique<ExceptionGroupBase>(group.message)), message{message} { }

        const char* what() const noexcept override
        {
            return message.c_str(); // TODO: warn
        }

        const QString groupMessage() const
        {
            return group->message;
        }

    };

    struct InternalErrorException: public ExceptionBase
    {
        InternalErrorException(): ExceptionBase("INTERNAL_ERROR", InternallExceptionGroup()) {}
    };

    struct TemplateErrorException: public ExceptionBase
    {
        TemplateErrorException(): ExceptionBase("TEMPLATE_ERROR", InternallExceptionGroup()) {}
    };

    struct UsbException: public ExceptionBase
    {
        UsbException(): ExceptionBase("USB_ERROR", USBExceptionGroup()) {}
    };

    struct UsbTimeoutException: public ExceptionBase
    {
        UsbTimeoutException(): ExceptionBase("USB_ERROR", USBExceptionGroup()) {}
    };

    struct UsbInitException: public ExceptionBase
    {
        UsbInitException(): ExceptionBase("USB_INIT_ERROR", USBExceptionGroup()) {}
    };

    struct UsbDeinitException: public ExceptionBase
    {
        UsbDeinitException(): ExceptionBase("USB_DEINIT_ERROR", USBExceptionGroup()) {}
    };

    struct UsbNotFoundException: public ExceptionBase
    {
        UsbNotFoundException(): ExceptionBase("USB_NOT_FOUND_ERROR", USBExceptionGroup()) {}
    };

    struct UsbUndefinedBehaviourException: public ExceptionBase
    {
        UsbUndefinedBehaviourException(): ExceptionBase("USB_UNDEF_BEHAVIOUR_ERROR", USBExceptionGroup()) {}
    };

    struct UsbReportException: public ExceptionBase
    {
        UsbReportException(): ExceptionBase("USB_REPORT_ERROR", USBExceptionGroup()) {}
    };
    /* Exceptions end */
}

#endif // EXCEPTIONS_H
