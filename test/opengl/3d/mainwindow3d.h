#pragma once
#include <QSurfaceFormat>
#include <QOpenGLFunctions>
#include <GL/glu.h>
#include <QOpenGLWindow>
#include <QTimer>
#include <qt5/QtGui/qopengl.h>
#include <qt5/QtGui/QGuiApplication>
#include <qt5/QtWidgets/QWidget>
#include <qt5/QtGui/QOpenGLWindow>

class MainWindow3d : public QOpenGLWindow
{
  Q_OBJECT
  public:
    explicit MainWindow3d(QWidget *parent = 0);
    ~MainWindow3d();

public slots:
  void updateAnimation();

  protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
  QOpenGLContext* context;
  QOpenGLFunctions* openGLFunctions;
  float rotation;
};


