#include "mouseevent.h"

mouseEvent::mouseEvent(QObject *parent) : QGraphicsScene(parent)
{

}

mouseEvent::~mouseEvent()
{

}

void mouseEvent::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // При нажатии кнопки мыши отрисовываем эллипс
//    addRect(event->scenePos().x(),
//               event->scenePos().y(),
//               10,
//               10,
//               QPen(Qt::NoPen),
//               QBrush(Qt::blue));
    //qDebug()<< event->scenePos().x()<<" "<<event->scenePos().y();

    // Сохраняем координаты точки нажатия
    if (event->button() & Qt::LeftButton)
    {
        leftMouseButtonPressed = true;
    previousPoint = event->scenePos();

    selection = new QGraphicsRectItem();
    selection->setBrush(QBrush(QColor(158,182,255,96)));
    selection->setPen(QPen(QColor(158,182,255,200),1));
    // добавляем его на графическую сцену
    addItem(selection);
    }
    QGraphicsScene::mousePressEvent(event);
}

void mouseEvent::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Отрисовываем линии с использованием предыдущей координаты
//    addLine(previousPoint.x(),
//            previousPoint.y(),
//            event->scenePos().x(),
//            event->scenePos().y(),
//            QPen(Qt::red,10,Qt::SolidLine,Qt::RoundCap));
//    addRect(previousPoint.x(),
//            previousPoint.y(),
//            abs(event->scenePos().x()-previousPoint.x()),
//            abs(event->scenePos().y()-previousPoint.y()),
//            QPen(Qt::NoPen),
//            QBrush(Qt::blue));

    if (leftMouseButtonPressed)
    {
        // формируем область выделения при движениюю мышью при зажатой ЛКМ
        auto dx = event->scenePos().x() - previousPoint .x();
        auto dy = event->scenePos().y() - previousPoint .y();
        auto resultDelta = qMax(qAbs(dx), qAbs(dy));
        selection->setRect((dx > 0) ? previousPoint .x() : previousPoint .x() - resultDelta,
                           (dy > 0) ? previousPoint .y() : previousPoint .y() - resultDelta,
                           qAbs(resultDelta),
                           qAbs(resultDelta));
    }
    QGraphicsScene::mouseMoveEvent(event);

    // Обновляем данные о предыдущей координате
//    previousPoint = event->scenePos();
}


void mouseEvent::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Отрисовываем линии с использованием предыдущей координаты
    qDebug()<< event->scenePos().x()<<" "<<event->scenePos().y();
//    clear();
    if (event->button() & Qt::LeftButton)
    {
        leftMouseButtonPressed = false;

        // При отпускании ЛКМ формируем обрезанную область
        selectionRect = selection->boundingRect().toRect();
      //  clippedImage(currentImageItem->copy(selectionRect));
        delete selection;
    }
    QGraphicsScene::mouseReleaseEvent(event);

}
