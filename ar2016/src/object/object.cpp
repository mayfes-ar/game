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

	auto loadMovie = [](string key, string name) -> bool {
		const string path = "movie/" + name + ".mpg";
		const int handle = LoadGraph(path.c_str());
		if (handle == -1) {
			return false;
		} else {
			Object::movieHandles[key] = handle;
			return true;
		}
	};
	
	// 使い方 isSuccess &= loadなんとか(登録するキー, ファイル名, ...);
	bool isSuccess = true;

	isSuccess &= loadImage("player", "mario");

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
	isSuccess &= loadImage("background", "s_game/background1");

	isSuccess &= loadImage("s_game_player", "s_game/hime_normal");
	isSuccess &= loadImage("s_game_player_damage", "s_game/hime_odoroki");
	isSuccess &= loadImage("s_game_player_over", "s_game/hime_gameover");
	isSuccess &= loadImage("s_game_player_drowned", "s_game/hime_obore");

	isSuccess &= loadImage("s_game_wanwan", "s_game/wolf");
	isSuccess &= loadImage("s_game_wanwan_damage", "s_game/wolf");
	isSuccess &= loadImage("s_game_wanwan_over", "s_game/wolf");

	isSuccess &= loadImage("s_game_block", "s_game/block1");

	isSuccess &= loadImage("s_game_bat", "s_game/bat");
	isSuccess &= loadImage("s_game_bat_damage", "s_game/bat");
	isSuccess &= loadImage("s_game_bat_over", "s_game/bat");
	isSuccess &= loadEffect("s_game_koumori","s_game/koumori", 3, 1, 95, 22);
	//isSuccess &= loadEffect("s_game_teresa_damage", "s_game/koumori", 1, 3, 96, 22);
	//isSuccess &= loadEffect("s_game_teresa_over", "s_game/koumori", 1, 3, 96, 22);


	isSuccess &= loadImage("s_game_ghorst", "s_game/ghorst");
	isSuccess &= loadImage("s_game_ghorst_damage", "s_game/ghorst");
	isSuccess &= loadImage("s_game_ghorst_over", "s_game/ghorst");

	isSuccess &= loadImage("s_game_fire", "s_game/fire2");
	isSuccess &= loadImage("s_game_fire_damage", "s_game/fire2");
	isSuccess &= loadImage("s_game_fire_over", "s_game/fire2");

	isSuccess &= loadImage("s_game_balloon", "s_game/balloon");

	isSuccess &= loadImage("s_game_drop", "s_game/drop");
	isSuccess &= loadImage("s_game_drop_damage", "s_game/drop");
	isSuccess &= loadImage("s_game_drop_over", "s_game/drop");

	isSuccess &= loadImage("s_game_ufo", "s_game/ufo");
	isSuccess &= loadImage("s_game_ufo_damage", "s_game/ufo");
	isSuccess &= loadImage("s_game_ufo_over", "s_game/drop");


	isSuccess &= loadImage("s_game_ray", "s_game/uforay");
	isSuccess &= loadImage("s_game_ray_damage", "s_game/uforay");
	isSuccess &= loadImage("s_game_ray_over", "s_game/uforay");

	isSuccess &= loadImage("s_game_cloud", "s_game/cloud");
	isSuccess &= loadImage("s_game_cloud_damage", "s_game/cloud");
	isSuccess &= loadImage("s_game_cloud_over", "s_game/cloud");

	isSuccess &= loadImage("s_game_eagle", "s_game/eagle");
	isSuccess &= loadImage("s_game_eagle_damage", "s_game/eagle");
	isSuccess &= loadImage("s_game_eagle_over", "s_game/eagle");

	isSuccess &= loadImage("s_game_water", "s_game/water");
	isSuccess &= loadImage("s_game_water_damage", "s_game/water");
	isSuccess &= loadImage("s_game_water_over", "s_game/water");

	isSuccess &= loadImage("s_game_switch", "s_game/switch1");
	isSuccess &= loadImage("s_game_switch_damage", "s_game/switch");
	isSuccess &= loadImage("s_game_switch_over", "s_game/switch");

	isSuccess &= loadImage("s_game_heart", "s_game/heart");

	isSuccess &= loadSound("s_game_bgm", "s_game/bgm.mp3");
	isSuccess &= loadSound("s_game_dead", "s_game/dead.wav");
	isSuccess &= loadSound("s_game_attack", "s_game/bomb.mp3");
	isSuccess &= loadSound("s_game_jump", "s_game/jump.mp3");
	isSuccess &= loadSound("s_game_shuzo", "s_game/shuzo.mp3"); 
	isSuccess &= loadSound("s_game_fireshot", "s_game/fireshot_kurage.mp3");

	isSuccess &= loadEffect("s_game_hit", "s_game/hit", 5, 2, 1200, 400);
	isSuccess &= loadEffect("s_game_enemy_over", "s_game/enemy", 10, 1, 1200, 200);
	isSuccess &= loadEffect("s_game_sword", "s_game/sword", 9, 1, 1080, 120);


	isSuccess &= loadEffect("s_game_coin", "s_game/coin", 5, 2, 1000, 400);
	// マーカー
	isSuccess &= loadImage("s_game_piyo", "s_game/piyo");
	isSuccess &= loadImage("s_game_sword", "s_game/sword");
	isSuccess &= loadImage("s_game_shield", "s_game/shield");
	isSuccess &= loadImage("s_game_invalid", "s_game/dead_sample");
	// リザルト
	isSuccess &= loadImage("s_game_result_clear", "s_game/s_game_clear");
	isSuccess &= loadImage("s_game_result_dead", "s_game/s_game_result");
	isSuccess &= loadImage("s_game_dead", "s_game/dead_sample");
	isSuccess &= loadSound("s_game_over", "s_game/game_over.wav");
	isSuccess &= loadEffect("s_game_over_hanabi", "s_game/hanabi", 14, 15, 7168, 7680);
	//isSuccess &= loadMovie("s_game_over_hanabi", "s_game/hanabi");

	// puzzle game
	isSuccess &= loadImage("p_block", "puzzle/block");
	isSuccess &= loadImage("p_block_x", "puzzle/block_x");
	isSuccess &= loadImage("s_block", "Brick02");
	isSuccess &= loadImage("p_man", "man");
	isSuccess &= loadImage("p_needle", "puzzle/needle");
	isSuccess &= loadImage("p_hime", "puzzle/hime_normal");
	isSuccess &= loadImage("p_hime_damaged", "puzzle/hime_gameover");
	isSuccess &= loadImage("p_explain1", "puzzle/explanation1");
	isSuccess &= loadImage("p_explain2", "puzzle/explanation2");
	isSuccess &= loadImage("p_goal", "goal");
	isSuccess &= loadImage("p_on", "switch_on");
	isSuccess &= loadImage("p_off", "switch_off");

	isSuccess &= loadImage("p_arrow", "arrow");
	isSuccess &= loadImage("p_castle", "puzzle/castle");
	isSuccess &= loadImage("p_saihu", "puzzle/saihu_kari");
	isSuccess &= loadImage("p_timer", "puzzle/clock_kari");
	isSuccess &= loadImage("s_brick1", "puzzle/brick1");
	isSuccess &= loadEffect("p_smog", "puzzle/smog", 5, 6, 640, 768);
	isSuccess &= loadEffect("p_ball", "puzzle/ball", 3, 10, 960, 2400);
	isSuccess &= loadEffect("p_coin", "puzzle/coin", 5, 2, 1000, 400);
	isSuccess &= loadEffect("p_coins", "puzzle/coins", 5, 6, 1200, 1440);
	isSuccess &= loadEffect("p_curtain_close", "puzzle/curtain_close", 1, 18, 640, 8640);
	isSuccess &= loadEffect("p_curtain_open", "puzzle/curtain_open", 1, 10, 640, 4800);
	isSuccess &= loadEffect("p_arrow1", "arrow1",10,1,1200,120);
	isSuccess &= loadEffect("p_crystal1", "puzzle/crystal1", 5, 4, 600, 480);
	isSuccess &= loadEffect("p_crystal2", "puzzle/crystal2", 5, 4, 600, 480);
	isSuccess &= loadEffect("p_warp", "puzzle/warp", 2, 8, 1600, 4800);
	isSuccess &= loadEffect("p_num", "puzzle/number", 10, 1, 390, 54);
	isSuccess &= loadEffect("p_init", "puzzle/init", 14, 1, 1680, 120);

	// breakout game
	isSuccess &= loadImage("ship_block", "shipBlock");
	isSuccess &= loadImage("pot", "pot");
	isSuccess &= loadImage("restore_ship", "restore_ship");
	isSuccess &= loadImage("damage_ship", "damage_ship");
	isSuccess &= loadImage("block_blue", "block_blue");
	isSuccess &= loadImage("block_red", "block_red");
	isSuccess &= loadImage("block_green", "block_green");
	isSuccess &= loadImage("b_forest", "breakout/forest");
	isSuccess &= loadImage("b_magma", "breakout/magma");
	isSuccess &= loadImage("b_game_over", "breakout/game_over");
	isSuccess &= loadImage("b_game_clear", "breakout/game_clear");
	isSuccess &= loadImage("b_explanation", "breakout/explanation");

	return isSuccess;
}
