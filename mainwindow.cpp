#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    updateDeviceList();
    serialPortPowerMeter= new SerialPortInterface();
    Q_EMIT(ondevice_comboBox_currentIndexChanged());
    connect(ui->device_comboBox, &QComboBox::currentTextChanged, this, &MainWindow::ondevice_comboBox_currentIndexChanged);
    connect(serialPortPowerMeter,&SerialPortInterface::serialPortNewData,this,&MainWindow::updateData);
    connect(serialPortPowerMeter,&SerialPortInterface::serialPortErrorSignal,this,&MainWindow::on_serialPortError);


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
        serialPortPowerMeter->setportName(ui->device_comboBox->currentData().toString());
        serialPortPowerMeter->setbaudRate(115200);
        serialPortPowerMeter->startPort();
        //Q_EMIT(on_set_pushButton_clicked());
        updateDeviceList();
    }

}

void MainWindow::on_disconnect_pushButton_clicked()
{
    qDebug()<<"on_disconnect_pushButton_clicked";
    serialPortPowerMeter->stopPort();
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
        qDebug()<<info.hasVendorIdentifier() <<QString::number(info.vendorIdentifier());
        if(info.hasVendorIdentifier() && QString::number(info.vendorIdentifier(),16)=="483")
        {
            QString busyText;
            QSerialPort serialPort(info);
            if (!serialPort.open(QIODevice::ReadWrite)) {
                busyText = tr(" [Busy]");
            } else {
                serialPort.close(); // Close immediately if opened
                busyText = "";
            }
            QString s = tr("Port: ") + info.portName() +
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

void MainWindow::on_set_pushButton_clicked()
{
    qDebug()<<"on_set_pushButton_clicked";
   // setFrequency(QString::number(ui->frequency_spinBox->value(),'d',0).rightJustified(4, '0'));
    //if(ui->correctionplus_radioButton->isChecked())
    //{
    //setOffset(QString(ui->correctionplus_radioButton->isChecked()?"+":"-")+QString::number(ui->offset_doubleSpinBox->value(),'f',1).rightJustified(4, '0'));
    //}
    //serialPortPowerMeter->writeData(QString("wv01150\n").toLatin1());
    serialPortPowerMeter->writeData((ui->write_data->text()+QString("\r\n")).toLatin1());
}



