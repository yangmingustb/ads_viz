#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
{
    // We must call setSurfaceType(QWindow::OpenGLSurface) to tell Qt we prefer
    // to use OpenGL to render the images to screen, instead of QPainter.
    setSurfaceType(QWindow::OpenGLSurface);

    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setVersion(2, 1);  // OpenGL 2.1
    setFormat(format);

    context = new QOpenGLContext;
    context->setFormat(format);
    context->create();
    context->makeCurrent(this);
    openGLFunctions = context->functions();
}

MainWindow::~MainWindow() {}

void MainWindow::initializeGL() { resizeGL(this->width(), this->height()); }

void MainWindow::resizeGL(int w, int h)
{
    // https://blog.csdn.net/jiangdf/article/details/8460012
    // Initialize Projection Matrix
    glMatrixMode(GL_PROJECTION);  // 矩阵投影模式，透视3D模式
    glLoadIdentity();             // 矩阵单位化

    glViewport(0, 0, w, h);  // 视野设置，图形显示在哪个区域
    qreal aspectRatio = qreal(w) / qreal(h);
    glOrtho(-1 * aspectRatio, 1 * aspectRatio, -1, 1, 1, -1);
}

void MainWindow::paintGL()
{
    // The geometric primitive types supported by OpenGL are points, lines,
    // linestrips, line loops, polygons, quads, quad strips, triangles, triangle
    // strips, and triangle fans.

    // Initialize clear color (cornflower blue)
    // glClearColor(40/256.0f, 44/256.0f, 52/256.0f, 1.f);
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);  // 设置背景色
    // Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT);  // 根据设置的背景色来刷新界面颜色

    glBegin(GL_QUADS);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(-0.5f, 0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(1.f, 0.f, 0.f);
    glVertex2f(-0.8f, -0.8f);

    glColor3f(1.f, 1.f, 0.f);
    glVertex2f(-0.5, -0.8f);

    glColor3f(0.f, 1.f, 0.f);
    glVertex2f(-0.5f, -0.5f);

    glColor3f(0.f, 0.f, 1.f);
    glVertex2f(-0.8f, -0.5f);
    glEnd();

    glFlush(); // 强制刷新缓存
}

void MainWindow::paintEvent(QPaintEvent *event) { paintGL(); }

void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeGL(this->width(), this->height());
    this->update();
}
