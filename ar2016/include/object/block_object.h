#pragma once

#include "object/object.h"

class BlockObject : public Object {
protected:
	bool willStay;
public:
	bool canHit = true;

	BlockObject(){}
	BlockObject(int x_, int y_, int width_, int height_, bool willStay_) {
		rect.x = x_, rect.y = y_, rect.width = width_, rect.height = height_;
		willStay = willStay_;
		layer = 20;
	}

	BlockObject(Rectan rect_, bool willStay_) {
		rect = rect_;
		willStay = willStay_;
		layer = 20;
	}

	virtual bool draw() {
		if (willStay) {
			DrawBox(left(), top(), right(), bottom(), GetColor(240, 117, 50), true);
		} else {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["block"], true);
			DrawFormatString(left(), top(), GetColor(255, 255, 255), _T("%.2f"), rect.rotate);
		}
		return willStay;
	}

	double rightHit() const { return willStay ? right() : right() - rect.width / 3; }
	double leftHit() const { return willStay ? left() : left() + rect.width / 3; }
	double topHit() const { return willStay ? top() : top() + rect.height / 3; }
	double bottomHit() const { return willStay ? bottom() : bottom() - rect.height / 3; }
		
};
