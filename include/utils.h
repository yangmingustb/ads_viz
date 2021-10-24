#pragma once
#include <osg/Array>
#include <osg/MatrixTransform>
#include <osg/Matrixd>
#include <osg/Matrixf>
#include "transform.h"

namespace cat
{
#if 1
typedef osg::Matrixd osg_Matrixr;
#else
typedef osg::Matrixf osg_Matrixr;
#endif

// convert tf to osg matrix
osg_Matrixr tfToOsgMat(const Transform3d &tf);

osg_Matrixr tf2DToOsgMat(const Transform2d &tf);
// convert osg matrix to a tf
Transform3d osgMatToTf(const osg_Matrixr &mat);
// convert a tf orientation matrix to an osg matrix
osg_Matrixr toOsgMat(const Matrix3d &orientation);
// get an osg matrix from a 3x3 orientation matrix and a point vector
osg_Matrixr toOsgMat(const Matrix3d &orientation, double x, double y, double z);
// extract Euler angles from a quaternion
void getEulerFromQuat(
        osg::Quat q, double &heading, double &attitude, double &bank);
// set transform with a 3x3 orientation matrix and a point vector
Transform3d setTransform(
        const Matrix3d &orientation, double x, double y, double z);
// convert a vector of cdl point to an osg array
osg::Vec3Array *toOsgVec3Array(std::vector<Vector3d> &vertices);

Matrix3d getRotMatFromYaw(double radian);

osg::ref_ptr<osg::MatrixTransform> eigenTf2OsgTf(const Transform3d &eigen_tf);

}  // namespace cat