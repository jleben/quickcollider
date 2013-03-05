#ifndef QUICK_COLLIDER_ARRAY_LAYOUT_INCLUDED
#define QUICK_COLLIDER_ARRAY_LAYOUT_INCLUDED

#include <QQuickItem>
#include <QCoreApplication>
#include <QDebug>


namespace QuickCollider {

class ArrayLayout : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY( Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged )
    Q_PROPERTY( qreal extent READ extent WRITE setExtent )
    Q_PROPERTY( qreal spacing READ spacing WRITE setSpacing )

public:
    ArrayLayout( QQuickItem * parent = 0 ):
        QQuickItem(parent),
        m_orientation(Qt::Horizontal),
        m_spacing(0.0),
        m_extent(0.0),
        m_dirty_flags(0)
    {}

    Qt::Orientation orientation() const { return m_orientation; }

    void setOrientation( Qt::Orientation orientation )
    {
        m_orientation = orientation;
        invalidate( DirtyGeometry );
        emit orientationChanged(m_orientation);
    }
/*
    bool stretch() const { return m_stretch; }

    void setStretch( qreal stretch ) {
        m_stretch = stretch;
        invalidate( DirtyGeometry );
    }
*/
    qreal extent() const { return m_extent; }

    void setExtent( qreal extent )
    {
        m_extent = qMax(0.0, extent);
        invalidate( DirtyGeometry );
    }

    qreal spacing() const { return m_spacing; }

    void setSpacing( qreal spacing )
    {
        m_spacing = qMax(0.0, spacing);
        invalidate( DirtyGeometry );
    }

signals:
    void orientationChanged( Qt::Orientation );

protected:
    virtual bool event( QEvent * event )
    {
        if (event->type() == QEvent::LayoutRequest)
            relayout();

        return QQuickItem::event(event);
    }

    virtual void itemChange(ItemChange change, const ItemChangeData & data)
    {
        switch (change)
        {
        case ItemChildAddedChange:
        case ItemChildRemovedChange:
        {
            invalidate(DirtyItems);
        }
        default:
            break;
        }

        QQuickItem::itemChange(change, data);
    }

    virtual void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry)
    {
        QQuickItem::geometryChanged(newGeometry, oldGeometry);
        invalidate( DirtyGeometry );
    }

private:
    enum DirtyFlag {
        DirtyGeometry = 1,
        DirtyItems = 2
    };

    void invalidate( DirtyFlag dirty_flag )
    {
        bool first_invalidation = !m_dirty_flags;

        m_dirty_flags |= dirty_flag;

        if (first_invalidation)
            QCoreApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));
    }

    bool hasContents( QQuickItem *item )
    {
        if (item->flags() & QQuickItem::ItemHasContents)
            return true;
        QList<QQuickItem*> children = item->childItems();
        foreach(QQuickItem* child, children)
            if (hasContents(child))
                return true;
    }

    void relayout()
    {
        if (!m_dirty_flags)
            return;

        int dirty_flags = m_dirty_flags;
        m_dirty_flags = 0;

        if (dirty_flags & DirtyItems)
        {
            m_managed_items.clear();

            QList<QQuickItem*> children = childItems();
            int children_count = children.count();
            for (int idx = 0; idx < children_count; ++idx)
            {
                if ( hasContents( children[idx] ) )
                    m_managed_items.append( children[idx] );
            }
        }

        int item_count = m_managed_items.count();

        if (!item_count)
            return;

        qreal shared_area = m_orientation == Qt::Horizontal ? height() : width();
        qreal divided_area = m_orientation == Qt::Horizontal ? width() : height();

        qreal division = m_extent;
        if (division == 0.0) {
            qreal combined_space = item_count > 1 ? (item_count - 1) * m_spacing : 0.0;
            division = qMax(0.0, divided_area - combined_space) / item_count;
        }

        if (m_orientation == Qt::Horizontal)
        {
            for (int idx = 0; idx < item_count; ++idx)
            {
                QQuickItem *child = m_managed_items[idx];
                child->setWidth( division );
                child->setHeight( shared_area );
                child->setX( idx * (division + m_spacing) );
                child->setY( 0.0 );
            }
        }
        else
        {
            for (int idx = 0; idx < item_count; ++idx)
            {
                QQuickItem *child = m_managed_items[idx];
                child->setWidth( shared_area );
                child->setHeight( division );
                child->setX( 0.0 );
                child->setY( idx * (division + m_spacing) );
            }
        }
    }

    Qt::Orientation m_orientation;
    qreal m_extent;
    qreal m_spacing;
    int m_dirty_flags;
    QList<QQuickItem*> m_managed_items;
};

} // namespace QuickCollider

#endif // QUICK_COLLIDER_ARRAY_LAYOUT_INCLUDED
