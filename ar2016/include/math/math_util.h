#pragma once

#include <cmath>
#define _USE_MATH_DEFINES
#include <Eigen/Core>
#include "object/shape.h"

namespace MathUtil {
	inline int cross(const Eigen::Vector2i& a, const Eigen::Vector2i& b) {
		return a.x() * b.y() - a.y() * b.x();
	}

	inline int distPointToLine(const Eigen::Vector2i& point, const Shape::Line& line) {
		int area = std::abs(MathUtil::cross(line.dir, point - line.point));
		int width = line.dir.norm();
		assert(width != 0);
		return area / width;
	}
} // namespace MathUtil
