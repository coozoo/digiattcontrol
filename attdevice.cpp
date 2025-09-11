#include "attdevice.h"

// model: DC-6GHZ-30DB
// baudrate: 9600(115200 usb no need to be set)
// step: 0.25
// max: 31.75
// -set: "att-00.00\r\n"
// -response: "attOK"
// -read data: "READ\r\n"
// -response: "ATT = -31.75"

// model: DC-6GHZ-90DB-V3
// baudrate: 9600(115200 usb no need to be set)
// step: 0.25
// max: 95.25
// -set: "att-000.00\r\n"
// -response: "attOK"
// -read data: "READ\r\n"
// -response: "ATT = -31.75"

// model: LF-6GHZ-120DB
// baudrate: 9600(115200 usb no need to be set)
// step: 0.25
// max: 124.75
// -set: "att-000.00\r\n"
// -response: "attOK"
// -read data: "READ\r\n"
// -response: "ATT = -31.75"

// there is such similar devices I'm not sure how to identify them
// I found some descriptions of their comunication
// but in fact I'm not sure about that
// it's possible to implement them only when own such device
// ??????
// ATT-6000
// "wv03125\r\n"

// ?????
// LF-6GHZ-120DB
// -set: "att-000.00\r\n"


AttDevice::AttDevice(QObject *parent) : QObject{parent}
{

}

AttDevice::~AttDevice()
{

}

QString AttDevice::readAtt()
{
    QString att="";

    return att;
}

bool AttDevice::setAtt(QString att)
{
    bool result=false;
    return result;
}
