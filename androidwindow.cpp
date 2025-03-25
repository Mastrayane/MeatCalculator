#include "androidwindow.h"
#include "ui_androidwindow.h"
#include <QScreen>
#include <QSvgRenderer>
#include <QPainter>
#include <QTimer>
#include <QFile>
#include <QPointer>


AndroidWindow::AndroidWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AndroidWindow)
{
    ui->setupUi(this);

    SetInterface();
}

AndroidWindow::~AndroidWindow()
{
    delete ui;
}

void AndroidWindow::SetInterface()
{
    // Получаем доступную геометрию экрана (без учета системных панелей)
    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();

    // Устанавливаем размер окна равным размеру экрана
    this->setFixedSize(screenGeometry.size());

    // Центрируем окно (опционально)
    this->move(screenGeometry.topLeft());

    SetFolder(":/background/images/background_blurry.png"); // Установка фонового изображения
    SetPixmap(ui->le_heading, ":/img_android/images/android/heading_android.png"); // установка надписи заголовка
    SetPixmap(ui->widget_substrate, ":/background/images/substrate.png"); // установка плашки подолжки

    // Устананавливаем блок ввода данных
    ui->le_input_total_weight->setStyleSheet("background: white; color: black;");
    ui->le_input_cup_weight->setStyleSheet("background: white; color: black;");

    SetPixmap(ui->lb_total_weight, ":/svg/images/svg/text_total_weight.svg", 2.0);
    SetPixmap(ui->lb_cup_weight, ":/svg/images/svg/text_cup_weight.svg", 2.0);

    // Устанавливаем изображение для кнопки
    ui->pushButton->setIcon(QIcon(":/background/images/calculate_button.png"));
    ui->pushButton->setIconSize(QSize(300, 100));

    // Устанавливаем блок вывода результатов
    SetPixmap(ui->lb_1_output, ":/svg/images/svg/result_text_1.svg", 1.5);
    SetPixmap(ui->lb_2_output, ":/svg/images/svg/result_text_2.svg", 1.5);
    SetPixmap(ui->lb_3_output, ":/svg/images/svg/result_text_3.svg", 1.5);

    ui->le_1_output->setStyleSheet("background: white; color: rgb(255,107,85)");
    ui->le_2_output->setStyleSheet("background: white; color: rgb(255,107,85)");
    ui->le_3_output->setStyleSheet("background: white; color: rgb(255,107,85)");

    FitImage();
}

void AndroidWindow::SetPixmap(const QString path) {
    active_pixmap = QPixmap(path);
    if (!active_pixmap.isNull()) {
        // Применяем эффект виньетирования
        active_pixmap = applyVignetteEffect(active_pixmap);
        FitImage();
    }
}

QPixmap AndroidWindow::applyVignetteEffect(const QPixmap &original, float intensity, float radius)
{
    QImage image = original.toImage();
    if (image.isNull()) return original;

    int width = image.width();
    int height = image.height();
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;
    float maxDist = qSqrt(centerX * centerX + centerY * centerY) * radius;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float dist = qSqrt((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));
            float factor = qMax(0.0f, 1.0f - intensity * (dist / maxDist));

            QRgb pixel = image.pixel(x, y);
            int r = qRed(pixel) * factor;
            int g = qGreen(pixel) * factor;
            int b = qBlue(pixel) * factor;

            image.setPixel(x, y, qRgb(r, g, b));
        }
    }

    return QPixmap::fromImage(image);
}

// Новая реализация метода с параметрами виньетирования
void AndroidWindow::SetPixmapWithVignette(const QString& path,
                                          float intensity,
                                          float radius)
{
    active_pixmap = QPixmap(path);
    if (!active_pixmap.isNull()) {
        active_pixmap = applyVignetteEffect(active_pixmap, intensity, radius);
        FitImage();
    }
}

