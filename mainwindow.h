#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

#include <memory>

#include "Modules/USBController/usbcontroller.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    USBController usbcontroller;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static void setBytesLabel(const QString& str);

private:
    static std::unique_ptr<Ui::MainWindow> ui;

};

#endif // MAINWINDOW_H
