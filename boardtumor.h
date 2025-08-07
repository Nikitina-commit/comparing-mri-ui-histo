#ifndef BOARDTUMOR_H
#define BOARDTUMOR_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QDebug>
#include <QGraphicsItem>
#include <imagelist.h>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsPathItem>

class boardTumor : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit boardTumor(QObject *parent = 0);
    ~boardTumor();

private:
    QPointF     previousPoint;      // Координаты предыдущей точки
    QGraphicsRectItem* selection          {nullptr};
    bool leftMouseButtonPressed           {false};
    QImage* currentImageItem {nullptr};

signals:
    void previousPositionChanged(const QPointF previousPosition);

private:
    // Для рисования используем события мыши
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);


};


#endif // BOARDTUMOR_H
