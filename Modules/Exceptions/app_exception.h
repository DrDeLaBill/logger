#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H


#include <string>
#include <exception>


namespace exceptions
{
    class ExceptionBase: public std::exception
    {
    public:
        ExceptionBase(const std::string& message): message{message} {}
        const char* what() const noexcept override
        {
            return message.c_str();
        }

    protected:
        std::string message;
    };

    struct InternalErrorException:         public ExceptionBase {InternalErrorException(const std::string& message): ExceptionBase(message) {}};
    struct UsbInitException:               public ExceptionBase {UsbInitException(const std::string& message): ExceptionBase(message) {}};
    struct UsbDeinitException:             public ExceptionBase {UsbDeinitException(const std::string& message): ExceptionBase(message) {}};
    struct UsbNotFoundException:           public ExceptionBase {UsbNotFoundException(const std::string& message): ExceptionBase(message) {}};
    struct UsbUndefinedBehaviourException: public ExceptionBase {UsbUndefinedBehaviourException(const std::string& message): ExceptionBase(message) {}};
}

#endif // EXCEPTIONS_H
