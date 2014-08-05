#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <player.h>
#include <QGLWidget>
#include <QSvgRenderer>

namespace Ui {
class GameWidget;
}

class GameWidget : public QGLWidget
{
    Q_OBJECT

public:
    GameWidget(const QGLFormat& format, QWidget *parent = 0);

protected:
    //virtual void initializeGL();
    //virtual void resizeGL(int w, int h);
    //virtual void paintGL();
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
private:
    QSvgRenderer m_svgrend;
    QPixmap m_composit;
    QPixmap m_level;
    int m_ticks;

    Player m_frode;

    void paintEvent(QPaintEvent * e);

private slots:
    void OnTimer();

};

#endif // GAMEWIDGET_H
