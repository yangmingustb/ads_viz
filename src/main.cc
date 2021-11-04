#include "utils.h"
#include "viewer.h"
#include <qt5/QtWidgets/QApplication>
#include <qt5/QtWidgets/QMainWindow>

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
public:
    osg::Vec4 purple{1.0f, 0.0f, 1.0f, 1.0f};
    osg::Vec4 blue{0.0f, 0.0f, 1.0f, 1.0f};
    osg::Vec4 red{1.0f, 0.0f, 0.0f, 1.0f};
    osg::Vec4 yellow{1.0f, 1.0f, 0.0f, 1.0f};
    osg::Vec4 black{0.0f, 0.0f, 0.0f, 1.0f};
    osg::Vec4 white{1.0f, 1.0f, 1.0f, 1.0f};
    osg::Vec4 grey{0.3f, 0.3f, 0.3f, 0.3f};
    osg::Vec4 green{0.0f, 1.0f, 0.0f, 1.0f};
    osg::Vec4 light_blue{0.0f, 0.0f, 1.0f, 0.3f};

private:
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> m_window;
    osg::ref_ptr<osgViewer::Viewer> m_viewer;
    qreal m_scale;
    osg::ref_ptr<osg::Camera> m_camera;
    osg::ref_ptr<osg::Group> m_root;
    // 和渲染相关的opengl状态机
    osg::StateSet* m_global_state_set;
    osg::TessellationHints* m_global_hints;
    osg::ref_ptr<osg::MatrixTransform> m_base_tf;
    osg::ref_ptr<osgGA::TrackballManipulator> m_manipulator;

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
        m_root = new osg::Group;
        m_global_state_set = new osg::StateSet();
        m_global_hints = new osg::TessellationHints;
        m_base_tf = new osg::MatrixTransform;
        m_manipulator = new osgGA::TrackballManipulator;

        osg::DisplaySettings::instance()->setNumMultiSamples(4);
        m_global_hints->setDetailRatio(0.5);

        // create the origin and axis of the global frame with length 5m for
        // each axis
        m_root->addChild(m_base_tf);

        m_camera->setViewport(0, 0, this->width(), this->height());
        m_camera->setClearColor(osg::Vec4(0.39f, 0.58f, 0.93f, 1.f));
        float aspectRatio = static_cast<float>(this->width()) /
                            static_cast<float>(this->height());
        m_camera->setProjectionMatrixAsPerspective(
                30.f, aspectRatio, 1.f, 1000.f);
        m_camera->setGraphicsContext(m_window);
        m_viewer->setCamera(m_camera);
        m_viewer->setSceneData(m_root.get());

        m_manipulator->setAllowThrow(false);
        this->setMouseTracking(true);
        m_viewer->setCameraManipulator(m_manipulator);
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
        osg::Material* material = new osg::Material;
        material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
        m_global_state_set->setAttributeAndModes(
                material, osg::StateAttribute::ON);
        m_global_state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
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
