#include <qt5/QtWidgets/QApplication>
#include <qt5/QtWidgets/QMainWindow>
#include "utils.h"

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

private:
    osgGA::EventQueue *getEventQueue() const
    {
        osgGA::EventQueue *eventQueue = m_window->getEventQueue();
        return eventQueue;
    }

public:
    QtOSGWidget(QWidget *parent = 0)
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
        createAxis(m_base_tf, 5, 0.05, 1);
        createWireFrameBox(m_base_tf, osg::Vec3(10, 10, 10), osg::Vec3(4, 6, 8), red, 1.0);

        m_camera->setViewport(0, 0, this->width(), this->height());
        m_camera->setClearColor(osg::Vec4(0.39f, 0.58f, 0.93f, 1.f));
        float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
        m_camera->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);
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
        this->getEventQueue()->windowResize(
                this->x() * m_scale, this->y() * m_scale, width * m_scale, height * m_scale);
        m_window->resized(
                this->x() * m_scale, this->y() * m_scale, width * m_scale, height * m_scale);
        osg::Camera *camera = m_viewer->getCamera();
        camera->setViewport(0, 0, this->width() * m_scale, this->height() * m_scale);
        createAxis(m_base_tf, 5, 0.05, 1);
    }

    virtual void initializeGL()
    {
        osg::Material *material = new osg::Material;
        material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
        m_global_state_set->setAttributeAndModes(material, osg::StateAttribute::ON);
        m_global_state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

        createAxis(m_base_tf, 5, 0.05, 1);
    }

    virtual void mouseMoveEvent(QMouseEvent *event)
    {
        this->getEventQueue()->mouseMotion(event->x() * m_scale, event->y() * m_scale);
    }

    virtual void mousePressEvent(QMouseEvent *event)
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
        this->getEventQueue()->mouseButtonPress(event->x() * m_scale, event->y() * m_scale, button);
    }

    virtual void mouseReleaseEvent(QMouseEvent *event)
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

    virtual void wheelEvent(QWheelEvent *event)
    {
        int delta = event->delta();
        osgGA::GUIEventAdapter::ScrollingMotion motion =
                delta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
        this->getEventQueue()->mouseScroll(motion);
    }

    virtual bool event(QEvent *event)
    {
        bool handled = QOpenGLWidget::event(event);
        this->update();
        return handled;
    }

