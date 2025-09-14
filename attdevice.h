#ifndef ATTDEVICE_H
#define ATTDEVICE_H

#include "serialportinterface.h"
#include <QString>
#include <QTimer>

struct DeviceType
{
    QString model;
    double step;
    double max;
    QString format;
};

static const DeviceType deviceTypes[] =
{
    { "DC-6GHZ-120DB",       0.25, 124.75,  "att-%06.2f\r\n" },
    { "DC-6GHZ-90DB-V3",     0.25,  95.25,  "att-%06.2f\r\n" },
    { "DC-3G-90DB-V2",       0.5,   94.5,   "att-%06.2f\r\n" },
    { "DC-8GHZ-30DB-0.1DB",  0.1,   30.0,   "att-%05.2f\r\n" },
    { "DC-6GHZ-30DB",        0.25,  31.75,  "att-%05.2f\r\n" }
};

class AttDevice : public SerialPortInterface
{
    Q_OBJECT

    Q_PROPERTY(QString model      READ model      WRITE setModel      NOTIFY modelChanged)
    Q_PROPERTY(double  step       READ step       WRITE setStep       NOTIFY stepChanged)
    Q_PROPERTY(double  max        READ max        WRITE setMax        NOTIFY maxChanged)
    Q_PROPERTY(QString format     READ format     WRITE setFormat     NOTIFY formatChanged)
    Q_PROPERTY(double currentValue   READ currentValue                  NOTIFY currentValueChanged)
    Q_PROPERTY(double expectedValue  READ expectedValue WRITE setExpectedValue NOTIFY expectedValueChanged)

public:
    explicit AttDevice(QObject *parent = nullptr);

    QString model() const;
    double step() const;
    double max() const;
    QString format() const;
    double currentValue() const;
    double expectedValue() const;

    void setModel(const QString &model);
    void setStep(double step);
    void setMax(double max);
    void setFormat(const QString &format);
    void setExpectedValue(double value);

    Q_INVOKABLE void probeDeviceType();
    Q_INVOKABLE void readValue();
    Q_INVOKABLE void writeValue(double value);

signals:
    void modelChanged(const QString &model);
    void stepChanged(double step);
    void maxChanged(double max);
    void formatChanged(const QString &format);
    void currentValueChanged(double value);
    void expectedValueChanged(double value);
    void valueMatched();
    void valueMismatched(double expected, double actual);
    void detectedDevice(const QString &model, double step, double max, const QString &format);
    void unknownDevice();

private slots:
    void onSerialPortNewData(QString line);
    void onExpectedValueChanged(double value);
    void onDevicePort_started();
    void onProbeTimeout();

private:
    enum ProbeState
    {
        ProbeIdle,
        ProbeWaitingSetOK,
        ProbeWaitingReadOK,
        ProbeWaitingValue
    };

    void startProbe();
    void tryCurrentProbe();
    void finishProbe(bool found);

    QString m_model;
    double  m_step         = 0.05;
    double  m_max          = 100.0;
    QString m_format       = "att-%05.2f\r\n";
    double  m_currentValue = 0.0;
    double  m_expectedValue = 0.0;

    int     m_probeTypeIdx = 0;
    double  m_probeValue   = 0.0;
    ProbeState m_probeState = ProbeIdle;
    QTimer  m_probeTimer;
    bool    m_inProbe      = false;
};

#endif // ATTDEVICE_H
