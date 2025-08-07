#ifndef CROPMOUSE_H
#define CROPMOUSE_H

#pragma once


#include <QtWidgets/QGraphicsScene>
#include <QDebug>

class QGraphicsSceneMouseEvent;
class QGraphicsPixmapItem;
class QGraphicsRectItem;

class CropMouse : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(QPointF previousPosition READ previousPosition WRITE setPreviousPosition NOTIFY previousPositionChanged)

public:
    CropMouse(QObject* parent);
    QPointF previousPosition() const;
    // Метод для замены изображения в QGraphicsScene
    void setImage(const QString& filePath);

signals:
    void previousPositionChanged(const QPointF previousPosition);
    void clippedImage(const QPixmap& pixmap);  // Сигнал, который передаёт вырезанную область в окно приложения для установки его в лейбл

public slots:
    void setPreviousPosition(const QPointF previousPosition);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;


private:
    QGraphicsRectItem* m_selection          {nullptr};
    QGraphicsPixmapItem* m_currentImageItem {nullptr};
    QPointF m_previousPosition;
    bool m_leftMouseButtonPressed           {false};
};

#endif // CROPMOUSE_H
