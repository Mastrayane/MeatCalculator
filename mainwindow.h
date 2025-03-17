#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SetInterface(); // Инициализация интерфейса

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
    Ui::MainWindow *ui; // Указатель на интерфейс
    QPixmap active_pixmap; // Активное изображение
    QLabel lbl_new_{this}; // Метка для изображения

private slots:
    void on_pushButton_clicked(); // Слот для обработки нажатия кнопки
    void on_pushButton_released(); // Слот для обработки отпускания кнопки
    void on_pushButton_pressed(); // Слот для обработки нажатой кнопки
};

#endif // MAINWINDOW_H
