#include "imageview.h"



void ImageView::mousePressEvent(QMouseEvent *e)
{
    m_start=mapToScene(e->pos());
    m_rectItem=scene()->addRect(QRectF(m_start,QSizeF(0,0)),
                                  QPen(Qt::red,2));
    qDebug()<<"Scene axis:"<<m_start;
}

void ImageView::mouseMoveEvent(QMouseEvent *e)
{
    if (m_rectItem) {
        QPointF cur = mapToScene(e->pos());
        m_rectItem->setRect(QRectF(m_start, cur).normalized());     // ★ 注意 normalized
    }
    QPointF p=mapToScene(e->pos());
    //qDebug()<<"Scene axis:"<<p;
}

void ImageView::mouseReleaseEvent(QMouseEvent *e)
{
    // QPointF p=mapToScene(e->pos());
    if(m_rectItem){
        QRectF roi=m_rectItem->rect();
        if(roi.width()>0&&roi.height()>0)
            emit roiChanged(roi);
    }
    qDebug()<<"ROI:"<<m_rectItem->rect();
}
