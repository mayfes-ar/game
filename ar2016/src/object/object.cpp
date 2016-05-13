#include "object/object.h"
#include <sstream>

using namespace std;

map<string, int> Object::imgHandles;
map<string, int> Object::movieHandles;
map<string, vector<int>> Object::effectHandles;
map<string, int> Object::soundHandles;
//std::map<std::string, int> Object::movieHandles;

// 一番最初に一回だけ呼ぶ
bool Object::load() {

	auto loadImage = [](string key, string name, string extension="png") -> bool {
		const string path = "img/" + name + "." + extension;
		const int handle = LoadGraph(path.c_str());
		if (handle == -1) {
			return false;
		} else {
			Object::imgHandles[key] = handle;
			return true;
		}
	};

	auto loadMovie = [](string key, string name, string extension="mpg") -> bool {
		const string path = "movie/" + name + "." + extension;
		const int handle = LoadGraph(path.c_str());
		if (handle == -1) {
			return false;
		} else {
			Object::movieHandles[key] = handle;
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
	isSuccess &= loadImage("s_game_ufo_over", "s_game/ufo");


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
	isSuccess &= loadEffect("s_game_fireshot", "s_game/ghorst_", 10, 1, 2400, 240);


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
	isSuccess &= loadImage("p_needle", "puzzle/needle");
	isSuccess &= loadImage("p_hime", "puzzle/hime_normal");
	isSuccess &= loadImage("p_hime_damaged", "puzzle/hime_gameover");
	isSuccess &= loadImage("p_explain1", "puzzle/explanation1");
	isSuccess &= loadImage("p_explain2", "puzzle/explanation2");
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
	isSuccess &= loadEffect("p_arrow", "puzzle/arrow",10,1,1200,120);
	isSuccess &= loadEffect("p_crystal1", "puzzle/crystal1", 5, 4, 600, 480);
	isSuccess &= loadEffect("p_crystal2", "puzzle/crystal2", 5, 4, 600, 480);
	isSuccess &= loadEffect("p_warp", "puzzle/warp", 2, 8, 1600, 4800);
	isSuccess &= loadEffect("p_num", "puzzle/number", 10, 1, 390, 54);
	isSuccess &= loadEffect("p_num_2", "puzzle/number_2", 10, 1, 1840, 223);
	isSuccess &= loadEffect("p_init", "puzzle/init", 14, 1, 1680, 120);
	isSuccess &= loadEffect("p_push_switch", "puzzle/snow", 3,10, 1920, 4800);
	isSuccess &= loadSound("p_bgm1", "puzzle/otenbahime.mp3");
	isSuccess &= loadSound("p_bgm2", "puzzle/hajimetenootsukai.mp3");
	isSuccess &= loadSound("p_bgm3", "puzzle/kibou.mp3");

	isSuccess &= loadImage("p_score", "puzzle/score");
	isSuccess &= loadImage("p_perfect", "puzzle/perfect");
	isSuccess &= loadMovie("p_flower", "puzzle/flower");

	// breakout game

	isSuccess &= loadImage("b_ship_block", "shipBlock");
	isSuccess &= loadImage("b_pot", "pot");
	isSuccess &= loadImage("b_restore_ship", "restore_ship");
	isSuccess &= loadImage("b_damage_ship", "damage_ship");
	isSuccess &= loadImage("b_enhance_ship", "breakout/star");
	isSuccess &= loadImage("b_restore_time", "breakout/clock");

	isSuccess &= loadImage("b_donald", "breakout/donald");
	isSuccess &= loadImage("b_right_hand", "breakout/right_hand");
	isSuccess &= loadImage("b_left_hand", "breakout/left_hand");
	isSuccess &= loadImage("b_snake_weak", "breakout/tokage");

	isSuccess &= loadImage("b_block_blue", "block_blue");
	isSuccess &= loadImage("b_block_red", "block_red");
	isSuccess &= loadImage("b_block_green", "block_green");

	isSuccess &= loadImage("b_hard_block", "breakout/hard_block");
	isSuccess &= loadImage("b_hatena_block", "breakout/hatena_block");
	isSuccess &= loadImage("b_unbreakable_block", "breakout/unbreakable_block");

	isSuccess &= loadImage("b_forest", "breakout/forest");
	isSuccess &= loadImage("b_magma", "breakout/magma");
	isSuccess &= loadImage("b_hill", "breakout/hill");
	isSuccess &= loadImage("b_grass", "breakout/grass");

	isSuccess &= loadImage("b_game_over", "breakout/game_over");
	isSuccess &= loadImage("b_game_clear", "breakout/game_clear");
	isSuccess &= loadImage("b_explanation", "breakout/explanation");

	isSuccess &= loadImage("b_easy", "breakout/easy");
	isSuccess &= loadImage("b_normal", "breakout/normal");
	isSuccess &= loadImage("b_hard", "breakout/hard");

	isSuccess &= loadImage("b_house01", "breakout/house01");
	isSuccess &= loadImage("b_hime", "puzzle/hime_normal");
	isSuccess &= loadImage("b_hime_damaged", "puzzle/hime_gameover");
	isSuccess &= loadImage("b_hime_odoroki", "s_game/hime_odoroki");

	isSuccess &= loadImage("b_castle", "breakout/castle");
	isSuccess &= loadImage("b_girl", "breakout/girl");
	isSuccess &= loadImage("b_boy", "breakout/boy");
	isSuccess &= loadImage("b_girl_damaged", "breakout/girl_damaged");
	isSuccess &= loadImage("b_boy_damaged", "breakout/boy_damaged");

	isSuccess &= loadImage("b_normal_fukidashi", "breakout/normal_fukidashi");
	isSuccess &= loadImage("b_bikkuri_fukidashi", "breakout/bikkuri_fukidashi");

	//info
	isSuccess &= loadImage("b_sunadokei", "breakout/sunadokei");

	//timer
	isSuccess &= loadImage("b_timer_start_str", "breakout/text_start");

	//effect

	isSuccess &= loadEffect("b_muteki", "breakout/muteki", 1, 6, 320, 1440);
	isSuccess &= loadEffect("b_burning", "breakout/burning", 1, 5, 240, 900);
	isSuccess &= loadEffect("b_explosion", "breakout/explosion", 7, 1, 840, 120);
	isSuccess &= loadEffect("b_fireball", "breakout/fireball", 5, 5, 500, 500);
	isSuccess &= loadEffect("b_fireball_reflect", "breakout/fireball_reflect", 14, 1, 1680, 120);
	isSuccess &= loadEffect("b_green_fireball", "breakout/green_fireball", 5, 5, 500, 500);
	isSuccess &= loadEffect("b_green_fireball_reflect", "breakout/green_fireball_reflect", 14, 1, 1680, 120);
	isSuccess &= loadEffect("b_strong_fireball_reflect", "breakout/strong_fireball_reflect", 14, 1, 3360, 240);
	isSuccess &= loadEffect("b_crawl", "breakout/crawl", 5, 2, 1200, 480);
	isSuccess &= loadEffect("b_enemy_vanish", "breakout/enemy_vanish", 5, 1, 1280, 256);
	isSuccess &= loadEffect("b_enemy_strong_fireball", "breakout/enemy_strong_fireball", 9, 3, 1800, 600);
	isSuccess &= loadEffect("b_player_strong_fireball", "breakout/player_strong_fireball", 10, 3, 2000, 600);
	isSuccess &= loadEffect("b_fire_frame", "breakout/fire_frame", 2, 5, 1280, 2400);
	isSuccess &= loadEffect("b_pot_light", "breakout/gra_effect_light", 7, 3, 1680, 720);


	isSuccess &= loadSound("b_bomb", "breakout/bomb.mp3");
	isSuccess &= loadSound("b_meramera", "breakout/meramera.mp3");
	isSuccess &= loadSound("b_heal", "breakout/heal.mp3");
	isSuccess &= loadSound("b_boss_damaged", "breakout/boss_damaged.wav");
	isSuccess &= loadSound("b_enemy_strong_fireball", "breakout/enemy_strong_fireball.mp3");
	isSuccess &= loadSound("b_fireball_reflect", "breakout/fireball_reflect.wav");
	isSuccess &= loadSound("b_muteki", "breakout/muteki.mp3");
	isSuccess &= loadSound("b_restore_pot", "breakout/restore_pot.wav");
	isSuccess &= loadSound("b_restore_time", "breakout/restore_time.wav");
	isSuccess &= loadSound("b_start_play", "breakout/start.mp3");
	isSuccess &= loadSound("b_final_play", "breakout/final.mp3");
	isSuccess &= loadSound("b_pot_exhare", "breakout/pot_exhare.mp3");
	isSuccess &= loadSound("b_timer_start", "breakout/timer_start.mp3");
	isSuccess &= loadSound("b_timer_count", "breakout/timer_count.mp3");

	for (int num = 0; num <= 9; ++num) {
		for (const auto& color : { "red", "blue", "yellow", "green" }) {
			std::ostringstream key;
			std::ostringstream name;
			key << color << "_" << num;
			name << "breakout/number/" << color << "/" << num;
			isSuccess &= loadImage(key.str(), name.str());
		}
	}

	//isSuccess &= loadMovie("b_mode_select", "breakout/mode_select.mp4");
	return isSuccess;
}
