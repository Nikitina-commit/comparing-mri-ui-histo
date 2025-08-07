#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QDebug>
#include <QGraphicsItem>
#include <imagelist.h>

class mouseEvent : public QGraphicsScene
{
    Q_OBJECT

public:
    QRect selectionRect;
    explicit mouseEvent(QObject *parent = 0);
    ~mouseEvent();
    void currentImg(ImageList* A){
       QImage currentImage = A->getImg();
       currentImageItem = &currentImage;
    }

    QRect getRect(){
        return selectionRect;
    };

private:
    QPointF     previousPoint;      // Координаты предыдущей точки
    QGraphicsRectItem* selection          {nullptr};
    bool leftMouseButtonPressed           {false};
    QImage* currentImageItem {nullptr};

signals:
    void previousPositionChanged(const QPointF previousPosition);
    void clippedImage(const QImage& pixmap);

private:
    // Для рисования используем события мыши
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);



};

#endif // MOUSEEVENT_H


