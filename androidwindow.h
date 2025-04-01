#ifndef ANDROIDWINDOW_H
#define ANDROIDWINDOW_H

#define DEBUG_ANIMATION 1

#include <QMainWindow>
#include <QLabel>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QPointer>
#include <functional>
#include <QPauseAnimation>
#include <QtConcurrent/QtConcurrent>
#include <QMutex>
#include <QFutureWatcher>

namespace Ui {
class AndroidWindow;
}

// Структура для параметров анимации каждого кадра
struct AnimationFrame {
    QString imagePath;          // Путь к изображению кадра
    float vignetteIntensity;    // Интенсивность виньетирования
    float vignetteRadius;       // Радиус виньетирования
    int duration;               // Длительность анимации (мс)
    bool hideWidgets;           // Флаг скрытия виджетов во время анимации
    QString debugName;          // Отладочное имя кадра
};

class AndroidWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AndroidWindow(QWidget *parent = nullptr);
    ~AndroidWindow();

    void SetInterface(); // Инициализация интерфейса
    void PreloadAnimationFrames(); // Предзагрузка изображений для анимации в кэш

private:

     //--- МЕТОДЫ РАБОТЫ С ИЗОБРАЖЕНИЯМИ ---
    void SetPixmap(const QString path); // Установка фонового изображения
    QPixmap applyVignetteEffect(const QPixmap& original, float intensity = 0.7f, float radius = 0.5f); // Применение виньетки
    void SetPixmapWithVignette(const QString& path, float intensity = 0.7f, float radius = 0.5f); // Установка изображения с виньеткой
    void SetPixmap(QWidget* widget, const QString& path, double multiplicity_size = 1.0); // Установка изображения для виджета
    void FitImage();  // Подгонка изображения под размер окна
    void SetFolder(const QString& d); // Установка фоновой папки

     //--- МЕТОДЫ АНИМАЦИИ ---
    void crossFadeToImage(const QString& newImagePath, float intensity, float radius, int duration); // Плавный переход между изображениями
    void addComplexTransition(QSequentialAnimationGroup* group, const AnimationFrame& frame); // Добавление сложного перехода
    QLabel* createOverlayLabel(); // Создание наложения для анимации
    void fadeWidgets(bool fadeIn, int duration, std::function<void()> callback = nullptr); // Плавное появление/исчезновение виджетов

    //--- ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ---
    void MakeCalculations(); // Вычисление данных на основе введенных значений
    QList<QWidget*> getAllWidgets(); // Получение списка всех виджетов окна
    void setWidgetsVisible(bool visible); // Управление видимостью виджетов

     //--- ОБРАБОТЧИКИ СОБЫТИЙ ---
    void resizeEvent(QResizeEvent *event) override; // Изменение размера окна
    void showEvent(QShowEvent *event) override; // Событие отображения окна

private:

    Ui::AndroidWindow *ui;       // Указатель на UI
    QPixmap active_pixmap;       // Текущее активное фоновое изображение
    QLabel lbl_new_{this};       // Метка для отображения фона
    QPointer<QSequentialAnimationGroup> currentAnimation; // Указатель на текущую анимацию
    bool widgetsHidden = false; // Флаг отслеживания состояния видимости виджетов
    bool widgetsForceVisible = false; // Флаг принудительной видимости виджетов

    //--- КЭШ ПРЕДЗАГРУЗКИ ---
    QVector<AnimationFrame> m_animationFrames; // Список кадров анимации
    QMap<QString, QPixmap> m_animationCache; // Кэш загруженных изображений

   //--- МНОГОПОТОЧНОСТЬ ---
    QFutureWatcher<void> m_preloadWatcher; // Наблюдатель за фоновой загрузкой
    QMutex m_cacheMutex;  // Мьютекс для синхронизации кэша
    bool m_essentialLoaded = false; // Флаг завершения начальной загрузки

    void asyncPreloadFrame(const AnimationFrame& frame); // Асинхронная загрузка кадра
    void finalizePreload(); // Завершение предзагрузки



private slots:
    void on_pushButton_clicked();    // Обработчик нажатия кнопки
    void on_pushButton_released();   // Обработчик отпускания кнопки
    void on_pushButton_pressed();    // Обработчик нажатия кнопки


};

#endif // ANDROIDWINDOW_H