public:
    osg::Geode *createArrowGeode(
            double height, double radius, const osg::Vec4 &color, osg::TessellationHints *hints)
    {
        // create axis shape
        osg::Geode *zg = new osg::Geode;
        osg::ShapeDrawable *z_shape = new osg::ShapeDrawable(
                new osg::Cylinder(osg::Vec3(0.0f, 0.0f, height / 2), radius, height), hints);

        z_shape->setColor(color);

        // create cone shape
        osg::ShapeDrawable *z_cone = new osg::ShapeDrawable(
                new osg::Cone(osg::Vec3(0.0f, 0.0f, height), 4 * radius, 8 * radius), hints);

        z_cone->setColor(color);
        zg->addDrawable(z_shape);
        zg->addDrawable(z_cone);

        return zg;
    }

    void setMaterialColor(osg::StateSet *state, const osg::Vec4 &color)
    {
        state->setMode(GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

        state->setAttribute(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
                osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

        osg::Material *material = new osg::Material;
        material->setAmbient(osg::Material::FRONT_AND_BACK, color);
        material->setDiffuse(osg::Material::FRONT_AND_BACK, color);

        state->setAttribute(material, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

        osg::LightModel *lm = new osg::LightModel();
        lm->setTwoSided(true);

        state->setAttribute(lm, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    }

    void createSphere(const osg::Vec3 &center, double radius, const osg::Vec4 &color)
    {
        // create s shape
        osg::Geode *sg = new osg::Geode;
        osg::ShapeDrawable *s_shape =
                new osg::ShapeDrawable(new osg::Sphere(center, radius), m_global_hints);

        osg::StateSet *state = sg->getOrCreateStateSet();
        // no lighting, get rid of normal setting
        // state->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
        setMaterialColor(state, color);

        s_shape->setColor(color);

        sg->addDrawable(s_shape);
        m_root->addChild(sg);
    }

    void createSphere(
            osg::Transform *tf, const osg::Vec3 &center, double radius, const osg::Vec4 &color)
    {
        // create s shape
        osg::Geode *sg = new osg::Geode;
        tf->addChild(sg);
        osg::ShapeDrawable *s_shape =
                new osg::ShapeDrawable(new osg::Sphere(center, radius), m_global_hints);

        osg::StateSet *state = sg->getOrCreateStateSet();
        // no lighting, get rid of normal setting
        setMaterialColor(state, color);

        sg->addDrawable(s_shape);
        m_root->addChild(tf);
    }

    void createLineStrip(
            osg::Transform *tf, osg::Vec3Array *vertices, const osg::Vec4 &color, double line_width)
    {
        osg::Geode *geode = new osg::Geode();
        osg::Geometry *linesGeom = new osg::Geometry();
        osg::LineWidth *linewidth = new osg::LineWidth();

        linewidth->setWidth(line_width);
        linesGeom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
        linesGeom->setVertexArray(vertices);

        osg::StateSet *state = linesGeom->getOrCreateStateSet();
        // no lighting, get rid of normal setting
        state->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
        setMaterialColor(state, color);

        // This time we simply use primitive, and hardwire the number of coords
        // to use since we know up front,
        linesGeom->addPrimitiveSet(
                new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, vertices->size()));

        geode->addDrawable(linesGeom);

        tf->addChild(geode);
        m_root->addChild(tf);
    }

    void createLineList(
            osg::Transform *tf, osg::Vec3Array *vertices, const osg::Vec4 &color, double line_width)
    {
        osg::Geode *geode = new osg::Geode();
        osg::Geometry *linesGeom = new osg::Geometry();
        osg::LineWidth *linewidth = new osg::LineWidth();

        linewidth->setWidth(line_width);
        linesGeom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
        linesGeom->setVertexArray(vertices);

        osg::StateSet *state = linesGeom->getOrCreateStateSet();
        // no lighting, get rid of normal setting
        state->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
        setMaterialColor(state, color);

        // This time we simply use primitive, and hardwire the number of coords
        // to use since we know up front,
        linesGeom->addPrimitiveSet(
                new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, vertices->size()));

        geode->addDrawable(linesGeom);

        tf->addChild(geode);
        m_root->addChild(tf);
    }

    void createLine(osg::Transform *tf, const osg::Vec3 &point_a, const osg::Vec3 &point_b,
            const osg::Vec4 &color, double line_width)
    {
        osg::Geode *geode = new osg::Geode();
        osg::Geometry *linesGeom = new osg::Geometry();
        osg::LineWidth *linewidth = new osg::LineWidth();

        osg::Vec3Array *vertices = new osg::Vec3Array(2);
        vertices->at(0) = point_a;
        vertices->at(1) = point_b;

        linewidth->setWidth(line_width);
        linesGeom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
        linesGeom->setVertexArray(vertices);

        osg::StateSet *state = linesGeom->getOrCreateStateSet();
        // no lighting, get rid of normal setting
        state->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
        setMaterialColor(state, color);

        // This time we simply use primitive, and hardwire the number of coords
        // to use since we know up front,
        linesGeom->addPrimitiveSet(
                new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, vertices->size()));

        geode->addDrawable(linesGeom);

        tf->addChild(geode);
        m_root->addChild(tf);
    }

    void createText(osg::Transform *tf, const osg::Vec3 &pos, double size, std::string &label,
            const osg::Vec4 &color)
    {
        osg::Geode *geode = new osg::Geode();
        osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;

        osg::StateSet *state = geode->getOrCreateStateSet();

        state->setAttribute(new osg::PolygonOffset(1.0f, 1.0f), osg::StateAttribute::ON);
        // state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

        {
            osgText::Text *text = new osgText::Text;
            geode->addDrawable(text);

            // text->setFont(timesFont);
            text->setPosition(pos);
            text->setFontResolution(40, 40);
            text->setColor(color);
            text->setCharacterSize(size);
            text->setAlignment(osgText::Text::RIGHT_BOTTOM);
            text->setAutoRotateToScreen(true);
            text->setCharacterSizeMode(
                    osgText::Text::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);
            text->setText(label);
        }

        tf->addChild(geode);
        m_root->addChild(tf);
    }

    void createLineLoop(
            osg::Transform *tf, osg::Vec3Array *vertices, const osg::Vec4 &color, double line_width)
    {
        osg::Geode *geode = new osg::Geode();
        osg::Geometry *linesGeom = new osg::Geometry();
        osg::LineWidth *linewidth = new osg::LineWidth();

        linewidth->setWidth(line_width);
        linesGeom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
        linesGeom->setVertexArray(vertices);

        osg::StateSet *state = linesGeom->getOrCreateStateSet();
        // no lighting, get rid of normal setting
        state->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
        setMaterialColor(state, color);

        // This time we simply use primitive, and hardwire the number of coords
        // to use since we know up front,
        linesGeom->addPrimitiveSet(
                new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));

        geode->addDrawable(linesGeom);

        tf->addChild(geode);
        m_root->addChild(tf);
    }

    void createRectangle(osg::Transform *tf, const osg::Vec3 &upper, const osg::Vec3 &lower,
            const osg::Vec4 &color, double line_width)
    {
        osg::Geode *geode = new osg::Geode();
        osg::Geometry *linesGeom = new osg::Geometry();
        osg::LineWidth *linewidth = new osg::LineWidth();

        osg::Vec3Array *osg_vertices = new osg::Vec3Array(4);

        osg_vertices->at(0)[0] = lower.x();
        osg_vertices->at(0)[1] = lower.y();
        osg_vertices->at(0)[2] = 0;

        osg_vertices->at(1)[0] = upper.x();
        osg_vertices->at(1)[1] = lower.y();
        osg_vertices->at(1)[2] = 0;

        osg_vertices->at(2)[0] = upper.x();
        osg_vertices->at(2)[1] = upper.y();
        osg_vertices->at(2)[2] = 0;

        osg_vertices->at(3)[0] = lower.x();
        osg_vertices->at(3)[1] = upper.y();
        osg_vertices->at(3)[2] = 0;

        linewidth->setWidth(line_width);
        linesGeom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
        linesGeom->setVertexArray(osg_vertices);

        osg::StateSet *state = linesGeom->getOrCreateStateSet();
        // no lighting, get rid of normal setting
        state->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
        setMaterialColor(state, color);

        // This time we simply use primitive, and hardwire the number of coords
        // to use since we know up front,
        linesGeom->addPrimitiveSet(
                new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, osg_vertices->size()));

        geode->addDrawable(linesGeom);

        tf->addChild(geode);
        m_root->addChild(tf);
    }

    void createFacesFromVertices(osg::Transform *tf, osg::Vec3Array *vertices,
            osg::Vec3iArray *triangles, const osg::Vec4 &color, double line_widht)
    {
        osg::Geode *geode = new osg::Geode();
        osg::Geometry *polyGeom = new osg::Geometry();
        std::size_t points_num = triangles->size() * 3;
        osg::Vec3Array *osg_vertices = new osg::Vec3Array(triangles->size() * 3);
        for (std::size_t i = 0; i < triangles->size(); i++)
        {
            osg_vertices->at(i * 3 + 0).set(vertices->at(triangles->at(i)[0]));
            osg_vertices->at(i * 3 + 1).set(vertices->at(triangles->at(i)[1]));
            osg_vertices->at(i * 3 + 2).set(vertices->at(triangles->at(i)[2]));
        }
        polyGeom->setVertexArray(osg_vertices);
        polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, points_num));
        osg::StateSet *state = polyGeom->getOrCreateStateSet();
        // no lighting, get rid of normal setting
        state->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
        setMaterialColor(state, color);
        geode->addDrawable(polyGeom);

        tf->addChild(geode);
        m_root->addChild(tf);
    }

    void createMeshFromVertices(
            osg::Transform *tf, osg::Vec3Array *vertices, const osg::Vec4 &color, double line_width)
    {
        if (vertices->size() % 4 == 0)
        {
            std::size_t group = vertices->size() / 4;
            // std::size_t index;
            osg::Vec3Array *line_a = new osg::Vec3Array(group);
            osg::Vec3Array *line_b = new osg::Vec3Array(group);
            osg::Vec3Array *line_c = new osg::Vec3Array(group);
            osg::Vec3Array *line_d = new osg::Vec3Array(group);
            std::string text;
            for (std::size_t i = 0; i < group; i++)
            {
                osg::Vec3Array *marker_vertices = new osg::Vec3Array(4);
                // std::cout << "group :" << i << std::endl;
                // 1
                marker_vertices->at(0) = vertices->at(i * 4);
                text = std::to_string(i * 4);
                // createText(tf, marker_vertices->at(0), 10, text, color);
                line_a->at(i) = marker_vertices->at(0);

                // 2
                marker_vertices->at(1) = vertices->at(i * 4 + 1);
                text = std::to_string(i * 4 + 1);
                // createText(tf, marker_vertices->at(1), 10, text, color);
                line_b->at(i) = marker_vertices->at(1);

                // 3
                marker_vertices->at(2) = vertices->at(i * 4 + 2);
                text = std::to_string(i * 4 + 2);
                // createText(tf, marker_vertices->at(2), 10, text, color);
                line_c->at(i) = marker_vertices->at(2);

                // 4
                marker_vertices->at(3) = vertices->at(i * 4 + 3);
                text = std::to_string(i * 4 + 3);
                // createText(tf, marker_vertices->at(3), 10, text, color);
                line_d->at(i) = marker_vertices->at(3);

                createLineLoop(tf, marker_vertices, color, line_width);
            }

            createLineStrip(tf, line_a, color, line_width);
            createLineStrip(tf, line_b, color, line_width);
            createLineStrip(tf, line_c, color, line_width);
            createLineStrip(tf, line_d, color, line_width);
        }
        else
        {
            std::cout << "Vertices number is not right." << std::endl;
        }
    }

    void createMeshFromPath(
            osg::Transform *tf, osg::Vec3Array *path, const osg::Vec4 &color, double line_width)
    {
        // get the path length
        double width = 1.0;
        if (path->size() >= 3)
        {
            double theta_s, theta_e;
            double delta_y, delta_x;
            std::vector<double> headings, left_heading;
            for (std::size_t i = 0; i < path->size() - 1; i++)
            {
                delta_x = path->at(i + 1)[0] - path->at(i)[0];
                delta_y = path->at(i + 1)[1] - path->at(i)[1];
                theta_s = atan2f(delta_y, delta_x);
                theta_e = theta_s;
                headings.push_back(theta_s);
            }
            headings.push_back(theta_e);

            // then use the heading vector to draw the points
            osg::Vec3Array *left_path = new osg::Vec3Array(path->size());
            osg::Vec3Array *right_path = new osg::Vec3Array(path->size());
            double heading, divider, actual_width;
            for (std::size_t i = 0; i < path->size(); i++)
            {
                if (0 == i || i == (path->size() - 1))
                {
                    double heading = headings[i] + osg::PI / 2.0;
                    left_path->at(i)[0] = cos(heading) * width / 2.0 + path->at(i)[0];
                    left_path->at(i)[1] = sin(heading) * width / 2.0 + path->at(i)[1];
                    left_path->at(i)[2] = path->at(i)[2];
                    heading = headings[i] - osg::PI / 2.0;
                    right_path->at(i)[0] = cos(heading) * width / 2.0 + path->at(i)[0];
                    right_path->at(i)[1] = sin(heading) * width / 2.0 + path->at(i)[1];
                    right_path->at(i)[2] = path->at(i)[2];
                }
                else
                {
                    heading = (headings[i - 1] + headings[i]) / 2.0 + osg::PI / 2.0;
                    divider = sin(heading - headings[i]);
                    if (divider == 0)
                    {
                        actual_width = width / 2.0;
                    }
                    else
                    {
                        actual_width = width / 2.0 / divider;
                    }
                    left_path->at(i)[0] = cos(heading) * actual_width + path->at(i)[0];
                    left_path->at(i)[1] = sin(heading) * actual_width + path->at(i)[1];
                    left_path->at(i)[2] = path->at(i)[2];
                    // the
                    heading = (headings[i - 1] + headings[i]) / 2.0 - osg::PI / 2.0;
                    divider = sin(headings[i] - heading);
                    if (divider == 0)
                    {
                        actual_width = width / 2.0;
                    }
                    else
                    {
                        actual_width = width / 2.0 / divider;
                    }
                    right_path->at(i)[0] = cos(heading) * actual_width + path->at(i)[0];
                    right_path->at(i)[1] = sin(heading) * actual_width + path->at(i)[1];
                    right_path->at(i)[2] = path->at(i)[2];
                }
                createSphere(tf, left_path->at(i), 0.1, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
                createSphere(tf, right_path->at(i), 0.1, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
            }

            createLineList(tf, path, color, line_width);
            createLineList(tf, left_path, color, line_width);
            createLineList(tf, right_path, color, line_width);
        }
    }

    void createWireFrameBox(osg::Transform *tf, const osg::Vec3 &center, const osg::Vec3 &sides,
            const osg::Vec4 &color, double line_width)
    {
        osg::Vec3Array *box_lines = new osg::Vec3Array(8);

        box_lines->at(0)[2] = center[2] - sides[2] / 2.0;
        box_lines->at(1)[2] = box_lines->at(0)[2];
        box_lines->at(2)[2] = box_lines->at(0)[2];
        box_lines->at(3)[2] = box_lines->at(0)[2];

        box_lines->at(4)[2] = center[2] + sides[2] / 2.0;
        box_lines->at(5)[2] = box_lines->at(4)[2];
        box_lines->at(6)[2] = box_lines->at(4)[2];
        box_lines->at(7)[2] = box_lines->at(4)[2];

        box_lines->at(0)[0] = center[0] + sides[0] / 2.0;
        box_lines->at(0)[1] = center[1] + sides[1] / 2.0;

        box_lines->at(1)[0] = center[0] - sides[0] / 2.0;
        box_lines->at(1)[1] = center[1] + sides[1] / 2.0;

        box_lines->at(2)[0] = center[0] - sides[0] / 2.0;
        box_lines->at(2)[1] = center[1] - sides[1] / 2.0;

        box_lines->at(3)[0] = center[0] + sides[0] / 2.0;
        box_lines->at(3)[1] = center[1] - sides[1] / 2.0;

        box_lines->at(4)[0] = center[0] + sides[0] / 2.0;
        box_lines->at(4)[1] = center[1] - sides[1] / 2.0;

        box_lines->at(5)[0] = center[0] - sides[0] / 2.0;
        box_lines->at(5)[1] = center[1] - sides[1] / 2.0;

        box_lines->at(6)[0] = center[0] - sides[0] / 2.0;
        box_lines->at(6)[1] = center[1] + sides[1] / 2.0;

        box_lines->at(7)[0] = center[0] + sides[0] / 2.0;
        box_lines->at(7)[1] = center[1] + sides[1] / 2.0;

        createLineLoop(tf, box_lines, color, line_width);
        createLine(tf, box_lines->at(1), box_lines->at(6), color, line_width);
        createLine(tf, box_lines->at(2), box_lines->at(5), color, line_width);
        createLine(tf, box_lines->at(0), box_lines->at(3), color, line_width);
        createLine(tf, box_lines->at(4), box_lines->at(7), color, line_width);
    }

    void createPolygon(
            osg::Transform *tf, osg::Vec3Array *vertices, const osg::Vec4 &color, double line_width)
    {
        osg::Geode *geode = new osg::Geode();
        osg::Geometry *polyGeom = new osg::Geometry();

        osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
        shared_normals->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));

        osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
        shared_colors->push_back(color);

        polyGeom->setVertexArray(vertices);

        polyGeom->setColorArray(shared_colors.get(), osg::Array::BIND_OVERALL);

        polyGeom->setNormalArray(shared_normals.get(), osg::Array::BIND_OVERALL);

        polyGeom->addPrimitiveSet(
                new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, vertices->size()));

        // osg::StateSet *state = linesGeom->getOrCreateStateSet();
        // no lighting, get rid of normal setting
        // state->setMode(GL_LIGHTING,
        // osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
        // setMaterialColor(state, color);

        // This time we simply use primitive, and hardwire the number of coords
        // to use since we know up front,
        // linesGeom->addPrimitiveSet(new
        // osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));

        geode->addDrawable(polyGeom);

        tf->addChild(geode);
        m_root->addChild(tf);
    }

    void createWireFrameCube(osg::Transform *tf_root, osg::Transform *tf, const osg::Vec3 &center,
            const osg::Vec3 &sides, const osg::Vec4 &color)
    {
        m_root->addChild(tf_root);
        osg::Geode *geo = new osg::Geode;
        osg::StateSet *state = geo->getOrCreateStateSet();
        osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;

        polyoffset->setFactor(-1.0f);
        polyoffset->setUnits(-1.0f);
        osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
        polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
        state->setAttributeAndModes(
                polyoffset, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
        state->setAttributeAndModes(
                polymode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
        osg::ref_ptr<osg::Material> material = new osg::Material;
        state->setAttributeAndModes(
                material, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
        state->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

        osg::ref_ptr<osg::ShapeDrawable> cube = new osg::ShapeDrawable;
        cube->setShape(new osg::Box(center, sides[0], sides[1], sides[2]));
        cube->setColor(color);
        geo->addChild(cube);
        tf->addChild(geo);
        tf_root->addChild(tf);
    }

    void createWireFrameCube(osg::Transform *tf, const osg::Vec3 &center, const osg::Vec3 &sides,
            const osg::Vec4 &color)
    {
        osg::Geode *geo = new osg::Geode;
        osg::StateSet *state = geo->getOrCreateStateSet();
        osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;

        polyoffset->setFactor(-1.0f);
        polyoffset->setUnits(-1.0f);
        osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
        polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
        state->setAttributeAndModes(
                polyoffset, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
        state->setAttributeAndModes(
                polymode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
        osg::ref_ptr<osg::Material> material = new osg::Material;
        state->setAttributeAndModes(
                material, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
        state->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

        osg::ref_ptr<osg::ShapeDrawable> cube = new osg::ShapeDrawable;
        cube->setShape(new osg::Box(center, sides[0], sides[1], sides[2]));
        cube->setColor(color);
        geo->addChild(cube);
        tf->addChild(geo);
        m_root->addChild(tf);
    }

    void createCube(osg::Transform *tf, const osg::Vec3 &center, const osg::Vec3 &sides,
            const osg::Vec4 &color)
    {
        osg::Geode *geo = new osg::Geode;
        osg::ref_ptr<osg::ShapeDrawable> cube = new osg::ShapeDrawable;
        cube->setShape(new osg::Box(center, sides[0], sides[1], sides[2]));
        cube->setColor(color);
        geo->addChild(cube);
        tf->addChild(geo);
        m_root->addChild(tf);
    }

    void createAxis(osg::Transform *previousJoint, double length, double radius, double transparent)
    {
        osg::TessellationHints *hints = new osg::TessellationHints;
        hints->setDetailRatio(0.2f);

        osg::MatrixTransform *zmt = new osg::MatrixTransform();
        osg::StateSet *state;

        osg::Vec4 blue(0.0f, 0.0f, 1.0f, transparent);
        osg::Vec4 red(1.0f, 0.0f, 0.0f, transparent);
        osg::Vec4 green(0.0f, 1.0f, 0.0f, transparent);
        // create an axis in the origin
        osg::Geode *zg = createArrowGeode(length, radius, blue, hints);
        std::string text = "Z";
        createText(previousJoint, osg::Vec3(0, 0, length), 10, text, blue);
        zmt->addChild(zg);
        previousJoint->addChild(zmt);
        state = zmt->getOrCreateStateSet();
        setMaterialColor(state, blue);

        // y direction
        osg::MatrixTransform *yMt = new osg::MatrixTransform();
        previousJoint->addChild(yMt);
        osg::Matrix yMatrix = osg::Matrix::rotate(-osg::PI_2, 1.0, 0.0, 0.0);
        yMt->setMatrix(yMatrix);
        text = "Y";
        createText(previousJoint, osg::Vec3(0, length, 0), 10, text, red);

        osg::Geode *yg = createArrowGeode(length, radius, red, hints);
        yMt->addChild(yg);
        state = yMt->getOrCreateStateSet();
        setMaterialColor(state, red);

        // x direction
        osg::Matrix xMatrix = osg::Matrix::rotate(osg::PI_2, 0.0, 1.0, 0.0);
        osg::MatrixTransform *xMt = new osg::MatrixTransform();
        xMt->setMatrix(xMatrix);
        previousJoint->addChild(xMt);
        osg::Geode *xg = createArrowGeode(length, radius, green, hints);
        xMt->addChild(xg);
        state = xMt->getOrCreateStateSet();
        setMaterialColor(state, green);
        text = "X";
        createText(previousJoint, osg::Vec3(length, 0, 0), 10, text, green);
    }

    void createChildAxis(osg::Transform *tf, double length, double radius, double transparent)
    {
        // create the origin and axis of the global frame with length 5m for
        // each axis
        createAxis(tf, length, radius, transparent);
        m_root->addChild(tf);
    }

    void setDisplayData() { m_viewer->setSceneData(m_root.get()); }
};

int main(int argc, char **argv)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
#else
    qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("1"));
#endif

    QApplication qapp(argc, argv);
    QMainWindow window;
    window.setMinimumSize(900, 600);
    QtOSGWidget *widget = new QtOSGWidget(&window);
    window.setCentralWidget(widget);
    window.show();

    return qapp.exec();
}
