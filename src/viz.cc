#include <viz.h>
#include "osg_camera_control.h"
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/TerrainManipulator>

namespace cat
{
osgGA::EventQueue *QtOSGWidget::getEventQueue() const
{
    osgGA::EventQueue *eventQueue = m_window->getEventQueue();
    return eventQueue;
}

QtOSGWidget::QtOSGWidget(QWidget *parent)
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

    osg::ref_ptr<osg::MatrixTransform> veh_tf;
    veh_tf = new osg::MatrixTransform;
    createGridPlane(veh_tf, osg::Vec3(0, 0, 0), green, 1.0, 100.0, 100.0);

    m_camera->setViewport(0, 0, this->width(), this->height());
    m_camera->setClearColor(osg::Vec4(0.39f, 0.58f, 0.93f, 1.f));
    float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
    m_camera->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);
    m_camera->setGraphicsContext(m_window);
    m_viewer->setCamera(m_camera);
    m_viewer->setSceneData(m_root.get());

    m_manipulator->setAllowThrow(false);
    this->setMouseTracking(true);

    //******************************** MANIPULATORS
    // set up the camera manipulators.
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator =
            new osgGA::KeySwitchMatrixManipulator;


    keyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
    keyswitchManipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator());
    keyswitchManipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator());
    keyswitchManipulator->addMatrixManipulator('4', "Terrain", new osgGA::TerrainManipulator());
    keyswitchManipulator->addMatrixManipulator('6', "Own2", new OSGCameraControls());

    m_viewer->setCameraManipulator(keyswitchManipulator.get());

    // m_viewer->setCameraManipulator(m_manipulator);
    m_viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
    m_viewer->realize();

    m_viewer->setUpViewInWindow(100, 100, 900, 900);
}

QtOSGWidget::~QtOSGWidget() {}

void QtOSGWidget::resizeGL(int width, int height)
{
    this->getEventQueue()->windowResize(
            this->x() * m_scale, this->y() * m_scale, width * m_scale, height * m_scale);
    m_window->resized(this->x() * m_scale, this->y() * m_scale, width * m_scale, height * m_scale);
    osg::Camera *camera = m_viewer->getCamera();
    camera->setViewport(0, 0, this->width() * m_scale, this->height() * m_scale);
    createAxis(m_base_tf, 5, 0.05, 1);
    // if(m_map)
    // {
    //     drawLanelet();
    // }
}

void QtOSGWidget::initializeGL()
{
    osg::Material *material = new osg::Material;
    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
    m_global_state_set->setAttributeAndModes(material, osg::StateAttribute::ON);
    m_global_state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    createAxis(m_base_tf, 5, 0.05, 1);
    if (m_map)
    {
        drawLanelet();
    }
}

void QtOSGWidget::mouseMoveEvent(QMouseEvent *event)
{
    this->getEventQueue()->mouseMotion(event->x() * m_scale, event->y() * m_scale);
}

void QtOSGWidget::mousePressEvent(QMouseEvent *event)
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

void QtOSGWidget::keyPressEvent(QKeyEvent *event)
{
    grabKeyboard();
    if (event->key() == Qt::Key::Key_Space)
    {
        this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KEY_Escape);
    }
}

void QtOSGWidget::mouseReleaseEvent(QMouseEvent *event)
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
    this->getEventQueue()->mouseButtonRelease(event->x() * m_scale, event->y() * m_scale, button);
}

void QtOSGWidget::keyReleaseEvent(QKeyEvent *event)
{
    releaseKeyboard();
    if(event->key()==Qt::Key::Key_Space)
    {
        this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KEY_Escape);
    }
}

void QtOSGWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();
    osgGA::GUIEventAdapter::ScrollingMotion motion =
            delta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
    this->getEventQueue()->mouseScroll(motion);
}

bool QtOSGWidget::event(QEvent *event)
{
    bool handled = QOpenGLWidget::event(event);
    this->update();
    return handled;
}

