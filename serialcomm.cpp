#include "serialcomm.h"

SerialComm::SerialComm(QObject *parent) : QObject(parent)
{
    connect(&serial_conn, &QSerialPort::errorOccurred, this, &SerialComm::collectErrorData);
    connect(&timeout_timer, &QTimer::timeout, this, &SerialComm::timeout);
    timeout_timer.setSingleShot(true);
    send_message_timer.setInterval(250);
}

void SerialComm::openSerialPort() {
    if (serial_conn.open(QIODevice::ReadWrite)) {
        QString successMessage = QString("Connected to %1 : %2, %3, %4, %5, %6")
                .arg(serial_conn.portName()).arg(serial_conn.baudRate())
                .arg(serial_conn.dataBits()).arg(serial_conn.parity())
                .arg(serial_conn.stopBits()).arg(serial_conn.flowControl());
        qDebug() << successMessage;
        send_message_timer.start();
    }
}

void SerialComm::closeSerialPort() {
    // Reset data related vars
    command_queue.clear();
    data_queue.clear();
    temp_data = "";
    timeout_timer.stop();
    send_message_timer.stop();

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

void SerialComm::startSendMessageTimer() {
    if (!send_message_timer.isActive()) {
        send_message_timer.start();
    }
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
