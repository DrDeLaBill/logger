#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <memory>
#include <vector>

#include <QTimer>
#include <QGroupBox>
#include <QMainWindow>

#include "ui_mainwindow.h"

#include "Timer.h"
#include "FiniteStateMachine.h"

#include "modbusbox.h"
#include "onewirebox.h"
#include "sensorlist.h"
#include "usbcontroller.h"
#include "comtableworker.h"
#include "onewireservice.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
\
    static void setError(const QString& str);
    static void setWarning(const QString& str);

    static void update();
    static void upgrade();

private:
    static std::vector<QMetaObject::Connection> m_modbus1Connection;

    static QTimer* saveTimer;
    static QTimer* infoTimer;

    QString port;

    void showSettings(const USBRequestType type);
    void showOneWireSensors();
    void clearSettings();
    void clearSensors();
    void updateScrollBar();
    void updateCOMSelect();

    void setMODBUS1Hidden(bool state);
    void setONEWIREHidden(bool state);


protected:
    static constexpr char TAG[] = "MAIN";

    static Ui::MainWindow* ui;
    static USBRequestType requestType;
    static USBController usbcontroller;

private slots:
    void on_updateBtn_clicked();
    void on_upgradeBtn_clicked();

    void on_record_period_textChanged();

    void on_send_period_textChanged();

    void on_updateTimeBtn_clicked();

    void on_verticalScrollBar_valueChanged(int value);

    void onSaveTimeout();
    void onInfoTimeout();

    void onSaveModbus1Sensor(const ModbusData& sensorData);

    void onOneWireRegister();

    void onUSBError(const QString& message);

    void on_dumpBtn_clicked();

    void onLoadLogProgressUpdated(uint32_t value);

    void on_serialPortSelect_activated(int index);

    void on_updatePortsBtn_clicked();

    void on_modbus1Btn_clicked();

    void on_onewireBtn_clicked();

public:
    static SensorList* modbus1ListBox;
    static ModbusBox* firstmodbus1Sensor;
    static std::vector<ModbusBox> modbus1Sensors;

    static SensorList* onewireListBox;
    static OneWireService* oneWireService;
    static std::vector<OneWireBox> oneWireSensors;


    void responseProccess(const USBRequestType type, const USBCStatus status);

    static void setLoading();
    static void resetLoading();

    static void disableAll();
    static void enableAll();

    static void scrollModbus1(int value);
    static void scrollOneWire(int value);

};

#endif // MAINWINDOW_H
