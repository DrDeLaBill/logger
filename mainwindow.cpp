#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>


std::unique_ptr<Ui::MainWindow> MainWindow::ui = std::make_unique<Ui::MainWindow>();


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui->setupUi(this);
    usbcontroller.proccess();
}

MainWindow::~MainWindow() { }

void MainWindow::setBytesLabel(const QString& str)
{
    ui->LabelBytes->setText(str);
}

void MainWindow::setError(const QString& str)
{
    setBytesLabel(str);

    QMessageBox messageBox;
    messageBox.critical(0, "Error", str);
    messageBox.setFixedSize(500, 200);
}
