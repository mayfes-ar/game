#pragma once

#include "DxLib.h"
#include <string>
#include <map>
#include <memory>
#include <list>


// abstract class
class Object {
protected:
	double x, y;
	int width, height;
	int layer;

	static std::map<std::string, int> imgHandles;

	virtual ~Object(){}

public:
	int getLayer() const { return layer; }
	double right() const { return x + width; }
	double left() const { return x; }
	double top() const { return y; }
	double bottom() const { return y + height; }
	virtual bool draw() const = 0;

	static bool load();
};
