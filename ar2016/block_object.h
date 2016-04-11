#pragma once

#include "object.h"
#define _USE_MATH_DEFINES // for C++
#include <math.h>

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
			double reti = (right() - left()) / 50.0;
			switch (rect.direction) {
				case 0:

					DrawRotaGraph(rect.x + rect.width / 2, rect.y + rect.height / 2, reti, 0, imgHandles["block"], true);
					break;
				case 1:
					DrawRotaGraph(rect.x + rect.width / 2, rect.y + rect.height / 2, reti, M_PI_2, imgHandles["block"], true);
					break;
				case 2:
					DrawRotaGraph(rect.x + rect.width / 2, rect.y + rect.height / 2, reti, M_PI, imgHandles["block"], true);
					break;
				case 3:
					DrawRotaGraph(rect.x + rect.width / 2, rect.y + rect.height / 2, reti, -1*M_PI_2, imgHandles["block"], true);
					break;
			}
		}
		return isStay;
	}

	double rightHit() const { return isStay ? right() : right() - rect.width / 3; }
	double leftHit() const { return isStay ? left() : left() + rect.width / 3; }
	double topHit() const { return isStay ? top() : top() + rect.height / 3; }
	double bottomHit() const { return isStay ? bottom() : bottom() - rect.height / 3; }
		
};