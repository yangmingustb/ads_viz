#include <osg_utils.h>

namespace cat
{
osg_Matrixr tfToOsgMat(const Transform3d& tf)
{
    osg_Matrixr osg_mat;
    Eigen::Map<Eigen::Matrix<double, 4, 4, Eigen::RowMajor>>(osg_mat.ptr()) =
        tf.matrix().transpose();
    return osg_mat;
}

osg_Matrixr tf2DToOsgMat(const Transform2d& tf)
{
    Transform3d tf3D;
    tf3D.linear() << tf.linear().row(0)(0), tf.linear().row(0)(1), 0.0,
        tf.linear().row(1)(0), tf.linear().row(1)(1), 0.0, 0.0, 0.0, 1.0;
    tf3D.translation() << tf.translation()(0), tf.translation()(1), 0.0;
    osg_Matrixr osg_mat;
    Eigen::Map<Eigen::Matrix<double, 4, 4, Eigen::RowMajor>>(osg_mat.ptr()) =
        tf3D.matrix().transpose();
    return osg_mat;
}

Transform3d osgMatToTf(const osg_Matrixr& mat)
{
    Transform3d tf;
    osg_Matrixr osgMatrix = mat;
    tf.matrix() =
        Eigen::Map<Eigen::Matrix<double, 4, 4, Eigen::RowMajor>>(osgMatrix.ptr());
    return tf;
}

osg_Matrixr toOsgMat(const Matrix3d& orientation)
{
    Transform3d tf;
    tf.setIdentity();
    tf.linear() = orientation;
    osg_Matrixr osg_mat = tfToOsgMat(tf);

    return osg_mat;
}

osg_Matrixr toOsgMatrix(const Matrix3d& orientation, double x, double y, double z)
{
    Transform3d tf;
    tf.setIdentity();
    tf.linear() = orientation;
    tf.translation()[0] = x;
    tf.translation()[1] = y;
    tf.translation()[2] = z;

    osg_Matrixr osg_mat = tfToOsgMat(tf);

    return osg_mat;
}

void getEulerFromQuat(osg::Quat q, double& heading, double& attitude, double& bank)
{
    double limit = 0.499999;
    double sqx = q.x() * q.x();
    double sqy = q.y() * q.y();
    double sqz = q.z() * q.z();

    double t = q.x() * q.y() + q.z() * q.w();

    if (t > limit)  // gimbal lock ?
    {
        heading = 2 * atan2(q.x(), q.w());
        attitude = osg::PI_2;
        bank = 0;
    }
    else if (t < -limit)
    {
        heading = -2 * atan2(q.x(), q.w());
        attitude = -osg::PI_2;
        bank = 0;
    }
    else
    {
        heading =
            atan2(2 * q.y() * q.w() - 2 * q.x() * q.z(), 1 - 2 * sqy - 2 * sqz);
        attitude = asin(2 * t);
        bank =
            atan2(2 * q.x() * q.w() - 2 * q.y() * q.z(), 1 - 2 * sqx - 2 * sqz);
    }
}

osg::Vec3Array* toOsgVec3Array(std::vector<Vector3d>& vertices)
{
    osg::Vec3Array* osg_vertices = new osg::Vec3Array(vertices.size());
    for (std::size_t i = 0; i < vertices.size(); i++)
    {
        osg_vertices->at(i)[0] = vertices.at(i)[0];
        osg_vertices->at(i)[1] = vertices.at(i)[1];
        osg_vertices->at(i)[2] = vertices.at(i)[2];
    }
    return osg_vertices;
}

template <typename S>
using AngleAxis = Eigen::AngleAxis<S>;
using AngleAxisr = AngleAxis<double>;

Matrix3d getRotMatFromYaw(double radian)
{
    Quaterniond rotation = AngleAxis<double>(0.0, Vector3d::UnitX()) *
                           AngleAxis<double>(0, Vector3d::UnitY()) *
                           AngleAxis<double>(radian, Vector3d::UnitZ());
    return rotation.matrix();
}

osg::ref_ptr<osg::MatrixTransform> eigenTf2OsgTf(const Transform3d& eigen_tf)
{
    osg::ref_ptr<osg::MatrixTransform> osg_tf = new osg::MatrixTransform;
    osg_tf->setMatrix(tfToOsgMat(eigen_tf));
    return osg_tf;
}

}  // namespace cat