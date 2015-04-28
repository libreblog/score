#include "TimeNodeView.hpp"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QApplication>
#include "TimeNodePresenter.hpp"
TimeNodeView::TimeNodeView(TimeNodePresenter& presenter,
                           QGraphicsObject* parent) :
    QGraphicsObject {parent},
    m_presenter{presenter}
{
    this->setParentItem(parent);
    this->setZValue(parent->zValue() + 1.5);
    this->setAcceptHoverEvents(true);

    m_color = Qt::darkRed;
}

void TimeNodeView::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget)
{
    QColor pen_color = m_color;


    if(isSelected())
    {
        pen_color = QApplication::palette().highlight().color();
    }

    QPen pen{QBrush(pen_color), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
    painter->setPen(pen);

    painter->drawLine(0, m_top, 0, m_bottom);
}

QRectF TimeNodeView::boundingRect() const
{
    return { -5., (qreal) m_top, 5., (qreal)(m_bottom - m_top) };
}

void TimeNodeView::setExtremities(int top, int bottom)
{
    m_top = top - 10;
    m_bottom = bottom + 10;
    this->update();
}

void TimeNodeView::setMoving(bool arg)
{
    m_moving = arg;
    update();
}

void TimeNodeView::changeColor(QColor newColor)
{
    m_color = newColor;
    this->update();
}


void TimeNodeView::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    emit m_presenter.pressed(event->scenePos());
}

void TimeNodeView::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    emit m_presenter.moved(event->scenePos());
}

void TimeNodeView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    emit m_presenter.released(event->scenePos());
}
