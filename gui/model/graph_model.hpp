/************************************************************************
*
* Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
************************************************************************/

#ifndef QC_GRAPH_H
#define QC_GRAPH_H

#include <QAbstractListModel>
#include <QPointF>
#include <QRectF>
#include <QColor>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFontMetrics>
#include <QPointer>
#include <QDebug>

namespace QuickCollider {

struct GraphElement {

    friend class GraphData;

    enum CurveType {
        Step = 0,
        Linear,
        Sine,
        Welch,
        Exponential,
        Quadratic,
        Cubic,
        Curvature
    };

    GraphElement( const QSize & sz = QSize() ) :
        size( sz ),
        curveType( Linear ),
        curvature( 0.0 ),
        editable( true ),
        selected( false ),
        _prev(0),
        _next(0)
    {}

    GraphElement( const QPointF & val ) :
        value( val ),
        curveType( Linear ),
        curvature( 0.0 ),
        editable( true ),
        selected( false ),
        _prev(0),
        _next(0)
    {}

    void setCurveType( CurveType type, double curve = 0.0 ) {
        curveType = type;
        curvature = curve;
    }

    GraphElement *prev() { return _prev; }
    GraphElement *next() { return _next; }

    QPointF value;
    QSize size;
    QString text;
    QColor fillColor;
    CurveType curveType;
    double curvature;
    bool editable;
    bool selected;

private:
    GraphElement *_prev;
    GraphElement *_next;
};

class GraphData : public QObject
{
    Q_OBJECT

public:

    GraphData( QObject * parent = 0 ) : QObject(parent) {}

    struct Connection {
        Connection( GraphElement * a_ = 0, GraphElement * b_ = 0 ) : a(a_), b(b_) {}
        GraphElement * a;
        GraphElement * b;
    };

    inline QList<GraphElement*> elements() const { return _elems; }

    inline GraphElement *elementAt( int i ) const { return _elems[i]; }

    inline int elementCount() const { return _elems.count(); }

    void append( GraphElement * e );

    void insert( int index, GraphElement * );

    void removeAt( int i );

    inline QList<Connection> connections() const { return _conns; }

    void connect( int xi, int yi ) {
        GraphElement *x = _elems[xi];
        GraphElement *y = _elems[yi];
        int i;
        for( i = 0; i < _conns.count(); ++i ) {
            Connection c = _conns[i];
            if( (c.a==x && c.b==y) || (c.a==y && c.b==x) ) return;
        }
        _conns.append( Connection(x,y) );
    }

Q_SIGNALS:
    void appended( GraphElement * );
    void removed( GraphElement * );

private:

    QList<Connection> _conns;
    QList<GraphElement*> _elems;
};

class GraphModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS( SelectionForm )
    Q_ENUMS( NodeData )
    //Q_PROPERTY( QObject * target READ target WRITE set_target )
    Q_PROPERTY( QRectF area READ area WRITE setArea )
    Q_PROPERTY( int count READ count )
    //Q_PROPERTY( VariantList value READ value WRITE setValue )
    //Q_PROPERTY( VariantList strings READ dummyVariantList WRITE setStrings );
    Q_PROPERTY( bool editable READ editable WRITE setEditable );
    Q_PROPERTY( double step READ step WRITE setStep )
    Q_PROPERTY( int selectionForm READ selectionForm WRITE setSelectionForm )
    Q_PROPERTY( bool horizontalOrder READ horizontalOrder WRITE setHorizontalOrder )
    Q_PROPERTY( int index READ index WRITE setIndex )
    Q_PROPERTY( int lastIndex READ lastIndex )
    //Q_PROPERTY( VariantList selectionIndexes READ selectionIndexes)
    Q_PROPERTY( float x READ currentX WRITE setCurrentX )
    Q_PROPERTY( float y READ currentY WRITE setCurrentY )

public:
    int count() const { return _model.elementCount(); }

    Q_INVOKABLE int addPosition( qreal x, qreal y );
    Q_INVOKABLE int addValue( qreal x, qreal y );
    Q_INVOKABLE void setValue(int index, qreal x, qreal y);

    Q_INVOKABLE QVariant data( int index, int role )
    {
        if (index < 0 || index >= _model.elementCount() )
            return QVariant();
        QModelIndex modelIndex = createIndex(index, 0);
        return data( modelIndex, role );
    }

    //Q_INVOKABLE void connectElements( int, VariantList );
    Q_INVOKABLE void setStringAt( int, const QString & );
    Q_INVOKABLE void setFillColorAt( int, const QColor & );
    Q_INVOKABLE void setEditableAt( int, bool );
    Q_INVOKABLE void setCurves( double curvature );
    Q_INVOKABLE void setCurves( int type );
    //Q_INVOKABLE void setCurves( const VariantList & curves );

    Q_INVOKABLE void select( int index, bool exclusive = true );
    Q_INVOKABLE void deselect( int index );
    Q_INVOKABLE void deselectAll();

    Q_INVOKABLE void grabSelection( qreal x, qreal y );
    Q_INVOKABLE void moveSelection( qreal x, qreal y );
    Q_INVOKABLE void keyPress( int key, int modifiers );

public:
    enum NodeData {
        NodeValue = Qt::UserRole,
        NodePosition,
        NodeCurveType,
        NodeCurvature,
        NodeEditable,
        NodeSelected
    };

