#ifndef QUICK_COLLIDER_MULTI_SLIDER_MODEL_INCLUDED
#define QUICK_COLLIDER_MULTI_SLIDER_MODEL_INCLUDED

#include <QAbstractListModel>
#include <QDebug>

#include <vector>

namespace quick_collider {

class multi_slider_model : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( qreal step READ step WRITE set_step NOTIFY stepChanged )
    Q_PROPERTY( int count READ count WRITE setCount )

public:

    multi_slider_model( QObject * parent = 0):
        QAbstractListModel(parent)
    {
        m_role_names.insert(Qt::UserRole, "value");
    }

    qreal step() const { return m_step; }

    void set_step( qreal step )
    {
        if (step == m_step)
            return;

        m_step = step;

        emit stepChanged(m_step);

        if (m_step != 0.0) {
            for (int i = 0; i < m_data.size(); ++i)
                m_data[i] = restricted_value(m_data[i]);
            emit dataChanged( createIndex(0, 0), createIndex(m_data.size()-1, 0) );
            for (int i = 0; i < m_data.size(); ++i)
                emit valueChanged(i, m_data[i]);
        }
    }

    int count() const { return m_data.size(); }

    void setCount( int count )
    {
        beginResetModel();
        m_data.resize(count);
        endResetModel();
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

    Q_INVOKABLE void setData( qreal start_index, qreal start_value,
                              qreal end_index, qreal end_value )
    {
        if (start_index > end_index) {
            qreal tmp_index = end_index;
            qreal tmp_value = end_value;
            end_index = start_index;
            end_value = start_value;
            start_index = tmp_index;
            start_value = tmp_value;
        }

        int first_index = start_index;
        int last_index = end_index;

        if (first_index == last_index) {
            setData(end_index, end_value);
            return;
        }

        qreal k = (end_value - start_value) / (end_index - start_index);
        qreal a = start_value - k * start_index;

        for (int i = first_index; i >= 0 && i < count() && i <= last_index; ++i)
            m_data[i] = restricted_value( k * i + a );

        emit dataChanged( createIndex(first_index, 0), createIndex(last_index, 0) );

        for (int i = first_index; i <= last_index; ++i)
            emit valueChanged(i, m_data[i]);
    }

    // Abstract interface

    virtual int rowCount(const QModelIndex & parent) const
    {
        return m_data.size();
    }

    QVariant data(const QModelIndex & index, int role) const
    {
        int row = index.row();
        return QVariant( m_data[row] );
    }

    QHash<int, QByteArray> roleNames() const
    {
        return m_role_names;
    }

signals:
    void stepChanged(qreal);
    void valueChanged(int index, qreal value);

private:
    qreal restricted_value( qreal value )
    {
        if (m_step != 0.0)
            value = qRound( value / m_step ) * m_step;
        value = qBound( 0.0, value, 1.0 );
        return value;
    }

    QHash<int, QByteArray> m_role_names;
    qreal m_step;
    std::vector<qreal> m_data;
};

} // namespace quick_collider

#endif // QUICK_COLLIDER_MULTI_SLIDER_MODEL_INCLUDED
