#ifndef QUICK_COLLIDER_RANGE_MODEL_INCLUDED
#define QUICK_COLLIDER_RANGE_MODEL_INCLUDED

#include <QObject>

namespace quick_collider {

class range_model : public QObject
{
    Q_OBJECT
    Q_PROPERTY( qreal minimum READ minimum WRITE set_minimum NOTIFY minimumChanged )
    Q_PROPERTY( qreal maximum READ maximum WRITE set_maximum NOTIFY maximumChanged )
    Q_PROPERTY( qreal step READ step WRITE set_step NOTIFY stepChanged )
    Q_PROPERTY( qreal value READ value WRITE set_value NOTIFY valueChanged )

public:
    range_model( QObject * parent =  0 ):
        QObject(parent),
        m_min(0.0),
        m_max(1.0),
        m_step(0.0),
        m_value(0.0)
    {}

    qreal minimum() const { return m_min; }
    qreal maximum() const { return m_max; }
    qreal step() const { return m_step; }
    qreal value() const { return m_value; }

    void set_minimum( qreal minimum )
    {
        if (minimum != m_min) {
            m_min = minimum;
            qreal old_max = m_max;
            m_max = qMax( m_min, old_max );
            qreal old_value = m_value;
            m_value = restricted_value(old_value);
            emit minimumChanged(m_min);
            if (m_max != old_max)
                emit maximumChanged(m_max);
            if (m_value != old_value)
                emit valueChanged(m_value);
        }
    }
    void set_maximum( qreal maximum )
    {
        if (maximum != m_max) {
            m_max = maximum;
            qreal old_min = m_min;
            m_min = qMin( old_min, m_max );
            qreal old_value = m_value;
            m_value = restricted_value(old_value);
            emit maximumChanged(m_max);
            if (m_min != old_min)
                emit minimumChanged(m_min);
            if (m_value != old_value)
                emit valueChanged(m_value);
        }
    }
    void set_range( qreal minimum, qreal maximum )
    {
        maximum = qMax( minimum, maximum );
        qreal old_min = m_min;
        qreal old_max = m_max;
        qreal old_value = m_value;
        m_min = minimum;
        m_max = maximum;
        m_value = restricted_value(old_value);
        if (m_min != old_min)
            emit minimumChanged(m_min);
        if (m_max != old_max)
            emit maximumChanged(m_max);
        if (m_value != old_value)
            emit valueChanged(m_value);
    }
    void set_step( qreal step)
    {
        if (step != m_step) {
            m_step = step;
            qreal old_value = m_value;
            m_value = restricted_value(old_value);
            emit stepChanged(m_step);
            if (m_value != old_value)
                emit valueChanged(m_value);
        }
    }
    void set_value( qreal value )
    {
        value = restricted_value(value);
        if (value != m_value) {
            m_value = value;
            emit valueChanged(m_value);
        }
    }

signals:
    void minimumChanged( qreal minimum );
    void maximumChanged( qreal maximum );
    void rangeChanged( qreal minimum, qreal maximum );
    void stepChanged( qreal step );
    void valueChanged( qreal value );

private:
    qreal restricted_value( qreal value )
    {
        if (m_step != 0.0)
            value = qRound( value / m_step ) * m_step;
        value = qBound( m_min, value, m_max );
        return value;
    }

    qreal m_min;
    qreal m_max;
    qreal m_step;
    qreal m_value;
};

} // namespace quick_collider

#endif // QUICK_COLLIDER_RANGE_MODEL_INCLUDED
