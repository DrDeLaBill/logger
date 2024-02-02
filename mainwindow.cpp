#include "mainwindow.h"
#include "ui_mainwindow.h"


std::unique_ptr<Ui::MainWindow> MainWindow::ui = std::make_unique<Ui::MainWindow>();


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    ui->setupUi(this);
    usbcontroller.search();
}

MainWindow::~MainWindow() { }

void MainWindow::setBytesLabel(const QString& str)  {
    ui->LabelBytes->setText(str);
}
