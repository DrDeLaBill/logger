#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

#include "log.h"

#include "usbcontroller.h"


Ui::MainWindow* MainWindow::ui = new Ui::MainWindow();
USBController MainWindow::usbcontroller;
bool MainWindow::waitResponse = false;


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui->setupUi(this);
    usbcontroller.loadSettings();
}

MainWindow::~MainWindow() { }

void MainWindow::setError(const QString& str)
{
    QMessageBox messageBox;
    messageBox.critical(0, "Error", str);
    messageBox.setFixedSize(500, 200);
}

void MainWindow::on_updateBtn_clicked()
{
#if !defined(QT_NO_DEBUG)
    printTagLog(TAG, "update button clicked");
#endif
    if (!waitResponse) {
        usbcontroller.loadSettings();
        waitResponse = true;
    }
}

void MainWindow::on_upgradeBtn_clicked()
{
#if !defined(QT_NO_DEBUG)
    printTagLog(TAG, "upgrade button clicked");
#endif
    if (!waitResponse) {
        usbcontroller.saveSettings();
        waitResponse = true;
    }
}

Ui_MainWindow* MainWindow::getUI()
{
    return ui;
}

void MainWindow::responseProccess(USBRequestType type)
{
    waitResponse = false;

    switch (type) {
    case USB_REQUEST_LOAD_SETTINGS:
        showSettings();
        break;
    case USB_REQUEST_SAVE_SETTINGS:
        showSettings();
        break;
    case USB_REQUEST_LOAD_LOG:
        MainWindow::setError(exceptions::USBExceptionGroup().message);
        break;
    default:
        throw exceptions::UsbUndefinedBehaviourException();
    }
}

void MainWindow::showSettings()
{
    ui->record_period->setText(std::to_string(DeviceSettings::record_period{}.get()).c_str());
    ui->send_period->setText(std::to_string(DeviceSettings::send_period{}.get()).c_str());
}

void MainWindow::on_record_period_textChanged()
{
    DeviceSettings::record_period{}.set(ui->record_period->toPlainText().toUInt());
}

void MainWindow::on_send_period_textChanged()
{
    DeviceSettings::record_period{}.set(ui->send_period->toPlainText().toUInt());
}