    enum SelectionForm {
        ElasticSelection,
        RigidSelection
    };

    enum Order {
        NoOrder,
        RigidOrder
    };

public:
    GraphModel( QObject * parent = 0);

    // data

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const
    {
        return _model.elementCount();
    }

    QVariant data(const QModelIndex & index, int role) const
    {
        GraphElement *elem = _model.elementAt(index.row());

        switch (role)
        {
        case NodeValue:
            return elem->value;
        case NodePosition:
            return positionForValue( elem->value );
        case NodeEditable:
            return elem->editable;
        case NodeSelected:
            return elem->selected;
        case NodeCurveType:
            return (int) elem->curveType;
        case NodeCurvature:
            return elem->curvature;
        default:
            return elem->value;
        }
    }

    QHash<int, QByteArray> roleNames() const
    {
        return m_role_names;
    }

    QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() )
    {
        return createIndex(row, column);
    }

    //void setValue( const VariantList & );
    //void setStrings( const VariantList &list );
    // set color to all nodes:
    void setFillColor( const QColor & c );

    // options

    bool editable() const { return _editable; }
    void setEditable( bool b ) { _editable = b; /*update();*/ }
    double step() const { return _step; }
    void setStep( double );
    int selectionForm() const { return (int)_selectionForm; }
    void setSelectionForm( int i ) { _selectionForm = (SelectionForm) i; }
    bool horizontalOrder() const { return _xOrder == RigidOrder; }
    void setHorizontalOrder( bool );

    // interaction state

    //VariantList value() const;
    GraphElement *currentElement() const;
    int index() const;
    int lastIndex() const { return _lastIndex; }
    void setIndex( int i );
    //VariantList selectionIndexes() const;
    float currentX() const;
    float currentY() const;
    void setCurrentX( float );
    void setCurrentY( float );

private Q_SLOTS:
    void onElementRemoved( GraphElement *e );

private:
    struct SelectedElement {
        SelectedElement( GraphElement *e ) : elem(e) {}
        bool operator == (const SelectedElement & other) {
            return elem == other.elem;
        }

        GraphElement *elem;
        QPointF moveOrigin; // in data domain
    };

private:
#if 0
    QObject * target() const { return m_target; }

    void set_target( QObject *target )
    {
        qDebug() << "target:" << target;
        if (!target) return;
        m_target = target;
        m_target->installEventFilter(this);
    }
#endif

    QRectF area() const
    {
        return m_area;
    }

    void setArea( const QRectF & area )
    {
        m_area = area;
        int elemCount = _model.elementCount();
        if (elemCount) {
            emit dataChanged( createIndex(0,0),
                              createIndex(elemCount-1,0),
                              QVector<int>() << NodePosition );
        }
    }

    QPointF valueForPosition( const QPointF & pos ) const
    {
        return QPointF ( (pos.x() - m_area.x()) / m_area.width(),
                         1.0 - (pos.y() - m_area.y()) / m_area.height() );
    }

    QPointF positionForValue( const QPointF & val ) const
    {
        return QPointF ( val.x() * m_area.width() + m_area.x(),
                         (1.0 - val.y()) * m_area.height() + m_area.x() );
    }

    void notifySelectionDataChanged(int role)
    {
        notifySelectionDataChanged( QVector<int>(1, role) );
    }

    void notifySelectionDataChanged(QVector<int> roles)
    {
        int count = _model.elementCount();
        for (int idx = 0; idx < count; ++idx)
        {
            if (_model.elementAt(idx)->selected) {
                QModelIndex modelIndex = createIndex(idx, 0);
                emit dataChanged( modelIndex, modelIndex, roles );
            }
        }
    }

    void notifyDataChanged(int index, QVector<int> roles)
    {
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged( modelIndex, modelIndex, roles );
    }

    void notifyDataChanged(int index, int role)
    {
        notifyDataChanged(index, QVector<int>(1, role));
    }

    void setAllDeselected();
    void setIndexSelected( int index, bool selected );
    void restrictValue( QPointF & );
    void orderRestrictValue( GraphElement *, QPointF &, bool selected );
    void setValue( GraphElement *, const QPointF & );
    void ensureOrder();
    void moveFree( GraphElement *, const QPointF & );
    void moveOrderRestricted( GraphElement *, const QPointF & );
    void moveSelected( const QPointF & dValue, SelectionForm, bool fromCache );

    bool eventFilter( QObject *, QEvent * );

    GraphData _model;
    QHash<int, QByteArray> m_role_names;

    QPointer<QObject> m_target;
    QRectF m_area;

    bool _editable;
    double _step;
    SelectionForm _selectionForm;
    Order _xOrder;

    struct Selection {
        Selection () : cached(false), shallMove(false) {}
        int size() const { return elems.size(); }
        int count() const { return elems.count(); }

        QList<SelectedElement> elems;
        bool cached;
        bool shallMove;
        QPointF moveOrigin; // in data domain
    } _selection;

    int _lastIndex;
};

} // namespace QuickCollider

#endif
