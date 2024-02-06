QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Modules/DeviceRecord/devicerecord.cpp \
    Modules/DeviceSettings/devicesettings.cpp \
    Modules/HIDService/hidservice.cpp \
    Modules/USBController/usbcontroller.cpp \
    Modules/USBController/usbdreport.cpp \
    Modules/USBController/usbhandler.cpp \
    $$files($$PWD/Modules/Utils/utils/Log/*.cpp, true) \
    $$files($$PWD/Modules/Utils/utils/Time/*.cpp, true) \
    $$files($$PWD/Modules/Utils/utils/Time/*.c, true) \
    $$files($$PWD/Modules/Utils/utils/Debug/*.c, true) \
    $$files($$PWD/Modules/Utils/utils/Utils/*.c, true) \
    Modules/USBController/usbhreport.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Modules/DeviceRecord/devicerecord.h \
    Modules/DeviceSettings/devicesettings.h \
    Modules/Exceptions/app_exception.h \
    Modules/HIDService/HIDStruct.h \
    Modules/HIDService/hidservice.h \
    Modules/HIDTable/HIDController.h \
    Modules/USBController/usbcontroller.h \
    Modules/USBController/usbdreport.h \
    Modules/USBController/usbhandler.h \
    Modules/USBController/usbhreport.h \
    Modules/HIDTable/HIDTable.h \
    Modules/HIDTable/HIDTuple.h \
    Modules/HIDTable/HIDHash.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    logger_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Custom includes
INCLUDEPATH += $$PWD/Modules/libusb/include/libusb-1.0
INCLUDEPATH += $$PWD/Modules/Utils/utils/Log
INCLUDEPATH += $$PWD/Modules/Utils/utils/Time
INCLUDEPATH += $$PWD/Modules/Utils/utils/Debug
INCLUDEPATH += $$PWD/Modules/Utils/utils/Utils
INCLUDEPATH += $$PWD/Modules/Utils/utils/TypeList

LIBS        += $$files($$PWD/Modules/libusb/lib/*.a, true)
