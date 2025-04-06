#include "graph.h"
#include <QScrollBar>
#include <QtWidgets>

graph::graph(QWidget* parent) : QGraphicsView(parent)
{

}

void graph::mouseMoveEvent(QMouseEvent *event)
{
	QPointF scenePoint = mapToScene(event->pos());
	x = scenePoint.x();
	y = scenePoint.y();
	//qDebug () << "Move" << scenePoint.x();
	if(event->buttons() & Qt::LeftButton) emit mouseMove();
}

void graph::wheelEvent(QWheelEvent *event)
{
	const QPointF p0scene = mapToScene(event->pos());

	scaleFactor = std::pow(1.2, event->delta()/240.0);
	scale(scaleFactor, scaleFactor);

	const QPointF p1mouse = mapFromScene(p0scene);
	const QPointF move = p1mouse - event->pos(); // The move
	horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());

	emit mouseWheel();
}

void graph::mousePressEvent(QMouseEvent *event)
{
	emit mousePress();
	//if(event->buttons() & Qt::RightButton) qDebug() <<"press";
}

void graph::mouseReleaseEvent(QMouseEvent *event)
{
	emit mouseRelease();
	//if(event->buttons() & Qt::RightButton) qDebug() <<"press";
}

void graph::leaveEvent(QEvent *event)
{
	emit mouseLeave();
	//qDebug () << "Leave";
}
