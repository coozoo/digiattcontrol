#ifndef ATTDEVICE_H
#define ATTDEVICE_H

#include <QObject>

class AttDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString portName
                   READ getportName
                       WRITE setportName
               )
    Q_PROPERTY(int baudRate
                   READ getbaudRate
                       WRITE setbaudRate
               )
public:
    explicit AttDevice(QObject *parent = nullptr);
    ~AttDevice();

    void setportName(QString m_portName)
    {
        portName = m_portName;
        //emit portName_changed();
    }
    QString getportName() const
    {
        return portName;
    }

    void setbaudRate(int m_baudRate)
    {
        baudRate = m_baudRate;
        //emit baudRate_changed();
    }
    int getbaudRate() const
    {
        return baudRate;
    }

    QString readAtt();
    bool setAtt(QString att);

private:
    QString portName;
    int baudRate;

};

#endif // ATTDEVICE_H
