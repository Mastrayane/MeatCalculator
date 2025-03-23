#include "mainwindow.h"
#include "androidwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


#ifdef Q_OS_ANDROID
    AndroidWindow w; // Создаем окно для Android
#else
    MainWindow w; // Создаем окно для десктопной версии
#endif
    w.show();
    return a.exec();
}
