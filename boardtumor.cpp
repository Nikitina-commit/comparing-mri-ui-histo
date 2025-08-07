#include "boardtumor.h"

boardTumor::boardTumor(QObject *parent) : QGraphicsScene(parent)
{

}


boardTumor::~boardTumor()
{

}



void boardTumor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // При нажатии кнопки мыши отрисовываем эллипс
    addEllipse(event->scenePos().x() - 1,
               event->scenePos().y() - 1,
               1,
               1,
               QPen(Qt::NoPen),
               QBrush(Qt::green));
    qDebug()<< event->scenePos().x()<<" "<<event->scenePos().y();

      previousPoint = event->scenePos();

    QGraphicsScene::mousePressEvent(event);
}

void boardTumor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    addLine(previousPoint.x(),
            previousPoint.y(),
            event->scenePos().x(),
            event->scenePos().y(),
            QPen(Qt::green,1,Qt::SolidLine,Qt::RoundCap));
    // Обновляем данные о предыдущей координате
    previousPoint = event->scenePos();

    QGraphicsScene::mouseMoveEvent(event);

}


void boardTumor::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
//    clear();


    QGraphicsScene::mouseReleaseEvent(event);

}
