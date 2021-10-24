#pragma once
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/StdVector>
#include <iostream>

namespace cat
{

using Transform2d = Eigen::Transform<double, 2, Eigen::Isometry>;
using Transform3d = Eigen::Transform<double, 3, Eigen::Isometry>;

using Translation2d = Eigen::Translation2d;
using Rotation2d = Eigen::Rotation2Dd;

using Vector4d = Eigen::Matrix<double, 4, 1>;
using Vector3d = Eigen::Matrix<double, 3, 1>;
using Vector2d = Eigen::Matrix<double, 2, 1>;
using Matrix2d = Eigen::Matrix<double, 2, 2>;
using Matrix3d = Eigen::Matrix<double, 3, 3>;

using Quaterniond = Eigen::Quaternion<double>;
} // namespace robot