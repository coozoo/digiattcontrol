#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    widget = new QtDigitalAttenuator(this);
    setCentralWidget(widget);
    adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

