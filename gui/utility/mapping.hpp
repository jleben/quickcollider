/*
  QuickCollider - Qt Quick based GUI for SuperCollider

  Copyright 2013 Jakob Leben (jakob.leben@gmail.com)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QUICK_COLLIDER_MAPPING_INCLUDED
#define QUICK_COLLIDER_MAPPING_INCLUDED

#include <QObject>
#include <QMatrix4x4>
#include <QPointer>

namespace QuickCollider {

class Mapping1D: public QObject
{
    Q_OBJECT
    Q_PROPERTY( qreal sourceOffset READ sourceOffset WRITE setSourceOffset )
    Q_PROPERTY( qreal sourceRange READ sourceRange WRITE setSourceRange )
    Q_PROPERTY( qreal targetOffset READ targetOffset WRITE setTargetOffset )
    Q_PROPERTY( qreal targetRange READ targetRange WRITE setTargetRange )
    Q_PROPERTY( bool invert READ invert WRITE setInvert )

public:
    Mapping1D( QObject * parent = 0 ):
        QObject(parent),
        m_source_offset(0.f),
        m_source_range(1.f),
        m_target_offset(0.f),
        m_target_range(1.f),
        m_invert(false),
        m_offset(0.f),
        m_scale(1.f)
    {}

    qreal sourceOffset() const { return m_source_offset; }

    void setSourceOffset( qreal offset )
    {
        m_source_offset = offset;
        updateMapping();
    }

    qreal sourceRange() const { return m_source_range; }

    void setSourceRange( qreal range )
    {
        m_source_range = range;
        updateMapping();
    }

    qreal targetOffset() const { return m_target_offset; }

    void setTargetOffset( qreal offset )
    {
        m_target_offset = offset;
        updateMapping();
    }

    qreal targetRange() const { return m_target_range; }

    void setTargetRange( qreal range )
    {
        m_target_range = range;
        updateMapping();
    }

    bool invert() const { return m_invert; }

    void setInvert( bool invert )
    {
        m_invert = invert;
        updateMapping();
    }

    Q_INVOKABLE
    qreal map( qreal source ) const
    {
        return source * m_scale + m_offset;
    }

signals:
    void mappingChanged();

private:
    void updateMapping()
    {
        m_scale = m_source_range > 0.f ? m_target_range / m_source_range : 0.f;
        if (m_invert)
            m_scale *= -1.f;

        m_offset = m_target_offset - m_scale * m_source_offset;
        if (m_invert)
            m_offset += m_target_range;

        emit mappingChanged();
    }

    qreal m_source_offset;
    qreal m_source_range;
    qreal m_target_offset;
    qreal m_target_range;
    bool m_invert;
    qreal m_offset;
    qreal m_scale;
};

class NumberMapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QObject* mapping READ mapping WRITE setMapping )
    Q_PROPERTY( qreal source READ source WRITE setSource )
    Q_PROPERTY( qreal value READ value NOTIFY valueChanged )

    QObject *mapping() const { return m_mapping; }

    void setMapping( QObject *mapping )
    {
        if (mapping == m_mapping)
            return;

        if (m_mapping)
            m_mapping->disconnect(this);

        m_mapping = qobject_cast<Mapping1D*>(mapping);
        if (mapping && !m_mapping)
            qWarning("PointMapper: the 'mapping' property can only be set to an instance of Mapping1D!");

        if (m_mapping)
            connect( m_mapping, SIGNAL(mappingChanged()), this, SLOT(updateTarget()) );

        updateTarget();
    }

    void setSource( qreal source )
    {
        if (source == m_source)
            return;

        m_source = source;

        updateTarget();
    }

    qreal source() const { return m_source; }
    qreal value() const { return m_target; }

signals:
    void valueChanged();

private slots:
    void updateTarget()
    {
        if (m_mapping) {
            m_target = m_mapping->map(m_source);
            emit valueChanged();
        }
    }

private:
    qreal m_target;
    qreal m_source;
    QPointer<Mapping1D> m_mapping;
};

class Mapping2D : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QRectF sourceRect READ sourceRect WRITE setSourceRect )
    Q_PROPERTY( QRectF targetRect READ targetRect WRITE setTargetRect )
    Q_PROPERTY( bool flip READ flip WRITE setFlip )
    Q_PROPERTY( bool invertX READ invertX WRITE setInvertX )
    Q_PROPERTY( bool invertY READ invertY WRITE setInvertY )

public:

    Mapping2D (QObject * parent = 0):
        QObject(parent),
        m_flip_matrix(0,1,0,0,
                      1,0,0,0,
                      0,0,1,0,
                      0,0,0,1),
        m_source_rect(0,0,1,1),
        m_target_rect(0,0,1,1),
        m_flip(false),
        m_invert_x(false),
        m_invert_y(false)
    {
        m_flip_matrix.optimize();
    }

    QRectF sourceRect() const { return m_source_rect; }

    void setSourceRect( QRectF rect )
    {
        m_source_rect = rect;
        updateMatrix();
    }

    QRectF targetRect() const { return m_target_rect; }

    void setTargetRect( QRectF rect )
    {
        m_target_rect = rect;
        updateMatrix();
    }

    bool flip() const { return m_flip; }

    void setFlip( bool flip )
    {
        m_flip = flip;
        updateMatrix();
    }

    bool invertX() const { return m_invert_x; }

    void setInvertX( bool invert )
    {
        if (m_invert_x == invert)
            return;

        m_invert_x = invert;

        float scale = m_invert_x ? -1 : 1;
        float translate = m_invert_x ? 1 : 0;
        float *matrix_data = m_invert_matrix.data();
        matrix_data[4*0+0] = scale;
        matrix_data[4*3+0] = translate;

        m_invert_matrix.optimize();

        updateMatrix();
    }

    bool invertY() const { return m_invert_y; }

    void setInvertY( bool invert )
    {
        if (m_invert_y == invert)
            return;

        m_invert_y = invert;

        float scale = m_invert_y ? -1 : 1;
        float translate = m_invert_y ? 1 : 0;
        float *matrix_data = m_invert_matrix.data();
        matrix_data[4*1+1] = scale;
        matrix_data[4*3+1] = translate;

        m_invert_matrix.optimize();

        updateMatrix();
    }

    Q_INVOKABLE QPointF map( const QPointF & point ) const { return m_matrix.map(point); }

    Q_INVOKABLE QRectF map( const QRectF & rect ) const
    {
        QPointF a = m_matrix.map(rect.topLeft());
        QPointF b = m_matrix.map(rect.bottomRight());

        QPointF pos( qMin(a.x(), b.x()),
                     qMin(a.y(), b.y()) );
        QSizeF size( qMax(a.x(), b.x()) - pos.x(),
                     qMax(a.y(), b.y()) - pos.y() );
        return QRectF( pos, size );
    }

signals:
    void mappingChanged();

private:
    void updateInvertMatrix()
    {

    }

    void updateMatrix()
    {
        QMatrix4x4 matrix;

        matrix.translate(m_target_rect.x(), m_target_rect.y());

        matrix.scale(m_target_rect.width(), m_target_rect.height());

        if (m_flip)
            matrix *= m_flip_matrix;

        if (m_invert_x || m_invert_y)
            matrix *= m_invert_matrix;

        matrix.scale( m_source_rect.width() ? 1.f / m_source_rect.width() : 0.f,
                      m_source_rect.height() ? 1.f / m_source_rect.height() : 0.f );

        matrix.translate(-m_source_rect.x(), -m_source_rect.y());

        m_matrix = matrix;

        emit mappingChanged();
    }

    QMatrix4x4 m_matrix;
    QMatrix4x4 m_flip_matrix;
    QMatrix4x4 m_invert_matrix;
    QRectF m_source_rect;
    QRectF m_target_rect;
    bool m_flip;
    bool m_invert_x;
    bool m_invert_y;
};

class PointMapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QObject* mapping READ mapping WRITE setMapping )
    Q_PROPERTY( QPointF source READ source WRITE setSource )
    Q_PROPERTY( qreal x READ x NOTIFY valueChanged )
    Q_PROPERTY( qreal y READ y NOTIFY valueChanged )

    QObject *mapping() const { return m_mapping; }

    void setMapping( QObject *mapping )
    {
        if (mapping == m_mapping)
            return;

        if (m_mapping)
            m_mapping->disconnect(this);

        m_mapping = qobject_cast<Mapping2D*>(mapping);
        if (mapping && !m_mapping)
            qWarning("PointMapper: the 'mapping' property can only be set to an instance of Mapping2D!");

        if (m_mapping)
            connect( m_mapping, SIGNAL(mappingChanged()), this, SLOT(updateValue()) );

        updateValue();
    }

    void setSource( QPointF source )
    {
        if (source == m_source)
            return;

        m_source = source;

        updateValue();
    }

    QPointF source() const { return m_source; }
    QPointF value() const { return m_target; }
    qreal x() const { return m_target.x(); }
    qreal y() const { return m_target.y(); }

signals:
    void valueChanged();

private slots:
    void updateValue()
    {
        if (m_mapping) {
            m_target = m_mapping->map(m_source);
            emit valueChanged();
        }
    }

private:
    QPointF m_target;
    QPointF m_source;
    QPointer<Mapping2D> m_mapping;
};

} // namespace QuickCollider

#endif // QUICK_COLLIDER_MAPPING_INCLUDED
