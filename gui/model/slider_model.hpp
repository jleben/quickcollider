#ifndef QUICK_COLLIDER_SLIDER_MODEL_INCLUDED
#define QUICK_COLLIDER_SLIDER_MODEL_INCLUDED

#include <QObject>

namespace QuickCollider {

class SliderModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY( qreal minimum READ minimum WRITE setMinimum NOTIFY minimumChanged )
    Q_PROPERTY( qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged )
    Q_PROPERTY( int steps READ steps WRITE setSteps NOTIFY stepsChanged )
    Q_PROPERTY( qreal value READ value WRITE setValue NOTIFY valueChanged )

public:
    SliderModel( QObject * parent =  0 ):
        QObject(parent),
        m_min(0.0),
        m_max(1.0),
        m_steps(0),
        m_value(0.0)
    {}

    qreal minimum() const { return m_min; }
    qreal maximum() const { return m_max; }
    int steps() const { return m_steps; }
    qreal value() const { return m_value; }

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
        }
    }

signals:
    void minimumChanged( qreal minimum );
    void maximumChanged( qreal maximum );
    void stepsChanged( int steps );
    void valueChanged( qreal value );

private:
    qreal restricted_value ( qreal value ) const
    {
        if (m_steps > 0)
            value = qRound( (value - m_min) * m_steps ) / (qreal) m_steps + m_min;
        value = qBound( m_min, value, m_max );
        return value;
    }

    qreal m_min;
    qreal m_max;
    int m_steps;
    qreal m_value;
};

} // namespace QuickCollider

#endif // QUICK_COLLIDER_SLIDER_MODEL_INCLUDED
