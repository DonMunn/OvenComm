#ifndef OVENCOMM_H
#define OVENCOMM_H

#include <QObject>
#include <QSerialPort>
#include "settingsdialog.h"
#include <QQueue>
#include <QTimer>

class OvenComm : public QObject
{
    Q_OBJECT

public:
    // enum for indicating the use of data that is read from serial for additional calulcations
    enum commands { NONE=0, GETTEMP=1, GETSETTEMP=30, SETTEMP=60,
                     GETOUTPUT=3, GETSENSORSTATUS=4, GETPOWERSTATUS=35, SETPOWERSTATUS=65};
        // type | what_its_for/calculations | range
        // double | temp  x 100 | (-32768, 32768)
        // double | output / 28800 (%) | (0, 28800)
        // bool | status | (0, 1)
    OvenComm();

    void updateSerialInfo(const SettingsDialog::Settings &settings);

    void setTemp(double temp); //Done
    void getTemp(); //Done
    void getSetTemp(); //Done

    void getOutput(); //Done
    void getSensorStatus(); //Done

    void setPowerStatus(bool on); //Done
    void getPowerStatus(); //Done

    void openSerialPort(); //Done
    void closeSerialPort(); //Done

    bool isOpen();

signals:
    void errorSignal(QSerialPort::SerialPortError error, QString error_string, commands command_sent);
    void displayDataSignal(int data, commands command_sent);

    void rawDataSignal(QString data);

private:
    void sendError(QSerialPort::SerialPortError error, QString error_message);
    bool verifyChecksum();
    void serialConnSendMessage();

    QSerialPort *serial_conn = new QSerialPort(this);

    QString temp_data = "";
    QQueue<commands> command_queue;
    QQueue<int> data_queue;
    QTimer *timer = new QTimer(this);

private slots:
    void collectErrorData(QSerialPort::SerialPortError error);
    void serialConnReceiveMessage();
    void timeout();
};

#endif // OVENCOMM_H
