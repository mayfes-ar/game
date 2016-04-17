#include "object/object.h"

using namespace std;

map<string, int> Object::imgHandles;
map<string, vector<int>> Object::effectHandles;

// 一番最初に一回だけ呼ぶ
bool Object::load() {

	auto loadImage = [](string key, string name) -> bool {
		const string path = "img/" + name + ".png";
		const int handle = LoadGraph(path.c_str());
		if (handle == -1) {
			return false;
		} else {
			Object::imgHandles[key] = handle;
			return true;
		}
	};

	// xNum: 横軸の画像枚数, yNum: 縦軸の画像枚数, width: 画像全体の横サイズ, height: 画像全体の縦サイズ
	auto loadEffect = [](string key, string name, int xNum, int yNum, int width, int height) -> bool {
		const string path = "effect/" + name + ".png";
		const int allNum = xNum * yNum;
		int* handles = new int[allNum];

		bool result;
		if (LoadDivGraph(path.c_str(), allNum, xNum, yNum, width / xNum, height / yNum, handles) == -1) {
			result = false;
		} else {
			for (int i = 0; i < allNum; i++) {
				Object::effectHandles[key].push_back(handles[i]);
			}
			result = true;
		}

		delete[] handles;
		return result;
	};

	// 使い方 isSuccess &= loadなんとか(登録するキー, ファイル名, ...);
	bool isSuccess = true;

	isSuccess &= loadImage("player", "mario");
	isSuccess &= loadImage("block", "block");

	//メニューで使う
	isSuccess &= loadImage("monhan", "monhan");
	isSuccess &= loadImage("rockman", "rockman");
	isSuccess &= loadImage("ar2016_logo", "wallpaper");
	isSuccess &= loadImage("menu_title", "menu_title");
	isSuccess &= loadEffect("effect1", "warp", 1, 6, 640, 2880);

	// Single Player Game
	isSuccess &= loadImage("s_game_bg", "wallpaper_alpha");
	isSuccess &= loadImage("s_game_result", "s_game_result");
	isSuccess &= loadImage("s_game_dead", "dead_sample");
	isSuccess &= loadImage("wanwan", "wanwan");

	return isSuccess;
}
