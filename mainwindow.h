#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSerialPort>
#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QSerialPortInfo>
#include <QDateTime>
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

private:
    Ui::MainWindow *ui;
    void updateDeviceList();
    AttDevice *serialAttenuator;
    QTimer *attenuation_doubleSpinBox_debounceTimer;

private slots:
    void ondevice_comboBox_currentIndexChanged();
    void updateData(QString data);
    void on_connect_pushButton_clicked();
    void on_disconnect_pushButton_clicked();
    void on_refreshDevices_toolbutton_clicked();
    void on_serialPortError(QString error);
    void on_attenuation_doubleSpinBox_valueChanged(double value);
    void on_set_pushButton_clicked();
    void on_send_pushButton_clicked();
    void on_currentAttenuation_changed(double value);
};
#endif // MAINWINDOW_H