void AndroidWindow::SetPixmap(QWidget* widget, const QString& path, double multiplicity_size) {
    if (path.endsWith(".svg", Qt::CaseInsensitive)) {
        // Для SVG используем QSvgRenderer
        QSvgRenderer renderer(path);
        if (!renderer.isValid()) return;

        // Увеличьте размер SVG-изображения
        QSize widgetSize = widget->size();
        QSize scaledSize = widgetSize * multiplicity_size; // Увеличьте в 2 раза (или другое значение)

        QPixmap pix(scaledSize);
        pix.fill(Qt::transparent); // Прозрачный фон
        QPainter painter(&pix);
        renderer.render(&painter, pix.rect());

        if (auto label = qobject_cast<QLabel*>(widget)) {
            label->setPixmap(pix);
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet("background: transparent;");
        } else {
            widget->setStyleSheet(
                QString("background-image: url(%1); background-repeat: no-repeat;"
                        "background-position: center; background-color: transparent;")
                    .arg(path)
                );
        }
    } else {
        // Для растровых изображений (PNG, JPEG и т.д.)
        QPixmap pix(path);
        if (pix.isNull()) return;

        QPixmap scaledPix = pix.scaled(widget->size(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

        if (auto label = qobject_cast<QLabel*>(widget)) {
            label->setPixmap(scaledPix);
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet("background: transparent;");
        } else {
            widget->setStyleSheet(
                QString("background-image: url(%1); background-repeat: no-repeat;"
                        "background-position: center; background-color: transparent;")
                    .arg(path)
                );
        }
    }
}

void AndroidWindow::SetFolder(const QString &d) {
    SetPixmapWithVignette(d); // Используем значения по умолчанию
}

void AndroidWindow::MakeCalculations()
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

void AndroidWindow::addImageTransition(QSequentialAnimationGroup *group,
                                       const AnimationFrame& frame)
{
    if (!QFile::exists(frame.imagePath)) {
        qWarning() << "Image file not found:" << frame.imagePath;
        return;
    }

    QPropertyAnimation *delay = new QPropertyAnimation(this, nullptr);
    delay->setDuration(frame.duration);
    group->addAnimation(delay);

    connect(delay, &QPropertyAnimation::finished, this, [this, frame]() {
        SetPixmapWithVignette(frame.imagePath,
                              frame.vignetteIntensity,
                              frame.vignetteRadius);
    });
}

QList<QWidget*> AndroidWindow::getAllWidgets()
{
    return QList<QWidget*>{
        ui->le_heading,
        ui->widget_substrate,
        ui->le_input_total_weight,
        ui->le_input_cup_weight,
        ui->lb_total_weight,
        ui->lb_cup_weight,
        ui->pushButton,
        ui->lb_1_output,
        ui->lb_2_output,
        ui->lb_3_output,
        ui->le_1_output,
        ui->le_2_output,
        ui->le_3_output
    };
}

void AndroidWindow::setWidgetsVisible(bool visible)
{
    for (auto widget : getAllWidgets()) {
        widget->setVisible(visible);
    }
}

void AndroidWindow::on_pushButton_clicked()
{
    // Создаем последовательность анимаций
    QSequentialAnimationGroup *animationGroup = new QSequentialAnimationGroup(this);

    // Первое изображение с сильным виньетированием
    SetPixmapWithVignette(":/background/images/background_click_2.png", 0.7f, 0.5f);

    // Определяем кадры анимации с разными параметрами виньетирования
    QVector<AnimationFrame> animationFrames = {
        {":/background/images/background_click_3.png", 0.6f, 0.6f, 200}, // Среднее виньетирование
        {":/background/images/background_click_4.png", 0.5f, 0.7f, 200},  // Сильное виньетирование
        {":/background/images/background_click_5.png", 0.4f, 0.8f, 200}, // Слабое виньетирование
        {":/background/images/background_click_6.png", 0.3f, 0.9f, 300},  // Сильное виньетирование
        {":/background/images/backgroundbackground_click_7.png", 0.0f, 1.0f, 1000},  // Очень слабое виньетирование
       // {":/background/images/background_click_8.png", 0.0f, 1.0f, 1000},
        {":/background/images/background_click_9.png", 0.0f, 1.0f, 1500},
        {":/background/images/background_click_10.png", 0.0f, 1.0f, 1000},

    };

    // Добавляем все кадры в анимацию
    for (const auto& frame : animationFrames) {
        addImageTransition(animationGroup, frame);
    }

    // Кадр перед последним - скрываем виджеты
    QPropertyAnimation *hideWidgets = new QPropertyAnimation(this, nullptr);
    hideWidgets->setDuration(0); // Мгновенное действие
    animationGroup->addAnimation(hideWidgets);
    connect(hideWidgets, &QPropertyAnimation::finished, this, [this]() {
        setWidgetsVisible(false); // Скрываем все виджеты
    });

    // Последний кадр - показываем виджеты и устанавливаем финальное изображение
    AnimationFrame finalFrame = {":/background/images/background_blurry.png", 0.7f, 0.5f, 321};
    addImageTransition(animationGroup, finalFrame);

    QPropertyAnimation *showWidgets = new QPropertyAnimation(this, nullptr);
    showWidgets->setDuration(0); // Мгновенное действие
    animationGroup->addAnimation(showWidgets);
    connect(showWidgets, &QPropertyAnimation::finished, this, [this]() {
        setWidgetsVisible(true); // Показываем все виджеты
    });

    // Завершение
    connect(animationGroup, &QSequentialAnimationGroup::finished, this, [this]() {
        MakeCalculations();
        sender()->deleteLater();
    });

    animationGroup->start();
}




void AndroidWindow::on_pushButton_released()
{
    // Возвращаем исходное изображение при отпускании
    ui->pushButton->setIcon(QIcon(":/background/images/calculate_button.png"));
    ui->pushButton->setIconSize(QSize(300, 100));
    // SetFolder(":/background/images/background_blurry.png");
   // FitImage();
}


void AndroidWindow::on_pushButton_pressed()
{
    // Меняем изображение при нажатии
   // ui->pushButton->setIcon(QIcon(":/background/images/calculate_button_press.png"));
   // ui->pushButton->setIconSize(QSize(300, 100));

}


