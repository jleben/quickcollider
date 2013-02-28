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
    Q_ENUMS( ElementStyle )
    Q_ENUMS( DataRole )
    Q_PROPERTY( QObject * target READ target WRITE set_target )
    Q_PROPERTY( QRectF area READ area WRITE setArea )
    Q_PROPERTY( qreal nodeMargin READ nodeMargin WRITE setNodeMargin NOTIFY nodeMarginChanged )
    //Q_PROPERTY( VariantList value READ value WRITE setValue )
    //Q_PROPERTY( VariantList strings READ dummyVariantList WRITE setStrings );
    Q_PROPERTY( int index READ index WRITE setIndex )
    Q_PROPERTY( int lastIndex READ lastIndex )
    //Q_PROPERTY( VariantList selectionIndexes READ selectionIndexes)
    //Q_PROPERTY( int thumbSize READ dummyInt WRITE setThumbSize );
    //Q_PROPERTY( int thumbWidth READ dummyInt WRITE setThumbWidth );
    //Q_PROPERTY( int thumbHeight READ dummyInt WRITE setThumbHeight );
    //Q_PROPERTY( QColor background READ background WRITE setBackground );
    //Q_PROPERTY( QColor strokeColor READ strokeColor WRITE setStrokeColor );
    //Q_PROPERTY( QColor fillColor READ dummyColor WRITE setFillColor );
    //Q_PROPERTY( QColor gridColor READ gridColor WRITE setGridColor );
    //Q_PROPERTY( QColor focusColor READ focusColor WRITE setFocusColor );
    //Q_PROPERTY( QColor selectionColor READ selectionColor WRITE setSelectionColor );
    //Q_PROPERTY( bool drawLines READ dummyBool WRITE setDrawLines );
    //Q_PROPERTY( bool drawRects READ dummyBool WRITE setDrawRects );
    //Q_PROPERTY( ElementStyle style READ elementStyle WRITE setElementStyle );
    //Q_PROPERTY( bool editable READ dummyBool WRITE setEditable );
    Q_PROPERTY( double step READ step WRITE setStep )
    Q_PROPERTY( int selectionForm READ selectionForm WRITE setSelectionForm )
    Q_PROPERTY( int horizontalOrder READ horizontalOrder WRITE setHorizontalOrder )
    Q_PROPERTY( float x READ currentX WRITE setCurrentX )
    Q_PROPERTY( float y READ currentY WRITE setCurrentY )
    Q_PROPERTY( QPointF grid READ grid WRITE setGrid )
    //Q_PROPERTY( bool gridOn READ dummyBool WRITE setGridOn );

public:
    Q_INVOKABLE void add( QPointF pos );
    Q_INVOKABLE void move( int, QPointF pos );

    Q_INVOKABLE int count() { return _model.elementCount(); }
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
    Q_INVOKABLE void setThumbHeightAt( int, int );
    Q_INVOKABLE void setThumbWidthAt( int, int );
    Q_INVOKABLE void setThumbSizeAt( int, int );
    Q_INVOKABLE void setCurves( double curvature );
    Q_INVOKABLE void setCurves( int type );
    //Q_INVOKABLE void setCurves( const VariantList & curves );

    Q_INVOKABLE void mousePress( QPointF pos, int buttons, int modifiers );
    Q_INVOKABLE void mouseMove( QPointF pos, int buttons, int modifiers );

public Q_SLOTS:
    Q_INVOKABLE void select( int index, bool exclusive = true );
    Q_INVOKABLE void deselect( int index );
    Q_INVOKABLE void deselectAll();

Q_SIGNALS:
    void nodeMarginChanged();
    void action();
    void metaAction();

public:


public:
    enum DataRole {
        ValueRole = Qt::UserRole,
        PositionRole,
        EnabledRole,
        SelectedRole,
        CurveTypeRole,
        CurvatureRole
    };

    enum SelectionForm {
        ElasticSelection,
        RigidSelection
    };

    enum Order {
        NoOrder,
        RigidOrder
    };

    enum ElementStyle {
        DotElements,
        RectElements,
    };

