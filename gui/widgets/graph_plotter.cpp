#include "graph_plotter.hpp"
#include "../model/graph_model.hpp"

#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QSGFlatColorMaterial>

#include <QPainter>

#include <cmath>

namespace QuickCollider {

void GraphPlotter::setModel( QObject * object )
{
    m_model = qobject_cast<GraphModel*>(object);
    if (m_model)
        connect( m_model, SIGNAL(dataChanged(QModelIndex, QModelIndex,QVector<int>)),
                 this, SLOT(onDataChanged(QModelIndex,QModelIndex,QVector<int>)) );
    update();
}

QSGNode * GraphPlotter::updatePaintNode(QSGNode * oldNode,
                                         UpdatePaintNodeData * updatePaintNodeData)
{
    if (!m_model) {
        delete oldNode;
        return 0;
    }

    GraphModel *model = static_cast<GraphModel*>(m_model.data());

    int count = model->rowCount();
    if (!(count > 1)) {
        delete oldNode;
        return 0;
    }

    QSGGeometryNode *node;
    QSGGeometry *geometry;

    node = static_cast<QSGGeometryNode*>(oldNode);
    if (!node) {
        node = new QSGGeometryNode;

        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), count);
        geometry->setDrawingMode(GL_LINE_STRIP);
        geometry->setLineWidth(1);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(QColor(20, 20, 20));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        geometry = node->geometry();
        geometry->allocate(count);
    }

    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
    for (int idx = 0; idx < count; ++idx)
    {
        QModelIndex modelIndex = model->index(idx, 0, QModelIndex());
        QPointF point = model->data(modelIndex, GraphModel::PositionRole).value<QPointF>();
        vertices[idx].set(point.x(), point.y());
    }
    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

////////////

void GraphPlotter2::setModel( QObject * object )
{
    m_model = qobject_cast<GraphModel*>(object);
    if (m_model) {
        connect( m_model, SIGNAL(modelReset()), this, SLOT(update()) );
        connect( m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(update()) );
        connect( m_model, SIGNAL(dataChanged(QModelIndex, QModelIndex,QVector<int>)),
                 this, SLOT(update()) );
    }
    update();
}

void GraphPlotter2::paint(QPainter * painter)
{
    if (!m_model)
        return;

    GraphModel *model = static_cast<GraphModel*>(m_model.data());

    int count = model->rowCount();
    if (!(count > 1))
        return;

    painter->setRenderHint( QPainter::Antialiasing, true );


    QPainterPath path;
    QModelIndex modelIndex = model->index(0, 0, QModelIndex());
    QPointF pt1 = model->data(modelIndex, GraphModel::PositionRole).value<QPointF>();
    for (int idx = 1; idx < count; ++idx)
    {
        QModelIndex modelIndex2 = model->index(idx, 0, QModelIndex());

        QPointF pt2 = model->data(modelIndex2, GraphModel::PositionRole).value<QPointF>();
        int type = model->data(modelIndex, GraphModel::CurveTypeRole).toInt();
        qreal curvature = model->data(modelIndex, GraphModel::CurveTypeRole).toInt();
        addConnection(path, pt1, pt2, type, curvature);

        modelIndex = modelIndex2;
        pt1 = pt2;
    }
    painter->drawPath(path);
}

