#pragma once

#include "DxLib.h"
#include <string>
#include <map>
#include <memory>
#include <list>


// abstract class
class Object {
protected:
	int x, y;
	int width, height;
	int layer;

	static std::map<std::string, int> imgHandles;

	virtual ~Object(){}

public:
	int getLayer() const { return layer; }
	virtual bool draw() const = 0;

	static bool load();
};


class Background : public Object {
	int& handle;

public:
	Background(int& handle_): handle(handle_) {
		layer = 0;
	}

	bool draw() const {
		DrawExtendGraph(160, 0, 1120, 720, handle, FALSE);
		return true;
	}
};


class Player : public Object {

public:
	Player(int x_, int y_, int width_, int height_) {
		x = x_;
		y = y_;
		width = width_;
		height = height_;
		layer = 100;
	}

	bool draw() const {
		DrawGraph(x, y, imgHandles["mario"], true);
		return true;
	}

	void action(const char key[]) {
		if (key[KEY_INPUT_RIGHT]) {
			x += 5;
		}
		if (key[KEY_INPUT_LEFT]) {
			x -= 5;
		}
		if (key[KEY_INPUT_UP]) {
			y -= 5;
		}
		if (key[KEY_INPUT_DOWN]) {
			y += 5;
		}
	}
};