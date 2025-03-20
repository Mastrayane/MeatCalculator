#include "androidwindow.h"
#include "ui_androidwindow.h"
#include <QScreen>


AndroidWindow::AndroidWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AndroidWindow)
{
    ui->setupUi(this);

    // Получаем доступную геометрию экрана (без учета системных панелей)
    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();

    // Устанавливаем размер окна равным размеру экрана
    this->setFixedSize(screenGeometry.size());

    // Центрируем окно (опционально)
    this->move(screenGeometry.topLeft());

    // Установка фонового изображения
    SetFolder(":/background/images/background.png");
    FitImage();
}

AndroidWindow::~AndroidWindow()
{
    delete ui;
}

void AndroidWindow::SetPixmap(const QString path) {
    active_pixmap = QPixmap(path);
}

void AndroidWindow::SetFolder(const QString &d) {
    SetPixmap(d);
}

void AndroidWindow::FitImage() {
    if (active_pixmap.isNull()) return;

    // Получаем текущий размер окна
    QSize windowSize = this->size();

    // Масштабируем изображение с сохранением пропорций
    QPixmap scaledPix = active_pixmap.scaled(
        windowSize,
        Qt::KeepAspectRatioByExpanding, // Заполняет экран без искажений
        Qt::SmoothTransformation
        );

    // Центрируем изображение
    int x = (windowSize.width() - scaledPix.width()) / 2;
    int y = (windowSize.height() - scaledPix.height()) / 2;

    lbl_new_.setPixmap(scaledPix);
    lbl_new_.setFixedSize(scaledPix.size());
    lbl_new_.move(x, y); // Центрируем вместо (0, 0)
}

void AndroidWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    FitImage();
}

void AndroidWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    FitImage();
}

