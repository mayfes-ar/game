#include "object/object.h"

using namespace std;

map<string, int> Object::imgHandles;
map<string, vector<int>> Object::effectHandles;
map<string, int> Object::soundHandles;

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

	auto loadSound = [](string key, string name) -> bool {
		const string path = "sound/" + name;
		const int handle = LoadSoundMem(path.c_str());
		if (handle == -1) {
			return false;
		} else {
			Object::soundHandles[key] = handle;
			return true;
		}
	};

	// 使い方 isSuccess &= loadなんとか(登録するキー, ファイル名, ...);
	bool isSuccess = true;

	isSuccess &= loadImage("player", "mario");
	isSuccess &= loadImage("block", "block");
	//isSuccess &= loadImage("brakout-background", "firefox");

	//メニューで使う
	isSuccess &= loadImage("monhan", "monhan");
	isSuccess &= loadImage("rockman", "rockman");
	isSuccess &= loadImage("ar2016_logo", "wallpaper");
	isSuccess &= loadImage("menu_title", "menu_title");
	isSuccess &= loadSound("menu_bgm", "welcome_to_the_Chu-2_byo_world!.mp3");
	isSuccess &= loadEffect("effect1", "warp", 1, 6, 640, 2880);

	// Single Player Game
	isSuccess &= loadImage("s_game_bg", "wallpaper_alpha");
	isSuccess &= loadImage("s_game_result", "s_game_result");
	isSuccess &= loadImage("s_game_dead", "dead_sample");
	isSuccess &= loadImage("wanwan", "wanwan");
	isSuccess &= loadImage("luigi", "luigi");
	isSuccess &= loadSound("dead", "dead.wav");
	isSuccess &= loadSound("game_over", "game_over.wav");
	isSuccess &= loadSound("s_game_bgm", "妄想凸守旋律～補完～.mp3");
	isSuccess &= loadSound("attack", "bomb.mp3");
	isSuccess &= loadSound("jump", "jump.mp3");

	// puzzle game
	isSuccess &= loadImage("p_explain", "puzzle_explanation");
	isSuccess &= loadImage("p_goal", "goal");
	isSuccess &= loadEffect("p_smog", "dark_smog", 13, 1, 3120, 240);
	isSuccess &= loadEffect("p_ball", "ball", 3, 10, 960, 2400);
	isSuccess &= loadEffect("p_coin", "coin", 5, 2, 1000, 400);

	return isSuccess;
}
