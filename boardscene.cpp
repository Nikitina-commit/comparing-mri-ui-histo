#include "boardscene.h"

boardScene::boardScene(QObject *parent) : QGraphicsScene(parent)
{

}

boardScene::~boardScene()
{

}



void boardScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // При нажатии кнопки мыши отрисовываем эллипс
    addEllipse(event->scenePos().x() - 1,
               event->scenePos().y() - 1,
               1,
               1,
               QPen(Qt::NoPen),
               QBrush(currentColor));
    qDebug()<< event->scenePos().x()<<" "<<event->scenePos().y();

      previousPoint = event->scenePos();

    QGraphicsScene::mousePressEvent(event);
}

void boardScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    addLine(previousPoint.x(),
            previousPoint.y(),
            event->scenePos().x(),
            event->scenePos().y(),
            QPen(currentColor,1,Qt::SolidLine,Qt::RoundCap));
    // Обновляем данные о предыдущей координате
    previousPoint = event->scenePos();

    QGraphicsScene::mouseMoveEvent(event);

}


void boardScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
//    clear();


    QGraphicsScene::mouseReleaseEvent(event);

}
