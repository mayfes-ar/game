#pragma once

#include "game/game.h"


class SinglePlayerGame : public Game {
	class Background : public Object {
		int& handle;

	public:
		Background(int& handle_) : handle(handle_) {
			layer = 0;
		}

		bool draw() {
			// DrawExtendGraph(CAP2IMG_SHIFT_X, CAP2IMG_SHIFT_Y, CAP2IMG_SHIFT_X + CAP2IMG_RATE*CAP_WIDTH, CAP2IMG_SHIFT_Y + CAP2IMG_RATE*CAP_HEIGHT, handle, FALSE);
			SetDrawBright(40, 40, 40);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["ar2016_logo"], true);
			SetDrawBright(255, 255, 255);
			return true;
		}
	};

	// BGM の処理
	class BGM : public Object {
		int mode;
		bool hasPlayerWon = true;
	public:
		BGM(int mode_) {
			mode = mode_;
		}
		BGM(int mode_, bool hasPlayerWon_) {
			mode = mode_;
			hasPlayerWon = hasPlayerWon_;
		}

		bool draw() {
			// 曲名でも画面のどこかに表示する？
			return true;
		}

		void start() {
			switch (mode) {
				case 0: {
					break;
				}
				case 1: {
					PlaySoundMem(soundHandles["s_game_bgm"] , DX_PLAYTYPE_LOOP, true);
					break;
				}
				case 2: {
					if (hasPlayerWon) {

					} else {
						PlaySoundMem(soundHandles["s_game_over"] , DX_PLAYTYPE_BACK, true);
					}
					break;
				}
				default: {
					break;
				}
			}
		}

		void stop() {
			switch (mode) {
				case 0: {
					break;
				}
				case 1: {
					StopSoundMem(soundHandles["s_game_bgm"]);
					break;
				}
				case 2: {
					StopSoundMem(soundHandles["s_game_over"]);
					break;
				}
				default: {
					break;
				}
			}
		}

		// プレイヤーが死んだときの効果音
		void playDeadSound() {
			PlaySoundMem(soundHandles["s_game_dead"] , DX_PLAYTYPE_NORMAL, true);
		}
	};

	// 認識したマーカーの扱い
	class Marker : public Object {
		int index = 0;

	public:
		Marker(int x_, int y_, int width_, int height_, bool willStay_) {
			rect.x = x_, rect.y = y_, rect.width = width_, rect.height = height_;
			layer = 101;
		}
		
		Marker(Rectan rect_, bool willStay_, int index_) {
			rect = rect_;
			index = index_;
			if (rect.width > 0 || rect.height > 0) {
				rect.width = 100;
				rect.height = 100;
			}
			else {
				rect.x = rect.y = -300;
			}
			layer = 101;
		}

		bool draw() {
			switch (index)
			{
			case 0: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_marker"], true);
				break;
			}
			case 1: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["p_goal"], true);
				break;
			}
			default:
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["block"], true);
				break;
			}
			return false;
		}

		double rightHit() const { return right() - rect.width / 3; }
		double leftHit() const { return left() + rect.width / 3; }
		double topHit() const { return top() + rect.height / 3; }
		double bottomHit() const { return bottom() - rect.height / 3; }
	};

	// 敵キャラクター
	class Enemy : public Object {
		double prevX;
		double prevY;
		bool isJumping = true;
		bool isAlive = true;
		bool isStopping1 = false;

		int enemyType; // 0 : 移動しない、 1 : 左右にぴょこぴょこ
		int moveDirection;//加速度 0:(0,3),1:(-3,0),2:(0,-3),3:(3,0)



		
		int damage = 0;
		int invincibleTime = 0;


	public:
		int turnCounter = 100;
		Enemy(int x_, int y_, int width_, int height_, int enemyType_){
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = width_;
			rect.height = height_;
			enemyType = enemyType_;
			if (enemyType == 4 ) {
				layer =101 ;
			}
			
			else {
				layer = 100;
			}

			switch (enemyType) {
			case 1: {
				moveDirection = 1;
				break;
			}
			case 2: {
				moveDirection = rand() % 4;
				switch (moveDirection)
				{
				case 0: {
					rect.x = prevX = WIDTH / 2;
					rect.y = prevY = 0 - 200;
					break;
				}
				case 1: {
					rect.x = prevX = WIDTH + 200;
					rect.y = prevY = HEIGHT / 2 + 50;
					break;
				}
				case 2: {
					rect.x = prevX = WIDTH / 2;
					rect.y = prevY = HEIGHT + 200;
					break;
				}
				case 3: {
					rect.x = prevX = 0 - 200;
					rect.y = prevY = HEIGHT / 2 + 50;
					break;
				}
	
				
				
				
				default:
					break;
				}
				break;
			}
			case 20: {
				prevX = rect.x + 2;
				break;
			}
			case 21: {
				prevX = rect.x + 5;
				break;
			}
			default: {
				moveDirection = 0;
				break;
			}
			}
		}

		bool draw() {
			switch (enemyType) {
			case 3: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_teresa"], true);
				break;
			}
			case 4: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["ufo"], true);
				break;
			}
			case 5: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["drop"], true);
				break;
			}
			case 6: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["washi"], true);
				break;
			}
			case 7: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["cloud"], true);
				break;
			}
			case 8: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["ray"], true);
				break;
			}
			case 11: {
				DrawExtendGraph(left(), top() - 80, right(), bottom() + 800, imgHandles["s_game_water"], true);
				break;
			}
			case 12: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_switch"], true);
				break;
			}
			case 20: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_heiho"], true);
				break;
			}
			case 21: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_fire"], true);
				break;
			}
			default: {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_wanwan"], true);
				break;
			}
			}
			return isAlive;
		}


		bool isWaterUp = true;

		bool getIsWaterUp(){
			return isWaterUp;
		}



		double rightHit() const { return right() - rect.width / 3; }
		double leftHit() const { return left() + rect.width / 3; }
		double topHit() const { return top() + rect.height / 3; }
		double bottomHit() const { return bottom() - rect.height / 3; }

		void update(const std::vector<std::shared_ptr<BlockObject>> blockList, const std::vector<std::shared_ptr<Marker>> markerList) {

			// 座標更新以外にも状態の更新を扱う？

			updateCoordinate(blockList, markerList);
		}

		// 座標の更新
		void updateCoordinate(const std::vector<std::shared_ptr<BlockObject>> blockList, const std::vector<std::shared_ptr<Marker>> markerList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			//const double diffX = x - prevX > 25 ? 15 : x - prevX < -25 ? -15 : x - prevX;
			//const double diffY = y - prevY > 25 ? 15 : y - prevY < -25 ? -15 : y - prevY;
			const double diffX = 40 * tanh((x - prevX) / 40);
			const double diffY = 40 * tanh((y - prevY) / 40);

			double acX = 0;
			double acY = 0;


			switch (enemyType) {
				case 0:{
					acX = -0.5 * (1 - (diffX <= 0) - (diffX < 0));
					acY = 4;
					break;
				}
				case 1:{
					acY = 2;
					// turnCounter : 方向転換するまでの残りカウンタ数
					// moveDirection とあわせて、方向転換の制御に用いる。
					// moveDirection は x軸方向の正とわざわざ対応付けてはいません。
					if (turnCounter <= 0 && !isJumping) {
						moveDirection *= -1;
						turnCounter = 100;
					} else {
						acX = moveDirection*0.2 * (moveDirection*diffX <= 2);
						turnCounter--;
					}
					if (!isJumping) {
						acY = -15;
					}
					isJumping = true;
					break;
				}
				case 2:{
					//moveDirection = 1;
					switch(moveDirection) {
						case 0:{
							acX = 0;
							acY = 10;
							break;
						}
						case 1:{
							acX = -10;
							acY = 0;
							break;
						}
						case 2:{
							acX = 0;
							acY = -10;
							break;
						}
						case 3:{
							acX = 10;
							acY = 0;
							break;
						}
						default:{
							break;
						}
					}
					break;
				}
				case 3:{
					acX = rand() % 5 -2;
					acY = rand() % 5 -2;
					break;
				}
				case 4: {
					acY = 0;

					if (turnCounter <= 0) {
						moveDirection *= -1;
						turnCounter = rand() % 100 + 1;
					}
					else {
						acX = moveDirection * 10 * (moveDirection * diffX <= 30);
						turnCounter--;
					}

					if (x == 200 || x == 500 || x == 600) {
						moveDirection = 0;
						isStopping1 = true;
					}
					if (x == 0) {
						x = WIDTH;
					}
					else if (x == WIDTH) {
						x = 0;
					}

					break;
				}
				case 5: {

					acX = 0;
					acY = 10;

					break;
					/*１フレームごとにｘ座標ランダムｙ座標０のしずくを出現させて自由落下させたかった*/
				}
				case 6: {
					acX = 8;
					acY = 30;
					break;
				}
				case 7: {
					acX = 5;
					acY = 0;
					break;
				}
				case 8: {
					acX = 0;
					acY = 0;
				}
				case 11: {
					if (isWaterUp == false) {
						acX = 0;
						acY = 0.005;
						break;
					}
					else {
						acX = 0;
						acY = -0.005;
						break;
					}
					break;
				}
				case 12: {
					acX = 0;
					acY = 0;
					break;
				}
				case 20: {
					acX = 0;
					acY = 0;
					if (turnCounter == 0) {
						PlaySoundMem(soundHandles["s_game_shuzo"], DX_PLAYTYPE_BACK, true);
					}
					else if(turnCounter < 0 && turnCounter > -10){
						acX = 1;
					}
					turnCounter--;
					break;
				}
				case 21: {
					acX = -1;
					acY = 0;
				}
				default:{
					break;
				}
			}


			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;

			if (enemyType == 2 || enemyType == 4) {

			} else {
				// ブロックとの当たり判定
				for (auto block : blockList) {
					if (left() < block->right() && top() < block->bottom() &&
						right() > block->left() && bottom() > block->top()) {

						if (prevY < block->bottomHit() && prevY + height > block->topHit()) {
							if (prevX >= block->rightHit()) {
								x = block->right();
							} else if (prevX + width <= block->leftHit()) {
								x = block->left() - width;
							} else {
								// TODO
							}
						} else {
							if (prevY >= block->bottomHit()) {
								y = block->bottom();
							} else if (prevY + height <= block->topHit()) {
								y = block->top() - height;
								isJumping = false;
							} else {
								// TODO
							}
						}
					}
				}
			}

			// マーカーとの当たり判定
			for (auto marker : markerList) {
				if (left() < marker->right() && top() < marker->bottom() &&
					right() > marker->left() && bottom() > marker->top()) {

					if (prevY < marker->bottomHit() && prevY + height > marker->topHit()) {
						if (prevX >= marker->rightHit()) {
							x = marker->right();
						}
						else if (prevX + width <= marker->leftHit()) {
							x = marker->left() - width;
						}
						else {
							// TODO
						}
					}
					else {
						if (prevY >= marker->bottomHit()) {
							y = marker->bottom();
						}
						else if (prevY + height <= marker->topHit()) {
							y = marker->top() - height;
							isJumping = false;
						}
						else {
							// TODO
						}
					}
				}
			}
		}

		// 死亡判定。マーカーで殴られたら死ぬ。

		bool deathDecision(const std::vector<std::shared_ptr<Marker>> markerList){

			// 生死判定

			std::map<std::string, int> field = {
				{"left", 0 - 200},
				{"right", WIDTH + 200},
				{"top", 0 - 200},
				{"bottom", HEIGHT + 200}
			};

			if (prevX < field["left"] || prevX > field["right"]) {
				isAlive = false;
			}
			if (prevY < field["top"] || prevY > field["bottom"]) {
				isAlive = false;
			}

			// この場合、ブロックに当たったら死ぬ、的な

			if (invincibleTime == 0) {
				for (auto marker : markerList) {
					if (left() < marker->right() && top() < marker->bottom() &&
						right() > marker->left() && bottom() > marker->top()) {
						if (isAlive) {
							PlaySoundMem(soundHandles["s_game_attack"], DX_PLAYTYPE_BACK, true);
						}
						damage++;
						isAlive = damageControl();
						invincibleTime = 15;

						return !isAlive;

					}
				}

			}
			else if (invincibleTime > 0) {
				invincibleTime--;
			}


			return !isAlive;
		}

		bool getIsStopping1() {
			return isStopping1;
		}

		bool getIsAlive() {
			return isAlive;
		}

		int getX() {
			return prevX;
		}
		int getY() {
			return prevY;
		}
		// ダメージが何点以上で死亡とするか。返り値は 死：false, 生：true
		bool damageControl() {
			int maxDamage;
			switch (enemyType)
			{
			case 1: {
				maxDamage = 2;
				break;
			}
			case 2: {
				maxDamage = 5;
				break;
			}
			case 3: {
				maxDamage = 2;
				break;
			}
			default:
				maxDamage = 1;
				break;
			}

			if (maxDamage < 0) {
				return true;
			}

			return damage < maxDamage;
		}
	};

	class Player : public Object {
		double prevX;
		double prevY;
		bool isJumping = true;
		bool isAlive = true;
		bool isToJump = false;
		int frameCount;
		int damage = 0;
		int invincibleTime = 0;
		std::string message;


		const int maxDamage = 6;

		int heart;

	public:
		Player(int x_, int y_, int width_, int height_) {
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = width_;
			rect.height = height_;
			layer = 100;
		}

		bool draw() {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_player"], true);
			if (isToJump) {
				DrawExtendGraph(left()-50, top()-rect.height - 50, right()+50, bottom()-rect.height, imgHandles["s_game_balloon"], true);
				int commentX = left() - 10;
				int commentY = top() - rect.height - 20;
				if (frameCount <= 60) {
					message = "I want to\njump!!";
					commentY = top() - rect.height - 40;
				}
				else if (frameCount <= 90 ) {
					message = "3";
				}
				else if (frameCount <= 120) {
					message = "2";
				}
				else if (frameCount <= 150) {
					message = "1";
				}

				DrawString(commentX, commentY, message.c_str(), GetColor(0, 0, 0));

			}
			DrawString(50, 50, std::to_string(damage).c_str(), GetColor(255, 255, 255));
			for (heart = 0; heart <= (maxDamage-damage-1); heart++) {
				DrawExtendGraph(50 + 50 * heart, 50, 100 + 50 * heart, 100, imgHandles["s_game_heart"], true);
			}

			return true;
		}

		void update(const char key[], const std::vector<std::shared_ptr<BlockObject>> blockList, const std::vector<std::shared_ptr<Marker>> markerList, const std::vector<std::shared_ptr<Enemy>> enemyList) {

			updateCoordinate(key, blockList, markerList, enemyList);

		}

		// プレイヤーキャラクターの座標更新
		void updateCoordinate(const char key[], const std::vector<std::shared_ptr<BlockObject>> blockList, const std::vector<std::shared_ptr<Marker>> markerList, const std::vector<std::shared_ptr<Enemy>> enemyList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			const double diffX = x - prevX;
			const double diffY = y - prevY;

			double acX = -0.5 * (1 - (diffX <= 0) - (diffX < 0));
			double acY = 3.2;

			if (key[KEY_INPUT_LSHIFT]) {
				isToJump = false;
				if (key[KEY_INPUT_UP] && !isJumping) {
					acY = -40;
					PlaySoundMem(soundHandles["s_game_jump"], DX_PLAYTYPE_BACK, true);
					isJumping = true;
				}				
				if (key[KEY_INPUT_RIGHT]) {
					acX = 1.5 * (diffX < 15);
				}
				if (key[KEY_INPUT_LEFT]) {
					acX = -1.5 * (diffX > -15);
				}
			}
			else if (acX == 0 && !isJumping && !isToJump) {
				if (rand() % 3 == 0) {
					frameCount = 0;
					isToJump = true;
				}
				else {
					acX = rand() % 30 - 15;
				}

			}

			if (isToJump) {
				if (frameCount >= 150) {
					acY = -40;
					PlaySoundMem(soundHandles["s_game_jump"], DX_PLAYTYPE_BACK, true);
					isJumping = true;
				}
				else {
					frameCount++;
							}
			}

			if (isJumping) {
				isToJump = false;
			}

			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;

			// ブロックとの当たり判定
			for (auto block : blockList) {
				if (left() < block->right() && top() < block->bottom() &&
					right() > block->left() && bottom() > block->top()) {

					if (prevY < block->bottomHit() && prevY + height > block->topHit()) {
						if (prevX >= block->rightHit()) {
							x = block->right();
						} else if (prevX + width <= block->leftHit()) {
							x = block->left() - width;
						} else {
							// TODO
						}
					} else {
						if (prevY >= block->bottomHit()) {
							y = block->bottom();
						} else if (prevY + height <= block->topHit()) {
							y = block->top() - height;
							isJumping = false;
						} else {
							// TODO
						}
					}
				}
			}
			// マーカーとの当たり判定
			/*for (auto marker : markerList) {
				if (left() < marker->right() && top() < marker->bottom() &&
					right() > marker->left() && bottom() > marker->top()) {

					if (prevY < marker->bottomHit() && prevY + height > marker->topHit()) {
						if (prevX >= marker->rightHit()) {
							x = marker->right();
						} else if (prevX + width <= marker->leftHit()) {
							x = marker->left() - width;
						} else {
							// TODO
						}
					} else {
						if (prevY >= marker->bottomHit()) {
							y = marker->bottom();
						} else if (prevY + height <= marker->topHit()) {
							y = marker->top() - height;
							isJumping = false;
						} else {
							// TODO
						}
					}
				}
			}*/
			// 敵との当たり判定
			// for (auto enemy : enemyList) {
			// 	if (left() < enemy->right() && top() < enemy->bottom() &&
			// 		right() > enemy->left() && bottom() > enemy->top()) {
			//
			// 		if (prevY < enemy->bottomHit() && prevY + height > enemy->topHit()) {
			// 			if (prevX >= enemy->rightHit()) {
			// 				x = enemy->right();
			// 			} else if (prevX + width <= enemy->leftHit()) {
			// 				x = enemy->left() - width;
			// 			} else {
			// 				// TODO
			// 			}
			// 		} else {
			// 			if (prevY >= enemy->bottomHit()) {
			// 				y = enemy->bottom();
			// 			} else if (prevY + height <= enemy->topHit()) {
			// 				y = enemy->top() - height;
			// 				isJumping = false;
			// 			} else {
			// 				// TODO
			// 			}
			// 		}
			// 	}
			// }
		}

		// プレイヤーキャラクターの生死判定更新
		bool deathDecision(std::vector<std::shared_ptr<Enemy>> enemyList) {

			std::map<std::string, int> field = {
				{"left", 0 - 200},
				{"right", WIDTH + 200},
				{"top", 0 - 200},
				{"bottom", HEIGHT + 200}
			};

			if (prevX < field["left"] || prevX > field["right"]) {
				isAlive = false;
			}
			if (prevY < field["top"] || prevY > field["bottom"]) {
				isAlive = false;
			}

			if (invincibleTime == 0) {
				for (auto enemy : enemyList) {
					if (left() <= enemy->right() && top() <= enemy->bottom() &&
						right() >= enemy->left() && bottom() >= enemy->top()) {
						damage += 1;
						invincibleTime = 50;
						break;
					}
				}
			} else if (invincibleTime > 0){
				invincibleTime--;
			}

			if (damage > (maxDamage-1)) {
				isAlive = false;
			}

			return !isAlive;
		}
	};
	std::thread thread;
	std::shared_ptr<Player> player;

	std::vector<std::shared_ptr<BlockObject>> blockList;
	std::vector<std::shared_ptr<Enemy>> enemyList;
	std::shared_ptr<Enemy> heihos[10] = {NULL};


	// 敵作成。enemyType については Enemy クラスを参照
	std::shared_ptr<Enemy> makeEnemy(int x, int y, int width, int height, int enemyType) {
		auto enemy = std::make_shared<Enemy>(x, y, width, height, enemyType);
		if (player->isContacted(enemy)) {
			enemy = std::make_shared<Enemy>(x, y-100, width, height, enemyType);
		}
		enemyList.push_back(enemy);
		drawList.push_back(enemy);
		return enemy;
	}

	

	std::shared_ptr<BGM> bgm;
	const int MAX_TIME = 30*40;
	int timer = MAX_TIME;
	bool hasPlayerWon;

