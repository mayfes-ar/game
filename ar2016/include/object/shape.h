#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace Shape {

	using Point = Eigen::Matrix<int, 2, 1, Eigen::DontAlign>;

	// 直線の定義
	struct Line {
		explicit Line(const Eigen::Vector2i& point, const Eigen::Vector2i& dir)
			: point(point), dir(dir)
		{}

		virtual ~Line() {}

		Line getRotatedLine(const Eigen::Vector2i& center, double rotation) {
			Eigen::Vector2i vector_c_a = point - center;
			Eigen::Vector2i vector_c_b = dir + vector_c_a;
			Eigen:: Matrix2d rot;
			rot = Eigen::Rotation2Dd(rotation);
			Eigen::Vector2i rotated_vector_c_a = (rot * vector_c_a.cast<double>()).cast<int>();
			Eigen::Vector2i rotated_vector_c_b = (rot * vector_c_b.cast<double>()).cast<int>();
			return Line(rotated_vector_c_a + center, rotated_vector_c_b - rotated_vector_c_a);
		}

		Eigen::Vector2i point = Eigen::Vector2i::Zero();
		Eigen::Vector2i dir = Eigen::Vector2i::Zero();
	};

	// 線分の定義
	// コンストラクタで端と端の点を渡すように
	struct LineSeg : public Line {
		explicit LineSeg(const Eigen::Vector2i& start_point, const Eigen::Vector2i& end_point)
			: start_point(start_point), end_point(end_point),
			  Line(start_point, end_point - start_point)
		{}

		Eigen::Vector2i start_point = Eigen::Vector2i::Zero();
		Eigen::Vector2i end_point = Eigen::Vector2i::Zero();
	};


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


		Eigen::Vector2i getLeftTopPoint() const {
			const Eigen::Vector2i ret = Eigen::Vector2i{ left(), top() };
			return ret;
		}

		Eigen::Vector2i getLeftBottomPoint() const {
			const Eigen::Vector2i ret = Eigen::Vector2i{ left(), bottom() };
			return ret;
		}

		Eigen::Vector2i getRightTopPoint() const {
			const Eigen::Vector2i ret = Eigen::Vector2i{ right(), top() };
			return ret;
		}

		Eigen::Vector2i getRightBottomPoint() const {
			const Eigen::Vector2i ret = Eigen::Vector2i{ right(), bottom() };
			return ret;
		}

		Eigen::Vector2i getCenterPoint() const {
			const Eigen::Vector2i ret = Eigen::Vector2i{ start_point.x() + width * 0.5, start_point.y() + height * 0.5 };
			return ret;
		}

		Line getLeftLine() const {
			Line line(getLeftTopPoint(), getLeftBottomPoint() - getLeftTopPoint());
			return line;
		}

		Line getRightLine() const {
			Line line(getRightTopPoint(), getRightBottomPoint() - getRightTopPoint());
			return line;
		}
		Line getTopLine() const {
			Line line(getLeftTopPoint(), getRightTopPoint() - getLeftTopPoint());
			return line;
		}
		Line getBottomLine() const {
			Line line(getLeftBottomPoint(), getRightBottomPoint() - getLeftBottomPoint());
			return line;
		}
	};

} // namespace Shape
