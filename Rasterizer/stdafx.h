#pragma once

#include <wx/wxprec.h>

#include <string>
#include <cstdint>

#include <Eigen/Dense>

template<int size>
using vec = Eigen::Matrix<float, size, 1>;

using mat4 = Eigen::Matrix4f;