public:
	SinglePlayerGame() {
		thread = std::thread::thread(capture, std::ref(share));
		player = std::make_shared<Player>(WIDTH/2 - 100/2, HEIGHT/2-150/2, 75, 100);
		hasPlayerWon = true;
	}

	bool onStart() {
		using namespace std;
		fps.isShow = true;

		srand((unsigned int)time(NULL));

		// mode 0
		mode.setMode([this]() {
			class Title : public Object {
			public:
				Title() {
					layer = 50;
				}

				bool draw() {
					DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_op"], true);
					DrawExtendGraph(WIDTH/2-429/2, 30, WIDTH/2+429/2, 30+47, imgHandles["s_game_op_title"], true);
					// DrawExtendGraph(WIDTH/2-50, 400, WIDTH/2+50, 400+150, imgHandles["s_game_player"], true);
					return true;
				}
			};

			drawList.clear();
			drawList.push_back(make_shared<Title>());

			bgm = make_shared<BGM>(0);
			bgm->start();

			for(int i = 0; i < 20; i++);
		}, 150);

		// mode 1
		mode.setMode([this]() {
			drawList.clear();
			auto makeBlock = [this](int x, int y, int width, int height) {
				auto block = make_shared<BlockObject>(x, y, width, height, true);
				blockList.push_back(block);
				drawList.push_back(block);
			};
			makeBlock(0-200, 600, WIDTH+200+200, 50);



			drawList.push_back(player);
			drawList.push_back(make_shared<Background>(share.handle));

			bgm = make_shared<BGM>(1);
			bgm->start();

		}, MAX_TIME);

		// mode 2
		mode.setMode([this]() {
			class Title : public Object {
				bool hasPlayerWon = true;
			public:
				Title(bool hasPlayerWon_) {
					layer = 50;
					hasPlayerWon = hasPlayerWon_;
				}

				bool draw() {
					if (hasPlayerWon) {
						DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_clear"], true);
					} else {
						DrawExtendGraph(WIDTH/2-400, 30, WIDTH/2+400, 30+296, imgHandles["s_game_result_dead"], true);
						DrawExtendGraph(WIDTH/2-75, 400, WIDTH/2+75, 400+150, imgHandles["s_game_dead"], true);
					}
					return true;
				}
			};

			drawList.clear();
			drawList.push_back(make_shared<Title>(hasPlayerWon));

			bgm->stop();
			bgm = make_shared<BGM>(2, hasPlayerWon);
			bgm->start();
		}, -1);

		return Game::onStart();
	}

	bool onUpdate() {
		bool willFinishMode = false;
		static std::shared_ptr <Enemy> water;
		static std::shared_ptr <Enemy> button;

		switch (mode.getMode())	{
			case 0:{ // イントロダクション
				static int counterForWaiting = 10;
				if (key[KEY_INPUT_RETURN] && counterForWaiting == 0) {
					willFinishMode = true;
				}
				if (counterForWaiting > 0) {
					counterForWaiting--;
				}
				break;
			}
			case 1: { // playing
				timer -= 1;
				if (timer <= 0) {
					willFinishMode = true;
				}

				std::vector<std::shared_ptr<Marker>> markerList;

				// 認識したマーカーを描画
				share.rectMutex.lock();
				for (int i = 0; i < share.rects.size(); i++) {
					auto marker = std::make_shared<Marker>(share.rects[i], false, i);
					markerList.push_back(marker);
					drawList.push_back(marker);
				}
				share.rectMutex.unlock();

				player->update(key, blockList, markerList, enemyList);


				for (auto enemy : enemyList) {
					enemy->deathDecision(markerList);
					enemy->update(blockList, markerList);
				}


				if(player->deathDecision(enemyList)){
					bgm->stop();
					bgm->playDeadSound();
					hasPlayerWon = false;
					willFinishMode = true;
				}

				// 敵の出現を管理する
				static std::shared_ptr<Enemy> ufo;
				static std::shared_ptr<Enemy> cloud;
				static std::shared_ptr<Enemy> washi;

				switch(MAX_TIME-timer) {					
					case 1: {

						water = makeEnemy(0, HEIGHT + 80, 1280, 800, 11);
						button = makeEnemy(WIDTH-100, 100, 40, 40, 12);

						makeEnemy(350, 200, 435/5, 349/5, 1);

						break;
						}
					case 1000:
					case 150: {
						//makeEnemy(WIDTH, HEIGHT / 2, 435 / 2, 349 / 2, 2);
						makeEnemy(200, 0, 435 / 3, 349 / 3, 1);
						makeEnemy(900, 0, 435/3, 349/3, 1);
						//makeEnemy(WIDTH/2, 200, 435 / 5, 349 / 5, 1);
						ufo = makeEnemy(WIDTH / 2, 10, 225 / 2, 225 / 2, 4); //ufo
						break;
						}
					case 180: {
						makeEnemy(rand() % (WIDTH / 2), 0, 430 / 4, 263 / 4, 6); //washi
						break;
					}
					case 200: {
						cloud = makeEnemy(0, 0, 304 / 3, 166 / 3, 7); //cloud
						break;

					}
					case 300: {
						makeEnemy(WIDTH, HEIGHT/2, 435/2, 349/2, 2);
						makeEnemy(WIDTH / 2, 200, 435 / 5, 349 / 5, 1);

						break;
					}
					case 500:
					case 700:
					case 900:
					case 1100: {
						makeEnemy(WIDTH, HEIGHT / 2, 435 / 2, 349 / 2, 2);
						break;
					}

					default: {
						//int i = rand() % 10;
						int i = 0;
						if (heihos[i] == NULL || heihos[i]->getIsAlive() == false) {
							if (rand() % 30 == 0) {
								heihos[i] = makeEnemy(WIDTH-100, HEIGHT - 210, 112 * 4 / 5, 112 * 4 / 5, 20);
							}
						}
						else if (heihos[i]->turnCounter == 0) {
							makeEnemy(heihos[i]->getX(), heihos[i]->getY(), 112 * 4 / 5, 112 * 4 / 5, 21);
						}
						break;
					}
							
				}
				if (button->getIsAlive() == false) {
					water->isWaterUp = false;
				}

				// 定期的に実行する場合など
				if (MAX_TIME - timer > 400 && (MAX_TIME - timer) % 10 == 0) {
					makeEnemy(rand() % (WIDTH - 200) + 100, rand() % (HEIGHT - 100), 112 * 4 / 5, 112 * 4 / 5, 3);
				}
				else if ((MAX_TIME - timer) % 30 == 0) {
					makeEnemy(rand() % (WIDTH - 200) + 100, rand() % (HEIGHT - 100), 112 * 4 / 5, 112 * 4 / 5, 3);
				}

				if (MAX_TIME - timer >= 150 && ufo->getIsStopping1() == true) {
					makeEnemy(ufo->left(), 100, 353 / 3, 1265 / 2, 8);
				}
				if (MAX_TIME - timer >= 200) {
					makeEnemy((cloud->left()) + rand() % (int)((cloud->right()) - (cloud->left())), 50, 200 / 8, 200 / 8, 5);
				}

				break;
				
			}
					

			case 2:{ // リザルト画面
				if (key[KEY_INPUT_RETURN]) {
					willFinishMode = true;
				}
				break;
			}

			default: 

				break;
			}


		if (key[KEY_INPUT_ESCAPE]) {
			share.willFinish = true;
		}
		if (key[KEY_INPUT_N]) {
			willFinishMode = true;
		}


		for ( auto& itr = enemyList.begin(); itr != enemyList.end();) {
			if ((*itr)->getIsAlive()) {
				++itr;
			} else {
				itr = enemyList.erase(itr);
			}
		}

		if (willFinishMode) {

			mode.goNext();
		}

		return Game::onUpdate();
	}

	bool onFinish() {
		bgm->stop();

		thread.join();
		return true;
	}
	
};