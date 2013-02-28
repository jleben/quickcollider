#ifndef QUICK_COLLIDER_SLIDER_MODEL_INCLUDED
#define QUICK_COLLIDER_SLIDER_MODEL_INCLUDED

#include <QObject>

namespace QuickCollider {

class SliderModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY( qreal minimum READ minimum WRITE setMinimum NOTIFY minimumChanged )
    Q_PROPERTY( qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged )
    Q_PROPERTY( qreal minimumPosition READ minimumPosition WRITE setMinimumPosition
                NOTIFY minimumPositionChanged )
    Q_PROPERTY( qreal maximumPosition READ maximumPosition WRITE setMaximumPosition
                NOTIFY maximumPositionChanged )
    Q_PROPERTY( int steps READ steps WRITE setSteps NOTIFY stepsChanged )
    Q_PROPERTY( bool inverted READ inverted WRITE setInverted NOTIFY invertedChanged )
    Q_PROPERTY( qreal value READ value WRITE setValue NOTIFY valueChanged )
    Q_PROPERTY( qreal position READ position WRITE setPosition NOTIFY positionChanged )

public:
    SliderModel( QObject * parent =  0 ):
        QObject(parent),
        m_min(0.0),
        m_max(1.0),
        m_steps(0),
        m_inverted(false),
        m_value(0.0),
        m_min_pos(0.0),
        m_max_pos(1.0)
    {}

    qreal minimum() const { return m_min; }
    qreal maximum() const { return m_max; }
    qreal minimumPosition() const { return m_min_pos; }
    qreal maximumPosition() const { return m_max_pos; }
    int steps() const { return m_steps; }
    bool inverted() const { return m_inverted; }
    qreal value() const { return m_value; }
    qreal position() const { return position_for_value(m_value); }

    void setMinimum( qreal minimum )
    {
        if (minimum == m_min)
            return;

        m_min = minimum;
        emit minimumChanged(m_min);
        setValue(m_value);
    }

    void setMaximum( qreal maximum )
    {
        if (maximum == m_max)
            return;

        m_max = maximum;
        emit maximumChanged(m_max);
        setValue(m_value);
    }

    void setMinimumPosition( qreal min_pos )
    {
        if (min_pos == m_min_pos)
            return;

        m_min_pos = min_pos;
        emit minimumPositionChanged(m_min_pos);
        emit positionChanged( position() );
    }

    void setMaximumPosition( qreal max_pos )
    {
        if(max_pos == m_max_pos)
            return;
        m_max_pos = max_pos;
        emit maximumPositionChanged(m_max_pos);
        emit positionChanged( position() );
    }

    void setSteps( int steps )
    {
        if (steps == m_steps)
            return;

        m_steps = steps;
        emit stepsChanged(m_steps);
        setValue( m_value );
    }

    void setValue( qreal value )
    {
        value = restricted_value(value);
        if (value != m_value) {
            m_value = value;
            emit valueChanged( m_value );
            emit positionChanged( position() );
        }
    }

    void setPosition( qreal pos )
    {
        setValue( value_for_position(pos) );
    }

    void setInverted( bool inverted )
    {
        if (inverted == m_inverted)
            return;

        m_inverted = inverted;
        emit invertedChanged(m_inverted);
        emit positionChanged( position() );
    }

signals:
    void minimumChanged( qreal minimum );
    void maximumChanged( qreal maximum );
    void minimumPositionChanged( qreal minimumPosition );
    void maximumPositionChanged( qreal maximumPosition );
    void stepsChanged( int steps );
    void invertedChanged( bool inverted );
    void valueChanged( qreal value );
    void positionChanged( qreal position );

private:
    qreal value_for_position ( qreal pos ) const
    {
        qreal pos_range = m_max_pos - m_min_pos;
        if (pos_range != 0.0) {
            qreal relative_pos = m_inverted ? m_max_pos - pos : pos - m_min_pos;
            return relative_pos / pos_range * (m_max - m_min) + m_min;
        }
        else {
            return m_min;
        }
    }

    qreal position_for_value ( qreal val ) const
    {
        qreal val_range = m_max - m_min;
        if (val_range != 0.0) {
            qreal relative_pos = (val - m_min) / val_range * (m_max_pos - m_min_pos);
            return m_inverted ? m_max_pos - relative_pos : m_min_pos + relative_pos;
        }
        else {
            return m_min_pos;
        }
    }

    qreal restricted_value ( qreal value ) const
    {
        if (m_steps > 0)
            value = qRound( (value - m_min) * m_steps ) / (qreal) m_steps + m_min;
        value = qBound( m_min, value, m_max );
        return value;
    }

    qreal m_min;
    qreal m_max;
    qreal m_min_pos;
    qreal m_max_pos;
    int m_steps;
    bool m_inverted;
    qreal m_value;
};

} // namespace QuickCollider

#endif // QUICK_COLLIDER_SLIDER_MODEL_INCLUDED
