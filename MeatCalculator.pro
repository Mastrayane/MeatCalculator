QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



CONFIG += c++17
CONFIG += console



# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    androidwindow.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    androidwindow.h \
    mainwindow.h

FORMS += \
    androidwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

# Укажите поддерживаемые архитектуры процессоров
ANDROID_ABIS = arm64-v8a # x86 x86_64

# Минимальная версия Android (API 21 = Android 5.0 Lollipop)
ANDROID_MIN_SDK_VERSION = 21

# Целевая версия Android (API 33 = Android 13)
ANDROID_TARGET_SDK_VERSION = 34

TEMPLATE = app

# Указываем папку с Android-ресурсами (опционально)
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
