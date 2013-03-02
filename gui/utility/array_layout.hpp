#ifndef QUICK_COLLIDER_ARRAY_LAYOUT_INCLUDED
#define QUICK_COLLIDER_ARRAY_LAYOUT_INCLUDED

#include <QQuickItem>
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
        m_dirty(false)
    {}

    Qt::Orientation orientation() const { return m_orientation; }

    void setOrientation( Qt::Orientation orientation )
    {
        m_orientation = orientation;
        requestRelayout();
        emit orientationChanged(m_orientation);
    }
/*
    bool stretch() const { return m_stretch; }

    void setStretch( qreal stretch ) {
        m_stretch = stretch;
        requestRelayout();
    }
*/
    qreal extent() const { return m_extent; }

    void setExtent( qreal extent ) {
        m_extent = qMax(0.0, extent);
        requestRelayout();
    }

    qreal spacing() const { return m_spacing; }

    void setSpacing( qreal spacing ) {
        m_spacing = qMax(0.0, spacing);
        requestRelayout();
    }

signals:
    void orientationChanged( Qt::Orientation );

protected:
    virtual void itemChange(ItemChange change, const ItemChangeData & data)
    {
        QQuickItem::itemChange(change, data);

        switch (change)
        {
        case ItemChildAddedChange:
        case ItemChildRemovedChange:
        {
            QQuickItem *childItem = data.item;
            if (childItem)
            {
                if (change == ItemChildAddedChange)
                    if (hasContents(childItem))
                        m_managed_items.append(childItem);
                else
                    m_managed_items.removeOne(childItem);
                requestRelayout();
            }
        }
        }
    }

    virtual void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry)
    {
        QQuickItem::geometryChanged(newGeometry, oldGeometry);

        requestRelayout();
    }

    virtual void updatePolish()
    {
        QQuickItem::updatePolish();

        if (!m_dirty)
            return;

        m_dirty = false;

        int child_count = m_managed_items.count();

        if (!child_count)
            return;

        qreal shared_area = m_orientation == Qt::Horizontal ? height() : width();
        qreal divided_area = m_orientation == Qt::Horizontal ? width() : height();

        qreal division = m_extent;
        if (division == 0.0) {
            qreal combined_space = child_count > 1 ? (child_count - 1) * m_spacing : 0.0;
            division = qMax(0.0, divided_area - combined_space) / child_count;
        }

        if (m_orientation == Qt::Horizontal)
        {
            for (int idx = 0; idx < child_count; ++idx)
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
            for (int idx = 0; idx < child_count; ++idx)
            {
                QQuickItem *child = m_managed_items[idx];
                child->setWidth( shared_area );
                child->setHeight( division );
                child->setX( 0.0 );
                child->setY( idx * (division + m_spacing) );
            }
        }
    }

private:
    void requestRelayout()
    {
        if (!m_dirty) {
            m_dirty = true;
            polish();
        }
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

    Qt::Orientation m_orientation;
    qreal m_extent;
    qreal m_spacing;
    bool m_dirty;
    QList<QQuickItem*> m_managed_items;
};

} // namespace QuickCollider

#endif // QUICK_COLLIDER_ARRAY_LAYOUT_INCLUDED
