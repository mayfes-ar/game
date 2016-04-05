#pragma once

#include "DxLib.h"
#include <string>
#include <map>
#include <memory>
#include <list>
#include <vector>


// abstract class
// 描画するものは全てこのクラスを継承して draw を実装する
// layer は大きいほど前面に来る
class Object {
protected:
	double x, y;
	int width, height;
	int layer;

	static std::map<std::string, int> imgHandles;
	static std::map<std::string, std::vector<int>> effectHandles;

	virtual ~Object(){}

public:
	int getLayer() const { return layer; }
	double right() const { return x + width; }
	double left() const { return x; }
	double top() const { return y; }
	double bottom() const { return y + height; }

	virtual bool draw() = 0;

	static bool load();
};
