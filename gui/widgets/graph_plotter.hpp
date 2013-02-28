#ifndef QUICK_COLLIDER_GRAPH_PLOTTER_HPP
#define QUICK_COLLIDER_GRAPH_PLOTTER_HPP

#include <QQuickItem>
#include <QAbstractItemModel>
#include <QPointer>
#include <QDebug>

#include <QQuickPaintedItem>
#include <QPainterPath>

namespace QuickCollider {

class GraphPlotter : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QObject* model READ model WRITE setModel)

public:
    GraphPlotter(QQuickItem * parent = 0):
        QQuickItem(parent)
    {
        setFlag( QQuickItem::ItemHasContents, true );
    }

    QObject *model() const { return m_model; }

    void setModel( QObject * object );

protected:
    virtual QSGNode *updatePaintNode(QSGNode * oldNode, UpdatePaintNodeData * updatePaintNodeData);

private slots:
    void onDataChanged(const QModelIndex & topLeft,
                       const QModelIndex & bottomRight,
                       const QVector<int> & roles = QVector<int> ())
    {
        update();
    }

private:
    QPointer<QAbstractItemModel> m_model;
};

class GraphPlotter2 : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QObject* model READ model WRITE setModel)

    QObject *model() const { return m_model; }

    void setModel( QObject * object );

    void paint(QPainter * painter);

private:
    void addConnection( QPainterPath &path,
                         const QPointF & pt1, const QPointF & pt2,
                         int type, qreal curvature );

    QPointer<QAbstractItemModel> m_model;
};

} // namespace QuickCollider

#endif // QUICK_COLLIDER_GRAPH_PLOTTER_HPP
