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

#include "sensorbox.h"
#include "sensorlist.h"
#include "usbcontroller.h"
#include "hidtableworker.h"


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
    static std::vector<QMetaObject::Connection> m_sensorConnection;

    static QTimer* saveTimer;
    static QTimer* infoTimer;

    void showSettings(const USBRequestType type);
    void clearSettings();
    void clearSensors();
    void updateScrollBar();

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

    void onInfoTimeout();
    void onSaveTimeout();

    void onSaveSensor(const SensorData& sensorData);

    void onUSBError(const QString& message);

public:
    static SensorList* sensorListBox;
    static SensorBox* firstSensor;
    static std::vector<SensorBox> sensors;

    void responseProccess(const USBRequestType type, const USBCStatus status);

    static void setLoading();
    static void resetLoading();

    static void disableAll();
    static void enableAll();

};

#endif // MAINWINDOW_H
