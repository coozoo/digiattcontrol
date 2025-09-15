#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->currentattenuation_lcdNumber->setSegmentStyle(QLCDNumber::Flat );
    ui->currentattenuation_lcdNumber->setStyleSheet("QLCDNumber{ background-color: green; color: yellow;}");
    attenuation_doubleSpinBox_debounceTimer = new QTimer(this);
    attenuation_doubleSpinBox_debounceTimer->setSingleShot(true);
    connect(attenuation_doubleSpinBox_debounceTimer, &QTimer::timeout, this, [this]() {
        if(ui->autoset_checkBox->isChecked())
        {
            serialAttenuator->writeValue(ui->attenuation_doubleSpinBox->value());
        }
    });
    updateDeviceList();
    serialAttenuator= new AttDevice();
    Q_EMIT(ondevice_comboBox_currentIndexChanged());
    connect(ui->device_comboBox, &QComboBox::currentTextChanged, this, &MainWindow::ondevice_comboBox_currentIndexChanged);
    connect(serialAttenuator,&AttDevice::serialPortNewData,this,&MainWindow::updateData);
    connect(serialAttenuator,&AttDevice::serialPortErrorSignal,this,&MainWindow::on_serialPortError);
    connect(serialAttenuator,&AttDevice::currentValueChanged,this,&MainWindow::on_currentAttenuation_changed);
    connect(serialAttenuator,&AttDevice::detectedDevice,this,&MainWindow::ondetectedDevice);
    connect(ui->attenuation_doubleSpinBox,&QDoubleSpinBox::valueChanged,this,&MainWindow::onattenuation_doubleSpinBox_valueChanged);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ondevice_comboBox_currentIndexChanged()
{
    qDebug()<<"ondevice_comboBox_currentIndexChanged"<<ui->device_comboBox->currentData().toString();
    if(ui->device_comboBox->currentText().isEmpty())
    {
        ui->connect_pushButton->setDisabled(true);
        ui->disconnect_pushButton->setDisabled(true);
    }
    else
    {
        ui->connect_pushButton->setDisabled(false);
        ui->disconnect_pushButton->setDisabled(false);
    }
}

void MainWindow::updateData(QString data)
{
    qDebug()<<"updateData"<<data;
    QString curdate=QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ss.zzz");
    ui->data_plainTextEdit->appendPlainText(curdate+" "+ui->device_comboBox->currentData().toString()+" "+data);
}

void MainWindow::on_connect_pushButton_clicked()
{
    qDebug()<<"on_connect_pushButton_clicked";

    if(!ui->device_comboBox->currentData().toString().isEmpty())
    {
        serialAttenuator->setportName(ui->device_comboBox->currentData().toString());
        serialAttenuator->setbaudRate(115200);
        serialAttenuator->startPort();

        //Q_EMIT(on_set_pushButton_clicked());
       updateDeviceList();
    }

}

void MainWindow::on_disconnect_pushButton_clicked()
{
    qDebug()<<"on_disconnect_pushButton_clicked";
    serialAttenuator->stopPort();
    ui->model_lineEdit->setText("");
    updateDeviceList();

}

void MainWindow::on_refreshDevices_toolbutton_clicked()
{
    qDebug()<<"on_refreshDevices_toolbutton_clicked";
    updateDeviceList();
}

void MainWindow::updateDeviceList()
{
    qDebug()<<"updateDeviceList";
    ui->device_comboBox->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {

        if(info.hasVendorIdentifier() && QString::number(info.vendorIdentifier(),16)=="483")
        {
            qDebug()<<info.hasVendorIdentifier() <<QString::number(info.vendorIdentifier());
            QString busyText;
            QSerialPort serialPort(info);
            if (!serialPort.open(QIODevice::ReadWrite)) {
                busyText = tr(" [Busy]");
            } else {
                serialPort.close();
                busyText = "";
            }
            QString s = info.portName() +
                        tr(" (") + info.systemLocation() +
                        tr(") ") + info.description() +
                        tr(" ") + info.manufacturer() +
                        tr(" ") + info.serialNumber() +
                        tr(" (") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) +
                        tr(":") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) +")"+
                        busyText;
            qInfo()<<info.portName();

            qInfo()<<s;
            ui->device_comboBox->addItem(s,info.portName());
        }
    }
}

void MainWindow::on_serialPortError(QString error)
{
    qDebug()<<"on_serialPortError";
    ui->statusbar->showMessage(error,1000);
    Q_EMIT(ondevice_comboBox_currentIndexChanged());
}

void MainWindow::on_send_pushButton_clicked()
{
    qDebug()<<"on_set_pushButton_clicked";
    serialAttenuator->writeData((ui->write_data->text()+QString("\r\n")).toLatin1());
}

void MainWindow::on_set_pushButton_clicked()
{
    qDebug()<<"on_set_pushButton_clicked";
    qDebug()<<ui->attenuation_doubleSpinBox->value();
    serialAttenuator->writeValue(ui->attenuation_doubleSpinBox->value());
}


void MainWindow::on_currentAttenuation_changed(double value)
{
    qDebug() << Q_FUNC_INFO<<value;
    ui->currentattenuation_lcdNumber->display(value);
}

void MainWindow::onattenuation_doubleSpinBox_valueChanged(double value)
{
    qDebug() << Q_FUNC_INFO<<value;
    attenuation_doubleSpinBox_debounceTimer->start(400);
}

void MainWindow::ondetectedDevice(const QString &model, double step, double max, const QString &format)
{
    qDebug() << Q_FUNC_INFO<<model<<step<<max<<format;
    ui->model_lineEdit->setText(model);
    disconnect(ui->attenuation_doubleSpinBox,&QDoubleSpinBox::valueChanged,this,&MainWindow::onattenuation_doubleSpinBox_valueChanged);
    ui->attenuation_doubleSpinBox->setValue(serialAttenuator->currentValue());
    connect(ui->attenuation_doubleSpinBox,&QDoubleSpinBox::valueChanged,this,&MainWindow::onattenuation_doubleSpinBox_valueChanged);
}
