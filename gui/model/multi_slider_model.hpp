#ifndef QUICK_COLLIDER_MULTI_SLIDER_MODEL_INCLUDED
#define QUICK_COLLIDER_MULTI_SLIDER_MODEL_INCLUDED

#include <QAbstractListModel>
#include <QRectF>
#include <QDebug>

#include <vector>

namespace quick_collider {

class multi_slider_model : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( int count READ count WRITE setCount NOTIFY countChanged )
    Q_PROPERTY( QRectF bounds READ bounds WRITE set_bounds NOTIFY boundsChanged )
    Q_PROPERTY( int orientation READ orientation WRITE set_orientation NOTIFY orientationChanged )
    Q_PROPERTY( int steps READ steps WRITE set_steps NOTIFY stepsChanged )

public:

    enum DataRole {
        ValueRole = Qt::UserRole,
        PositionRole
    };

    multi_slider_model( QObject * parent = 0):
        QAbstractListModel(parent)
    {
        m_role_names.insert(ValueRole, "value");
        m_role_names.insert(PositionRole, "position");
    }

    int count() const { return m_data.size(); }

    void setCount( int count )
    {
        beginResetModel();
        m_data.resize(count);
        endResetModel();
        emit countChanged(count);
    }

    QRectF bounds() const { return m_bounds; }

    void set_bounds( const QRectF & bounds )
    {
        if (bounds == m_bounds)
            return;
        m_bounds = bounds;
        emit boundsChanged(m_bounds);
        emit dataChanged( createIndex(0, 0), createIndex(m_data.size()-1, 0) );
    }

    int orientation() const { return m_orientation; }

    void set_orientation( int arg )
    {
        Qt::Orientation orientation = arg == 2 ? Qt::Vertical : Qt::Horizontal;
        if (orientation == m_orientation)
            return;
        m_orientation = orientation;
        emit orientationChanged(m_orientation);
        emit dataChanged( createIndex(0, 0), createIndex(m_data.size()-1, 0) );
    }

    qreal steps() const { return m_steps; }

    void set_steps( qreal steps )
    {
        if (steps == m_steps)
            return;
        m_steps = steps;
        emit stepsChanged(m_steps);
        if (m_steps != 0.0)
            update_values();
    }

    Q_INVOKABLE void setData( qreal index, qreal value )
    {
        if (index < 0 || index >= count())
            return;

        qreal old_data = m_data[index];
        m_data[index] = restricted_value(value);

        if (old_data != m_data[index]) {
            emit dataChanged( createIndex(index, 0), createIndex(index, 0) );
            emit valueChanged( index, m_data[index] );
        }
    }

    Q_INVOKABLE void setPositions( qreal start_x, qreal start_y,
                                   qreal end_x, qreal end_y )
    {
        //qDebug() << "setPositions" << start_x << start_y << end_x << end_y;
        qreal start_index, start_value, end_index, end_value;
        if (m_orientation == Qt::Vertical) {
            start_index = start_x; end_index = end_x;
            start_value = start_y; end_value = end_y;
        } else {
            start_index = start_y; end_index = end_y;
            start_value = start_x; end_value = end_x;
        }

        //qDebug() << "input" << start_index << start_value << end_index << end_value;

        start_index = index_for_position(start_index);
        end_index = index_for_position(end_index);
        start_value = value_for_position(start_value);
        end_value = value_for_position(end_value);

        //qDebug() << "output" << start_index << start_value << end_index << end_value;

        int first_index = start_index;
        int last_index = end_index;

        if (first_index == last_index) {
            setData(end_index, end_value);
            return;
        }

        qreal k = (end_value - start_value) / (end_index - start_index);
        qreal a = start_value - k * start_index;
        int count = this->count();

        if (last_index > first_index) {
            for (int i = first_index; i < last_index; ++i)
                if (i >= 0 && i < count)
                    m_data[i] = restricted_value( k * (i + 1) + a );
        }
        else {
            for (int i = first_index; i > last_index; --i)
                if (i >= 0 && i < count)
                    m_data[i] = restricted_value( k * i + a );
        }

        if (last_index  >= 0 && last_index < count)
            m_data[last_index] = restricted_value(end_value);

        int change_begin = qMax(0, qMin(first_index, last_index));
        int change_end =  qMin(count-1, qMax(first_index, last_index));

        if (change_begin < count && change_end > 0) {
            emit dataChanged( createIndex(change_begin, 0), createIndex(change_end, 0) );
            for (int i = change_begin; i <= change_end; ++i)
                emit valueChanged(i, m_data[i]);
        }
    }

    // Abstract interface

    virtual int rowCount(const QModelIndex & parent) const
    {
        return m_data.size();
    }

    QVariant data(const QModelIndex & index, int role) const
    {
        qreal value = m_data[index.row()];

        switch (role)
        {
        case ValueRole:
            return value;
        case PositionRole: {
            return position_for_value( value );
        }
        default:
            return value;
        }
    }

    QHash<int, QByteArray> roleNames() const
    {
        return m_role_names;
    }

signals:
    void countChanged(int);
    void boundsChanged(const QRectF &);
    void orientationChanged(int);
    void stepsChanged(qreal);
    void valueChanged(int index, qreal value);

private:
    void update_values()
    {
        for (int i = 0; i < m_data.size(); ++i)
            m_data[i] = restricted_value(m_data[i]);
        emit dataChanged( createIndex(0, 0), createIndex(m_data.size()-1, 0) );
        for (int i = 0; i < m_data.size(); ++i)
            emit valueChanged(i, m_data[i]);
    }

    qreal position_for_value( qreal value ) const
    {
        if (m_orientation == Qt::Horizontal)
            return value * m_bounds.width() + m_bounds.left();
        else
            return (1.0 - value) * m_bounds.height() + m_bounds.top();
    }

    qreal value_for_position( qreal position ) const
    {
        if (m_orientation == Qt::Horizontal) {
            qreal pos_range = m_bounds.width();
            return (pos_range > 0.0)
                    ? (position - m_bounds.left()) / pos_range
                    : 0.0;
        }
        else {
            qreal pos_range = m_bounds.height();
            return pos_range > 0.0
                    ? (m_bounds.bottom() - position) / pos_range
                    : 0.0;
        }
    }

    qreal index_for_position( qreal position ) const
    {
        if (m_orientation == Qt::Horizontal) {
            if (m_bounds.height() <= 0.0)
                return -1;
            return (position - m_bounds.top()) / m_bounds.height() * count();
        }
        else {
            if (m_bounds.width() <= 0.0)
                return -1;
            return (position - m_bounds.left()) / m_bounds.width() * count();
        }
    }

    qreal restricted_value( qreal value ) const
    {
        if (m_steps > 0)
            value = qRound( value * m_steps ) / (qreal) m_steps;
        value = qBound( 0.0, value, 1.0 );
        return value;
    }

    QHash<int, QByteArray> m_role_names;
    QRectF m_bounds;
    Qt::Orientation m_orientation;
    int m_steps;
    std::vector<qreal> m_data;
};

} // namespace quick_collider

#endif // QUICK_COLLIDER_MULTI_SLIDER_MODEL_INCLUDED
