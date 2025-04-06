#ifndef GRAPH_H
#define GRAPH_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QEvent>

class graph : public QGraphicsView
{
	Q_OBJECT
public:
	//
	explicit graph(QWidget *parent = 0);

	void mouseMoveEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void leaveEvent(QEvent *);
	void wheelEvent(QWheelEvent *);
	quint32 x,y;
	float scaleFactor;

signals:
	void mouseMove();
	void mousePress();
	void mouseRelease();
	void mouseLeave();
	void mouseWheel();
};

#endif // GRAPH_H
