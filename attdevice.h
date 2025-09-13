/****************************************************************************
** AttDevice - Programmable Attenuator Device Class
**
** Inherits SerialPortInterface and provides:
** - Device type properties (model, step, max, format) with Q_PROPERTY, signals
** - expectedValue and currentValue properties for set/read logic
** - Asynchronous device probing logic (auto-detects type/format)
** - Signals for value matches/mismatches and device detection
**
** Author: coozoo
** License: MIT (or your project’s license)
****************************************************************************/

#ifndef ATTDEVICE_H
#define ATTDEVICE_H

#include "serialportinterface.h"
#include <QString>

/**
 * @brief Represents a programmable attenuator device with auto-type detection.
 */
class AttDevice : public SerialPortInterface
{
    Q_OBJECT

    // Device identification and behavior
    Q_PROPERTY(QString model      READ model      WRITE setModel      NOTIFY modelChanged)
    Q_PROPERTY(double  step       READ step       WRITE setStep       NOTIFY stepChanged)
    Q_PROPERTY(double  max        READ max        WRITE setMax        NOTIFY maxChanged)
    Q_PROPERTY(QString format     READ format     WRITE setFormat     NOTIFY formatChanged)

    // Attenuation values
    Q_PROPERTY(double currentValue   READ currentValue                  NOTIFY currentValueChanged)
    Q_PROPERTY(double expectedValue  READ expectedValue WRITE setExpectedValue NOTIFY expectedValueChanged)

public:
    explicit AttDevice(QObject *parent = nullptr);

    // Device properties
    QString model()    const;
    double  step()     const;
    double  max()      const;
    QString format()   const;

    // Attenuation values
    double currentValue() const;
    double expectedValue() const;

    // Setters
    void setModel(const QString &model);
    void setStep(double step);
    void setMax(double max);
    void setFormat(const QString &format);
    void setExpectedValue(double value);

    // Device probe and I/O
    Q_INVOKABLE void probeDeviceType();
    Q_INVOKABLE void readValue();
    Q_INVOKABLE void writeValue(double value);

signals:
    // Device property changes
    void modelChanged(const QString &model);
    void stepChanged(double step);
    void maxChanged(double max);
    void formatChanged(const QString &format);

    // Value changes and matching
    void currentValueChanged(double value);
    void expectedValueChanged(double value);
    void valueMatched();
    void valueMismatched(double expected, double actual);

    // Device detection
    void detectedDevice(const QString &model, double step, double max, const QString &format);
    void unknownDevice();

private slots:
    void onSerialPortNewData(QString line);
    void onCurrentValueChanged(double value);
    void onDevicePort_started();

private:
    // Properties
    QString m_model;
    double  m_step         = 0.05;
    double  m_max          = 100.0;
    QString m_format       = "att-%05.2f\r\n";
    double  m_currentValue = 0.0;
    double  m_expectedValue= 0.0;

    // Probing state
    enum ProbeState { NotProbing, ProbingFormat1, ProbingFormat2, ProbeDone } m_probeState = NotProbing;
    int     m_probeFormatIdx = 0;
    double  m_probeValue     = 0.0;
    QString m_probeFormats[2] = { "att-%05.2f\r\n", "att-%06.2f\r\n" };

};

#endif // ATTDEVICE_H
