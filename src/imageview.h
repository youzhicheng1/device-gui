#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsRectItem>

class ImageView: public QGraphicsView
{
    Q_OBJECT
public:
    using QGraphicsView::QGraphicsView;
private:
    void mousePressEvent(QMouseEvent* e)override;
    void mouseMoveEvent(QMouseEvent* e)override;
    void mouseReleaseEvent(QMouseEvent* e)override;

private:
    QPointF m_start;
    QGraphicsRectItem* m_rectItem=nullptr;

signals:
    void roiChanged(const QRectF& rect);
};

#endif // IMAGEVIEW_H
