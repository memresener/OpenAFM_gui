#include "alignbox.h"

ResizeHandle::ResizeHandle(Qt::Alignment alignment, QGraphicsItem* parent)
    : QGraphicsRectItem(parent), align(alignment)
{
    setRect(0, 0, 10, 10);
    setBrush(QColor(53,173,236));
}


void ResizeHandle::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    offset = event->pos();
}

void ResizeHandle::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem* parent = parentItem();
    if (parent && parent->isWidget())
    {
        QGraphicsWidget* parentWidget = static_cast<QGraphicsWidget*>(parent);
        QRectF parentGeometry = parentWidget->geometry();

        QRectF geometry = boundingRect();
        QPointF newPos=parentWidget->mapToParent(mapToParent(event->pos())) - offset;

        if(newPos.x() > 0
                && newPos.x() < 200
                && newPos.y() > 0
                && newPos.y() < 200){

            geometry.moveTo(newPos);

            if (align & Qt::AlignLeft)
                parentGeometry.setLeft(geometry.left());
            if (align & Qt::AlignRight)
                parentGeometry.setRight(geometry.right());
            if (align & Qt::AlignTop)
                parentGeometry.setTop(geometry.top());
            if (align & Qt::AlignBottom)
                parentGeometry.setBottom(geometry.bottom());

            parentWidget->setGeometry(parentGeometry);
        }
    }
}


AlignWidget::AlignWidget(QGraphicsItem* parent) : QGraphicsWidget(parent)
{
    setPreferredSize(100,100);
    setX(50);
    setY(50);
    setFlag(ItemIsMovable);


    createHandle(Qt::AlignTop);
    createHandle(Qt::AlignBottom);
    createHandle(Qt::AlignLeft);
    createHandle(Qt::AlignRight);

    createHandle(Qt::AlignTop | Qt::AlignLeft);
    createHandle(Qt::AlignTop | Qt::AlignRight);
    createHandle(Qt::AlignBottom | Qt::AlignLeft);
    createHandle(Qt::AlignBottom | Qt::AlignRight);
    createHandle(Qt::AlignCenter);

}

void AlignWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const int gridSize = 5;

    qreal left = int(this->rect().left()) - (int(this->rect().left()) % gridSize);
    qreal top = int(this->rect().top()) - (int(this->rect().top()) % gridSize);

    QVarLengthArray<QLineF, 200> lines;

    for (qreal x = left; x < this->rect().right(); x += gridSize)
        lines.append(QLineF(x, this->rect().top(), x, this->rect().bottom()));
    for (qreal y = top; y < this->rect().bottom(); y += gridSize)
        lines.append(QLineF(this->rect().left(), y, this->rect().right(), y));

    painter->drawLines(lines.data(), lines.size());
    Q_UNUSED(widget);
    QPen pen(QColor(53,173,236), 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawRect(option->rect);

}
void AlignWidget::gridUpdate(QPainter* painter, int gridSize){

    qreal left = int(this->rect().left()) - (int(this->rect().left()) % gridSize);
    qreal top = int(this->rect().top()) - (int(this->rect().top()) % gridSize);

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x < this->rect().right(); x += gridSize)
        lines.append(QLineF(x, this->rect().top(), x, this->rect().bottom()));
    for (qreal y = top; y < this->rect().bottom(); y += gridSize)
        lines.append(QLineF(this->rect().left(), y, this->rect().right(), y));

    painter->drawLines(lines.data(), lines.size());
}


void AlignWidget::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    QGraphicsItem::mouseMoveEvent(event);

    if (x() < 0)
    {
        setPos(0, y());
    }
    else if (x() + boundingRect().right() > scene()->width())
    {
        setPos(this->scene()->width() - boundingRect().width(), y());
    }

    if (y() < 0)
    {
        setPos(x(), 0);
    }
    else if ( y()+ boundingRect().bottom() > scene()->height())
    {
        setPos(x(), scene()->height() - boundingRect().height());
    }
}



void AlignWidget::setGeometry(const QRectF& rect)
{
    QGraphicsWidget::setGeometry(rect);

    QHashIterator<Qt::Alignment, ResizeHandle*> it(resizeHandles);
    while (it.hasNext())
    {
        it.next();
        adjustHandle(it.key(), it.value());
    }
}

void AlignWidget::createHandle(Qt::Alignment align)
{
    resizeHandles[align] = new ResizeHandle(align, this);
}

void AlignWidget::adjustHandle(Qt::Alignment align, ResizeHandle* handle)
{
    QRectF bounds = boundingRect();
    QRectF handleBounds = handle->boundingRect();

    handleBounds.moveCenter(bounds.center());

    if (align & Qt::AlignLeft)
        handleBounds.moveLeft(bounds.left());
    if (align & Qt::AlignRight)
        handleBounds.moveRight(bounds.right());
    if (align & Qt::AlignTop)
        handleBounds.moveTop(bounds.top());
    if (align & Qt::AlignBottom)
        handleBounds.moveBottom(bounds.bottom());

    handle->setPos(handleBounds.topLeft());
}


