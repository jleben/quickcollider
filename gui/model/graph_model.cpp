/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "graph_model.hpp"

#include <cmath>
#include <QDebug>

namespace QuickCollider {

void GraphData::append( GraphElement * e ) {
    if( _elems.count() ) {
        GraphElement *prev = _elems.last();
        prev->_next = e;
        e->_prev = prev;
    }
    _elems.append(e);
    Q_EMIT( appended(e) );
}

void GraphData::insert( int index, GraphElement * e )
{
    _elems.insert(index, e);
    if (index > 0) {
        _elems[index-1]->_next = e;
        e->_prev = _elems[index-1];
    }
    if (index < _elems.count() - 1)
    {
        _elems[index+1]->_prev = e;
        e->_next = _elems[index+1];
    }
    Q_EMIT( appended(e) );
}

void GraphData::removeAt( int i ) {
    GraphElement *e = _elems[i];
    int ci = _conns.count();
    while( ci-- ) {
        Connection c = _conns[ci];
        if( c.a == e || c.b == e ) _conns.removeAt(ci);
    }
    if( e->_prev ) e->_prev->_next = e->_next;
    if( e->_next ) e->_next->_prev = e->_prev;
    _elems.removeAt(i);
    Q_EMIT( removed(e) );
    delete e;
}



GraphModel::GraphModel( QObject *parent ) :
    QAbstractListModel(parent),
    _editable( true ),
    _step( 0.01 ),
    _selectionForm( ElasticSelection ),
    _xOrder( NoOrder ),
    _lastIndex(-1)
{
    m_role_names.insert(NodeValue, "value");
    m_role_names.insert(NodePosition, "position");
    m_role_names.insert(NodeCurveType, "curveType");
    m_role_names.insert(NodeCurvature, "curvature");
    m_role_names.insert(NodeEditable, "editable");
    m_role_names.insert(NodeSelected, "selected");

    connect( &_model, SIGNAL(removed(GraphElement*)), this, SLOT(onElementRemoved(GraphElement*)) );
}

int GraphModel::addPosition( qreal x, qreal y )
{
    QPointF value = valueForPosition( QPointF(x,y) );
    return addValue( value.x(), value.y() );
}

int GraphModel::addValue( qreal x, qreal y )
{
    QPointF value(x,y);
    restrictValue( value );
    GraphElement *e = new GraphElement(value);
    if (_xOrder == RigidOrder) {
        int count = _model.elementCount();
        int idx = 0;
        for (; idx < count; ++idx) {
            if (_model.elementAt(idx)->value.x() > value.x())
                break;
        }
        beginInsertRows( QModelIndex(), idx, idx );
        _model.insert(idx, e);
        endInsertRows();
        return idx;
    }
    else {
        int idx = _model.elementCount();
        beginInsertRows( QModelIndex(), idx, idx );
        _model.append(e);
        endInsertRows();
        return idx;
    }
}

#if 0
VariantList QcGraph::value() const
{
    VariantList x;
    VariantList y;
    QList<QcGraphElement*> elems = _model.elements();
    Q_FOREACH( QcGraphElement* e, elems ) {
        QPointF val = e->value;
        x.data.append( val.x() );
        y.data.append( val.y() );
    }

    VariantList values;
    values.data.append( QVariant::fromValue<VariantList>(x) );
    values.data.append( QVariant::fromValue<VariantList>(y) );

    return values;
}
#endif
GraphElement *GraphModel::currentElement() const
{
    return _selection.count() ? _selection.elems.first().elem : 0;
}

int GraphModel::index() const
{
    GraphElement *e = currentElement();
    return e ? _model.elements().indexOf(e) : -1;
}
#if 0
VariantList QcGraph::selectionIndexes() const
{
    VariantList result;
    int c = _model.elementCount();
    for( int i = 0; i < c; ++i ) {
        QcGraphElement *e = _model.elementAt(i);
        if(e->selected) result.data.append(i);
    }
    return result;
}
#endif
float GraphModel::currentX() const
{
    GraphElement *e = currentElement();
    return e ? e->value.x() : 0.f;
}

float GraphModel::currentY() const
{
    GraphElement *e = currentElement();
    return e ? e->value.y() : 0.f;
}
#if 0
void QcGraph::setValue( const VariantList &list )
{
    if( list.data.count() != 2 ) return;
    VariantList xList = list.data[0].value<VariantList>();
    VariantList yList = list.data[1].value<VariantList>();

    int newc = qMin( xList.data.count(), yList.data.count() );
    if( !newc ) return;

    int c = _model.elementCount();
    while( c > newc )
    {
        --c;
        _model.removeAt( c );
    }

    int i;
    for( i = 0; i < newc; ++i )
    {
        QPointF val( xList.data[i].value<float>(),
                     yList.data[i].value<float>() );
        if( i < c ) {
            QcGraphElement *e = _model.elementAt(i);
            setValue( e, val );
        }
        else {
            QcGraphElement *e = new QcGraphElement(_defaultThumbSize);
            setValue( e, val );
            _model.append( e );
        }
    }

    if( newc ) ensureOrder();

    _geometryDirty = true;

    //update();
}
#endif
#if 0
void QcGraph::setStrings( const VariantList &list )
{
    int strc = list.data.count();
    int c = _model.elementCount();
    int i;
    for( i = 0; i < c && i < strc; ++i ) {
        QcGraphElement *e = _model.elementAt(i);
        e->text = list.data[i].toString();
    }
    //update();
}
#endif
#if 0
void QcGraph::setCurves( const VariantList & curves )
{
    for( int i = 0; i < curves.data.size() && i < _model.elementCount(); ++i ) {
        QVariant data = curves.data[i];
        QcGraphElement::CurveType type;
        double curvature;
        if( data.type() == QVariant::Int ) {
            type = (QcGraphElement::CurveType) data.toInt();
            curvature = 0.0;
        }
        else {
            type = QcGraphElement::Curvature;
            curvature = data.value<double>();
        }
        _model.elementAt(i)->setCurveType( type, curvature );
    }
    //update();
}
#endif
void GraphModel::setCurves( double curvature )
{
    Q_FOREACH( GraphElement* e, _model.elements() )
        e->setCurveType( GraphElement::Curvature, curvature );
    //update();
}

void GraphModel::setCurves( int typeId )
{
    GraphElement::CurveType type = (GraphElement::CurveType)typeId;
    Q_FOREACH( GraphElement* e, _model.elements() )
        e->setCurveType( type );
    //update();
}

void GraphModel::setStringAt( int i, const QString & str )
{
    int c = _model.elementCount();
    if( i >= 0 && i < c ) {
        GraphElement *e = _model.elementAt(i);
        e->text = str;
        //update();
    }
}
#if 0
void QcGraph::connectElements( int src, VariantList targets )
{
    int c = _model.elementCount();
    if( src < 0 || src >= c ) return;

    Q_FOREACH( QVariant var, targets.data ) {
        int trg = var.toInt();
        if( trg < 0 || trg >= c ) continue;
        _model.connect( src, trg );
    }

    //update();
}
#endif
void GraphModel::setIndex( int i ) {
    select(i);
}

void GraphModel::select( int i, bool exclusive ) {
    if( i >= 0 && i < _model.elementCount() ) {
        if( exclusive ) setAllDeselected();
        setIndexSelected( i, true );
    }
}

void GraphModel::deselect( int i ) {
    if( i >= 0 && i < _model.elementCount() ) {
        setIndexSelected( i, false );
    }
}

void GraphModel::deselectAll() {
    setAllDeselected();
}

void GraphModel::setCurrentX( float f )
{
    GraphElement *e = currentElement();
    if(!e) return;

    QPointF val = e->value;
    val.setX( f );
    if( _xOrder != NoOrder ) orderRestrictValue(e,val,true);
    else restrictValue(val);
    e->value = val;

    //update();
}

void GraphModel::setCurrentY( float f )
{
    GraphElement *e = currentElement();
    if(!e) return;

    QPointF val = e->value;
    val.setY( f );
    if( _xOrder != NoOrder ) orderRestrictValue(e,val,true);
    else restrictValue(val);
    e->value = val;

    //update();
}

void GraphModel::setFillColor( const QColor & color )
{
    int c = _model.elementCount();
    for( int i=0; i<c; ++i ) {
        GraphElement *e = _model.elementAt(i);
        e->fillColor = color;
    }
    //update();
}

void GraphModel::setFillColorAt( int i, const QColor & color )
{
    int c = _model.elementCount();
    if( i >= 0 && i < c ) {
        GraphElement *e = _model.elementAt(i);
        e->fillColor = color;
        //update();
    }
}

void GraphModel::setEditableAt( int i, bool b )
{
    int c = _model.elementCount();
    if( i >= 0 && i < c ) {
        GraphElement *e = _model.elementAt(i);
        e->editable = b;
    }
}

void GraphModel::setStep( double step )
{
    _step = qMax( 0.0, step );

    if( _model.elementCount() ) {
        ensureOrder();
        //update();
    };
}

void GraphModel::setHorizontalOrder(bool on) {
    _xOrder = on ? RigidOrder : NoOrder;
    if( _xOrder == RigidOrder ) {
        ensureOrder();
        //update();
    }
}

void GraphModel::onElementRemoved( GraphElement *e )
{
    _selection.elems.removeAll( SelectedElement(e) );
}

void GraphModel::setAllDeselected()
{
    int c = _model.elementCount();
    for( int i = 0; i < c; ++i ) {
        GraphElement *e = _model.elementAt(i);
        bool was_selected = e->selected;
        e->selected = false;
        if (was_selected)
            notifyDataChanged(i, NodeSelected);
    }
    _selection.elems.clear();
}

void GraphModel::setIndexSelected( int index, bool select )
{
    Q_ASSERT( index >= 0 && index < _model.elementCount() );

    GraphElement *e = _model.elementAt( index );
    if( e->selected == select ) return;

    if( select ) {
        e->selected = true;
        int c = _model.elementCount();
        int si = 0;
        int i = 0;
        while( i < index ) {
            if( _model.elementAt(i)->selected ) ++si;
            ++i;
        }
        _selection.elems.insert( si, SelectedElement(e) );
        _lastIndex = index;
    }
    else {
        e->selected = false;
        _selection.elems.removeAll( SelectedElement(e) );
    }

    notifyDataChanged(index, NodeSelected);
    //update();
}

inline static void qc_graph_round( double &val, double &step, bool &grid )
{
    if( val < 0.0 ) {
        val = 0.0;
    }
    else if ( grid ) {
        double ratio = ( val + (step*0.5) > 1.0 ) ? floor(1.0/step) : round(val/step);
        val = ratio * step;
    }
    else if ( val > 1.0 ) {
        val = 1.0;
    }
}

inline void GraphModel::restrictValue( QPointF & val )
{
    // FIXME: add rounding to 'steps'

    /*double x = val.x();
    double y = val.y();
    bool grid = _step > 0.0;
    qc_graph_round(x,_step,grid);
    qc_graph_round(y,_step,grid);*/
    val.setX( qBound(0.0, val.x(), 1.0) );
    val.setY( qBound(0.0, val.y(), 1.0) );
}

void GraphModel::orderRestrictValue( GraphElement *e, QPointF & val, bool selected )
{
    restrictValue(val);

    double x0 = e->value.x();
    double x = val.x();

    if( x == x0 ) {
        return;
    }
    else if( x < x0 ) {
        // new x is smaller, check if not too small;
        GraphElement *prev = e->prev();
        if( prev && (selected || !prev->selected) && x < prev->value.x() )
            val.setX( prev->value.x() );
    }
    else {
        // new x is larger, check if not too large;
        GraphElement *next = e->next();
        if( next && (selected || !next->selected) && x > next->value.x() )
            val.setX( next->value.x() );
    }
}

inline void GraphModel::setValue( GraphElement * e, const QPointF& pt )
{
    QPointF val(pt);
    restrictValue( val );
    e->value = val;
}

void GraphModel::ensureOrder()
{
    int c = _model.elementCount();
    double x_min = 0.0;
    for( int i = 0; i < c; ++i ) {
        GraphElement *e = _model.elementAt(i);
        QPointF val = e->value;
        if( _xOrder != NoOrder && val.x() < x_min ) val.setX(x_min);
        setValue( e, val );
        x_min = e->value.x();
    }
}

void GraphModel::moveFree( GraphElement *e, const QPointF & val )
{
    if( !e->editable ) return;
    setValue( e, val );
}

void GraphModel::moveOrderRestricted( GraphElement *e, const QPointF & val )
{
    if( !e->editable ) return;
    QPointF v(val);
    orderRestrictValue( e, v, true );
    e->value = v;
}

void GraphModel::moveSelected( const QPointF & dif, SelectionForm form, bool cached )
{
    int c = _selection.count();

    switch( form ) {

    case ElasticSelection: {

        switch( _xOrder ) {

        case NoOrder:

            for( int i = 0; i < c; ++i ) {
                SelectedElement & se = _selection.elems[i];
                moveFree( se.elem, (cached ? se.moveOrigin : se.elem->value) + dif );
            }

            break;

        case RigidOrder:

            if( dif.x() <= 0 ) {
                for( int i = 0; i < c; ++i ) {
                    SelectedElement & se = _selection.elems[i];
                    moveOrderRestricted( se.elem, (cached ? se.moveOrigin : se.elem->value) + dif );
                }
            }
            else {
                for( int i = _selection.count() - 1; i >= 0; --i ) {
                    SelectedElement & se = _selection.elems[i];
                    moveOrderRestricted( se.elem, (cached ? se.moveOrigin : se.elem->value) + dif );
                }
            }

            break;
        }

        break;
    }

    case RigidSelection: {

        // reduce dif until acceptable by all nodes
        QPointF d(dif);
        for( int i = 0; i < c; ++i ) {
            SelectedElement & se = _selection.elems[i];
            // if any node in selection is not editable, abort, since
            // we want to keep the selection form!
            if( !se.elem->editable ) return;
            QPointF val0 = (cached ? se.moveOrigin : se.elem->value);
            QPointF val = val0 + d;
            if( _xOrder == NoOrder ) {
                restrictValue( val );
            }
            else {
                orderRestrictValue( se.elem, val, false );
            }
            d = val - val0;
        }

        // if no dif left, do not bother moving
        if( d.isNull() ) return;

        // move all with the new dif
        for( int i = 0; i < c; ++i ) {
            SelectedElement & se = _selection.elems[i];
            if( !se.elem->editable ) continue;
            se.elem->value = (cached ? se.moveOrigin : se.elem->value) + d;
        }

        break;
    }


    }

    notifySelectionDataChanged(NodePosition);
}

bool GraphModel::eventFilter( QObject *object, QEvent *event )
{
    qDebug("filter");
    switch(event->type())
    {
#if 0
    case QEvent::MouseButtonPress:
        mousePressEvent( static_cast<QMouseEvent*>(event) );
        event->accept();
        return true;
    case QEvent::MouseMove:
        mouseMoveEvent( static_cast<QMouseEvent*>(event) );
        event->accept();
        return true;
    case QEvent::KeyPress:
        keyPressEvent( static_cast<QKeyEvent*>(event) );
        event->accept();
        return true;
#endif
    default:
        return QAbstractListModel::eventFilter(object, event);
    }
}

void GraphModel::grabSelection( qreal x, qreal y )
{
    QPointF value = valueForPosition( QPointF(x,y) );
    _selection.cached = false;
    _selection.moveOrigin = value;
}

void GraphModel::moveSelection( qreal x, qreal y )
{
    if( !_editable || !_selection.size() ) return;

    if( !_selection.cached ) {
        int c = _selection.count();
        for( int i = 0; i < c; ++i ) {
            SelectedElement &se = _selection.elems[i];
            se.moveOrigin = se.elem->value;
        }
        _selection.cached = true;
    }

    QPointF value_dif = valueForPosition( QPointF(x,y) ) - _selection.moveOrigin;

    moveSelected( value_dif, _selectionForm, true );
}

void GraphModel::keyPress( int key, int mods )
{
    if( mods & Qt::AltModifier || mods & Qt::ShiftModifier )
    {
        // selection mode

        int c = _model.elementCount();
        if(!c) return;

        switch( key ) {
        case Qt::Key_Right:
        {
            // select the index after the last selected one (wrapping)
            // or extend selection to the right

            int i = -1;
            if(_selection.count()) {
                for (i = c - 1; i >= 0; --i) {
                    if(_model.elementAt(i)->selected)
                        break;
                }
            }

            if( mods & Qt::ShiftModifier ) {
                i = qMin(i + 1, c - 1);
                setIndexSelected( i, true );
            }
            else {
                ++i;
                if (i >= c) i = 0;
                setAllDeselected();
                setIndexSelected( i, true );
            }

            break;
        }
        case Qt::Key_Left:
        {
            // select the index before the first selected one (wrapping)
            // or extend selection to the left
            int i = c;
            if(_selection.count()) {
                for(i = 0; i < c; ++i) {
                    if(_model.elementAt(i)->selected)
                        break;
                }
            }

            if( mods & Qt::ShiftModifier ) {
                i = qMax(i - 1, 0);
                setIndexSelected( i, true );
            }
            else {
                --i;
                if (i < 0) i = c - 1;
                setAllDeselected();
                setIndexSelected( i, true );
            }

            break;
        }
        default: break;
        }
    }
    else
    {
        // editing mode

        if( !_editable || !_selection.size() ) return;

        QPointF dValue;

        switch( key ) {
        case Qt::Key_Up:
            dValue.setY( _step );
            break;
        case Qt::Key_Down:
            dValue.setY( - _step );
            break;
        case Qt::Key_Right:
            dValue.setX( _step );
            break;
        case Qt::Key_Left:
            dValue.setX( - _step );
            break;
        default:
            return;
        }

        moveSelected( dValue, _selectionForm, false );

        //update();
    }
}

} // namespace QuickCollider
