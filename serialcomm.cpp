#include "serialcomm.h"

SerialComm::SerialComm(QObject *parent) : QObject(parent)
{
    connect(&serial_conn, &QSerialPort::errorOccurred, this, &SerialComm::collectErrorData);
    connect(&timer, &QTimer::timeout, this, &SerialComm::timeout);
    timer.setSingleShot(true);
}

void SerialComm::openSerialPort() {
    if (serial_conn.open(QIODevice::ReadWrite)) {
        QString successMessage = QString("Connected to %1 : %2, %3, %4, %5, %6")
                .arg(serial_conn.portName()).arg(serial_conn.baudRate())
                .arg(serial_conn.dataBits()).arg(serial_conn.parity())
                .arg(serial_conn.stopBits()).arg(serial_conn.flowControl());
        qDebug() << successMessage;
    }
}

void SerialComm::closeSerialPort() {
    // Reset data related vars
    command_queue.clear();
    data_queue.clear();
    temp_data = "";
    timer.stop();

    if (isOpen()) {
        serial_conn.clear();
        serial_conn.close();
        qDebug() << "Disconnected";
    } else {
        qDebug() << "No open connection";
    }
}

bool SerialComm::isOpen() {
    return serial_conn.isOpen();
}

void SerialComm::updateSerialInfo(const SettingsDialog::Settings &settings) {
    serial_conn.setPortName(settings.name);
    serial_conn.setBaudRate(settings.baudRate);
    serial_conn.setDataBits(settings.dataBits);
    serial_conn.setParity(settings.parity);
    serial_conn.setStopBits(settings.stopBits);
    serial_conn.setFlowControl(settings.flowControl);
}

//Private
void SerialComm::collectErrorData(QSerialPort::SerialPortError error) {
    //clearError causes another NoError signal to be sent
    if (error != QSerialPort::NoError) {
        sendError(error, serial_conn.errorString());
        serial_conn.clearError();
    }
}

void SerialComm::timeout() {
    if (temp_data != "") {
        qDebug() << "Return data:" << temp_data;
        sendError(QSerialPort::TimeoutError, "Timeout partial data");
    }
}
