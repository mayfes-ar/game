#pragma once

#include "const.h"
#include "DxLib.h"
#include <string>
#include <map>
#include <memory>
#include <list>
#include <vector>
#include <random>


// double x, y;　左上の座標
// int width, height;
// double  rotate; 回転角（ラジアン）
// int id; マーカーの種類
class Rectan {
public:

	double x, y;
	int width, height;
	double rotate;

	Rectan() : x(0), y(0), width(0), height(0), rotate(0) {}
	Rectan(int x_, int y_, int width_, int height_) : x(x_), y(y_), width(width_), height(height_), rotate(0) {}
	Rectan(int x_, int y_, int width_, int height_, int rotate_) : x(x_), y(y_), width(width_), height(height_), rotate(rotate_) {}

	void scale(double rate) {
		x *= rate; y *= rate; 
        width = static_cast<double>(width * rate); 
        height = static_cast<double>(height * rate);
	}

	void translate(int xShift, int yShift) {
		x += static_cast<double>(xShift); 
        y += static_cast<double>(yShift);
	}
};


// abstract class
// 描画するものは全てこのクラスを継承して draw を実装する
// layer は大きいほど前面に来る
class Object {
protected:
	Rectan rect;
	int layer;

	static std::map<std::string, int> imgHandles;
	static std::map<std::string, std::vector<int>> effectHandles;
	static std::map<std::string, int> Object::soundHandles;

	virtual ~Object(){}

public:
	int getLayer() const { return layer; }
	double right() const { return rect.x + rect.width; }
	double left() const { return rect.x; }
	double top() const { return rect.y; }
	double bottom() const { return rect.y + rect.height; }

	bool isContacted(std::shared_ptr<Object> object) {
		return  (left() < object->right() && top() < object->bottom() &&
			right() > object->left() && bottom() > object->top());
	}

	virtual bool draw() = 0;

	static bool load();
};
