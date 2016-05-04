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

	//メニューで使う
	isSuccess &= loadImage("monhan", "monhan");
	isSuccess &= loadImage("rockman", "rockman");
	isSuccess &= loadImage("ar2016_logo", "wallpaper");
	isSuccess &= loadImage("menu_title", "menu_title");
	isSuccess &= loadSound("menu_bgm", "welcome_to_the_Chu-2_byo_world!.mp3");
	isSuccess &= loadEffect("effect1", "warp", 1, 6, 640, 2880);

	// Single Player Game
	// オープニング
	isSuccess &= loadImage("s_game_op_title", "s_game_op_title");
	isSuccess &= loadImage("s_game_op", "s_game_op");

	//　ゲーム本編
	isSuccess &= loadImage("s_game_player", "mario");
	isSuccess &= loadImage("s_game_wanwan", "wanwan");
	isSuccess &= loadImage("s_game_marker", "luigi");
	isSuccess &= loadImage("s_game_teresa", "teresa");
	isSuccess &= loadImage("s_game_heiho", "heiho");
	isSuccess &= loadImage("s_game_fire", "fire");
	isSuccess &= loadImage("s_game_balloon", "balloon");
	isSuccess &= loadImage("ufo", "ufo");
	isSuccess &= loadImage("drop", "drop");
	isSuccess &= loadImage("ray", "uforay");
	isSuccess &= loadImage("cloud", "cloud");
	isSuccess &= loadImage("washi", "wasi");
	isSuccess &= loadImage("s_game_water", "water");
	isSuccess &= loadImage("s_game_switch", "switch");


	isSuccess &= loadSound("s_game_bgm", "妄想凸守旋律～補完～.mp3");
	isSuccess &= loadSound("s_game_dead", "dead.wav");
	isSuccess &= loadSound("s_game_attack", "bomb.mp3");
	isSuccess &= loadSound("s_game_jump", "jump.mp3");
	isSuccess &= loadSound("s_game_shuzo", "shuzo.mp3");


	// リザルト
	isSuccess &= loadImage("s_game_result_clear", "s_game_clear");
	isSuccess &= loadImage("s_game_result_dead", "s_game_result");
	isSuccess &= loadImage("s_game_dead", "dead_sample");

	isSuccess &= loadSound("s_game_over", "game_over.wav");

	// isSuccess &= loadImage("s_game_bg", "wallpaper_alpha");

	// puzzle game
	isSuccess &= loadImage("p_man", "man");
	isSuccess &= loadImage("p_girl",  "girl");
	isSuccess &= loadImage("p_explain", "puzzle_explanation");
	isSuccess &= loadImage("p_goal", "goal");
	isSuccess &= loadImage("p_on", "switch_on");
	isSuccess &= loadImage("p_off", "switch_off");
	isSuccess &= loadEffect("p_smog", "dark_smog", 13, 1, 3120, 240);
	isSuccess &= loadEffect("p_ball", "ball", 3, 10, 960, 2400);
	isSuccess &= loadEffect("p_coin", "coin", 5, 2, 1000, 400);


	return isSuccess;
}
