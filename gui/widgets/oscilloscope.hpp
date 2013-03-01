/************************************************************************
*
* Copyright 2010-2011 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of Qt GUI for SuperCollider.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
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

#ifndef QUICKCOLLIDER_OSCILLOSCOPE_HPP_INCLUDED
#define QUICKCOLLIDER_OSCILLOSCOPE_HPP_INCLUDED

#include <QQuickItem>
#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QSGFlatColorMaterial>
#include <QTimer>


// FIXME: Due to Qt bug #22829, moc can not process headers that include
// boost/type_traits/detail/has_binary_operator.hp from boost 1.48, so
// we have to wrap the shm interface into a separate class.

namespace QuickCollider
{
class OscilloscopeShm;
class PlotNode2D;

class Oscilloscope : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS( Mode )
    Q_PROPERTY( int server READ serverPort WRITE setServerPort )
    Q_PROPERTY( int buffer READ bufferNumber WRITE setBufferNumber )
    Q_PROPERTY( float xOffset READ xOffset WRITE setXOffset )
    Q_PROPERTY( float yOffset READ yOffset WRITE setYOffset )
    Q_PROPERTY( float xZoom READ xZoom WRITE setXZoom )
    Q_PROPERTY( float yZoom READ yZoom WRITE setYZoom )
    Q_PROPERTY( Mode mode READ mode WRITE setMode )
    Q_PROPERTY( QVariantList trackColors READ trackColors WRITE setTrackColors )
    Q_PROPERTY( int updateInterval READ updateInterval WRITE setUpdateInterval )
    Q_PROPERTY( bool running READ running WRITE setRunning NOTIFY runningChanged )

public:
    enum Mode {
        TrackMode,
        OverlayMode,
        XYMode
    };

    Oscilloscope( QQuickItem * parent = 0 );
    ~Oscilloscope();
    int serverPort() const { return _srvPort; }
    void setServerPort( int );

    void setBufferNumber( int );
    int bufferNumber() const { return _scopeIndex; }

    float xOffset() const { return mXOffset; }
    void setXOffset( float f ) { mXOffset = f; }

    float yOffset() const { return mYOffset; }
    void setYOffset( float f ) { mYOffset = f; }

    float xZoom() const { return mXZoom; }
    void setXZoom( float f ) { mXZoom = f; }

    float yZoom() const { return mYZoom; }
    void setYZoom( float f ) { mYZoom = f; }

    Mode mode() const { return _mode; }
    void setMode( Mode mode ) { _mode = mode; m_dirty_top_node = true; }

    QVariantList trackColors() const { return m_colors; }
    void setTrackColors( const QVariantList & colors )
    {
        m_colors = colors;
        m_dirty_colors = true;
        update();
    }

    int updateInterval() const;
    void setUpdateInterval( int i );

    bool running() const { return _running; }
    void setRunning( bool running )
    {
        if (running)
            start();
        else
            stop();
    }

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    void runningChanged( bool running );

protected:
    QSGNode * updatePaintNode(QSGNode * oldNode, UpdatePaintNodeData * updatePaintNodeData);

private Q_SLOTS:
    void pullData();

private:
    void connectSharedMemory( int port );
    void initScopeReader( OscilloscopeShm *, int index );

    int _srvPort;
    int _scopeIndex;

    OscilloscopeShm *_shm;

    bool _running;
    float *_data;
    uint _availableFrames;

    QTimer *timer;
    float mXOffset;
    float mYOffset;
    float mXZoom;
    float mYZoom;
    Mode _mode;
    QVariantList m_colors;

    bool m_dirty_top_node;
    bool m_dirty_colors;
};

class MultiTrackPlotter : public QSGNode
{
public:
    MultiTrackPlotter();
    void setOverlay( bool overlay );
    void setDataFormat( int channels, int frames );
    void setChannelData( int channel, float *data );
    void setColor( const QColor & color );
    void setChannelColor( int channel, const QColor & color );
    void setSize( qreal width, qreal height );
    void setScaling( qreal scaling );

private:
    void updateTransform();

    int m_frame_count;
    int m_channel_count;
    bool m_overlay;
    qreal m_scaling;
    qreal m_width;
    qreal m_height;
};

class XYPlotter : public QSGTransformNode
{
public:
    XYPlotter();

    void setSize( qreal width, qreal height );
    void setScaling( qreal x_scaling, qreal y_scaling );
    void setColor( const QColor & color );
    void setData( float * x_data, float * y_data, int count );

private:
    void updateTransform();

    PlotNode2D *m_geom_node;
    qreal m_width;
    qreal m_height;
    qreal m_x_scaling;
    qreal m_y_scaling;
};

class PlotNode1D : public QSGGeometryNode
{
public:
    PlotNode1D();
    void setData( float * data, int count );
    void setColor( const QColor & color );

private:
    QSGGeometry m_geometry;
    QSGFlatColorMaterial m_material;
};

class PlotNode2D: public QSGGeometryNode
{
public:
    PlotNode2D();
    void setData( float * x_data, float * y_data, int count );
    void setColor( const QColor & color );

private:
    QSGGeometry m_geometry;
    QSGFlatColorMaterial m_material;
};

} // namespace QuickCollider

#endif
