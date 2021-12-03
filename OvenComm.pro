QT += widgets serialport
requires(qtConfig(combobox))

TARGET = OvenCommTest
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ovenComm.cpp \
    settingsdialog.cpp

HEADERS += \
    mainwindow.h \
    ovenComm.h \
    settingsdialog.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    OvenComm.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/serialport/terminal
INSTALLS += target
