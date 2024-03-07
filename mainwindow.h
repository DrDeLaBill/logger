#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <memory>

#include <QTimer>
#include <QMainWindow>

#include "ui_mainwindow.h"

#include "Timer.h"
#include "FiniteStateMachine.h"

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

    static void update();
    static void upgrade();

private:
    static QTimer* saveTimer;

    static void showSettings();
    static void clearSettings();

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

public:
    static void responseProccess(const USBRequestType type, const USBCStatus status);

    static void setLoading();
    static void resetLoading();

    static void disableAll();
    static void enableAll();

public slots:
    void onInfoTimeout();
    void onSaveTimeout();

};

#endif // MAINWINDOW_H