void GraphPlotter2::addConnection( QPainterPath &path,
                                     const QPointF & pt1, const QPointF & pt2,
                                     int type, qreal curvature )
{
    // coefficients for control points of cubic curve
    // approximating first quarter of sinusoid
    // technically: y = sin(pi*x/2) over x = [0,1]
    static const float ax = 1.0/3.0;
    static const float ay = 0.52359877f; // pi/6
    static const float bx = 2.0/3.0;
    static const float by = 1.0;

    switch( type ) {
    case GraphElement::Step:
        path.moveTo( pt1 );
        path.lineTo( pt1.x(), pt2.y() );
        path.lineTo( pt2 );
        break;
    case GraphElement::Linear:
        path.moveTo( pt1 );
        path.lineTo( pt2 );
        break;

    case GraphElement::Quadratic: {
        path.moveTo( pt1 );
        const qreal sqrtBegin = std::sqrt(pt1.y());
        const qreal sqrtEnd   = std::sqrt(pt2.y());
        const qreal n = 100.f;
        const qreal grow = (sqrtEnd - sqrtBegin) / n;

        qreal x = pt1.x();
        qreal y = pt1.y();

        const float dx = (pt2.x() - pt1.x()) / n;

        for (int i = 0; i != n; ++i) {
            x += dx;
            y += grow;
            qreal yCoord = y*y;
            path.lineTo( x, yCoord );
        }

        path.lineTo( pt2 );

        break;
    }

    case GraphElement::Cubic: {
        path.moveTo( pt1 );
        const qreal cubrtBegin = std::pow(pt1.y(), qreal(1/3.0));
        const qreal cubrtEnd   = std::pow(pt2.y(), qreal(1/3.0));
        const qreal n = 100.f;
        const qreal grow = (cubrtEnd - cubrtBegin) / n;

        qreal x = pt1.x();
        qreal y = pt1.y();

        const float dx = (pt2.x() - pt1.x()) / n;

        for (int i = 0; i != n; ++i) {
            x += dx;
            y += grow;
            qreal yCoord = y*y*y;
            path.lineTo( x, yCoord );
        }

        path.lineTo( pt2 );

        break;
    }

    case GraphElement::Sine: {
        // half of difference between end points
        float dx = (pt2.x() - pt1.x()) * 0.5f;
        float dy = (pt2.y() - pt1.y()) * 0.5f;

        // middle point
        QPointF mid = pt1 + QPointF( dx, dy );

        path.moveTo( pt1 );
        path.cubicTo( pt1 + QPointF( dx*(1-bx), dy*(1-by) ), pt1 + QPointF( dx*(1-ax), dy*(1-ay) ), mid );
        path.cubicTo( mid + QPointF( dx*ax, dy*ay ), mid + QPointF( dx*bx, dy*by ), pt2 );

        break;
    }
    case GraphElement::Welch: {
        // difference between points
        float dx = (pt2.x() - pt1.x());
        float dy = (pt2.y() - pt1.y());

        path.moveTo( pt1 );
        if( dy > 0 )
            path.cubicTo( pt1 + QPointF( dx*ax, dy*ay ), pt1 + QPointF( dx*bx, dy*by ), pt2 );
        else
            path.cubicTo( pt1 + QPointF( dx*(1-bx), dy*(1-by) ), pt1 + QPointF( dx*(1-ax), dy*(1-ay) ), pt2 );

        break;
    }
    case GraphElement::Exponential: {

        // FIXME: find a Bezier curve approximation

        path.moveTo( pt1 );

        float dx = (pt2.x() - pt1.x());
        float dy = (pt2.y() - pt1.y());

        // prevent NaN, optimize
        if( pt1.y() <= 0.f || pt2.y() <= 0.f ) {
            path.lineTo( dy < 0 ? QPointF(pt1.x(),pt2.y()) : QPointF(pt2.x(), pt1.y()) );
            path.lineTo( pt2 );
        }
        else {
            const float n = 100.f;
            const float yratio = pt2.y() / pt1.y();
            for( float ph=1/n; ph<=(1-1/n); ph+=1/n ) {
                qreal y = pt1.y() * pow( yratio, ph );
                path.lineTo( pt1.x() + (dx * ph), y );
            }
            path.lineTo( pt2 );
        }

        break;
    }
    case GraphElement::Curvature:

        // FIXME: find a Bezier curve approximation

        path.moveTo( pt1 );

        // prevent NaN
        double curve = qBound( -100.0, curvature, 100.0 );

        if( std::abs( curve ) < 0.0001 ) {
            path.lineTo( pt2 );
        }
        else {
            float dx = (pt2.x() - pt1.x());
            float dy = (pt2.y() - pt1.y());
            double denom = 1.0 - exp( curve );
            const float n = 100.f;
            for( float ph=1/n; ph<=(1-1/n); ph+=1/n ) {
                double numer = 1.0 - exp( ph * curve );
                qreal y = pt1.y() + dy * (numer / denom);
                path.lineTo( pt1.x() + (dx * ph), y );
            }
            path.lineTo( pt2 );
        }
        break;
    }
}

} // namespace QuickCollider
