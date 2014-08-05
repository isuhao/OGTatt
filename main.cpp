#include "gamewidget.h"
#include <QApplication>
#include <QGLFormat>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGLFormat glFormat;
    glFormat.setVersion(4,3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);

    GameWidget w(glFormat);
    w.setWindowState(Qt::WindowFullScreen);
    w.show();

    return a.exec();
}
