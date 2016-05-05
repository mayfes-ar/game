#include "math/collision_detection.h"
#include "math/math_util.h"
#include <math.h>


namespace CollisionDetection {

	// 円が線分に触れているか
	bool isOnLine(const Shape::Circle& child, const Shape::Line& parent, const float speed) {
		const auto dist = MathUtil::distPointToLine(child.center, parent);
		if (dist < child.radius + (int)speed) { 
			const Eigen::Vector2f vector_a_to_center = (child.center - parent.point).cast<float>();
			const Eigen::Vector2f vector_b_to_center = (child.center - (parent.point + parent.dir)).cast<float>();
			const float a_dot_p = vector_a_to_center.dot(parent.dir.cast<float>());
			const float b_dot_p = vector_b_to_center.dot(parent.dir.cast<float>());
			if ((a_dot_p >= 0 && b_dot_p <= 0) || (a_dot_p <= 0 && b_dot_p >=0)) 
				return true;
			else if ((float)child.radius > vector_a_to_center.norm() || (float)child.radius > vector_b_to_center.norm())
				return true;
		}
		return false;
	}

	// 円の中心が線分に収まっているか
	bool isInLine(const Shape::Circle& child, const Shape::Line& parent) {
		const auto dist = MathUtil::distPointToLine(child.center, parent);
		if (dist < child.radius) {
			const Eigen::Vector2i vector_a_to_circle_corner1 = (child.center - (parent.dir * child.radius) / parent.dir.norm()) - parent.point;
			const Eigen::Vector2i vector_b_to_circle_corner1 = (child.center - (parent.dir * child.radius) / parent.dir.norm()) - (parent.point - parent.dir);
			const Eigen::Vector2i vector_a_to_circle_corner2 = (child.center + (parent.dir * child.radius) / parent.dir.norm()) - parent.point;
			const Eigen::Vector2i vector_b_to_circle_corner2 = (child.center + (parent.dir * child.radius) / parent.dir.norm()) - (parent.point - parent.dir);
			const int a_dot_corner1 = vector_a_to_circle_corner1.dot(parent.dir);
			const int b_dot_corner1 = vector_b_to_circle_corner1.dot(parent.dir);
			const int a_dot_corner2 = vector_a_to_circle_corner2.dot(parent.dir);
			const int b_dot_corner2 = vector_b_to_circle_corner2.dot(parent.dir);
			if ((a_dot_corner1 >= 0 && b_dot_corner1 <= 0) || (a_dot_corner1 <= 0 && b_dot_corner1 >= 0) &&
				(a_dot_corner2 >= 0 && b_dot_corner2 <= 0) || (a_dot_corner2 <= 0 && b_dot_corner2 >= 0)) {
				return true;
			}
		}
		return false;
	}

	// 円が円の輪郭にあるかかどうか
	bool isOnCircle(const Shape::Circle& child, const Shape::Circle& parent)
	{
		const auto dist_center_to_center = (child.center - parent.center).norm();

		if (dist_center_to_center > parent.radius + child.radius) {
			return false;
		}

		return true;
	}
	// 円が円の内部にあるかかどうか
	bool isInCircle(const Shape::Circle& child, const Shape::Circle& parent)
	{
		if (parent.radius < child.radius) {
			return false;
		}

		const auto dist_center_to_center = (child.center - parent.center).norm();

		if (dist_center_to_center > parent.radius) {
			return false;
		}

		return true;
	}

	// 点が四角形の内部にあるかどうか
	// http://www.endo-yuta.com/2011/09/blog-post_06.html
	bool isInRectangle(const Shape::Point& child, const Shape::Rectangle& parent)
	{
		// 半時計回りに点と長方形の辺で外積をとる。
		// すべての外積が負の場合、点は長方形の内側にある
		const Eigen::Vector2i to_left = Eigen::Vector2i(-parent.width, 0);
		const Eigen::Vector2i point_to_left = child - parent.getRightTopPoint();
		const auto cross_left = MathUtil::cross(to_left, point_to_left);
		if (cross_left >= 0) {
			return false;
		}

		const Eigen::Vector2i to_bottom = Eigen::Vector2i(0, parent.height);
		const Eigen::Vector2i point_to_bottom = child - parent.getLeftTopPoint();
		const auto cross_bottom = MathUtil::cross(to_bottom, point_to_bottom);
		if (cross_bottom >= 0) {
			return false;
		}

		const Eigen::Vector2i to_right = Eigen::Vector2i(parent.width, 0);
		const Eigen::Vector2i point_to_right = child - parent.getLeftBottomPoint();
		const auto cross_right = MathUtil::cross(to_right, point_to_right);
		if (cross_right >= 0) {
			return false;
		}

		const Eigen::Vector2i to_top = Eigen::Vector2i(0, -parent.height);
		const Eigen::Vector2i point_to_top = child - parent.getRightBottomPoint();
		const auto cross_top = MathUtil::cross(to_top, point_to_top);
		if (cross_right >= 0) {
			return false;
		}

		return true;
	}

	// 円が四角形の内部にあるかどうか
	bool isInRectangle(const Shape::Circle& child, const Shape::Rectangle& parent) {
		if (isOnRectangle(child, parent)) {
			return false;
		}

		// 外積と内積を用いて二つのベクトルの角度を計算
		// http://1st.geocities.jp/shift486909/program/collision.html
		auto calc_rad = [](Shape::Point begin, Shape::Point end, Shape::Point target) -> double {
			const auto b_to_e = end - begin;
			const auto b_to_t = target - begin;
			const double inner = static_cast<double>(b_to_e.dot(b_to_t));
			const double outer = static_cast<double>(MathUtil::cross(b_to_e, b_to_t));
			return std::atan2(outer, inner);
		};

		const auto rad_a = calc_rad(parent.getLeftTopPoint(), parent.getRightTopPoint(), child.center);
		const auto rad_b = calc_rad(parent.getRightBottomPoint(), parent.getLeftBottomPoint(), child.center);

		if (rad_a >=  M_PI / 2.0 || rad_b >= M_PI / 2.0) {
			return false;
		}

		return true;

	}

	// 四角形が四角形の内部にあるかどうか
	bool isInRectangle(const Shape::Rectangle& child, const Shape::Rectangle& parent)
	{
		if (!isInRectangle(child.getLeftBottomPoint(), parent)) {
			return false;
		}
		else if (!isInRectangle(child.getLeftTopPoint(), parent)) {
			return false;
		}
		else if (!isInRectangle(child.getRightBottomPoint(), parent)) {
			return false;
		}
		else if (!isInRectangle(child.getRightTopPoint(), parent)) {
			return false;
		}

		return true;
	}

	// 円が四角形の輪郭の上にいるかどうか
	bool isOnRectangle(const Shape::Circle& child, const Shape::Rectangle& parent)
	{
		if (!isOnLine(child, parent.getLeftLine())) {
			return false;
		}
		else if (!isOnLine(child, parent.getTopLine())) {
			return false;
		}
		else if (!isOnLine(child, parent.getRightLine())) {
			return false;
		}
		else if (!isOnLine(child, parent.getBottomLine())) {
			return false;
		}

		return true;
	}

} // namespace CollisionDetection