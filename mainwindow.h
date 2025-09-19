#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSerialPort>
#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QDebug>
#include "qtdigitalattenuator.h"
#include "serialportinterface.h"
#include "attdevice.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QtDigitalAttenuator *widget;

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
