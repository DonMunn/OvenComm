#ifndef OVENCOMM_H
#define OVENCOMM_H

#include "serialcomm.h"
#include "settingsdialog.h"


class OvenComm : public SerialComm
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
    explicit OvenComm(QObject *parent = nullptr);

    void setTemp(double temp); //Done
    void getTemp(); //Done
    void getSetTemp(); //Done

    void getOutput(); //Done
    void getSensorStatus(); //Done

    void setPowerStatus(bool on); //Done
    void getPowerStatus(); //Done

protected:
    void sendError(QSerialPort::SerialPortError error, const QString &error_message) override;
    void serialConnSendMessage() override;

private:
    bool verifyChecksum();

private slots:
    void serialConnReceiveMessage() override;
};

#endif // OVENCOMM_H
