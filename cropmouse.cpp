#include "cropmouse.h"

#include <QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGraphicsRectItem>


CropMouse::CropMouse(QObject* parent) : QGraphicsScene(parent)
{

}

void CropMouse::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() & Qt::LeftButton)
    {
        // При зажатой левой кнопки мыши, запоминаем позицию
        m_leftMouseButtonPressed = true;
        setPreviousPosition(event->scenePos());
         qDebug()<<"нажата ЛКМ";

        // создаём квадрат выделения
        m_selection = new QGraphicsRectItem();
        m_selection->setBrush(QBrush(QColor(158,182,255,96)));
        m_selection->setPen(QPen(QColor(158,182,255,200),1));
        // добавляем его на графическую сцену
        addItem(m_selection);
    }

    QGraphicsScene::mousePressEvent(event);
}

void CropMouse::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_leftMouseButtonPressed)
    {
        // формируем область выделения при движениюю мышью при зажатой ЛКМ
        auto dx = event->scenePos().x() - m_previousPosition.x();
        auto dy = event->scenePos().y() - m_previousPosition.y();
        auto resultDelta = qMax(qAbs(dx), qAbs(dy));
        m_selection->setRect((dx > 0) ? m_previousPosition.x() : m_previousPosition.x() - resultDelta,
                           (dy > 0) ? m_previousPosition.y() : m_previousPosition.y() - resultDelta,
                           qAbs(resultDelta),
                           qAbs(resultDelta));
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void CropMouse::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() & Qt::LeftButton)
    {
        m_leftMouseButtonPressed = false;

        // При отпускании ЛКМ формируем обрезанную область
        QRect selectionRect = m_selection->boundingRect().toRect();
        clippedImage(m_currentImageItem->pixmap().copy(selectionRect));
        delete m_selection;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void CropMouse::setPreviousPosition(const QPointF previousPosition)
{
    if (m_previousPosition == previousPosition)
        return;

    m_previousPosition = previousPosition;
    emit previousPositionChanged(m_previousPosition);
}

QPointF CropMouse::previousPosition() const
{
    return m_previousPosition;
}

void CropMouse::setImage(const QString& filePath)
{
    if (m_currentImageItem)
    {
        this->removeItem(m_currentImageItem);
    }
    m_currentImageItem = new QGraphicsPixmapItem(QPixmap(filePath));
    addItem(m_currentImageItem);
}
