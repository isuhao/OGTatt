#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QGLWidget>
#include <QSvgRenderer>

namespace Ui {
class GameWidget;
}

class GameWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QGLWidget *parent = 0);

private:
    QSvgRenderer m_svgrend;
    QPixmap m_composit;
    QPixmap m_level;
    int m_ticks;

    int m_player_x;
    int m_player_y;
    int m_player_size;

    void paintEvent(QPaintEvent * e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
private slots:
    void OnTimer();

};

#endif // GAMEWIDGET_H
