#pragma once

#include "DxLib.h"


class Object {
protected:
	int x, y;
	int width, height;

public:
	
	virtual bool draw() const = 0;
};

class Player : public Object {

public:
	Player(int x_, int y_, int width_, int height_) {
		x = x_;
		y = y_;
		width = width_;
		height = height_;
	}

	bool draw() const {
		DrawBox(x, y, x + width, y + height, GetColor(255, 0, 0), true);
		return true;
	}

	void action(const char key[]) {
		if (key[KEY_INPUT_RIGHT]) {
			x += 5;
		}
		if (key[KEY_INPUT_LEFT]) {
			x -= 5;
		}
	}
};