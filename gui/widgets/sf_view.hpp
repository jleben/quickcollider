/*
  QuickCollider - Qt Quick based GUI for SuperCollider

  Copyright 2010-2013 Jakob Leben (jakob.leben@gmail.com)

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

#ifndef QUICK_COLLIDER_SOUND_FILE_VIEW_INCLUDED
#define QUICK_COLLIDER_SOUND_FILE_VIEW_INCLUDED

#include <QQuickPaintedItem>
#include <QThread>

#include <sndfile.h>

namespace QuickCollider {

class SoundFileView;
class SoundCacheStream;

struct SoundCache {
    SoundCache() : min(0), max(0), sum(0), sum2(0) {};
    ~SoundCache() {
        delete [] min;
        delete [] max;
        delete [] sum;
        delete [] sum2;
    }
    short *min;
    short *max;
    float *sum;
    float *sum2;
};

class SoundFileView : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY( float readProgress READ loadProgress )

    Q_PROPERTY( int firstFrame READ firstFrame )
    Q_PROPERTY( int frames READ frames )

    Q_PROPERTY( qreal firstVisibleFrame READ firstVisibleFrame WRITE setFirstVisibleFrame )
    Q_PROPERTY( qreal visibleFrames READ visibleFrames WRITE setVisibleFrames )
    Q_PROPERTY( qreal firstVisibleTime READ firstVisibleTime WRITE setFirstVisibleTime )
    Q_PROPERTY( qreal visibleDuration READ visibleDuration WRITE setVisibleDuration )
    Q_PROPERTY( qreal scroll READ scroll WRITE setScroll )
    Q_PROPERTY( qreal zoom READ zoom WRITE setZoom NOTIFY visibleRangeChanged )
    Q_PROPERTY( qreal framesPerPixel READ framesPerPixel NOTIFY visibleRangeChanged );

    Q_PROPERTY( int currentSelection READ currentSelection WRITE setCurrentSelection )
    Q_PROPERTY( QVariantList selections READ selections )

    Q_PROPERTY( qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged )
    Q_PROPERTY( qreal scale READ scale WRITE setScale )

    Q_PROPERTY( bool cursorVisible READ cursorVisible WRITE setCursorVisible )
    Q_PROPERTY( bool cursorEditable READ cursorEditable WRITE setCursorEditable )
    Q_PROPERTY( int cursorPosition READ cursorPosition WRITE setCursorPosition )
    Q_PROPERTY( bool gridVisible READ gridVisible WRITE setGridVisible )
    Q_PROPERTY( float gridOffset READ gridOffset WRITE setGridOffset )
    Q_PROPERTY( float gridResolution READ gridResolution WRITE setGridResolution )
    Q_PROPERTY( bool drawsWaveform READ drawsWaveform WRITE setDrawsWaveform )
    Q_PROPERTY( QColor peakColor READ peakColor WRITE setPeakColor )
    Q_PROPERTY( QColor rmsColor READ rmsColor WRITE setRmsColor )
    Q_PROPERTY( QColor cursorColor READ cursorColor WRITE setCursorColor )
    Q_PROPERTY( QColor gridColor READ gridColor WRITE setGridColor )

public:

    Q_INVOKABLE void load( const QString& filename );

    // NOTE: Using int instead of sf_count_t for accessibility from SC language.
    Q_INVOKABLE void load( const QString& filename, int beginning, int duration );

    Q_INVOKABLE void load( const QVector<double> & data, int offset = 0,
                           int channels = 1, int samplerate = 44100 );

    Q_INVOKABLE void allocate ( int frames, int channels = 1, int samplerate = 44100 );

    Q_INVOKABLE void write( const QVector<double> & data, int offset );

    Q_INVOKABLE qreal frameAt( qreal position )
    {
        QRectF rect = contentsBoundingRect();
        qreal relativePosition = rect.width() ? position / rect.width() : 0.0;
        return (relativePosition * _dur + _beg);
    }

public:

    struct Selection {
        Selection() : start(0), size(0), editable(true), color(QColor(0,0,150)) {}
        sf_count_t start;
        sf_count_t size;
        bool editable;
        QColor color;
    };

    SoundFileView( QQuickItem * parent = 0 );
    ~SoundFileView();

    float loadProgress();

    sf_count_t firstFrame() const { return _rangeBeg; }
    sf_count_t frames() const { return _rangeDur; }

    qreal firstVisibleFrame() const { return _beg; }
    qreal visibleFrames() const { return _dur; }
    void setFirstVisibleFrame( qreal frame );
    void setVisibleFrames( qreal frames );

    qreal firstVisibleTime() const;
    qreal visibleDuration() const;
    void setFirstVisibleTime( qreal seconds );
    void setVisibleDuration( qreal seconds );

    qreal zoom() const;
    void setZoom( qreal zoom );
    Q_INVOKABLE void zoomTo( qreal position, qreal zoom );
    qreal scroll() const; // as fraction of scrolling range
    void setScroll( qreal scroll ); // as fraction of scrolling range

    qreal framesPerPixel() { return _fpp; }

    qreal scale() const { return _yZoom; }
    void setScale( qreal scale ) { _yZoom = scale; update(); }

    qreal spacing() const { return m_spacing; }
    void setSpacing( qreal spacing ) {
        m_spacing = qMax(0.0, spacing);
        update();
        emit spacingChanged(m_spacing);
    }

    QVariantList selections() const;
    int currentSelection() const { return _curSel; }
    void setCurrentSelection( int i );
    // for SC: selection start relative to first loaded frame
    Q_INVOKABLE QVariantList selection( int index ) const;
    // for SC: selection start relative to first loaded frame
    Q_INVOKABLE void setSelection( int index, QVariantList data );
    void setSelection( int i, sf_count_t a, sf_count_t b );
    Q_INVOKABLE void setSelectionStart( int i, sf_count_t frame );
    Q_INVOKABLE void setSelectionEnd( int i, sf_count_t frame );
    Q_INVOKABLE void setSelectionEditable( int index, bool editable );
    Q_INVOKABLE void setSelectionColor( int index, const QColor &clr );
    Q_PROPERTY( QVariantList waveColors READ waveColors WRITE setWaveColors );

    bool cursorVisible() const { return _showCursor; }
    void setCursorVisible( bool b ) { _showCursor = b; update(); }
    int cursorPosition() const { return _cursorPos; }
    void setCursorPosition( int pos ) { _cursorPos = pos; update(); }
    bool cursorEditable() const { return _cursorEditable; }
    void setCursorEditable( bool b ) { _cursorEditable = b; }

    bool gridVisible() const { return _showGrid; }
    void setGridVisible( bool b ) { _showGrid = b; update(); }
    float gridOffset() const { return _gridOffset; }
    void setGridOffset( float f ) { _gridOffset = f; update(); }
    float gridResolution() const { return _gridResolution; }
    void setGridResolution( float f ) { _gridResolution = f; update(); }

    bool drawsWaveform() const { return _drawWaveform; }
    void setDrawsWaveform( bool b ) { _drawWaveform = b; update(); }
    QVariantList waveColors() const;
    void setWaveColors( const QVariantList & );

    const QColor & peakColor() const { return _peakColor; }
    void setPeakColor( const QColor &clr ) { _peakColor = clr; update(); }
    const QColor & rmsColor() const { return _rmsColor; }
    void setRmsColor( const QColor &clr ) { _rmsColor = clr; update(); }
    const QColor & cursorColor() const { return _cursorColor; }
    void setCursorColor( const QColor &c ) { _cursorColor = c; update(); }
    const QColor & gridColor() const { return _gridColor; }
    void setGridColor( const QColor &c ) { _gridColor = c; update(); }

    //QSize sizeHint() const { return QSize( 400, 200 ); }
    //QSize minimumSizeHint() const { return QSize( 100, 20 ); }

public slots:
    void scrollToStart();
    void scrollToEnd();
    void zoomAllOut();
    void zoomSelection( int selectionIndex );

signals:
    void loadProgress( int );
    void loadingDone();

    void spacingChanged( qreal );
    void visibleRangeChanged();

protected:
#if 0
    virtual void resizeEvent( QResizeEvent * );
    virtual void paintEvent( QPaintEvent * );
    virtual void keyPressEvent( QKeyEvent * );
    virtual void mousePressEvent( QMouseEvent * );
    virtual void mouseDoubleClickEvent ( QMouseEvent * );
    virtual void mouseMoveEvent( QMouseEvent * );
#endif
    virtual void geometryChanged (const QRectF & old_geom, const QRectF & new_geom);
    virtual void paint(QPainter *);

private:

    void doLoad( SNDFILE *new_sf, const SF_INFO &new_info, sf_count_t beginning, sf_count_t duration );
    void updateFPP()
    {
        qreal width = contentsBoundingRect().width();
        _fpp = width > 0.0 ? (double) _dur / width : 0.0;
    }
    void rebuildCache ( int maxFramesPerCache, int maxRawFrames );
    //void draw ( QPixmap *, int x, int width, double beginning, double duration );

    // data
    SNDFILE *sf;
    SF_INFO sfInfo;
    sf_count_t _rangeBeg;
    sf_count_t _rangeDur;
    sf_count_t _rangeEnd;

    SoundCacheStream *_cache;

    // selections
    Selection _selections[64];
    int _curSel;

    // cursor
    bool _showCursor;
    sf_count_t _cursorPos;
    bool _cursorEditable;

    //grid
    bool _showGrid;
    float _gridResolution;
    float _gridOffset;

    // view
    double _beg;
    double _dur;
    double _fpp;
    float _yZoom;

    // painting
    qreal m_spacing;
    QColor _peakColor;
    QColor _rmsColor;
    QColor _cursorColor;
    QColor _gridColor;
    bool dirty;
    bool _drawWaveform;
    QList<QColor> _waveColors;

    // interaction
    enum DragAction {
        NoDragAction,
        Navigate,
        Select,
        MoveSelection,
        MoveCursor
    };
    DragAction _dragAction;
    QPointF _dragPoint;
    sf_count_t _dragFrame;
    double _dragData;
    double _dragData2;
};

class SoundStream {
public:
    inline int channels() { return _ch; }

    inline sf_count_t beginning() { return _beg; }

    inline sf_count_t duration() { return _dur; }

    virtual bool displayData( int channel, double offset, double duration,
                              short *minBuffer,
                              short *maxBuffer,
                              short *minRMS,
                              short *maxRMS,
                              int bufferSize ) = 0;

    virtual short *rawFrames( int channel, sf_count_t beginning, sf_count_t duration, bool *interleaved ) = 0;

protected:
    SoundStream()
        : _ch( 0 ), _beg( 0 ), _dur( 0 ) {}

    SoundStream( int channels, sf_count_t beginning, sf_count_t duration ) :
        _ch( channels ), _beg( beginning ), _dur( duration ) {}

    int _ch;
    sf_count_t _beg;
    sf_count_t _dur;
};

class SoundFileStream : public SoundStream
{
public:
    SoundFileStream();
    SoundFileStream( SNDFILE *sf, const SF_INFO &sf_info, sf_count_t beginning, sf_count_t duration );
    ~SoundFileStream();
    void load( SNDFILE *sf, const SF_INFO &sf_info, sf_count_t beginning, sf_count_t duration );
    bool integrate( int channel, double offset, double duration,
                    short *minBuffer,
                    short *maxBuffer,
                    float *sumBuffer,
                    float *sum2Buffer,
                    int bufferSize );
    bool displayData( int channel, double offset, double duration,
                      short *minBuffer,
                      short *maxBuffer,
                      short *minRMS,
                      short *maxRMS,
                      int bufferSize );
    short *rawFrames( int channel, sf_count_t beginning, sf_count_t duration, bool *interleaved );
private:
    short *_data;
    sf_count_t _dataSize;
    sf_count_t _dataOffset;
};

class SoundCacheLoader;

class SoundCacheStream : public QObject, public SoundStream
{
    friend class SoundCacheLoader;

    Q_OBJECT

public:
    SoundCacheStream();
    ~SoundCacheStream();
    void load( const QVector<double> & data, int frames, int offset, int channels );
    void load( SNDFILE *sf, const SF_INFO &info, sf_count_t beg, sf_count_t dur,
               int maxFramesPerUnit, int maxRawFrames );
    void allocate ( int frames, int channels );
    void write( const QVector<double> & data, int offset, int count );

    inline double fpu() { return _fpu; }
    inline bool ready() { return _ready; }
    inline bool loading() { return _loading; }
    inline int loadProgress() { return _loadProgress; }
    bool displayData( int channel, double offset, double duration,
                      short *minBuffer,
                      short *maxBuffer,
                      short *minRMS,
                      short *maxRMS,
                      int bufferSize );
    short *rawFrames( int channel, sf_count_t beginning, sf_count_t duration, bool *interleaved );

Q_SIGNALS:
    void loadProgress( int );
    void loadingDone();

private Q_SLOTS:
    void onLoadProgress( int );
    void onLoadingDone();

private:
    SoundCache *_caches;
    double _fpu; // soundfile frames per cache unit
    sf_count_t _dataOffset; // offset into soundfile of first frame cached (in frames)
    sf_count_t _dataSize; // amount of cache units
    bool _ready;
    bool _loading;
    SoundCacheLoader *_loader;
    int _loadProgress;
};

class SoundCacheLoader : public QThread
{
    Q_OBJECT
public:
    SoundCacheLoader( SoundCacheStream *cache ) : QThread( cache ), _cache( cache ), _sf(0) {}
    void load( SNDFILE *sf, const SF_INFO &info );

Q_SIGNALS:
    void loadProgress( int );
    void loadingDone();
private:
    void run();

    SoundCacheStream *_cache;
    SNDFILE *_sf;
    SF_INFO _info;
};

} // namespace QuickCollider

#endif // QUICK_COLLIDER_SOUND_FILE_VIEW_INCLUDED
