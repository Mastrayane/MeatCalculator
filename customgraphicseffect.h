#ifndef CUSTOMGRAPHICSEFFECT_H
#define CUSTOMGRAPHICSEFFECT_H

#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QImage>

class CustomGraphicsEffect : public QGraphicsEffect
{
    Q_OBJECT

public:
    explicit CustomGraphicsEffect(QObject *parent = nullptr);

    // Установка эффекта тени
    void setShadowEffect(QGraphicsDropShadowEffect *shadowEffect);

protected:
    // Переопределение метода draw для отрисовки эффектов
    void draw(QPainter *painter) override;

private:
    // Метод для применения гауссова размытия
    QImage applyGaussianBlur(const QImage& source, qreal radius);

    QGraphicsDropShadowEffect *m_shadowEffect; // Эффект тени
};

#endif // CUSTOMGRAPHICSEFFECT_H
