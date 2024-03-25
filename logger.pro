QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$files($$PWD/Modules/USBController/COMTable/*.c, true) \
    $$files($$PWD/Modules/USBController/*.cpp, true) \
    $$files($$PWD/Modules/Utils/utils/Log/*.cpp, true) \
    $$files($$PWD/Modules/Utils/utils/Time/*.cpp, true) \
    $$files($$PWD/Modules/Utils/utils/Time/*.c, true) \
    $$files($$PWD/Modules/Utils/utils/Debug/*.c, true) \
    $$files($$PWD/Modules/Utils/utils/Utils/*.c, true) \
    $$files($$PWD/Modules/DeviceRecord/*.cpp, true) \
    $$files($$PWD/Modules/DeviceInterface/*.cpp, true) \
    $$files($$PWD/Widgets/SensorList/*.cpp, true) \
    $$files($$PWD/Widgets/SensorBox/*.cpp, true) \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Modules/Exceptions/app_exception.h \
    Modules/USBController/COMService/comservice.h \
    Modules/USBController/COMTable/COMController.h \
    Modules/USBController/comtableworker.h \
    Modules/USBController/usbcstatus.h \
    Modules/USBController/usbcontroller.h \
    Modules/USBController/COMTable/COMTable.h \
    Modules/USBController/COMTable/COMTuple.h \
    Modules/USBController/COMTable/COMHash.h \
    Widgets/SensorBox/sensordata.h \
    Widgets/SensorBox/sensorbox.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    logger_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations
CONFIG += qml_debug

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Custom includes
INCLUDEPATH += $$PWD/Modules/DeviceInterface
INCLUDEPATH += $$PWD/Modules/Exceptions
INCLUDEPATH += $$PWD/Modules/USBController/COMService
INCLUDEPATH += $$PWD/Modules/USBController/COMTable
INCLUDEPATH += $$PWD/Modules/USBController/USBReport
INCLUDEPATH += $$PWD/Modules/USBController
INCLUDEPATH += $$PWD/Modules/Utils/utils/Log
INCLUDEPATH += $$PWD/Modules/Utils/utils/Time
INCLUDEPATH += $$PWD/Modules/Utils/utils/Debug
INCLUDEPATH += $$PWD/Modules/Utils/utils/Utils
INCLUDEPATH += $$PWD/Modules/Utils/utils/FSM
INCLUDEPATH += $$PWD/Modules/Utils/utils/Buffer
INCLUDEPATH += $$PWD/Modules/Utils/utils/TypeList
INCLUDEPATH += $$PWD/Modules/Utils/utils/HALDEFS
INCLUDEPATH += $$PWD/Widgets/SensorList
INCLUDEPATH += $$PWD/Widgets/SensorBox

QMAKE_CXXFLAGS += -Og
