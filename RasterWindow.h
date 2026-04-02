/* date = March 24th 2026 2:49 pm */

#ifndef _RASTER_WINDOW_H
#define _RASTER_WINDOW_H

#include <QSpinBox>
#include <QLabel>

struct RenderArea : public QWidget
{
    enum Shape 
	{ 
		Line, 
		Points, 
		Polyline, 
		Polygon, 
		Rect, 
		RoundedRect, 
		Ellipse, 
		Arc,
		Chord, 
		Pie, 
		Path, 
		Text, 
		Pixmap
	};
	
    RenderArea(QWidget *parent);
	~RenderArea();
	
	QSize sizeHint() const 				   override;
    QSize minimumSizeHint() const 		    override;
	void paintEvent(QPaintEvent *event)	   override;
	
    void setShape(Shape shape);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
    void setTransformed(bool transformed);
	
    Shape shape;
    QPen pen;
    QBrush brush;
    bool antialiased;
    bool transformed;
    QPixmap pixmap;
};

RenderArea::RenderArea(QWidget *parent) : QWidget(parent)
{
    shape = Polygon;
    antialiased = false;
    transformed = false;
    //pixmap.load(":/images/qt-logo.png");
	
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

RenderArea::~RenderArea(){}

QSize RenderArea::sizeHint() const
{
    return QSize(400, 200);
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100); 
}

void RenderArea::paintEvent(QPaintEvent *event)
{
	static const QPoint points[4] = 
	{
        QPoint(10, 80),
        QPoint(20, 10),
        QPoint(80, 30),
        QPoint(90, 70)
    };
	
    QRect rect(10, 20, 80, 60);
	
    QPainterPath path;
    path.moveTo(20, 80);
    path.lineTo(20, 30);
    path.cubicTo(80, 0, 50, 50, 80, 80);
	
    int startAngle = 20 * 16;
    int arcLength = 120 * 16;
}

struct SubWindow : public QWidget
{
	RenderArea *renderArea;
	QSpinBox *penWidthSpinBox;
    QLabel *labels[6];
    QComboBox *Dropdowns[4];
	QComboBox *proto;
};

struct RasterWindow : public QWindow
{
    explicit RasterWindow(QWindow *parent = nullptr);
	~RasterWindow();
	
    virtual void render(QPainter *painter);
    void renderLater();
    void renderNow();
	
    bool event(QEvent *event) 			override;
    void resizeEvent(QResizeEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;
	
	SubWindow sub_window;
    QScopedPointer<QBackingStore> m_backingStore;
};

void RasterWindow::renderLater()
{
    requestUpdate();
}

// NOTE:  If RasterWindow inherits the QWindow(...) constructor, why can't it call it in its own
//		definition ? Why is the initilizer list required to call the QWindow constructor ?
//		Why can't I set the m_backingStore pointer inside the RasterWindow(...) function ?

RasterWindow::RasterWindow(QWindow *parent) : QWindow(parent), m_backingStore(new QBackingStore(this))
{
    setGeometry(200, 200, 800, 800);
}

RasterWindow::~RasterWindow(){}


void RasterWindow::render(QPainter *painter)
{
	painter->drawText(QRectF(0, 0, width(), height()), 
					  Qt::AlignCenter, 
					  QStringLiteral("QWindow"));
	
	QRectF rec_shape(10, 10, width() / 2, height() / 2);
	painter->drawRect(rec_shape);
}

bool RasterWindow::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest) 
	{
        renderNow();
        return true;
    }
    return QWindow::event(event);
}

void RasterWindow::resizeEvent(QResizeEvent *resizeEvent)
{
    m_backingStore->resize(resizeEvent->size());
}

void RasterWindow::exposeEvent(QExposeEvent *)
{
    if (isExposed())
	{
        renderNow();
	}
}

void RasterWindow::renderNow()
{
    if (isExposed() == 0)
	{
        return;		
	}
	
    QRect rect(0, 0, width(), height());
    m_backingStore->beginPaint(rect);
	
    QPaintDevice *device = m_backingStore->paintDevice();
    QPainter painter(device);
	
    painter.fillRect(0, 0, width(), height(), QGradient::PerfectBlue);
    render(&painter);
    painter.end();
	
    m_backingStore->endPaint();
    m_backingStore->flush(rect);
}

#endif //_RASTER_WINDOW_H
