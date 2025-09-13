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

// model: DC-3G-90DB-V2
// baudrate: 9600(115200 usb no need to be set)
// step: 0.5
// max: 94.5
// -set: "att-000.00\r\n"
// -response: "attOK"
// -read data: "READ\r\n"
// -response: "ATT = -31.75"

// model: DC-8GHZ-30DB-0.1DB
// baudrate: 9600(115200 usb no need to be set)
// step: 0.1
// max: 30
// -set: "att-00.00\r\n"
// -response: "attOK"
// -read data: "READ\r\n"
// -response: "ATT = -30.00"

// model: DC-6GHZ-120DB
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

/****************************************************************************
** AttDevice - Implementation
****************************************************************************/
#include "attdevice.h"
#include <QRegularExpression>
#include <QDebug>
#include <cstdio>

struct DeviceType {
    QString model;
    double step;
    double max;
    QString format;
};

static const DeviceType deviceTypes[] = {
    { "DC-6GHZ-30DB",        0.25,  31.75,  "att-%05.2f\r\n" },
    { "DC-6GHZ-90DB-V3",     0.25,  95.25,  "att-%06.2f\r\n" },
    { "DC-3G-90DB-V2",       0.5,   94.5,   "att-%06.2f\r\n" },
    { "DC-8GHZ-30DB-0.1DB",  0.1,   30.0,   "att-%05.2f\r\n" },
    { "DC-6GHZ-120DB",       0.25, 124.75,  "att-%06.2f\r\n" },
    };

AttDevice::AttDevice(QObject *parent)
    : SerialPortInterface(parent)
{
    qDebug() << Q_FUNC_INFO;
    connect(this, &SerialPortInterface::serialPortNewData, this, &AttDevice::onSerialPortNewData);
    connect(this, &AttDevice::currentValueChanged, this, &AttDevice::onCurrentValueChanged);
    connect(this, &SerialPortInterface::portOpened, this, &AttDevice::onDevicePort_started);
}

QString AttDevice::model() const {
    qDebug() << Q_FUNC_INFO;
    return m_model;
}
double AttDevice::step() const {
    qDebug() << Q_FUNC_INFO;
    return m_step;
}
double AttDevice::max() const {
    qDebug() << Q_FUNC_INFO;
    return m_max;
}
QString AttDevice::format() const {
    qDebug() << Q_FUNC_INFO;
    return m_format;
}
double AttDevice::currentValue() const {
    qDebug() << Q_FUNC_INFO;
    return m_currentValue;
}
double AttDevice::expectedValue() const {
    qDebug() << Q_FUNC_INFO;
    return m_expectedValue;
}

void AttDevice::setModel(const QString &model) {
    qDebug() << Q_FUNC_INFO << model;
    if (m_model == model) return;
    m_model = model;
    emit modelChanged(model);
}
void AttDevice::setStep(double step) {
    qDebug() << Q_FUNC_INFO << step;
    if (qFuzzyCompare(m_step, step)) return;
    m_step = step;
    emit stepChanged(step);
}
void AttDevice::setMax(double max) {
    qDebug() << Q_FUNC_INFO << max;
    if (qFuzzyCompare(m_max, max)) return;
    m_max = max;
    emit maxChanged(max);
}
void AttDevice::setFormat(const QString &format) {
    qDebug() << Q_FUNC_INFO << format;
    if (m_format == format) return;
    m_format = format;
    emit formatChanged(format);
}
void AttDevice::setExpectedValue(double value) {
    qDebug() << Q_FUNC_INFO << value;
    if (qFuzzyCompare(m_expectedValue, value)) return;
    m_expectedValue = value;
    emit expectedValueChanged(value);
    writeValue(value);
}

void AttDevice::readValue() {
    qDebug() << Q_FUNC_INFO;
    writeData("READ\r\n");
}
void AttDevice::writeValue(double value) {
    qDebug() << Q_FUNC_INFO << value;
    char buffer[32];
    snprintf(buffer, sizeof(buffer), m_format.toStdString().c_str(), value);
    writeData(buffer);
}

void AttDevice::probeDeviceType() {
    qDebug() << Q_FUNC_INFO;
    m_probeValue = deviceTypes[0].max;
    for (const auto &type : deviceTypes) {
        if (type.max > m_probeValue) m_probeValue = type.max;
    }
    m_probeFormatIdx = 0;
    m_probeState = ProbingFormat1;
    setExpectedValue(m_probeValue);
    char buf[32];
    snprintf(buf, sizeof(buf), m_probeFormats[m_probeFormatIdx].toStdString().c_str(), m_probeValue);
    writeData(buf);
}

void AttDevice::onSerialPortNewData(QString line) {
    qDebug() << Q_FUNC_INFO << line;
    QRegularExpression re("ATT\\s*=\\s*(-?[0-9]+\\.[0-9]+)");
    auto match = re.match(line);
    if (match.hasMatch()) {
        double val = match.captured(1).toDouble();
        if (!qFuzzyCompare(m_currentValue, val)) {
            m_currentValue = val;
            emit currentValueChanged(val);
        }
    }
}

void AttDevice::onCurrentValueChanged(double value) {
    qDebug() << Q_FUNC_INFO << value;
    if (m_probeState == ProbingFormat1 || m_probeState == ProbingFormat2) {
        if (qFuzzyCompare(m_expectedValue, value)) {
            for (const auto &type : deviceTypes) {
                if (qFuzzyCompare(type.max, value) && type.format == m_probeFormats[m_probeFormatIdx]) {
                    setModel(type.model);
                    setStep(type.step);
                    setMax(type.max);
                    setFormat(type.format);
                    emit detectedDevice(type.model, type.step, type.max, type.format);
                    m_probeState = ProbeDone;
                    return;
                }
            }
        }
        m_probeFormatIdx++;
        if (m_probeFormatIdx < 2) {
            m_probeState = (m_probeFormatIdx == 1) ? ProbingFormat2 : ProbeDone;
            char buf[32];
            snprintf(buf, sizeof(buf), m_probeFormats[m_probeFormatIdx].toStdString().c_str(), m_probeValue);
            setExpectedValue(m_probeValue);
        } else {
            setModel("UNKNOWN");
            setStep(0.05);
            setMax(100);
            setFormat(m_probeFormats[0]);
            emit unknownDevice();
            m_probeState = ProbeDone;
        }
        return;
    }

    if (!qFuzzyCompare(m_expectedValue + 1, 1)) {
        if (qFuzzyCompare(m_expectedValue, m_currentValue)) emit valueMatched();
        else emit valueMismatched(m_expectedValue, m_currentValue);
    }
}

void AttDevice::onDevicePort_started()
{
    qDebug() << Q_FUNC_INFO << "Port started, beginning probe...";
    probeDeviceType();
}
