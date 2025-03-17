#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <vector>

// Функция подгоняет изображение под нужный размер окна.
QPixmap ResizeImgToFit(const QPixmap &src, int window_width, int window_height) {
    int img_w = src.width();
    int img_h = src.height();

    double w_ratio = double(img_w) / window_width;
    double h_ratio = double(img_h) / window_height;

    if ( w_ratio > h_ratio ) {
        return src.scaledToHeight(window_height);
    } else {

        return src.scaledToWidth(window_width);

    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifndef Q_OS_ANDROID
    setMinimumSize(500, 600); // Только для десктопных платформ
#endif

    SetInterface();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetInterface()
{
    // Принудительно обновляем макет перед установкой изображений
    ui->centralwidget->layout()->activate();
    qApp->processEvents(); // Даем приложению обработать изменения


    // Найти внутренний виджет внутри widget_substrate
    QWidget* innerWidget = ui->widget_substrate->findChild<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    if (innerWidget) {
        innerWidget->setStyleSheet("background: transparent;");
        innerWidget->setAttribute(Qt::WA_TranslucentBackground);
    }

    SetFolder(":/background/images/background.png"); // установка фонового изображения
    SetPixmap(ui->le_heading, ":/background/images/heading.png"); // установка надписи заголовка
    SetPixmap(ui->widget_substrate, ":/background/images/substrate.png"); // установка плашки подолжки


    // Устананавливаем блок ввода данных
    ui->le_input_total_weight->setStyleSheet("background: white; color: black;");
    ui->le_input_cup_weight->setStyleSheet("background: white; color: black;");

    SetPixmap(ui->lb_total_weight, ":/background/images/text_total_weight.png");
    SetPixmap(ui->lb_cup_weight, ":/background/images/text_cup_weight.png");


    // Устанавливаем блок вывода результатов
    SetPixmap(ui->lb_1_output, ":/background/images/result_text_1.png");
    SetPixmap(ui->lb_2_output, ":/background/images/result_text_2.png");
    SetPixmap(ui->lb_3_output, ":/background/images/result_text_3.png");

    ui->le_1_output->setStyleSheet("background: white; color: rgb(255,107,85)");
    ui->le_2_output->setStyleSheet("background: white; color: rgb(255,107,85)");
    ui->le_3_output->setStyleSheet("background: white; color: rgb(255,107,85)");

    // Устанавливаем изображение для кнопки
    ui->pushButton->setIcon(QIcon(":/background/images/calculate_button.png"));
    ui->pushButton->setIconSize(QSize(300, 100));

    // Отложенный вызов для корректного обновления размеров
    QTimer::singleShot(100, this, [this]() {
        FitImage();
    });
}

void MainWindow::SetPixmap(const QString path) {
    active_pixmap = QPixmap(path);
}

void MainWindow::SetPixmap(QWidget* widget, const QString& path) {
    QPixmap pix(path);
    if (pix.isNull()) return;

    QPixmap scaledPix = pix.scaled(widget->size(),
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation);

    if (auto label = qobject_cast<QLabel*>(widget)) {
        label->setPixmap(scaledPix);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("background: transparent;");
        label->setScaledContents(false); // Отключено авторастяжение
    } else {
        widget->setStyleSheet(
            QString("background-image: url(%1); background-repeat: no-repeat;"
                    "background-position: center; background-color: transparent;")
                .arg(path)
            );
    }
}

void MainWindow::FitImage()
{
    Q_ASSERT(!active_pixmap.isNull());

    // Сперва получить размер окна
    auto width = this->width();
    auto height = this->height();
    // 1. Вызовите ResizeImgToFit.
    auto active_pixmap_resize = ResizeImgToFit(active_pixmap, width, height);
    // 2. Поместите изображение в lbl_img.
    lbl_new_.setPixmap(active_pixmap_resize);
    // 3. Измените размер lbl_img.
    lbl_new_.setFixedSize(active_pixmap_resize.size());
    // 4. Переместите lbl_img, пользуясь формулами из условия.
    int x = (width - active_pixmap_resize.width()) / 2;
    int y = (height - active_pixmap_resize.height()) / 2;
    lbl_new_.move(x, y);

    UpdateQLabelSize();

}


void MainWindow::SetFolder(const QString &d)
{
    SetPixmap(d);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
QMainWindow::resizeEvent(event);
    FitImage();
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    // Принудительно обновляем размеры после отображения окна
    FitImage();
}

void MainWindow::UpdateQLabelSize()
{

    // Принудительно обновляем макет перед масштабированием
    ui->centralwidget->layout()->activate();
    qApp->processEvents();

    // Принудительно обновляем геометрию всех меток перед масштабированием
    ui->lb_total_weight->updateGeometry();
    ui->lb_cup_weight->updateGeometry();
    ui->lb_1_output->updateGeometry();
    ui->lb_2_output->updateGeometry();
    ui->lb_3_output->updateGeometry();

    // Массив пар: указатель на QLabel и путь к изображению
    const QList<QPair<QLabel*, QString>> labels = {
        {ui->le_heading,       ":/background/images/heading.png"},
        {ui->lb_total_weight,  ":/background/images/text_total_weight.png"},
        {ui->lb_cup_weight,    ":/background/images/text_cup_weight.png"},
        {ui->lb_1_output,      ":/background/images/result_text_1.png"},
        {ui->lb_2_output,      ":/background/images/result_text_2.png"},
        {ui->lb_3_output,      ":/background/images/result_text_3.png"}
    };

    for (const auto& [label, path] : labels) {
        if (!label) continue;

        QPixmap pixmap(path);
        if (pixmap.isNull()) continue;

        // Масштабируем с учетом доступного пространства
        QPixmap scaled = pixmap.scaled(
            label->size(),
            Qt::KeepAspectRatio, // Используем KeepAspectRatio
            Qt::SmoothTransformation
            );

        label->setPixmap(scaled);
        label->setAlignment(Qt::AlignCenter);
    }
}

void MainWindow::MakeCalculations()
{
    bool ok;
    int int_total_weight = ui->le_input_total_weight->text().toInt(&ok);

    if (ok) {
        // Преобразование прошло успешно, можно использовать значение t
        qDebug() << "Введенное число:" << int_total_weight;
    } else {
        // Преобразование не удалось, возможно, введен некорректный текст
        qDebug() << "Ошибка: введено не число!";
    }

    int int_cup_weight = ui->le_input_cup_weight->text().toInt(&ok);

    if (ok) {
        // Преобразование прошло успешно, можно использовать значение t
        qDebug() << "Введенное число:" << int_cup_weight;
    } else {
        // Преобразование не удалось, возможно, введен некорректный текст
        qDebug() << "Ошибка: введено не число!";
    }


    int weight_day = int_total_weight / 4;

    std::vector<QLineEdit*> le_outputs = {ui->le_1_output, ui->le_2_output, ui->le_3_output};

    for (auto le_output : le_outputs) {
        int_total_weight -= weight_day;
        le_output->setText(QString::number(int_total_weight + int_cup_weight));
    }
}



void MainWindow::on_pushButton_clicked()
{
    MakeCalculations();
}


void MainWindow::on_pushButton_released()
{
    // Возвращаем исходное изображение при отпускании
    ui->pushButton->setIcon(QIcon(":/background/images/calculate_button.png"));
    ui->pushButton->setIconSize(QSize(300, 100));
}


void MainWindow::on_pushButton_pressed()
{
    // Меняем изображение при нажатии
    ui->pushButton->setIcon(QIcon(":/background/images/calculate_button_press.png"));
    ui->pushButton->setIconSize(QSize(300, 100));
}

