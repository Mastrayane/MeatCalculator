#include "customgraphicseffect.h"
#include <QPainter>
#include <QImage>
#include <QtMath>

// Конструктор
CustomGraphicsEffect::CustomGraphicsEffect(QObject *parent)
    : QGraphicsEffect(parent), m_shadowEffect(nullptr)
{
}

// Установка эффекта тени
void CustomGraphicsEffect::setShadowEffect(QGraphicsDropShadowEffect *shadowEffect)
{
    m_shadowEffect = shadowEffect;
    update(); // Обновляем эффект
}

// Функция для применения гауссова размытия
QImage CustomGraphicsEffect::applyGaussianBlur(const QImage& source, qreal radius) {
    if (radius <= 0) return source;

    int size = qCeil(radius * 2);
    if (size % 2 == 0) size++; // Размер ядра должен быть нечетным

    QImage result = source;

    // Применяем размытие по горизонтали
    for (int y = 0; y < source.height(); ++y) {
        for (int x = 0; x < source.width(); ++x) {
            qreal r = 0, g = 0, b = 0, a = 0;
            qreal weightSum = 0;

            for (int i = -size / 2; i <= size / 2; ++i) {
                int px = x + i;
                if (px < 0 || px >= source.width()) continue;

                QRgb pixel = source.pixel(px, y);
                qreal weight = qExp(-(i * i) / (2 * radius * radius));

                r += qRed(pixel) * weight;
                g += qGreen(pixel) * weight;
                b += qBlue(pixel) * weight;
                a += qAlpha(pixel) * weight;
                weightSum += weight;
            }

            r /= weightSum;
            g /= weightSum;
            b /= weightSum;
            a /= weightSum;

            result.setPixel(x, y, qRgba(r, g, b, a));
        }
    }

    // Применяем размытие по вертикали
    for (int x = 0; x < source.width(); ++x) {
        for (int y = 0; y < source.height(); ++y) {
            qreal r = 0, g = 0, b = 0, a = 0;
            qreal weightSum = 0;

            for (int i = -size / 2; i <= size / 2; ++i) {
                int py = y + i;
                if (py < 0 || py >= source.height()) continue;

                QRgb pixel = result.pixel(x, py);
                qreal weight = qExp(-(i * i) / (2 * radius * radius));

                r += qRed(pixel) * weight;
                g += qGreen(pixel) * weight;
                b += qBlue(pixel) * weight;
                a += qAlpha(pixel) * weight;
                weightSum += weight;
            }

            r /= weightSum;
            g /= weightSum;
            b /= weightSum;
            a /= weightSum;

            result.setPixel(x, y, qRgba(r, g, b, a));
        }
    }

    return result;
}

// Переопределение метода draw для отрисовки эффектов
void CustomGraphicsEffect::draw(QPainter *painter)
{
    if (!painter || !m_shadowEffect) return;

    // Получаем исходное изображение виджета
    QPoint offset;
    QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset, PadToEffectiveBoundingRect);

    // 1. Рисуем тень
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Создаем изображение для тени
    QImage shadowImage = pixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);

    // Применяем гауссово размытие к тени
    qreal gaussianBlurRadius = m_shadowEffect->blurRadius() / 2.0; // Регулируем интенсивность размытия
    QImage blurredShadow = applyGaussianBlur(shadowImage, gaussianBlurRadius);

    // Закрашиваем тень нужным цветом
    QPainter colorPainter(&blurredShadow);
    colorPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    colorPainter.fillRect(blurredShadow.rect(), m_shadowEffect->color());
    colorPainter.end();

    // Рисуем тень с учетом смещения
    painter->drawImage(offset + m_shadowEffect->offset(), blurredShadow);
    painter->restore();

    // 2. Рисуем исходное изображение виджета
    drawSource(painter);
}
