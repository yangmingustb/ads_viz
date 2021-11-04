#pragma once
#include <osg_utils.h>
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

#include <osg/Camera>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/ref_ptr>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>

namespace cat
{
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
    osg::StateSet *m_global_state_set;
    osg::TessellationHints *m_global_hints;
    osg::ref_ptr<osg::MatrixTransform> m_base_tf;
    osg::ref_ptr<osgGA::TrackballManipulator> m_manipulator;

public:
    QtOSGWidget(QWidget *parent = 0);
    virtual ~QtOSGWidget();

protected:
    virtual void paintGL() { m_viewer->frame(); }

    virtual void resizeGL(int width, int height);

    virtual void initializeGL();

    virtual void mouseMoveEvent(QMouseEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);

    virtual void mouseReleaseEvent(QMouseEvent *event);

    virtual void wheelEvent(QWheelEvent *event);

    virtual bool event(QEvent *event);

public:
    osg::Geode *createArrowGeode(
            double height, double radius, const osg::Vec4 &color, osg::TessellationHints *hints);

    void setMaterialColor(osg::StateSet *state, const osg::Vec4 &color);

    void createSphere(const osg::Vec3 &center, double radius, const osg::Vec4 &color);

    void createSphere(
            osg::Transform *tf, const osg::Vec3 &center, double radius, const osg::Vec4 &color);

    void createLineStrip(osg::Transform *tf, osg::Vec3Array *vertices, const osg::Vec4 &color,
            double line_width);

    void createLineList(osg::Transform *tf, osg::Vec3Array *vertices, const osg::Vec4 &color,
            double line_width);

    void createLine(osg::Transform *tf, const osg::Vec3 &point_a, const osg::Vec3 &point_b,
            const osg::Vec4 &color, double line_width);

    void createText(osg::Transform *tf, const osg::Vec3 &pos, double size, std::string &label,
            const osg::Vec4 &color);

    void createLineLoop(osg::Transform *tf, osg::Vec3Array *vertices, const osg::Vec4 &color,
            double line_width);

    void createRectangle(osg::Transform *tf, const osg::Vec3 &upper, const osg::Vec3 &lower,
            const osg::Vec4 &color, double line_width);

    void createFacesFromVertices(osg::Transform *tf, osg::Vec3Array *vertices,
            osg::Vec3iArray *triangles, const osg::Vec4 &color, double line_widht);

    void createMeshFromVertices(osg::Transform *tf, osg::Vec3Array *vertices,
            const osg::Vec4 &color, double line_width);

    void createMeshFromPath(
            osg::Transform *tf, osg::Vec3Array *path, const osg::Vec4 &color, double line_width);

    void createWireFrameBox(osg::Transform *tf, const osg::Vec3 &center, const osg::Vec3 &sides,
            const osg::Vec4 &color, double line_width);

    void createPolygon(osg::Transform *tf, osg::Vec3Array *vertices, const osg::Vec4 &color,
            double line_width);

    void createWireFrameCube(osg::Transform *tf_root, osg::Transform *tf, const osg::Vec3 &center,
            const osg::Vec3 &sides, const osg::Vec4 &color);

    void createWireFrameCube(osg::Transform *tf, const osg::Vec3 &center, const osg::Vec3 &sides,
            const osg::Vec4 &color);

    void createCube(osg::Transform *tf, const osg::Vec3 &center, const osg::Vec3 &sides,
            const osg::Vec4 &color);

    void createAxis(
            osg::Transform *previousJoint, double length, double radius, double transparent);

    void createChildAxis(osg::Transform *tf, double length, double radius, double transparent);

    void setDisplayData() { m_viewer->setSceneData(m_root.get()); }

private:
    osgGA::EventQueue *getEventQueue() const;
};

}  // namespace cat