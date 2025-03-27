#include "androidwindow.h"
#include "ui_androidwindow.h"
#include <QScreen>
#include <QSvgRenderer>
#include <QPainter>
#include <QTimer>
#include <QFile>
#include <QPointer>
#include <QEasingCurve>
#include <QPixmapCache>
#include <QResizeEvent>

AndroidWindow::AndroidWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AndroidWindow)
{
    ui->setupUi(this);
    SetInterface(); // Инициализация интерфейса при создании окна
}

AndroidWindow::~AndroidWindow()
{
    delete ui; // Освобождаем ресурсы UI
}

// Инициализация пользовательского интерфейса
void AndroidWindow::SetInterface()
{
    // Настройка размеров окна под экран
    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    this->setFixedSize(screenGeometry.size());
    this->move(screenGeometry.topLeft());

    // Установка начального фона
    SetFolder(":/background/images/background_blurry.png");

    // Настройка элементов интерфейса
    SetPixmap(ui->le_heading, ":/img_android/images/android/heading_android.png");
    SetPixmap(ui->widget_substrate, ":/background/images/substrate.png");

    // Стилизация полей ввода
    ui->le_input_total_weight->setStyleSheet("background: white; color: black;");
    ui->le_input_cup_weight->setStyleSheet("background: white; color: black;");

    // Установка текстовых меток
    SetPixmap(ui->lb_total_weight, ":/svg/images/svg/text_total_weight.svg", 2.0);
    SetPixmap(ui->lb_cup_weight, ":/svg/images/svg/text_cup_weight.svg", 2.0);

    // Настройка кнопки
    ui->pushButton->setIcon(QIcon(":/background/images/calculate_button.png"));
    ui->pushButton->setIconSize(QSize(300, 100));

    // Настройка блоков вывода
    SetPixmap(ui->lb_1_output, ":/svg/images/svg/result_text_1.svg", 1.5);
    SetPixmap(ui->lb_2_output, ":/svg/images/svg/result_text_2.svg", 1.5);
    SetPixmap(ui->lb_3_output, ":/svg/images/svg/result_text_3.svg", 1.5);

    // Стилизация полей вывода
    ui->le_1_output->setStyleSheet("background: white; color: rgb(255,107,85)");
    ui->le_2_output->setStyleSheet("background: white; color: rgb(255,107,85)");
    ui->le_3_output->setStyleSheet("background: white; color: rgb(255,107,85)");

    FitImage(); // Подгонка изображения под размер окна

    PreloadAnimationFrames(); // Предзагрузка всех ресурсов анимации
}

void AndroidWindow::PreloadAnimationFrames() {
    // Инициализируем параметры анимации
    m_animationFrames = {
        {":/background/images/background_click_2.png", 0.4f, 0.8f, 0,   false, "Начальный кадр"},
        {":/background/images/background_click_3.png", 0.4f, 0.8f, 700, false, "Кадр 3"},
        {":/background/images/background_click_4.png", 0.4f, 0.8f, 700, false, "Кадр 4"},
        {":/background/images/background_click_5.png", 0.4f, 0.8f, 700, false, "Кадр 5"},
        {":/background/images/background_click_6.png", 0.3f, 0.9f, 700, false, "Кадр 6"},
        {":/background/images/background_click_7.png", 0.3f, 0.9f, 700, false, "Кадр 7"},
        {":/background/images/background_click_8.png", 0.3f, 0.9f, 700, false, "Кадр 8"},
        {":/background/images/background_click_9.png", 0.0f, 1.0f, 700, true,  "Кадр 9 (скрытие)"},
        {":/background/images/background_blurry.png",  0.7f, 0.5f, 0,   false, "Базовый фон"}
    };

    // Очищаем кэши
    QPixmapCache::clear();
    m_animationCache.clear();

    // Предзагрузка с учетом параметров
    for (const AnimationFrame& frame : m_animationFrames) {
        QPixmap pix(frame.imagePath);

        if (!pix.isNull()) {
            // Применяем виньетирование
            QPixmap processed = applyVignetteEffect(pix,
                                                    frame.vignetteIntensity,
                                                    frame.vignetteRadius
                                                    );

            // Масштабируем
            processed = processed.scaled(
                this->size() * 1.2,
                Qt::KeepAspectRatioByExpanding,
                Qt::SmoothTransformation
                );

            // Ключ кэша: путь + параметры
            QString cacheKey = QString("%1_%2_%3")
                                   .arg(frame.imagePath)
                                   .arg(frame.vignetteIntensity)
                                   .arg(frame.vignetteRadius);

            QPixmapCache::insert(cacheKey, processed);
            m_animationCache.insert(cacheKey, processed);

            qDebug() << "Preprocessed:" << cacheKey;
        }
    }
}

