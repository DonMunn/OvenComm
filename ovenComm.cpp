#include "ovenComm.h"
#include <QSerialPort>
#include <QDebug>
#include "settingsdialog.h"
#include <QRegExp>
#include <QTimer>
#include <QThread>

OvenComm::OvenComm() {
    connect(&serial_conn, &QSerialPort::readyRead, this, &OvenComm::serialConnReceiveMessage);
}

void OvenComm::sendError(QSerialPort::SerialPortError error, const QString &error_message) {
    // clear serial internal read/write buffers
    if (isOpen()) {
        serial_conn.clear();
    }

    // Dequeue command if one is associated with the error
    if (command_queue.isEmpty()) {
        emit errorSignal(error, error_message, commands::NONE);
    } else {
        data_queue.dequeue();
        temp_data = "";
        emit errorSignal(error, error_message, command_queue.dequeue());
    }
}

void OvenComm::serialConnReceiveMessage() {
    // complete data example: *01f4fb^
    // construct message from parts
    temp_data += serial_conn.readAll();

    if(QRegExp("^\\*[a-fA-F0-9]{6}\\^$").exactMatch(temp_data)) {
        emit rawDataSignal(temp_data);
        timer.stop();

        if (verifyChecksum()) {
            temp_data = "";
            data_queue.dequeue();
            command_queue.dequeue();

            // send another message when previous is finished
            if (!command_queue.isEmpty()) {
                serialConnSendMessage();
            }
        }
    }
}

void OvenComm::serialConnSendMessage() {
    //construct message
    QByteArray data = QString::number(command_queue.head()).rightJustified(2, '0').toUtf8();
    data += QString::number(data_queue.head(), 16).rightJustified(4, '0').toUtf8();
    //calculate checksum
    qint16 sum_of_bytes = 0;
    for(int i=0; i<data.length(); i++) {
        sum_of_bytes += ((QChar)data[i]).unicode();
    }
    data += QString::number(sum_of_bytes%256, 16).rightJustified(2, '0').toUtf8();
    data += '\r';
    data.prepend('*');

    qDebug() << "final data:" << data;

    if (serial_conn.write(data) == -1) { // -1 indicates error occurred
        // send QSerialPort::NotOpenError if QOIDevice::NotOpen is triggered
        if (serial_conn.error() == QSerialPort::NoError) {
            sendError(QSerialPort::NotOpenError, "No open connection");
        } //else UNNEEDED as the QSerialPort will emit its own signal for other errors
    } else {
        timer.start(1000);
    }
}

bool OvenComm::verifyChecksum() {
    const QString checksum = temp_data.mid(5,2);
    const QString data = temp_data.mid(1,4);

    // Verify checksum matches the data received
    qint16 sum_of_bytes = 0;
    for (int i=0; i<data.length(); i++) {
        sum_of_bytes += data[i].unicode();
    }

    if ((sum_of_bytes%256) != checksum.toInt(nullptr, 16)) {
        sendError(QSerialPort::ParityError, "Checksum mismatched");
        return false;
    }

    int return_data = data.toInt(nullptr, 16);

    qDebug() << " read data:" << return_data;
    //emit displayDataSignal(return_data, command_queue.head());
    return true;
}

void OvenComm::setTemp(double temp) {
    if (isOpen()) {
        if(!command_queue.isEmpty()) {
            command_queue.enqueue(SETTEMP);
            data_queue.enqueue(temp*100.0);
        } else {
            command_queue.enqueue(SETTEMP);
            data_queue.enqueue(temp*100.0);
            serialConnSendMessage();
        }
    } else {
        sendError(QSerialPort::NotOpenError, "No open connection");
    }
}


void OvenComm::getTemp() {
    if (isOpen()) {
        if(!command_queue.isEmpty()) {
            command_queue.enqueue(GETTEMP);
            data_queue.enqueue(0);
        } else {
            command_queue.enqueue(GETTEMP);
            data_queue.enqueue(0);
            serialConnSendMessage();
        }
    } else {
        sendError(QSerialPort::NotOpenError, "No open connection");
    }
}


void OvenComm::getSetTemp() {
    if (isOpen()) {
        if(!command_queue.isEmpty()) {
            command_queue.enqueue(GETSETTEMP);
            data_queue.enqueue(0);
        } else {
            command_queue.enqueue(GETSETTEMP);
            data_queue.enqueue(0);
            serialConnSendMessage();
        }
    } else {
        sendError(QSerialPort::NotOpenError, "No open connection");
    }
}


void OvenComm::getOutput() {
    if (isOpen()) {
        if(!command_queue.isEmpty()) {
            command_queue.enqueue(GETOUTPUT);
            data_queue.enqueue(0);
        } else {
            command_queue.enqueue(GETOUTPUT);
            data_queue.enqueue(0);
            serialConnSendMessage();
        }
    } else {
        sendError(QSerialPort::NotOpenError, "No open connection");
    }
}


void OvenComm::getSensorStatus() {
    if (isOpen()) {
        if(!command_queue.isEmpty()) {
            command_queue.enqueue(GETSENSORSTATUS);
            data_queue.enqueue(0);
        } else {
            command_queue.enqueue(GETSENSORSTATUS);
            data_queue.enqueue(0);
            serialConnSendMessage();
        }
    } else {
        sendError(QSerialPort::NotOpenError, "No open connection");
    }
}


void OvenComm::setPowerStatus(bool on) {
    if (isOpen()) {
        if(!command_queue.isEmpty()) {
            command_queue.enqueue(SETPOWERSTATUS);
            data_queue.enqueue(on);
        } else {
            command_queue.enqueue(SETPOWERSTATUS);
            data_queue.enqueue(on);
            serialConnSendMessage();
        }
    } else {
        sendError(QSerialPort::NotOpenError, "No open connection");
    }
}


void OvenComm::getPowerStatus() {
    if (isOpen()) {
        if(!command_queue.isEmpty()) {
            command_queue.enqueue(GETPOWERSTATUS);
            data_queue.enqueue(0);
        } else {
            command_queue.enqueue(GETPOWERSTATUS);
            data_queue.enqueue(0);
            serialConnSendMessage();
        }
    } else {
        sendError(QSerialPort::NotOpenError, "No open connection");
    }
}
