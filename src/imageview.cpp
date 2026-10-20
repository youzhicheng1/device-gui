#include "imageview.h"



void ImageView::mousePressEvent(QMouseEvent *e)
{
    m_start=mapToScene(e->pos());
    m_rectItem=scene()->addRect(
        QRectF(m_start,QSizeF(0,0)),QPen(Qt::red,2)
        );
}

void ImageView::mouseMoveEvent(QMouseEvent *e)
{
    if(m_rectItem){
        QPointF cur=mapToScene(e->pos());
        m_rectItem->setRect(QRectF(m_start,cur).normalized());
    }
}

void ImageView::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_rectItem){
        QRectF roi=m_rectItem->rect();
        if(roi.width()>0&&roi.height()>0){
            emit roiChanged(roi);
        }
    }
}
