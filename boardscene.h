#ifndef BOARDSCENE_H
#define BOARDSCENE_H

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

class boardScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit boardScene(QObject *parent = 0);
    ~boardScene();
    void currentPen(QColor sRgb){
       currentColor = sRgb;
    }
    QPointF getcursor(){
        return previousPoint;
    }

private:
    QPointF     previousPoint;      // Координаты предыдущей точки
    QGraphicsRectItem* selection          {nullptr};
    bool leftMouseButtonPressed           {false};
    QImage* currentImageItem {nullptr};
    QColor currentColor;


signals:
    void previousPositionChanged(const QPointF previousPosition);


private:
    // Для рисования используем события мыши
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);


};

#endif // MOUSEEVENT_H
