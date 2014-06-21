#include <cassert>
#include <QDebug>

#include <QWidget>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QSvgRenderer>
#include <cmath>

#include "gamewidget.h"

GameWidget::GameWidget(QGLWidget *parent) :
    QGLWidget(parent),
    m_composit(800,600),
    m_level(QString("://level")),
    m_ticks(0),
    m_player_x(400),
    m_player_y(300),
    m_player_size(32)
{
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
    this->repaint();
}

//ON EVERY REPAINT
void GameWidget::paintEvent(QPaintEvent *)
{
    QPainter compositor(&m_composit);
    compositor.drawPixmap(0,0,m_composit.width(),m_composit.height(),m_level);
    compositor.setBrush(QBrush(QColor(255,128,64,255)));
    compositor.drawRect(((4*m_ticks)%1024)-64,128,64,32);
    compositor.setBrush(QBrush(QColor(64,128,255,255)));
    compositor.drawRect((((4*m_ticks)+256)%1024)-64,128,64,32);
    compositor.setBrush(QBrush(QColor(64,128,64,255)));
    compositor.drawRect(-((3*m_ticks)%1280)+1024,196,64,32);
    m_svgrend.render(&compositor, QRect( (m_player_x-(m_player_size/2)),
                                    (m_player_y-(m_player_size/2)),
                                     m_player_size, m_player_size ));
    QSvgRenderer carrend(QString("://car_kangoo"));
    carrend.render(&compositor, QRect( 128,
                                    256,
                                     101, 161 ));
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(0,0,this->width(),this->height(),m_composit);
}

//ON EVERY KEYS PRESS
void GameWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()){
        case Qt::Key_Up: --m_player_y;
    break;
        case Qt::Key_Down: ++m_player_y;
    break;
        case Qt::Key_Left:
    break;
        case Qt::Key_Right:
    break;
        case Qt::Key_Q: this->close();
    break;
        default:
    break;
    }
}

//ON EVERY KEYS RELEASE
void GameWidget::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key()){
        default:
    break;
    }
}
