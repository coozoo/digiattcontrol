// model: DC-6GHZ-30DBpp
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

#include "attdevice.h"
#include <QRegularExpression>
#include <QDebug>

AttDevice::AttDevice(QObject *parent)
    : SerialPortInterface(parent)
{
    connect(this, &SerialPortInterface::serialPortNewData, this, &AttDevice::onSerialPortNewData);
    connect(this, &AttDevice::expectedValueChanged, this, &AttDevice::onExpectedValueChanged);
    connect(this, &SerialPortInterface::portOpened, this, &AttDevice::onDevicePort_started);
    connect(&m_probeTimer, &QTimer::timeout, this, &AttDevice::onProbeTimeout);
    m_probeTimer.setSingleShot(true);
}

QString AttDevice::model() const { return m_model; }
double AttDevice::step() const { return m_step; }
double AttDevice::max() const { return m_max; }
QString AttDevice::format() const { return m_format; }
double AttDevice::currentValue() const { return m_currentValue; }
double AttDevice::expectedValue() const { return m_expectedValue; }

void AttDevice::setModel(const QString &model)
{
    m_model = model;
    emit modelChanged(m_model);
}

void AttDevice::setStep(double step)
{
    if (qFuzzyCompare(m_step, step))
        return;
    m_step = step;
    emit stepChanged(m_step);
}

void AttDevice::setMax(double max)
{
    if (qFuzzyCompare(m_max, max))
        return;
    m_max = max;
    emit maxChanged(m_max);
}

void AttDevice::setFormat(const QString &format)
{
    if (m_format == format)
        return;
    m_format = format;
    emit formatChanged(m_format);
}

void AttDevice::setCurrentValue(double value)
{
    if (!qFuzzyCompare(m_currentValue, value))
        {
            m_currentValue = value;
            emit currentValueChanged(value);
        }
}

void AttDevice::setExpectedValue(double value)
{
    qDebug() << Q_FUNC_INFO << value;
    if (qFuzzyCompare(m_expectedValue, value)) return;
    m_expectedValue = value;
    emit expectedValueChanged(value);
}

void AttDevice::writeValue(double value)
{
    qDebug() << Q_FUNC_INFO << value << "format:" << m_format;
    QString cmd = QString::asprintf(m_format.toStdString().c_str(), value);
    qDebug() << "final command:" << cmd;
    writeData(cmd.toUtf8());
    m_probeState = ProbeWaitingSetOK;
}

void AttDevice::readValue()
{
    qDebug() << Q_FUNC_INFO;
    writeData("READ\r\n");
}

void AttDevice::onExpectedValueChanged(double value)
{
    qDebug() << Q_FUNC_INFO << value;
    writeValue(value);
}

void AttDevice::onDevicePort_started()
{
    qDebug() << Q_FUNC_INFO;
    probeDeviceType();
}

void AttDevice::probeDeviceType()
{
    qDebug() << Q_FUNC_INFO;
    startProbe();
}

void AttDevice::startProbe()
{
    qDebug() << Q_FUNC_INFO;
    m_probeTypeIdx = 0;
    m_inProbe = true;
    tryCurrentProbe();
}

void AttDevice::tryCurrentProbe()
{
    qDebug() << Q_FUNC_INFO << m_probeTypeIdx;
    m_probeTimer.stop();
    if (m_probeTypeIdx >= int(sizeof(deviceTypes) / sizeof(DeviceType)))
        {
            finishProbe(false);
            return;
        }
    const DeviceType &dev = deviceTypes[m_probeTypeIdx];
    setFormat(dev.format);
    m_probeValue = dev.max;
    writeValue(m_probeValue);
    m_probeTimer.start(1000);
}

void AttDevice::onProbeTimeout()
{
    qDebug() << Q_FUNC_INFO << "timeout for type" << m_probeTypeIdx;
    m_probeState = ProbeIdle;
    m_probeTypeIdx++;
    tryCurrentProbe();
}

void AttDevice::finishProbe(bool found)
{
    qDebug() << Q_FUNC_INFO << (found ? "found" : "not found");
    m_probeTimer.stop();
    m_probeState = ProbeIdle;
    m_inProbe = false;
    if (found)
        {
            const DeviceType &dev = deviceTypes[m_probeTypeIdx];
            qDebug() << "finishProbe: model=" << dev.model << "format=" << dev.format;
            setModel(dev.model);
            setStep(dev.step);
            setMax(dev.max);
            setCurrentValue(dev.max);
            setFormat(dev.format);
            emit detectedDevice(m_model, m_step, m_max, m_format);
        }
    else
        {
            emit unknownDevice();
        }
}

void AttDevice::onSerialPortNewData(QString line)
{
    static QString buffer;
    buffer += line;

    QRegularExpression re(R"(attOK|ATT\s*=\s*-?\d+\.\d+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator it = re.globalMatch(buffer);

    int lastEnd = 0;
    while (it.hasNext())
        {
            QRegularExpressionMatch fullMatch = it.next();
            QString message = fullMatch.captured(0);
            lastEnd = fullMatch.capturedEnd();

            qDebug() << Q_FUNC_INFO << message;
            QString trimmed = message.trimmed();
            QRegularExpression valueRe("ATT\\s*=\\s*-?([0-9]+\\.[0-9]+)", QRegularExpression::CaseInsensitiveOption);
            auto match = valueRe.match(trimmed);

            if (m_probeState == ProbeWaitingSetOK && trimmed.compare("attOK", Qt::CaseInsensitive) == 0)
                {
                    m_probeState = ProbeWaitingValue;
                    readValue();
                    continue;
                }
            if (m_probeState == ProbeWaitingValue && match.hasMatch())
                {
                    double val = match.captured(1).toDouble();
                    if (m_inProbe)
                        {
                            qDebug() << "PROBE: val=" << val << "m_probeValue=" << m_probeValue
                                     << "compare=" << qFuzzyCompare(val + 1, m_probeValue + 1);
                            if (qFuzzyCompare(val + 1, m_probeValue + 1))
                                {
                                    emit currentValueChanged(val);
                                    finishProbe(true);
                                    m_probeState = ProbeIdle;
                                    continue;
                                }
                            else
                                {
                                    m_probeTypeIdx++;
                                    tryCurrentProbe();
                                    continue;
                                }
                        }
                    else
                        {
                            m_currentValue = val;
                            emit currentValueChanged(val);
                            if (qFuzzyCompare(m_expectedValue + 1, val + 1))
                                emit valueMatched();
                            else
                                emit valueMismatched(m_expectedValue, val);
                            m_probeState = ProbeIdle;
                            continue;
                        }
                }

            if (m_probeState == ProbeIdle && match.hasMatch())
                {
                    double val = match.captured(1).toDouble();
                    if (!qFuzzyCompare(m_currentValue, val))
                        {
                            m_currentValue = val;
                            emit currentValueChanged(val);
                            if (qFuzzyCompare(m_expectedValue + 1, val + 1))
                                emit valueMatched();
                            else
                                emit valueMismatched(m_expectedValue, val);
                        }
                    continue;
                }
        }

    buffer = buffer.mid(lastEnd);

    if (buffer.length() > 4096)
        buffer.clear();
}
