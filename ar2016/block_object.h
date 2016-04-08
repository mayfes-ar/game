#pragma once

#include "object.h"

class BlockObject : public Object {
	bool isStay;
public:
	BlockObject(int x_, int y_, int width_, int height_, bool isStay_) {
		rect.x = x_, rect.y = y_, rect.width = width_, rect.height = height_;
		isStay = isStay_;
		layer = 20;
	}

	BlockObject(Rectan rect_, bool isStay_) {
		rect = rect_;
		isStay = isStay_;
		layer = 20;
	}

	bool draw() {
		if (isStay) {
			DrawBox(left(), top(), right(), bottom(), GetColor(240, 117, 28), false);
		} else {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["block"], true);
		}
		return isStay;
	}

	double rightHit() const { return isStay ? right() : right() - rect.width / 3; }
	double leftHit() const { return isStay ? left() : left() + rect.width / 3; }
	double topHit() const { return isStay ? top() : top() + rect.height / 3; }
	double bottomHit() const { return isStay ? bottom() : bottom() - rect.height / 3; }
		
};