// Создание временного overlay для плавных переходов
QLabel* AndroidWindow::createOverlayLabel()
{
    QLabel* overlay = new QLabel(this);
    overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    overlay->setAlignment(Qt::AlignCenter);
    overlay->resize(this->size());
    overlay->hide();
    return overlay;
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
    // Генерируем ключ кэша
    QString cacheKey = QString("%1_%2_%3")
                           .arg(path)
                           .arg(intensity)
                           .arg(radius);

    QPixmap pix;

    // Пытаемся получить из кэша
    if (m_animationCache.contains(cacheKey)) {
        pix = m_animationCache.value(cacheKey);
    }
    else if (!QPixmapCache::find(cacheKey, &pix)) {
        // Если нет в кэше, обрабатываем в реальном времени
        pix = QPixmap(path);
        pix = applyVignetteEffect(pix, intensity, radius);
        pix = pix.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        QPixmapCache::insert(cacheKey, pix);
    }

    if (!pix.isNull()) {
        active_pixmap = pix;
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

 /*
void AndroidWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    FitImage();
}
*/

// В обработчике изменения размера окна
void AndroidWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    // Пересоздаем кэш при изменении размера
    if (event->size() != event->oldSize()) {
        PreloadAnimationFrames();
    }

    FitImage();
}

void AndroidWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    FitImage();
}


// Плавный переход к новому изображению с эффектом виньетирования
void AndroidWindow::crossFadeToImage(const QString& newImagePath,
                                     float intensity,
                                     float radius,
                                     int duration)
{
#if DEBUG_ANIMATION
    qDebug() << "crossFadeToImage: начало для" << newImagePath;
#endif

    QLabel* overlay = createOverlayLabel();
#if DEBUG_ANIMATION
    qDebug() << "Создан overlay:" << overlay;
#endif

    QPixmap newPixmap(newImagePath);
    if (newPixmap.isNull()) {
#if DEBUG_ANIMATION
        qDebug() << "Ошибка: не удалось загрузить изображение" << newImagePath;
#endif
        overlay->deleteLater();
        return;
    }

    newPixmap = applyVignetteEffect(newPixmap, intensity, radius);
    newPixmap = newPixmap.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    overlay->setPixmap(newPixmap);
    overlay->show();
    overlay->raise();
#if DEBUG_ANIMATION
    qDebug() << "Overlay показан с новым изображением";
#endif

    QPropertyAnimation* fadeIn = new QPropertyAnimation(overlay, "windowOpacity");
    fadeIn->setDuration(duration);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
     fadeIn->setEasingCurve(QEasingCurve::InOutCubic);
    // fadeIn->setEasingCurve(QEasingCurve::InOutSine);
    // fadeIn->setEasingCurve(QEasingCurve::OutInElastic);
   // fadeIn->setEasingCurve(QEasingCurve::Linear);
   //  fadeIn->setEasingCurve(QEasingCurve::OutBack);

#if DEBUG_ANIMATION
    connect(fadeIn, &QPropertyAnimation::stateChanged, [](QAbstractAnimation::State newState, QAbstractAnimation::State oldState){
        qDebug() << "Состояние анимации изменилось:" << oldState << "->" << newState;
    });
#endif

    connect(fadeIn, &QPropertyAnimation::finished, this, [=]() {
#if DEBUG_ANIMATION
        qDebug() << "Анимация crossFadeToImage завершена для" << newImagePath;
#endif
        active_pixmap = newPixmap;
        lbl_new_.setPixmap(newPixmap);
        lbl_new_.setFixedSize(newPixmap.size());
#if DEBUG_ANIMATION
        qDebug() << "Основное изображение обновлено";
#endif
        overlay->deleteLater();
    });

    fadeIn->start(QPropertyAnimation::DeleteWhenStopped);
#if DEBUG_ANIMATION
    qDebug() << "Анимация crossFadeToImage запущена";
#endif
}

// Плавное появление/исчезновение виджетов
void AndroidWindow::fadeWidgets(bool fadeIn, int duration, std::function<void()> callback)
{
    // Убрать проверки на видимость для принудительного обновления
    qDebug() << (fadeIn ? "Принудительный показ" : "Принудительное скрытие")
             << "виджетов, длительность:" << duration;

    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);

    for (QWidget* widget : getAllWidgets()) {
        widget->setAttribute(Qt::WA_TransparentForMouseEvents, fadeIn ? false : true);
        if (fadeIn) widget->show();

        QPropertyAnimation* anim = new QPropertyAnimation(widget, "windowOpacity", group);
        anim->setDuration(duration);
        anim->setStartValue(fadeIn ? 0.0 : widget->windowOpacity());
        anim->setEndValue(fadeIn ? 1.0 : 0.0);
        anim->setEasingCurve(QEasingCurve::InOutSine);
        group->addAnimation(anim);
    }

    connect(group, &QParallelAnimationGroup::finished, [=]() {
        if (!fadeIn) {
            qDebug() << "Финализация скрытия виджетов";
            for (QWidget* widget : getAllWidgets()) widget->hide();
        }
        if (callback) callback();
        group->deleteLater();
    });

    group->start();
}

