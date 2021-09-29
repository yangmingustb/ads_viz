#ifndef MAINWINDOW2d_H
#define MAINWINDOW2d_H

#include <qt5/QtGui/qopengl.h>
#include <QOpenGLFunctions>
#include <QOpenGLWindow>
#include <QSurfaceFormat>

class MainWindow2d : public QOpenGLWindow
{
    Q_OBJECT
public:
    explicit MainWindow2d(QWidget *parent = 0);
    ~MainWindow2d();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QOpenGLContext *context;
    QOpenGLFunctions *openGLFunctions;
};
#endif  // MAINWINDOW2d_H
