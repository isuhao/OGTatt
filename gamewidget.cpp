#include <cassert>
#include <QDebug>

#include <QWidget>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QSvgRenderer>
#include <cmath>

#include "gamewidget.h"

GameWidget::GameWidget(const QGLFormat &format, QWidget *parent) :
    QGLWidget(parent),
    m_composit(800,600),
    m_level(QString("://level")),
    m_ticks(0),
    m_frode(0),
    m_kangoo()
{
    //HIDE MOUSECURSOR
    setCursor(Qt::BlankCursor);
    //CREATE TIME
    QTimer * const timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(OnTimer()));
    timer->setInterval(23);
    timer->start();
}

//ON EVERY TIMER SIGNAL
void GameWidget::OnTimer()
{
    ++m_ticks;
    m_frode.KeyAction();
    m_frode.Move();
    m_kangoo.Move();
    this->repaint();
}

//ON EVERY REPAINT
void GameWidget::paintEvent(QPaintEvent *)
{
    QPainter compositor(&m_composit);
    compositor.setRenderHint(compositor.RenderHint::Antialiasing);

    //DRAW BACKDROP
    compositor.drawPixmap(0,0,m_composit.width(),m_composit.height(),m_level);

    //DRAW PASSING RECTANGLES
    compositor.setBrush(QBrush(QColor(255,128,64,255)));
    compositor.drawRect(((4*m_ticks)%1024)-64,150,64,32);
    compositor.setBrush(QBrush(QColor(64,128,255,255)));
    compositor.drawRect((((4*m_ticks)+256)%1024)-64,148,64,32);
    compositor.setBrush(QBrush(QColor(64,128,64,255)));
    compositor.drawRect(-((3*m_ticks)%1280)+1024,64,64,32);

    //DRAW GAME OBJECTS
    m_frode.Draw(&compositor);
    m_kangoo.Draw(&compositor);

    //DRAW COMPOSED PIXMAP TO WIDGET
    QPainter painter(this);
    painter.drawPixmap(0,0,this->width(),this->height(),m_composit);
}

//ON EVERY KEYS PRESS
void GameWidget::keyPressEvent(QKeyEvent *e)
{
    m_frode.KeyPress(e->key());

    //QUIT
    switch (e->key()){
        case Qt::Key_Q: this->close(); break;
////////////////TEMP!!!!!!!!!!!!!!!!!!!!!!
        case Qt::Key_Return: {if (m_frode.GetVehicle()) m_frode.LeaveVehicle();
                              else m_frode.EnterVehicle(&m_kangoo); break;}
    default:break;
    }

}

//ON EVERY KEYS RELEASE
void GameWidget::keyReleaseEvent(QKeyEvent *e)
{
    m_frode.KeyRelease(e->key());
}
