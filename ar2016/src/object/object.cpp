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
	isSuccess &= loadImage("monhan", "menu/monhan");
	isSuccess &= loadImage("rockman", "menu/rockman");
	isSuccess &= loadImage("ar2016_logo", "menu/wallpaper");
	isSuccess &= loadImage("menu_title", "menu/menu_title");
	isSuccess &= loadSound("menu_bgm", "menu/welcome_to_the_Chu-2_byo_world!.mp3");
	isSuccess &= loadEffect("effect1", "warp", 1, 6, 640, 2880);

	// Single Player Game
	// オープニング
	isSuccess &= loadImage("s_game_op_title", "s_game/s_game_op_title");
	isSuccess &= loadImage("s_game_op", "s_game/s_game_op");

	//　ゲーム本編
	isSuccess &= loadImage("s_game_player", "s_game/hime_normal");
	isSuccess &= loadImage("s_game_player_damage", "s_game/hime_odoroki");

	isSuccess &= loadImage("s_game_wanwan", "s_game/wolf");
	isSuccess &= loadImage("s_game_marker", "s_game/luigi");
	isSuccess &= loadImage("s_game_teresa", "s_game/teresa");


	isSuccess &= loadSound("s_game_bgm", "s_game/bgm.mp3");

	isSuccess &= loadImage("s_game_water", "s_game/water");
	isSuccess &= loadImage("s_game_switch", "s_game/switch");

	isSuccess &= loadSound("s_game_dead", "s_game/dead.wav");
	isSuccess &= loadSound("s_game_attack", "s_game/bomb.mp3");
	isSuccess &= loadSound("s_game_jump", "s_game/jump.mp3");

	isSuccess &= loadEffect("s_game_coin", "s_game/coin", 5, 2, 1000, 400);

	isSuccess &= loadImage("s_game_piyo", "s_game/piyo");
	isSuccess &= loadImage("s_game_sword", "s_game/sword");
	isSuccess &= loadImage("s_game_shield", "s_game/shield");



	// リザルト
	isSuccess &= loadImage("s_game_result_clear", "s_game/s_game_clear");
	isSuccess &= loadImage("s_game_result_dead", "s_game/s_game_result");
	isSuccess &= loadImage("s_game_dead", "s_game/dead_sample");

	isSuccess &= loadSound("s_game_over", "s_game/game_over.wav");

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
