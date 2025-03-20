#ifndef ANDROIDWINDOW_H
#define ANDROIDWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGuiApplication>

namespace Ui {
class AndroidWindow;
}

class AndroidWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AndroidWindow(QWidget *parent = nullptr);
    ~AndroidWindow();

private:
    void SetPixmap(const QString path); // Установка активного изображения
    void SetPixmap(QWidget* widget, const QString& path); // Установка изображения для виджета
    void FitImage(); // Подгонка изображения под размер окна
    void SetFolder(const QString& d); // Установка фонового изображения
    void UpdateQLabelSize(); // Обновление размеров QLabel
    void MakeCalculations(); // Выполнение расчетов

    void resizeEvent(QResizeEvent *event) override; // Обработчик изменения размера окна

protected:
    void showEvent(QShowEvent *event) override; // Обработчик события отображения окна

private:
    Ui::AndroidWindow *ui; // Указатель на интерфейс
    QPixmap active_pixmap; // Активное изображение
    QLabel lbl_new_{this}; // Метка для изображения
};

#endif // ANDROIDWINDOW_H
