#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QSerialPort>
#include <QDebug>
#include "settingsdialog.h"

class SerialComm : public QObject
{
    Q_OBJECT
public:
    //enum class commands{}; Implemented in child class
    explicit SerialComm(QObject *parent = nullptr);
    void openSerialPort();
    void closeSerialPort();
    bool isOpen();
    void updateSerialInfo(const SettingsDialog::Settings &settings);

protected:
    virtual void serialConnSendMessage() = 0;
    virtual void sendError(QSerialPort::SerialPortError error, const QString &error_message) = 0;

    QSerialPort serial_conn;
    QString temp_data = "";
    QQueue<int> command_queue;
    QQueue<QString> data_queue;
    QTimer timer;

signals:
    void rawDataSignal(QString data);
    void returnData(QString data, int command_sent);
    void errorSignal(QSerialPort::SerialPortError error, QString error_string, int command_sent);

private slots:
    virtual void serialConnReceiveMessage() = 0;
    void collectErrorData(QSerialPort::SerialPortError error);
    void timeout();
};

#endif // SERIALCOMM_H