osg::Geode *QtOSGWidget::createArrowGeode(
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

void QtOSGWidget::setMaterialColor(osg::StateSet *state, const osg::Vec4 &color)
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

void QtOSGWidget::createSphere(const osg::Vec3 &center, double radius, const osg::Vec4 &color)
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

void QtOSGWidget::createSphere(
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

void QtOSGWidget::createLineStrip(
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

void QtOSGWidget::createLineList(
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

void QtOSGWidget::createLine(osg::Transform *tf, const osg::Vec3 &point_a, const osg::Vec3 &point_b,
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

void QtOSGWidget::createText(osg::Transform *tf, const osg::Vec3 &pos, double size,
        std::string &label, const osg::Vec4 &color)
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

void QtOSGWidget::createLineLoop(
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

void QtOSGWidget::createRectangle(osg::Transform *tf, const osg::Vec3 &upper,
        const osg::Vec3 &lower, const osg::Vec4 &color, double line_width)
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

void QtOSGWidget::createFacesFromVertices(osg::Transform *tf, osg::Vec3Array *vertices,
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

void QtOSGWidget::createMeshFromVertices(
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

void QtOSGWidget::createMeshFromPath(
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

void QtOSGWidget::createWireFrameBox(osg::Transform *tf, const osg::Vec3 &center,
        const osg::Vec3 &sides, const osg::Vec4 &color, double line_width)
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

void QtOSGWidget::createGridPlane(osg::Transform *tf, const osg::Vec3 &center,
        const osg::Vec4 &color, const double &line_width, const double &width, const double &hight)
{
    osg::Vec3 start, end;
    double half_width, half_height;

    half_width = width * 0.5;
    half_height = hight * 0.5;

    start = center;
    start.x() -= half_width;
    start.y() -= half_height;
    end=center;
    end.x() += half_width;
    end.y() += half_height;

    createRectangle(tf, end, start, color, line_width);

    double step = 5.0;

    osg::Vec3 line_start = start;
    line_start[0] += step;

    osg::Vec3 line_end = line_start;
    line_end[1] = end[1];

    while (line_start[0] < end[0])
    {
        createLine(tf, line_start, line_end, color, line_width);
        line_start[0] += step;
        line_end[0] += step;
    }

    line_start = start;
    line_start[1] += step;

    line_end = line_start;
    line_end[0] = end[0];

    while (line_start[1] < end[1])
    {
        createLine(tf, line_start, line_end, color, line_width);
        line_start[1] += step;
        line_end[1] += step;
    }
}

void QtOSGWidget::createPolygon(
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

    polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, vertices->size()));

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

void QtOSGWidget::createWireFrameCube(osg::Transform *tf_root, osg::Transform *tf,
        const osg::Vec3 &center, const osg::Vec3 &sides, const osg::Vec4 &color)
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
    state->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    osg::ref_ptr<osg::Material> material = new osg::Material;
    state->setAttributeAndModes(material, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    state->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

    osg::ref_ptr<osg::ShapeDrawable> cube = new osg::ShapeDrawable;
    cube->setShape(new osg::Box(center, sides[0], sides[1], sides[2]));
    cube->setColor(color);
    geo->addChild(cube);
    tf->addChild(geo);
    tf_root->addChild(tf);
}

void QtOSGWidget::createWireFrameCube(
        osg::Transform *tf, const osg::Vec3 &center, const osg::Vec3 &sides, const osg::Vec4 &color)
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
    state->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    osg::ref_ptr<osg::Material> material = new osg::Material;
    state->setAttributeAndModes(material, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    state->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

    osg::ref_ptr<osg::ShapeDrawable> cube = new osg::ShapeDrawable;
    cube->setShape(new osg::Box(center, sides[0], sides[1], sides[2]));
    cube->setColor(color);
    geo->addChild(cube);
    tf->addChild(geo);
    m_root->addChild(tf);
}

void QtOSGWidget::createCube(
        osg::Transform *tf, const osg::Vec3 &center, const osg::Vec3 &sides, const osg::Vec4 &color)
{
    osg::Geode *geo = new osg::Geode;
    osg::ref_ptr<osg::ShapeDrawable> cube = new osg::ShapeDrawable;
    cube->setShape(new osg::Box(center, sides[0], sides[1], sides[2]));
    cube->setColor(color);
    geo->addChild(cube);
    tf->addChild(geo);
    m_root->addChild(tf);
}

void QtOSGWidget::createAxis(
        osg::Transform *previousJoint, double length, double radius, double transparent)
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

void QtOSGWidget::createChildAxis(
        osg::Transform *tf, double length, double radius, double transparent)
{
    // create the origin and axis of the global frame with length 5m for
    // each axis
    createAxis(tf, length, radius, transparent);
    m_root->addChild(tf);
}

void QtOSGWidget::loadHDMap(HDMap *map)
{
    m_map = map;
}

using namespace lanelet;
void QtOSGWidget::drawLanelet()
{
    PointLayer &point_layer = ((m_map->map)->pointLayer);

    size_t size = point_layer.size();
    std::cout << "node size" << size << std::endl;

    osg::Vec3 point;

    for (auto i = point_layer.begin(); i != point_layer.end(); i++)
    {
        point.x() = i->x();
        point.y() = i->y();
        point.z() = i->z();
        createSphere(point, 1.0, black);
    }

    Point3d aPoint = *point_layer.begin();
    printf("point[%.3f,%.3f,%.3f], id[%d] \n", aPoint.x(), aPoint.y(), aPoint.z(), aPoint.id());

    LaneletLayer &lane_layer = ((m_map->map)->laneletLayer);
    size = lane_layer.size();
    std::cout << "lane size" << size << std::endl;

    LineString3d left, right;

    size_t lane_num = 0;
    for (auto i = lane_layer.begin(); i != lane_layer.end(); i++)
    {
        left = i->leftBound();
        osg::Vec3Array *left_bound = new osg::Vec3Array(left.size());

        std::cout << "left size" << left.size() << std::endl;
        for (std::size_t j = 0; j < left.size(); j++)
        {
            left_bound->at(j)[0] = left[j].x();
            left_bound->at(j)[1] = left[j].y();
            left_bound->at(j)[2] = left[j].z();
            printf("point[%.3f,%.3f,%.3f], id[%d] \n", left[j].x(), left[j].y(), left[j].z(),
                    int32_t(j));
        }
        createLineList(m_base_tf, left_bound, white, 0.8);

        right = i->rightBound();
        osg::Vec3Array *right_bound = new osg::Vec3Array(right.size());
        std::cout << "right size" << right.size() << std::endl;
        for (std::size_t j = 0; j < right.size(); j++)
        {
            right_bound->at(j)[0] = right[j].x();
            right_bound->at(j)[1] = right[j].y();
            right_bound->at(j)[2] = right[j].z();
            printf("point[%.3f,%.3f,%.3f], id[%d] \n", right[j].x(), right[j].y(), right[j].z(),
                    int32_t(j));
        }
        createLineList(m_base_tf, right_bound, white, 1.0);

        lane_num++;
        // if (lane_num > 100)
        // {
        //     break;
        // }
    }
}
}  // namespace cat