/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description: Minimalistic project example that uses both Qt and
 * OpenSceneGraph libraries.
 *
 *        Version:  1.0
 *        Created:  30-Jun-16 10:23:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Victoria Rudakova (vicrucann@gmail.com),
 *   Organization:  vicrucann.github.io
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <osg/Geode>
#include <osg/LineStipple>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgUtil/ShaderGen>

#include <osg/BlendFunc>
#include <osg/LightModel>

#include <osg/GraphicsContext>
#include <osgDB/ReadFile>
#include <osgGA/StateSetManipulator>
#include <osgText/Font>
#include <osgText/Text>
#include <osgViewer/Viewer>

#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QScreen>
#include <QWheelEvent>
#include <QWindow>
#include <QtGlobal>

#include <viewer.h>
#include <osg/Camera>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/ref_ptr>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>

class QtOSGWidget : public QOpenGLWidget
{
private:
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> m_window;
    osg::ref_ptr<osgViewer::Viewer> m_viewer;
    qreal m_scale;
    osg::ref_ptr<osg::Camera> m_camera;

private:
    osgGA::EventQueue* getEventQueue() const
    {
        osgGA::EventQueue* eventQueue = m_window->getEventQueue();
        return eventQueue;
    }

public:
    QtOSGWidget(QWidget* parent = 0)
        : QOpenGLWidget(parent),
          m_window(new osgViewer::GraphicsWindowEmbedded(
                  this->x(), this->y(), this->width(), this->height())),
          m_viewer(new osgViewer::Viewer),
          m_scale(QApplication::desktop()->devicePixelRatio())
    {
        m_camera = new osg::Camera;

        // this->setMinimumSize(1000, 800);  // 主窗口初始化

        osg::Cylinder* cylinder =
                new osg::Cylinder(osg::Vec3(0.f, 0.f, 0.f), 0.25f, 0.5f);
        osg::ShapeDrawable* sd = new osg::ShapeDrawable(cylinder);
        sd->setColor(osg::Vec4(0.8f, 0.5f, 0.2f, 1.f));
        osg::Geode* geode = new osg::Geode;
        geode->addDrawable(sd);

        m_camera->setViewport(0, 0, this->width(), this->height());
        m_camera->setClearColor(osg::Vec4(0.9f, 0.9f, 1.f, 1.f));
        float aspectRatio = static_cast<float>(this->width()) /
                            static_cast<float>(this->height());
        m_camera->setProjectionMatrixAsPerspective(
                30.f, aspectRatio, 1.f, 1000.f);
        m_camera->setGraphicsContext(m_window);

        m_viewer->setCamera(m_camera);
        m_viewer->setSceneData(geode);
        osgGA::TrackballManipulator* manipulator =
                new osgGA::TrackballManipulator;
        manipulator->setAllowThrow(false);
        this->setMouseTracking(true);
        m_viewer->setCameraManipulator(manipulator);
        m_viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
        m_viewer->realize();
    }

    virtual ~QtOSGWidget() {}

protected:
    virtual void paintGL() { m_viewer->frame(); }

    virtual void resizeGL(int width, int height)
    {
        this->getEventQueue()->windowResize(this->x() * m_scale,
                this->y() * m_scale, width * m_scale, height * m_scale);
        m_window->resized(this->x() * m_scale, this->y() * m_scale,
                width * m_scale, height * m_scale);
        osg::Camera* camera = m_viewer->getCamera();
        camera->setViewport(
                0, 0, this->width() * m_scale, this->height() * m_scale);
    }

    virtual void initializeGL()
    {
        osg::Geode* geode = dynamic_cast<osg::Geode*>(m_viewer->getSceneData());
        osg::StateSet* stateSet = geode->getOrCreateStateSet();
        osg::Material* material = new osg::Material;
        material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
        stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
        stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    }

    virtual void mouseMoveEvent(QMouseEvent* event)
    {
        this->getEventQueue()->mouseMotion(
                event->x() * m_scale, event->y() * m_scale);
    }

    virtual void mousePressEvent(QMouseEvent* event)
    {
        unsigned int button = 0;
        switch (event->button())
        {
            case Qt::LeftButton:
                button = 1;
                break;
            case Qt::MiddleButton:
                button = 2;
                break;
            case Qt::RightButton:
                button = 3;
                break;
            default:
                break;
        }
        this->getEventQueue()->mouseButtonPress(
                event->x() * m_scale, event->y() * m_scale, button);
    }

    virtual void mouseReleaseEvent(QMouseEvent* event)
    {
        unsigned int button = 0;
        switch (event->button())
        {
            case Qt::LeftButton:
                button = 1;
                break;
            case Qt::MiddleButton:
                button = 2;
                break;
            case Qt::RightButton:
                button = 3;
                break;
            default:
                break;
        }
        this->getEventQueue()->mouseButtonRelease(
                event->x() * m_scale, event->y() * m_scale, button);
    }

    virtual void wheelEvent(QWheelEvent* event)
    {
        int delta = event->delta();
        osgGA::GUIEventAdapter::ScrollingMotion motion =
                delta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP
                          : osgGA::GUIEventAdapter::SCROLL_DOWN;
        this->getEventQueue()->mouseScroll(motion);
    }

    virtual bool event(QEvent* event)
    {
        bool handled = QOpenGLWidget::event(event);
        this->update();
        return handled;
    }
};

int main(int argc, char** argv)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
#else
    qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("1"));
#endif

    QApplication qapp(argc, argv);
    QMainWindow window;
	window.setMinimumSize(900, 600);
    QtOSGWidget* widget = new QtOSGWidget(&window);
    window.setCentralWidget(widget);
    window.show();

    return qapp.exec();
}