// Добавление комплексного перехода (фон + виджеты)
void AndroidWindow::addComplexTransition(QSequentialAnimationGroup* group,
                                         const AnimationFrame& frame)
{
    QParallelAnimationGroup* parallelGroup = new QParallelAnimationGroup(group);

    // Анимация фона
    QVariantAnimation* bgAnim = new QVariantAnimation(parallelGroup);
    bgAnim->setDuration(frame.duration);
    bgAnim->setStartValue(0);
    bgAnim->setEndValue(1);

    // Точка срабатывания - 90% прогресса анимации
    connect(bgAnim, &QVariantAnimation::valueChanged, [=](const QVariant &value) {
        if (frame.hideWidgets && !widgetsHidden && value.toFloat() >= 0.9f) {
            widgetsHidden = true;
            qDebug() << "Скрытие виджетов для кадра:" << frame.debugName;
            fadeWidgets(false, frame.duration/4, nullptr); // Укороченная длительность
        }
    });

    connect(bgAnim, &QVariantAnimation::finished, [=]() {
        crossFadeToImage(frame.imagePath, frame.vignetteIntensity,
                         frame.vignetteRadius, frame.duration);
    });
    parallelGroup->addAnimation(bgAnim);

    group->addAnimation(parallelGroup);
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

// Обработчик нажатия кнопки с анимацией
void AndroidWindow::on_pushButton_clicked()
{
    if (!currentAnimation.isNull() && currentAnimation->state() == QAbstractAnimation::Running) {
        qDebug() << "Анимация уже выполняется, пропускаем";
        return;
    }

    // Принудительный сброс состояния виджетов
    for (auto widget : getAllWidgets()) {
        widget->setWindowOpacity(1.0);
        widget->show();
    }
    widgetsHidden = false;

    qDebug() << "=== Новая анимация запущена ===";
    widgetsHidden = false; // Сбрасываем флаг перед началом новой анимации

    if (!currentAnimation.isNull()) {
        currentAnimation->stop();
        currentAnimation->deleteLater();
    }

    currentAnimation = new QSequentialAnimationGroup(this);
    currentAnimation->setObjectName("MainAnimationGroup");

    // Устанавливаем начальный кадр из кэша
    const AnimationFrame& firstFrame = m_animationFrames[0]; // background_click_2.png
    SetPixmapWithVignette(firstFrame.imagePath, firstFrame.vignetteIntensity, firstFrame.vignetteRadius);

    // Добавляем основные кадры анимации (со 2 по 8)
    for (int i = 1; i < m_animationFrames.size() - 1; ++i) {
        addComplexTransition(currentAnimation, m_animationFrames[i]);
    }

    // Возврат к базовому фону (последний кадр в списке)
    const AnimationFrame& finalFrame = m_animationFrames.last();
    QVariantAnimation* finalAnim = new QVariantAnimation(currentAnimation);
    finalAnim->setDuration(700);
    finalAnim->setStartValue(0);
    finalAnim->setEndValue(1);
    connect(finalAnim, &QVariantAnimation::finished, [=]() {
        crossFadeToImage(finalFrame.imagePath, finalFrame.vignetteIntensity, finalFrame.vignetteRadius, 100);


    // Явный принудительный показ виджетов
    QTimer::singleShot(50, [this]() {
        qDebug() << "Принудительный показ виджетов";
        for (auto widget : getAllWidgets()) {
            widget->show();
            widget->setWindowOpacity(1.0);
        }
        fadeWidgets(true, 700, [this]() {
            MakeCalculations();
            widgetsHidden = false;
            qDebug() << "Виджеты восстановлены";
        });
    });
 });
    currentAnimation->addAnimation(finalAnim);

    connect(currentAnimation, &QSequentialAnimationGroup::finished, this, [this]() {
        qDebug() << "Все анимации завершены";
        currentAnimation->deleteLater();
        currentAnimation.clear();
    });

    currentAnimation->start();
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
     ui->pushButton->setIcon(QIcon(":/background/images/calculate_button_press.png"));
     ui->pushButton->setIconSize(QSize(300, 100));

}
