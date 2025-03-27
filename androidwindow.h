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

namespace Ui {
class AndroidWindow;
}

// Структура для параметров анимации каждого кадра
struct AnimationFrame {
    QString imagePath;
    float vignetteIntensity;
    float vignetteRadius;
    int duration;
    bool hideWidgets;
    QString debugName; // Добавляем поле для отладочной информации
};

class AndroidWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AndroidWindow(QWidget *parent = nullptr);
    ~AndroidWindow();

    void SetInterface(); // Инициализация интерфейса
    void PreloadAnimationFrames(); //Предзагрузка изоюоажений в кэш

private:
    // Методы работы с изображениями
    void SetPixmap(const QString path);
    QPixmap applyVignetteEffect(const QPixmap& original, float intensity = 0.7f, float radius = 0.5f);
    void SetPixmapWithVignette(const QString& path, float intensity = 0.7f, float radius = 0.5f);
    void SetPixmap(QWidget* widget, const QString& path, double multiplicity_size = 1.0);
    void FitImage();
    void SetFolder(const QString& d);

    // Методы анимации
    void crossFadeToImage(const QString& newImagePath, float intensity, float radius, int duration);
    void addComplexTransition(QSequentialAnimationGroup* group, const AnimationFrame& frame);
    QLabel* createOverlayLabel();


    // Вспомогательные методы
    void UpdateQLabelSize();
    void MakeCalculations();
    QList<QWidget*> getAllWidgets();
    void setWidgetsVisible(bool visible);

    // Обработчики событий
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::AndroidWindow *ui;       // Указатель на UI
    QPixmap active_pixmap;       // Текущее фоновое изображение
    QLabel lbl_new_{this};       // Метка для отображения фона
    QPointer<QSequentialAnimationGroup> currentAnimation; // Указатель на текущую анимацию
    bool widgetsHidden = false; // отслеживания состояния виджетов
    bool widgetsForceVisible = false; // Новый флаг принудительной видимости
    QVector<AnimationFrame> m_animationFrames; // Храним параметры анимации
    QMap<QString, QPixmap> m_animationCache; // Альтернативный кэш

private slots:
    void on_pushButton_clicked();    // Обработчик нажатия кнопки
    void on_pushButton_released();   // Обработчик отпускания кнопки
    void on_pushButton_pressed();    // Обработчик нажатия кнопки
    void fadeWidgets(bool fadeIn, int duration, std::function<void()> callback = nullptr);
     // Удаляем параметр по умолчанию из объявления
};

#endif // ANDROIDWINDOW_H
