/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"
#include "ovenComm.h"

#include <QLabel>
#include <QMessageBox>

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_settings(new SettingsDialog),
    o_serial(new OvenComm)


{

    m_ui->setupUi(this);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);
    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();

    connect(o_serial, &OvenComm::errorSignal, this, &MainWindow::handleError);
    connect(o_serial, &OvenComm::rawDataSignal, this, &MainWindow::displayRawData);
    connect(o_serial, &OvenComm::displayDataSignal, this, &MainWindow::displayData);
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

void MainWindow::openSerialPort() //Done
{
    o_serial->updateSerialInfo(m_settings->settings());
    o_serial->openSerialPort();
    if (o_serial->isOpen()) {
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected"));
    }
}

void MainWindow::closeSerialPort()
{
    o_serial->closeSerialPort();
    if (o_serial->isOpen() == false) {
        m_ui->actionConnect->setEnabled(true);
        m_ui->actionDisconnect->setEnabled(false);
        m_ui->actionConfigure->setEnabled(true);
        showStatusMessage(tr("Disconnected"));
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About OvenComm "),
                       tr("The <b>CommTest</b> Sends commands to Oven Ind "
                          "card and interacts "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}


void MainWindow::displayRawData(QString data)
{
    m_ui->plainTextEdit->appendPlainText(data);
}





void MainWindow::handleError(QSerialPort::SerialPortError error, QString error_string, OvenComm::commands command_sent)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), error_string);
        closeSerialPort();
    } else {
        qDebug() << error;
        QMessageBox::warning(this, tr("Warning"), error_string);
    }
}


void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_console, &Console::clear);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}



void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

void MainWindow::displayData(int data, OvenComm::commands command_sent) {
    switch(command_sent) {
        case OvenComm::GETOUTPUT:
            m_ui->lcdNumberSensorStatus->display((double)data / 28800.0);
            break;
        case OvenComm::GETTEMP:
            m_ui->lcdNumberCurrentTemp->display((double)data / 100.0);
            break;
        case OvenComm::GETSETTEMP:
            m_ui->lcdNumberSetTemp->display((double)data / 100.0);
            break;
        case OvenComm::GETPOWERSTATUS:
            m_ui->lcdNumberPowerStatus->display((bool)data);
            break;
        case OvenComm::GETSENSORSTATUS:
            m_ui->lcdNumberSensorStatus->display((bool)data);
            break;
    }
}

void MainWindow::on_pushButtonSetTemp_clicked()
{
    o_serial->setTemp(m_ui->spinBoxSetTemp->value());
}

void MainWindow::on_pushButtonReadTemp_clicked()
{
    o_serial->getTemp();
}

void MainWindow::on_pushButtonFSetTemp_clicked()
{
    o_serial->setTemp(40.0);
}


void MainWindow::on_actionAbout_triggered()
{
    about();
}


void MainWindow::on_pushButtonReadSet_clicked()
{
    o_serial->getSetTemp();
}

void MainWindow::on_pushButtonReadSensorStatus_clicked() {
    o_serial->getOutput();
}

void MainWindow:: on_pushButtonReadPowerStatus_clicked() {
    o_serial->getPowerStatus();
}

