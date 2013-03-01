/************************************************************************
*
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "oscilloscope.hpp"
#include "oscilloscope_shm.hpp"

#include <QSGGeometryNode>
#include <QSGTransformNode>
#include <QSGFlatColorMaterial>
#include <QDebug>

namespace QuickCollider {

Oscilloscope::Oscilloscope( QQuickItem * parent) :
    QQuickItem(parent),
    _srvPort(-1),
    _scopeIndex(-1),
    _shm(new OscilloscopeShm(this)),
    _running(false),
    _data(0),
    _availableFrames(0),
    mXOffset( 0.f ),
    mYOffset( 0.f ),
    mXZoom( 1.f ),
    mYZoom( 1.f ),
    _mode( TrackMode )
{
    setFlag( QQuickItem::ItemHasContents, true );

    timer = new QTimer( this );
    timer->setInterval( 50 );
    connect( timer, SIGNAL( timeout() ), this, SLOT( pullData() ) );
}

Oscilloscope::~Oscilloscope()
{
    stop();
}

void Oscilloscope::setServerPort( int port )
{
    if( _running ) {
        qWarning( "Oscilloscope: Can not change server port while running!" );
        return;
    }

    _srvPort = port;
}

void Oscilloscope::setBufferNumber( int n )
{
    if( _running ) {
        // TODO: release used reader?
        initScopeReader( _shm, n );
    }
    _scopeIndex = n;
}

int Oscilloscope::updateInterval() const {
    return timer->interval();
}

void Oscilloscope::setUpdateInterval( int interval ) {
    timer->setInterval( qMax(0, interval) );
}

void Oscilloscope::start()
{
    if( _running ) return;
    if( _srvPort < 0 || _scopeIndex < 0 ) return;

    connectSharedMemory( _srvPort );
    if( !_shm->client ) {
        stop();
        return;
    }

    initScopeReader( _shm, _scopeIndex );

    timer->start();

    _running = true;

    emit runningChanged(_running);
}

void Oscilloscope::stop()
{
    // TODO: release used reader?

    delete _shm->client;
    _shm->client = 0;

    timer->stop();

    _running = false;

    emit runningChanged(_running);
}

void Oscilloscope::pullData()
{
    bool valid = _shm->reader.valid();
    //qcDebugMsg(1, tr("valid = %1").arg(valid));
    if(!valid) return;

    bool ok = _shm->reader.pull( _availableFrames );
    //qcDebugMsg(1, tr("Got %1 frames").arg(_availableFrames) );
    if(ok) {
        _data = _shm->reader.data();
        update();
    }
}

QSGNode * Oscilloscope::updatePaintNode(QSGNode * oldNode,
                                         UpdatePaintNodeData * updatePaintNodeData)
{
    if (!_running || _availableFrames < 2) {
        delete oldNode;
        return 0;
    }

    int channel_count = _shm->reader.channels();
    int max_frame_count = _shm->reader.max_frames();

    if (m_dirty_top_node) {
        delete oldNode;
        oldNode = 0;
        m_dirty_top_node = false;
    }

    switch (_mode)
    {
    case TrackMode:
    case OverlayMode:
    {
        MultiTrackPlotter *node = static_cast<MultiTrackPlotter*>(oldNode);
        if (!node)
            node = new MultiTrackPlotter;

        node->setDataFormat( channel_count, _availableFrames );
        node->setSize( width(), height() );
        node->setScaling(mYZoom);
        node->setOverlay(_mode == OverlayMode);

        for( int ch = 0; ch < channel_count; ++ch ) {
            float *channel_data = _data + (ch * max_frame_count); //frame vector
            node->setChannelData(ch, channel_data);
        }

        if (m_dirty_colors && m_colors.count())
        {
            int colors_count = m_colors.count();
            for (int ch = 0; ch < channel_count; ++ch) {
                int color_idx = ch % colors_count;
                node->setChannelColor( ch, m_colors[color_idx].value<QColor>() );
            }
        }

        return node;
    }
    case XYMode:
    {
        XYPlotter *node = static_cast<XYPlotter*>(oldNode);
        if (!node)
            node = new XYPlotter;

        node->setSize( width(), height() );
        node->setScaling(mXZoom, mYZoom);

        if ( channel_count >= 2)
        {
            node->setData( _data,
                           _data + max_frame_count,
                           _availableFrames );
        }

        if (m_dirty_colors && m_colors.count())
            node->setColor( m_colors[0].value<QColor>() );

        return node;
    }
    default:
        return oldNode;
    }
}

void Oscilloscope::connectSharedMemory( int port )
{
    try {
        server_shared_memory_client * client = new server_shared_memory_client(port);
        _shm->client = client;
        qDebug("Oscilloscope: Shared memory connected");
    } catch (std::exception & e) {
        _shm->client = 0;
        qWarning() << "Oscilloscope: Cannot connect to shared memory:" << e.what();
    }
}

void Oscilloscope::initScopeReader( OscilloscopeShm *shm, int index )
{
    shm->reader = shm->client->get_scope_buffer_reader( index );
    qDebug() << "Oscilloscope: Initialized scope buffer reader for index:" << index;
}

MultiTrackPlotter::MultiTrackPlotter():
    m_frame_count(0),
    m_channel_count(0),
    m_overlay(false),
    m_scaling(1.0),
    m_width(1.0),
    m_height(1.0)
{}

void MultiTrackPlotter::setDataFormat( int channels, int frames )
{
    if (channels == m_channel_count && frames == m_frame_count)
        return;

    m_frame_count = frames;

    if (channels != m_channel_count)
    {
        m_channel_count = channels;

        removeAllChildNodes();

        for (int idx = 0; idx < m_channel_count; ++idx)
        {
            PlotNode1D *plotNode = new PlotNode1D;
            QSGTransformNode *transformNode = new QSGTransformNode;
            transformNode->appendChildNode(plotNode);
            this->appendChildNode(transformNode);
        }
    }

    updateTransform();
}

void MultiTrackPlotter::setChannelData( int channel, float *data )
{
    Q_ASSERT(channel >= 0 && channel < m_channel_count);

    QSGNode *transformNode = childAtIndex(channel);
    PlotNode1D *plotNode = static_cast<PlotNode1D*>( transformNode->childAtIndex(0) );
    plotNode->setData(data, m_frame_count);
}

void MultiTrackPlotter::setOverlay( bool overlay )
{
    if (m_overlay == overlay)
        return;
    m_overlay = overlay;
    updateTransform();
}

void MultiTrackPlotter::setSize( qreal width, qreal height )
{
    if (width == m_width && height == m_height)
        return;
    m_width = width;
    m_height = height;
    updateTransform();
}

void MultiTrackPlotter::setScaling( qreal scaling )
{
    if (scaling == m_scaling)
        return;
    m_scaling = scaling;
    updateTransform();
}

void MultiTrackPlotter::setColor( const QColor & color )
{
    for (int idx = 0; idx < m_channel_count; ++idx)
    {
        setChannelColor(idx, color);
    }
}

void MultiTrackPlotter::setChannelColor( int channel, const QColor & color )
{
    QSGNode *transformNode = childAtIndex(channel);
    PlotNode1D *plotNode = static_cast<PlotNode1D*>( transformNode->childAtIndex(0) );
    plotNode->setColor(color);
}

void MultiTrackPlotter::updateTransform()
{
    qreal y_scaling = 1.0;
    y_scaling *= m_height * 0.5;
    if (!m_overlay && m_channel_count > 1)
        y_scaling /= m_channel_count;

    qreal x_scaling = m_width;
    if (m_frame_count > 1)
        x_scaling /= (m_frame_count - 1);

    for (int idx = 0; idx < m_channel_count; ++idx)
    {
        qreal y_translation = 1.0;
        if (!m_overlay)
            y_translation += idx * 2.0;

        QMatrix4x4 matrix;
        matrix.scale(x_scaling, y_scaling);
        matrix.translate(0.0, y_translation);
        matrix.scale(1.0, -m_scaling); // flip y axis!

        QSGTransformNode *transformNode = static_cast<QSGTransformNode*>( childAtIndex(idx) );;
        transformNode->setMatrix(matrix);
    }
}

XYPlotter::XYPlotter():
    m_width(1.0),
    m_height(1.0),
    m_x_scaling(1.0),
    m_y_scaling(1.0)
{
    m_geom_node = new PlotNode2D;
    appendChildNode( m_geom_node );
}

void XYPlotter::setSize( qreal width, qreal height )
{
    if (width == m_width && height == m_height)
        return;
    m_width = width;
    m_height = height;
    updateTransform();
}

void XYPlotter::setScaling( qreal x_scaling, qreal y_scaling )
{
    if(m_x_scaling == x_scaling && m_y_scaling == y_scaling)
        return;
    m_x_scaling = x_scaling;
    m_y_scaling = y_scaling;
    updateTransform();
}

void XYPlotter::setColor( const QColor & color )
{
    m_geom_node->setColor(color);
}

void XYPlotter::setData( float * x_data, float * y_data, int count )
{
    m_geom_node->setData(x_data, y_data, count);
}

void XYPlotter::updateTransform()
{
    qreal size_scale = qMin(m_width, m_height);

    QMatrix4x4 matrix;
    matrix.translate(m_width * 0.5,
                     m_height * 0.5);
    matrix.scale(0.5 * size_scale * m_x_scaling,
                 -0.5 * size_scale * m_y_scaling); // flip y axis!
    setMatrix(matrix);
}

PlotNode1D::PlotNode1D():
    m_geometry(QSGGeometry::defaultAttributes_Point2D(), 0)
{
    m_geometry.setDrawingMode(GL_LINE_STRIP);
    m_geometry.setLineWidth(1);
    //m_geometry.setVertexDataPattern(QSGGeometry::DynamicPattern);

    setGeometry(&m_geometry);
    setMaterial(&m_material);
}

void PlotNode1D::setData( float * data, int count )
{
    if (m_geometry.vertexCount() != count)
        m_geometry.allocate(count);

    QSGGeometry::Point2D *vertices = m_geometry.vertexDataAsPoint2D();

    for (int idx = 0; idx < count; ++idx)
        vertices[idx].set( (float) idx, data[idx] );

    markDirty(QSGNode::DirtyGeometry);
}

void PlotNode1D::setColor( const QColor & color )
{
    m_material.setColor(color);
    markDirty(QSGNode::DirtyMaterial);
}

PlotNode2D::PlotNode2D():
    m_geometry(QSGGeometry::defaultAttributes_Point2D(), 0)
{
    m_geometry.setDrawingMode(GL_LINE_STRIP);
    m_geometry.setLineWidth(1);
    //m_geometry.setVertexDataPattern(QSGGeometry::DynamicPattern);

    setGeometry(&m_geometry);
    setMaterial(&m_material);
}

void PlotNode2D::setData( float * x_data, float * y_data, int count )
{
    if (m_geometry.vertexCount() != count)
        m_geometry.allocate(count);

    QSGGeometry::Point2D *vertices = m_geometry.vertexDataAsPoint2D();

    for (int idx = 0; idx < count; ++idx)
        vertices[idx].set( x_data[idx], y_data[idx] );

    markDirty(QSGNode::DirtyGeometry);
}

void PlotNode2D::setColor( const QColor & color )
{
    m_material.setColor(color);
    markDirty(QSGNode::DirtyMaterial);
}

} // namespace QuickCollider
