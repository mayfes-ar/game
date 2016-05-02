/*
	当たり判定
*/
#pragma once


#include "object/shape.h"

namespace CollisionDetection {

	// 円が線分上にあるかどうか
	bool isOnLine(const Shape::Circle& child, const Shape::Line& parent);

	// 円が線分内にあるかどうか
	bool isInLine(const Shape::Circle& child, const Shape::Line& parent);

	// 円が円の輪郭上にあるかどうか
	bool isOnCircle(const Shape::Circle& child, const Shape::Circle& parent);

	// 円が円の内部にあるかどうか
	bool isInCircle(const Shape::Circle& child, const Shape::Circle& parent);

	// 点が四角形の内部にあるかどうか
	bool isInRectangle(const Shape::Point& child, const Shape::Rectangle& parent);

	// 円が四角形の内部にあるかどうか
	bool isInRectangle(const Shape::Circle& child, const Shape::Rectangle& parent);

	// 四角形が四角形の内部にあるかどうか
	bool isInRectangle(const Shape::Rectangle& child, const Shape::Rectangle& parent);

	// 円が四角形の輪郭の上にいるかどうか
	bool isOnRectangle(const Shape::Circle& child, const Shape::Rectangle& parent);

} // namespace CollisionDetection