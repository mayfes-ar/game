/*
	図形関連のデータ
	円と四角形が定義されている
*/
#pragma once

#include <Eigen/Core>

namespace Shape {


	struct Circle {
		Circle() {}
		Circle(const Eigen::Vector2i& c, int r)
			: center(c), radius(r) {}
		
		Eigen::Vector2i center = Eigen::Vector2i::Zero();
		int radius = 0;
	};

	struct Rectangle {
		Rectangle() {}
		Rectangle(const Eigen::Vector2i& s, int width, int height)
			: start_point(s), width(width), height(height)
		{}

		Eigen::Vector2i start_point = Eigen::Vector2i::Zero();
		int width = 0;
		int height = 0;

		int left() const {
			return start_point.x();
		}

		int top() const {
			return start_point.y();
		}

		int right() const {
			return start_point.x() + width;
		}

		int bottom() const {
			return start_point.y() + height;
		}

		Eigen::Vector2i getLeftTopPoints() const {
			const Eigen::Vector2i ret = Eigen::Vector2i{ left(), top() };
			return ret;
		}

		Eigen::Vector2i getLeftBottomPoints() const {
			const Eigen::Vector2i ret = Eigen::Vector2i{ left(), bottom() };
			return ret;
		}

		Eigen::Vector2i getRightTopPoints() const {
			const Eigen::Vector2i ret = Eigen::Vector2i{ right(), top() };
			return ret;
		}

		Eigen::Vector2i getRightBottomPoints() const {
			const Eigen::Vector2i ret = Eigen::Vector2i{ right(), bottom() };
			return ret;
		}
	};

} // namespace Shape
