QT += widgets serialport
requires(qtConfig(combobox))

TARGET = OvenCommTest
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ovenComm.cpp \
    serialcomm.cpp \
    settingsdialog.cpp

HEADERS += \
    mainwindow.h \
    ovenComm.h \
    serialcomm.h \
    settingsdialog.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    OvenComm.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/serialport/terminal
INSTALLS += target
