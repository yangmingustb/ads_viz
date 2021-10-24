#pragma once
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Texture2D>
#include <osgUtil/ShaderGen>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Node>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineStipple>
#include <osg/LineWidth>

#include <osg/BlendFunc>
#include <osg/LightModel>


#include <osgViewer/Viewer>
#include <osgGA/StateSetManipulator>
#include <osgDB/ReadFile>
#include <osgText/Font>
#include <osgText/Text>
#include <osg/GraphicsContext>


class Viewer {

  public:
    Viewer();
    ~Viewer() = default;


    osg::Vec4 purple{1.0f, 0.0f, 1.0f, 1.0f};
    osg::Vec4 blue{0.0f, 0.0f, 1.0f, 1.0f};
    osg::Vec4 red{1.0f, 0.0f, 0.0f, 1.0f};
    osg::Vec4 yellow{1.0f, 1.0f, 0.0f, 1.0f};
    osg::Vec4 black{0.0f, 0.0f, 0.0f, 1.0f};
    osg::Vec4 white{1.0f, 1.0f, 1.0f, 1.0f};
    osg::Vec4 grey{0.3f, 0.3f, 0.3f, 0.3f};
    osg::Vec4 green{0.0f, 1.0f, 0.0f, 1.0f};
    osg::Vec4 light_blue{0.0f, 0.0f, 1.0f, 0.3f};


    osg::ref_ptr<osg::Group> root;
    osg::StateSet *global_state_set;
    osg::TessellationHints* global_hints;

    osg::ref_ptr<osgViewer::Viewer> viewer;

    osg::Geode* createArrowGeode(double height,
                                 double radius,
                                 const osg::Vec4& color,
                                 osg::TessellationHints* hints);

    void createAxis(osg::Transform* previousJoint,
                    double length,
                    double radious = 0.05,
                    double transparent = 0.3);

    void createChildAxis(osg::Transform* tf,
                    double length,
                    double radious = 0.05,
                    double transparent = 0.3);

    // add sphere to the global frame
    void createSphere(const osg::Vec3 &center,
                      double radius,
                      const osg::Vec4 &color);

    void createSphere(osg::Transform* tf,
                      const osg::Vec3 &center,
                      double radius,
                      const osg::Vec4 &color);

    void createText(osg::Transform* tf,
                    const osg::Vec3& pos,
                    double size,
                    std::string& label,
                    const osg::Vec4& color = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

    void createLineStrip(osg::Transform* tf,
                         osg::Vec3Array *vertices,
                         const osg::Vec4 &color,
                         double line_width = 1.0);

    void createLine(osg::Transform* tf,
                    const osg::Vec3 &point_a,
                    const osg::Vec3 &point_b,
                    const osg::Vec4 &color,
                    double line_width = 1.0);

    void createLineList(osg::Transform* tf,
                        osg::Vec3Array *vertices,
                        const osg::Vec4 &color,
                        double line_width = 1.0);

    void createLineLoop(osg::Transform* tf,
                        osg::Vec3Array *vertices,
                        const osg::Vec4 &color,
                        double line_width = 1.0);

    void createPolygon(osg::Transform* tf,
                        osg::Vec3Array *vertices,
                        const osg::Vec4 &color,
                        double line_width = 1.0);

    void createRectangle(osg::Transform* tf,
                         const osg::Vec3 &upper,
                         const osg::Vec3 &lower,
                         const osg::Vec4 &color,
                         double line_width = 1.0);

    void createMeshFromPath(osg::Transform* tf,
                            osg::Vec3Array *path,
                            const osg::Vec4 &color,
                            double line_widht = 1.0);

    void createMeshFromVertices(osg::Transform* tf,
                                osg::Vec3Array *vertices,
                                const osg::Vec4 &color,
                                double line_widht = 1.0);

    void createFacesFromVertices(osg::Transform* tf,
                                osg::Vec3Array *vertices,
                                osg::Vec3iArray *triangles,
                                const osg::Vec4 &color,
                                double line_width = 1.0);


    void createWireFrameCube(osg::Transform* tf,
                             const osg::Vec3 &center,
                             const osg::Vec3 &sides,
                             const osg::Vec4 &color);

    void createWireFrameBox(osg::Transform* tf,
                             const osg::Vec3 &center,
                             const osg::Vec3 &sides,
                             const osg::Vec4 &color,
                             double line_width = 1.0);

    void createWireFrameCube(osg::Transform* tf_root,
                             osg::Transform* tf,
                             const osg::Vec3 &center,
                             const osg::Vec3 &sides,
                             const osg::Vec4 &color);

    void createCube(osg::Transform* tf,
                    const osg::Vec3 &center,
                    const osg::Vec3 &sides,
                    const osg::Vec4 &color);

    void setMaterialColor(osg::StateSet *state, const osg::Vec4 &color);

    void setMainWindowSize(uint32_t width, uint32_t height);

    void setDisplayData();

    void setWindowName(std::string window_name);

  private:
    uint32_t main_window_width;
    uint32_t main_window_height;

};