public:
    GraphModel( QObject * parent = 0);

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const
    {
        return _model.elementCount();
    }

    QVariant data(const QModelIndex & index, int role) const
    {
        GraphElement *elem = _model.elementAt(index.row());

        switch (role)
        {
        case ValueRole:
            return elem->value;
        case PositionRole:
            return positionForValue( elem->value );
        case EnabledRole:
            return elem->editable;
        case SelectedRole:
            return elem->selected;
        case CurveTypeRole:
            return (int) elem->curveType;
        case CurvatureRole:
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

    //VariantList value() const;
    GraphElement *currentElement() const;
    int index() const;
    int lastIndex() const { return _lastIndex; }
    //VariantList selectionIndexes() const;
    float currentX() const;
    float currentY() const;
    QPointF grid() const { return _gridMetrics; }

    //void setValue( const VariantList & );
    //void setStrings( const VariantList &list );

    void setIndex( int i );
    void setCurrentX( float );
    void setCurrentY( float );
    void setThumbSize( int f );
    void setThumbWidth( int f );
    void setThumbHeight( int f );
#if 0
    const QColor & background() const
    { return _bkg.isValid() ? _bkg : palette().color(QPalette::Base); }
    void setBackground( const QColor &c ) { _bkg = c; /*update();*/ }

    const QColor & selectionColor() const
    { return _selectColor.isValid() ? _selectColor : palette().color(QPalette::Highlight); }
    void setSelectionColor( const QColor &c ) { _selectColor = c; /*update();*/ }

    const QColor & strokeColor() const
    { return _strokeColor.isValid() ? _strokeColor :  palette().color(QPalette::Text); }
    void setStrokeColor( const QColor & c ) { _strokeColor = c; /*update();*/ }

    void setFillColor( const QColor & c );

    QColor gridColor() const
    {
        if(_gridColor.isValid())
            return _gridColor;
        else {
            QColor c = palette().color(QPalette::Text);
            c.setAlpha(40);
            return c;
        }
    }
    void setGridColor( const QColor & c ) { _gridColor = c; /*update();*/ }
#endif

    ElementStyle elementStyle() const { return _style; }
    void setElementStyle(ElementStyle s) { _style = s; _geometryDirty = true; /*update();*/ }
    void setDrawLines( bool b ) { _drawLines = b; /*update();*/ }
    void setDrawRects( bool b ) { _drawRects = b; /*update();*/ }
    void setEditable( bool b ) { _editable = b; /*update();*/ }

    double step() const { return _step; }
    void setStep( double );
    int selectionForm() const { return (int)_selectionForm; }
    void setSelectionForm( int i ) { _selectionForm = (SelectionForm) i; }
    int horizontalOrder() const { return (int)_xOrder; }
    void setHorizontalOrder( int i );
    void setGrid( const QPointF &pt ) { _gridMetrics = pt; /*update();*/ }
    void setGridOn( bool b ) { _gridOn = b; /*update();*/ }
    QSize sizeHint() const { return QSize( 200,200 ); }
    QSize minimumSizeHint() const { return QSize( 50,50 ); }

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

    QObject * target() const { return m_target; }

    void set_target( QObject *target )
    {
        qDebug() << "target:" << target;
        if (!target) return;
        m_target = target;
        m_target->installEventFilter(this);
    }

    QRectF area() const
    {
        return m_area;
    }

    void setArea( const QRectF & area )
    {
        m_area = area;
    }

    qreal nodeMargin() const { return m_node_margin; }

    void setNodeMargin( qreal margin )
    {
        m_node_margin = margin;
        emit nodeMarginChanged();
    }

    QPointF valueForPosition( const QPointF & pos ) const
    {
        return QPointF ( (pos.x() - m_area.x()) / m_area.width(),
                         (pos.y() - m_area.y()) / m_area.height() );
    }

    QPointF positionForValue( const QPointF & val ) const
    {
        return QPointF ( val.x() * m_area.width() + m_area.x(),
                         val.y() * m_area.height() + m_area.x() );
    }

    void notifySelectionDataChanged(int role)
    {
        int count = _model.elementCount();
        for (int idx = 0; idx < count; ++idx)
        {
            if (_model.elementAt(idx)->selected) {
                QModelIndex modelIndex = createIndex(idx, 0);
                emit dataChanged( modelIndex, modelIndex, QVector<int>(1, role) );
            }
        }
    }

    void notifyDataChanged(int index, int role)
    {
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged( modelIndex, modelIndex, QVector<int>(1, role) );
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
//    void mousePressEvent( QMouseEvent * );
//    void mouseMoveEvent( QMouseEvent * );
    void keyPressEvent( QKeyEvent * );

    GraphData _model;
    QHash<int, QByteArray> m_role_names;

    QPointer<QObject> m_target;
    QRectF m_area;
    qreal m_node_margin;

    QColor _bkg;
    QColor _strokeColor;
    QColor _gridColor;
    QColor _selectColor;

    QSize _defaultThumbSize;
    QPointF _gridMetrics;
    bool _gridOn;

    ElementStyle _style;
    bool _drawLines;
    bool _drawRects;

    bool _editable;
    double _step;
    SelectionForm _selectionForm;
    Order _xOrder;

    bool _geometryDirty;
    QSize _largestThumbSize;

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
