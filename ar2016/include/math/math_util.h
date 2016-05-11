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

	template <typename T>
	inline T clamp(T val, T min, T max) {
		assert(min <= max);
		return std::max(min, std::min(val, max));
	}

	template <typename T>
	inline T wrap(T val, T low, T high) {
		assert(low < high);
		const T n = std::fmod(val - low, high - low);
		return (n >= 0) ? n + low : n + high;
	}

	template <>
	inline int wrap(int val, int low, int high) {
		assert(low < high);
		const int n = (val - low) % (high - low);
		return (n >= 0) ? n + low : n + high;
	}
} // namespace MathUtil
