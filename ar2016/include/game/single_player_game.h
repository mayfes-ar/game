#pragma once

#include "game/game.h"

class SinglePlayerGame : public Game {
	/* Layer の基準
		Player : 100
		Marker : 200
		Enemy 
			RocketWanWan : 165
			Teresa : 160
			Switch : 130
			Inundation : 300
			Cloud : 140
			Drop : 139
			Ufo :  150
			Ray :  180 TODO: 半透明
			Heiho :	152
			Fire : 151
		Effect : 周りの描画に影響しそうな大きなエフェクト
			??? :
		Block : 50
		背景 : 0
				
	*/

	// 指定フレーム後に、登録した関数を実行してくれる
	class FuncTimer {
		std::vector<std::function<void()>> funcs;
		std::vector<int> timers;
	public:
		void update() {
			auto funcsItr = funcs.begin();
			for (auto timersItr = timers.begin(); timersItr != timers.end();) {
				(*timersItr)--;
				if ((*timersItr) > 0) {
					funcsItr++;
					timersItr++;
				}
				else {
					(*funcsItr)();
					funcsItr = funcs.erase(funcsItr);
					timersItr = timers.erase(timersItr);
				}

			}
		}
		void set(std::function<void()> func, int timer) {
			funcs.push_back(func);
			timers.push_back(timer);
		}
		void clear() {
			funcs.clear();
			timers.clear();
		}
	} funcTimer;

	//背景
	class Background : public Object {
		int& handle;

	public:
		Background(int& handle_) : handle(handle_) {
			layer = 0;
		}

		bool draw() {
			// DrawExtendGraph(CAP2IMG_SHIFT_X, CAP2IMG_SHIFT_Y, CAP2IMG_SHIFT_X + CAP2IMG_RATE*CAP_WIDTH, CAP2IMG_SHIFT_Y + CAP2IMG_RATE*CAP_HEIGHT, handle, FALSE);
			SetDrawBright(230, 230, 230);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["background"], true);
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
				PlaySoundMem(soundHandles["s_game_bgm"], DX_PLAYTYPE_LOOP, true);
				break;
			}
			case 2: {
				if (hasPlayerWon) {

				}
				else {
					PlaySoundMem(soundHandles["s_game_over"], DX_PLAYTYPE_BACK, true);
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
			PlaySoundMem(soundHandles["s_game_dead"], DX_PLAYTYPE_BACK, true);
		}
	};

	class SingleGameObject : public Object {
	public:
		/*virtual bool draw() {
			return true;
		}*/

		double rightHit() const { return right() - rect.width / 3; }
		double leftHit() const { return left() + rect.width / 3; }
		double topHit() const { return top() + rect.height / 3; }
		double bottomHit() const { return bottom() - rect.height / 3; }

		void drawImage(int handle, int imageWidth, int imageHeight, int shiftX = 0, int shiftY = 0, bool isReverse = false) const {
			if (isReverse) {
				DrawExtendGraph(left() - shiftX + imageWidth, top() - shiftY, left() - shiftX, top() - shiftY + imageHeight, handle, true);
			}
			else {
				DrawExtendGraph(left() - shiftX, top() - shiftY, left() - shiftX + imageWidth, top() - shiftY + imageHeight, handle, true);
			}
		}

		void drawDark(std::function<void()> func) {
			SetDrawBright(100, 100, 100);
			func();
			SetDrawBright(255, 255, 255);
		}
	};

	// エフェクトクラス
	// 実際のエフェクトの作成はmakeEffect関数から行う
	// effectHandleKey:ハンドルキー, x, y, width, height, willStay:持続するか, layer, framePerCount:エフェクトの速度, counter:何カウント目からエフェクトを始めるか
	class Effect : public SingleGameObject {
		const std::vector<int> effectHandle;
		const bool willStay;
		const int maxCount;
		const int framePerCount;
		int counter;

	public :
		Effect(std::string effectHandleKey_, int x_ = 0, int y_ = 0, int width_ = WIDTH, int height_ = HEIGHT, bool willStay_ = false, int layer_ = 150, int framePerCount_ = 1, int counter_ = 0) : effectHandle(effectHandles[effectHandleKey_]), willStay(willStay_), maxCount(effectHandles[effectHandleKey_].size()*(framePerCount_ > 0 ? framePerCount_ : 1)), framePerCount((framePerCount_ > 0 ? framePerCount_ : 1)) {
			rect.x = x_;
			rect.y = y_;
			rect.width = width_;
			rect.height = height_;
			counter = counter_*(framePerCount_ > 0 ? framePerCount_ : 1);
			layer = layer_;
		}
	
		bool draw() {
			if (willStay) {
				if (counter == maxCount) {
					counter = 0;
				}
			}

			drawWithRect(effectHandle[counter/framePerCount]);
			counter++;

			return willStay || counter != maxCount;
		}
	};

	// ブロックの描画
	// 現在、姫様の足場でしか使っていない
	class SingleGameBlockObject : public SingleGameObject{
	protected:
		bool willStay;
	public:
		bool canHit = true;

		SingleGameBlockObject() {}
		SingleGameBlockObject(int x_, int y_, int width_, int height_, bool willStay_) {
			rect.x = x_, rect.y = y_, rect.width = width_, rect.height = height_;
			willStay = willStay_;
			layer = 50;
		}

		SingleGameBlockObject(Rectan rect_, bool willStay_) {
			rect = rect_;
			willStay = willStay_;
			layer = 50;
		}

		virtual bool draw() {
			if (willStay) {
				drawWithRect(imgHandles["s_game_block"]);
			}
			else {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["block"], true);
			}
			return willStay;
		}
	};

	// マーカー
	class Marker : public SingleGameObject {
		SinglePlayerGame& game;
		const int index;
		int imgHandle;
		Rectan prevRect;

		enum MoveDirection {
			LEFT,RIGHT,
		};
		MoveDirection moveDirection = LEFT;

		enum MarkerType {
			PIYO, SWORD, SHIELD,
		};
		const MarkerType markerType;

		bool isEnable = true;
		bool canGuard;
		bool canAttack;

		void modifyRect() {
			if (rect.width > 0 || rect.height > 0) {
				rect.width = markerType == PIYO ? 1920/14 : markerType == SWORD ? 765/3 : 256;
				rect.height = markerType == PIYO ? 1409/14 : markerType == SWORD ? 765/3 : 256;
			}
			else {
				rect.x = rect.y = -300;
			}
		}

	public:
		Marker(Rectan rect_, bool willStay_, int index_, SinglePlayerGame& game_) :index(index_), game(game_), markerType(index_ < 3 ? PIYO : index_ < 6 ? SWORD : SHIELD){
			rect = rect_;
			modifyRect();
			prevRect = rect;
			layer = 300;
			imgHandle = markerType == PIYO ? imgHandles["s_game_piyo"] : markerType == SWORD ? imgHandles["s_game_sword"] : imgHandles["s_game_shield"];
			canGuard = markerType == SWORD? false : true;
			canAttack = markerType == SHIELD ? false : true;
		}

		bool draw() {
			if (isEnable) {
				switch (moveDirection)
				{
				case RIGHT: {
					drawImage(imgHandle, rect.width, rect.height, 0, 0, true);
					break;
				}

				case LEFT: {
					drawImage(imgHandle, rect.width, rect.height);
					break;
				}
				default:
					break;
				}
			}
			else {
				drawDark([this]() {drawImage(imgHandles["s_game_invalid"], rect.width, rect.height); });
			}
			return true;
		}

		void setRect(Rectan rect_) {
			prevRect = rect;
			rect = rect_;
			modifyRect();
			if (rect.x - prevRect.x < -10) {
				moveDirection = LEFT;
			}
			else if (rect.x - prevRect.x > 10) {
				moveDirection = RIGHT;
			}
			else {
			}
			layer = isEnable ? 300 : 30;
		}

		int getIndex() {
			return index;
		}

		void effectHit() {
			switch (moveDirection)
			{
			case LEFT:
				game.makeEffect("s_game_coin", left() - 30, top()+rect.width/2, 40, 40);
				break;
			case RIGHT:
				game.makeEffect("s_game_coin", right() + 30, top() + rect.width / 2, 40, 40);
				break;
			default:
				break;
			}
		}

		void on() {
			isEnable = true;
		}

		void off() {
			isEnable = false;
		}

		bool getEnable() {
			return isEnable;
		}

		bool getCanAttack() {
			return canAttack;
		}

		bool getCanGuard() {
			return canGuard;
		}
	};

	//キャラクター一般
	class Character : public SingleGameObject {
	public:
		
		enum MoveDirection // x軸方向のみ
		{
			LEFT, NOMOVE, RIGHT,
		};
		MoveDirection moveDirection = NOMOVE;

		enum CharacterState
		{
			NORMAL, DAMAGE, OVER,
		};
		CharacterState characterState = NORMAL;

		std::map<CharacterState, int> imgHandle;
		const int maxDamage;

		double prevX;
		double prevY;
		bool isJumping = true;

		bool isAlive = true;
		int damage = 0;

		const int maxInvincibleTime;
		int invincibleTime = 0;

		const int overBufferMax;
		int overBuffer;

		SinglePlayerGame& game;

		Character(const int x_, const int y_, const int width_, const int height_, const std::string imgHandleKey_, const int maxDamage_, SinglePlayerGame& game_, const int overBufferMax_, int const maxInvincibleTime_) : maxDamage(maxDamage_), game(game_), overBufferMax(overBufferMax_), maxInvincibleTime(maxInvincibleTime_) {
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = width_;
			rect.height = height_;
			imgHandle[NORMAL] = imgHandles[imgHandleKey_];
			imgHandle[DAMAGE] = imgHandles[imgHandleKey_+"_damage"];
			imgHandle[OVER] = imgHandles[imgHandleKey_+"_over"];
			overBuffer = overBufferMax;
		}

		virtual bool draw() {
			if (characterState == DAMAGE) {
				SetDrawBright(160, 160, 160); // DAMAGE のとき画像を暗くする
			}
			if (characterState == OVER) {
				SetDrawBright(80, 80, 80); // DAMAGE のとき画像を暗くする
				layer = 51;
			}
			switch (moveDirection)
			{
			case RIGHT: {
				
				drawWithRect(imgHandle[characterState], 0, true);
				break;
			}

			case NOMOVE:
			case LEFT: {
				drawWithRect(imgHandle[characterState]);
				break;
			}
			default:
				break;
			}
			SetDrawBright(255, 255, 255);

			return getIsAlive();
		}

		virtual void update() {};

		virtual void updateCoordinate(double& acX, double& acY) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			const double diffX = 40 * tanh((x - prevX) / 40);
			const double diffY = 40 * tanh((y - prevY) / 40);


			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;
		}

		virtual void setAc(double& acX, double& acY) {};

		virtual void moveBecauseBlockCollision(const std::vector<std::shared_ptr<SingleGameBlockObject>> objectList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			for (auto block : objectList) {
				if (left() < block->right() && top() < block->bottom() &&
					right() > block->left() && bottom() > block->top()) {

					if (prevY < block->bottomHit() && prevY + height > block->topHit()) {
						if (prevX >= block->rightHit()) {
							x = block->right();
						}
						else if (prevX + width <= block->leftHit()) {
							x = block->left() - width;
						}
						else {
							// TODO
						}
					}
					else {
						if (prevY >= block->bottomHit()) {
							y = block->bottom();
						}
						else if (prevY + height <= block->topHit()) {
							y = block->top() - height;
							isJumping = false;
						}
						else {
							// TODO
						}
					}
				}
			}
		}

		virtual void moveBecauseMarkerCollision(const std::vector<std::shared_ptr<Marker>> objectList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			for (auto marker : objectList) {
				if (!marker->getEnable() || !marker->getCanGuard()) { continue; }

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

		virtual bool deathDecision() {
			
			if (rect.x < game.field.at("left") || rect.x > game.field.at("right")) {
				die();
			}
			else if (rect.y < game.field.at("top") || rect.y > game.field.at("bottom")) {
				die();
			}
			else if (overBuffer < overBufferMax) {
				die();
			}
			return !isAlive;
		}
		
		virtual bool getIsAlive() {
			return isAlive;
		}

		virtual void die() {
			changeCharacterState(OVER);
			if (overBuffer <= 0) isAlive = false;
			overBuffer--;
		}

		virtual void damagecontrol() {};

		void changeCharacterState(CharacterState newState) {
			if (characterState == OVER) {

			}
			else {
				characterState = newState;
			}
		}

		int getMaxDamage() {
			return maxDamage;
		}
		
	};

	// 敵キャラクター
	class Enemy : public Character {
	public:
		Enemy(int x_, int y_, int width_, int height_, std::string imgHandleKey_, int maxDamage_, SinglePlayerGame& game_, int overBufferMax_ = FPS, int maxInvincibleTime_ = FPS) : Character(x_,y_,width_,height_,imgHandleKey_,maxDamage_,game_, overBufferMax_, maxInvincibleTime_) {
			layer = 101;
		}
		
		virtual void update() {
			if (characterState == OVER) { return; }
			double acX = 0;
			double acY = 0;
			setAc(acX, acY);
			updateCoordinate(acX, acY);
			moveBecauseBlockCollision(game.blockList);
			moveBecauseMarkerCollision(game.markerList);

			moveDirection = acX > 0 ? RIGHT : acX < 0 ? LEFT : NOMOVE;
		}
		
		virtual void setAc(double& acX, double& acY) {
			acX = 0;
			acY = 0;
		}

		// 死亡判定。マーカーで殴られたら死ぬ。
		virtual bool deathDecision() {
			if (invincibleTime == 0 && characterState != OVER) {
				changeCharacterState(NORMAL);
				for (auto marker : game.markerList) {
					if (!marker->getEnable() || !marker->getCanAttack()) { continue; }

					if (left() < marker->right() && top() < marker->bottom() &&
						right() > marker->left() && bottom() > marker->top()) {
						if (isAlive) {
							PlaySoundMem(soundHandles["s_game_attack"], DX_PLAYTYPE_BACK, true);
						}
						damageControl();
						marker->effectHit();
						invincibleTime = FPS;

						return !isAlive;

					}
				}

			}
			else if (invincibleTime > 0) {
				invincibleTime--;
			}

			return Character::deathDecision();
		}

		// ダメージが何点以上で死亡とするか。
		virtual void damageControl() {
			damage++;
			
			if (maxDamage < 0) {
				// 無敵
			}
			else if (damage < maxDamage) {
				// HP残ってる
				changeCharacterState(DAMAGE);
			}
			else {
				die();
			}
		}
	};

	class Teresa : public Enemy {
	public:
		static const int width = 100;
		static const int height = 100;

		Teresa(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 2, std::string imgHandleKey_ = "s_game_teresa") : Enemy(x_, y_, width*size, height*size, imgHandleKey_, maxDamage_, game_) {
			layer = 160;
		}

		void setAc(double& acX, double& acY) {
			acX = rand() % 5 - 2;
			acY = rand() % 5 - 2;
		}
		
	};

	class RocketWanwan : public Enemy {
	public :
		static const int width = 430;
		static const int height = 275;

		RocketWanwan(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 3, std::string imgHandleKey_ = "s_game_wanwan") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			layer = 165;
			moveDirection = x_ < WIDTH/2 ? RIGHT : LEFT ;
		}

		void update(){
			if (characterState == OVER) { return; }
			double acX = 0;
			double acY = 0;
			setAc(acX, acY);
			updateCoordinate(acX, acY);
			moveBecauseMarkerCollision(game.markerList);
		}

		void setAc(double& acX, double& acY) {
			acY = -0.5 * (1 - (rect.y - prevY <= 0) - (rect.y - prevY< 0));
			switch (moveDirection) {
			case LEFT: {
				acX = -7;
				break;
			}
			case RIGHT: {
				acX = 7;
				break;
			}
			default: {
				break;
			}
			}
		}
	};

	class Switch : public Enemy {
		static const int width = 100;
		static const int height = 100;

	public:
		Switch(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 2, std::string imgHandleKey_ = "s_game_switch") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			layer = 130;
			moveDirection = RIGHT;
		}

		void update() {}

	};

	class Inundation : public Enemy {
		public: 
		static const int width = 1280 + 200;
		static const int height = 720;
		int inundationCounter = 400;

		bool isWaterUp = true;
		std::shared_ptr<Switch> button;
		Inundation(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = -1, std::string imgHandleKey_ = "s_game_water") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			layer = 300;
			button = game_.makeSwitch(100, 100, 1);
		}

		bool draw() {
			DrawExtendGraph(left(), top() - 150, right(), bottom() + 800, imgHandles["s_game_water"], true);
			return getIsAlive();
		}

		void update() {
			if (!button->getIsAlive()) {
				isWaterUp = false;
			}
			else if (inundationCounter > 0){
				inundationCounter--;
			}
			double acX = 0;
			double acY = 0;
			setAc(acX, acY);
			updateCoordinate(acX, acY);
		}

		void setAc(double& acX, double& acY) {
			acX = 0;
			acY = isWaterUp ? inundationCounter == 0 ? -0.5*(rect.y-prevY) : -0.005 : 0.02;
		}
	};

	class Ray : public Enemy {
	public:
		static const int width = 150;
		static const int height = 720;

		Ray(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = -1, std::string imgHandleKey_ = "s_game_ray") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_, 0) {
			layer = 180;
		}

		void update() {}

		void die() {
			isAlive = false;
		}
	};

	class Ufo : public Enemy {
		static const int width = 225 / 2;
		static const int height = 225 / 2;
		int stopCount = FPS * 7;
		int turnCounter = FPS * 5;
		std::shared_ptr<Ray> ray = nullptr;

	public:
		Ufo(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 3, std::string imgHandleKey_ = "s_game_ufo") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			layer = 150;
		};

		void update() {
			if (characterState == OVER) { return; }
			stopCount--;
			if (stopCount == FPS * 2) {
				ray = game.makeRay(rect.x + rect.width / 2 - Ray::width / 2, bottom(), 1);
			}
			else if (stopCount < FPS * 2) {
				if (stopCount == 0) {
					ray->die();
					ray = nullptr;

					stopCount = FPS * 7;
				}
			}
			else {
				double acX = 0;
				double acY = 0;
				setAc(acX, acY);
				updateCoordinate(acX, acY);
			}
			turnCounter--;
			if (turnCounter == 0) {
				moveDirection = rand() % 2 == 0 ? LEFT : RIGHT;
				turnCounter = (rand() % 4 + 1) * FPS;
			}
			moveBecauseMarkerCollision(game.markerList);
		}

		void updateCoordinate(double acX, double acY) {
			double& x = rect.x;
			double& y = rect.y;

			const double diffX = moveDirection == RIGHT ? 8 : -8;
			const double diffY = 0;

			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;
			if (x < 0 - rect.width) {
				x += WIDTH + 200;
			}
			if (x > WIDTH + rect.width) {
				x -= WIDTH + 200;
			}
		}

		int getStopCount() {
			return stopCount;
		}

		void die() {
			if (ray != nullptr) {
				ray->die();
			}
			isAlive = false;
		}
	};

	class Drop : public Enemy {
	public:
		static const int width = 200 / 8;
		static const int height = 200 / 8;
		Drop(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 1, std::string imgHandleKey_ = "s_game_drop") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_, 0, 0) {
			layer = 139;
		};
		
		void update() {
			double acX = 0;
			double acY = 0;
			setAc(acX, acY);
			updateCoordinate(acX, acY);
			moveBecauseMarkerCollision(game.markerList);
			moveBecauseBlockCollision(game.blockList);
		}

		void setAc(double& acX, double& acY) {
			acX = 0;
			acY = 10;
		}

		bool deathDecision() {
			if (invincibleTime == 0) {
				for (auto marker : game.markerList) {
					if (!marker->getEnable() || !marker->getCanAttack()) { continue; }

					if (left() < marker->right() && top() < marker->bottom() &&
						right() > marker->left() && bottom() > marker->top()) {
						if (isAlive) {
							PlaySoundMem(soundHandles["s_game_attack"], DX_PLAYTYPE_BACK, true);
						}
						damageControl();
						marker->effectHit();
						invincibleTime = maxInvincibleTime;

						return !isAlive;

					}
				}
				for (auto block : game.blockList) {
					if (left() <= block->right() && top() <= block->bottom() &&
						right() >= block->left() && bottom() >= block->top()) {
						if (isAlive) {
							// PlaySoundMem(soundHandles["s_game_attack"], DX_PLAYTYPE_BACK, true);
						}
						damageControl();
						invincibleTime = maxInvincibleTime;

						return !isAlive;

					}
				}
			}
			else if (invincibleTime > 0) {
				invincibleTime--;
			}

			return Character::deathDecision();
		}
		/*
		void die() {
			// TODO
			isAlive = false;
		}*/
	};
	
	class Cloud : public Enemy {
		static const int width = 304 / 3;
		static const int height = 166 / 3;
		int fallDropCounter = FPS*2;
		int turnCounter = FPS * 5;

	public:
		Cloud(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 5, std::string imgHandleKey_ = "s_game_cloud") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			moveDirection = RIGHT;
			layer = 140;
		};

		void update() {
			if (characterState == OVER) { return; }
			fallDropCounter--;
			if (fallDropCounter < 40 && fallDropCounter % 4 == 0) {
				game.makeDrop(rect.x + rand() % (rect.width) - Drop::width / 2, bottom(), 1);
			}
			if (fallDropCounter == 0) {
				fallDropCounter = rand() % 100 + FPS * 2;
			}

			turnCounter--;
			if (turnCounter == 0) {
				moveDirection = rand() % 2 == 0 ? LEFT : RIGHT;
				turnCounter = (rand() % 5 + 2) * FPS;
			}
			double acX = 0;
			double acY = 0;
			setAc(acX, acY);
			updateCoordinate(acX, acY);
		}

		void updateCoordinate(double acX, double acY) {
			double& x = rect.x;
			double& y = rect.y;

			const double diffX = moveDirection == RIGHT ? 3 : -3;
			const double diffY = 0;

			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;

			if (x < 0 - rect.width) {
				x += WIDTH + 200;
			}
			if (x > WIDTH + rect.width) {
				x -= WIDTH + 200;
			}
		}
	};

	class Eagle : public Enemy {
		static const int width = 430 / 4;
		static const int height = 263 / 4;
	public:
		Eagle(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 2, std::string imgHandleKey_ = "s_game_eagle") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
		};

		void update() {
			if (characterState == OVER) { return; }
			double acX = 0;
			double acY = 0;
			setAc(acX, acY);
			updateCoordinate(acX, acY);
			moveBecauseMarkerCollision(game.markerList);
		}

		void setAc(double& acX, double& acY) {
			acX = 4;
			acY = 2;
		}

		void updateCoordinate(double acX, double acY) {
			double& x = rect.x;
			double& y = rect.y;

			const double diffX = x - prevX;
			const double diffY = y - prevY;


			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;
		}
	};

	class Heiho : public Enemy {
		static const int width = 345 / 4;
		static const int height = 333 / 4;
		int frameCounter = 30;
	public:
		Heiho(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 2, std::string imgHandleKey_ = "s_game_heiho") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			moveDirection = LEFT;
			layer = 152;
		}

		void update() {
			if (characterState == OVER) { return; }
			if (frameCounter == 0) {
				game.makeFire(left(), (top() + bottom()) / 2, 1);
			}
			double acX = 0;
			double acY = 0;
			setAc(acX, acY);
			updateCoordinate(acX, acY);
			moveBecauseBlockCollision(game.blockList);
			moveBecauseMarkerCollision(game.markerList);
		}

		void setAc(double& acX, double& acY) {
			acY = 4;
		}

		void updateCoordinate(double acX, double acY) {
			double& x = rect.x;
			double& y = rect.y;
			frameCounter--;
			
			double diffX;
			if (frameCounter > 0) {
				diffX = -5;
			}
			else if (frameCounter == 0) {
				PlaySoundMem(soundHandles["s_game_shuzo"], DX_PLAYTYPE_BACK, true);
				diffX = 0;
			}
			else {
				diffX = 5;
			}
			const double diffY = 0;


			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;
		}
	};

	class Fire : public Enemy {
	public:
		static const int width = 280 / 3;
		static const int height = 194 / 3;
		Fire(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 1, std::string imgHandleKey_ = "s_game_fire") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_, 0, 0) {
			moveDirection = LEFT;
			layer = 151;
		}

		void update() {
			if (characterState == OVER) { return; }
			double acX = 0;
			double acY = 0;
			setAc(acX, acY);
			updateCoordinate(acX, acY);
			moveBecauseMarkerCollision(game.markerList);
		}
		
		void setAc(double& acX, double& acY) {
			switch (moveDirection)
			{
			case SinglePlayerGame::Character::LEFT:
			case SinglePlayerGame::Character::NOMOVE:
				acX = -10;
				break;
			case SinglePlayerGame::Character::RIGHT:
				acX = 10;
				break;
			default:
				break;
			}
			acY = 0;
		}
	};

	// 姫様
	class Player : public Character {
		bool isToJump = false;
		int frameCount;
		std::string message;

	public:
		static const int width = 75;
		static const int height = 100;

		Player(int x_, int y_, int width_, int height_, std::string imgHandleKey_, int maxDamage_, SinglePlayerGame& game_) : Character(x_, y_, width_, height_, imgHandleKey_, maxDamage_, game_, 0, FPS*2) {
			layer = 100;	
			// overBuffer = overBufferMax;
		}

		bool draw() {
			if (isToJump) {
				DrawExtendGraph(left() - 50, top() - rect.height - 50, right() + 50, bottom() - rect.height, imgHandles["s_game_balloon"], true);
				int commentX = left() - 10;
				int commentY = top() - rect.height - 20;

				if (frameCount <= FPS * 2) {
					message = "I want to\njump!!";
					commentY = top() - rect.height - 40;
				}
				else if (frameCount <= FPS * 2 + FPS) {
					message = "3";
				}
				else if (frameCount <= FPS * 2 + FPS*2) {
					message = "2";
				}
				else if (frameCount <= FPS * 2 + FPS*3) {
					message = "1";
				}

				DrawString(commentX, commentY, message.c_str(), GetColor(0, 0, 0));
			}
			for (int heart = 0; heart <= (maxDamage - damage - 1); heart++) {
				DrawExtendGraph(50 + 50 * heart, 50, 100 + 50 * heart, 100, imgHandles["s_game_heart"], true);
			}

			DrawString(50, 50, std::to_string(damage).c_str(), GetColor(255, 255, 255));
			
			Character::draw();

			return true;
			
		}

		void update(const char key[]) {
			if (!isAlive || characterState == OVER) { return; }
			double acX = 0;
			double acY = 0;
			setAc(acX, acY, key);
			updateCoordinate(acX, acY);
			moveBecauseBlockCollision(game.blockList);
			moveBecauseMarkerCollision(game.markerList);
		}

		void updateCoordinate(double& acX, double& acY) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			const double diffX = x - prevX;
			const double diffY = y - prevY;


			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;
		}

		// プレイヤーキャラクターの座標更新
		void setAc(double& acX, double& acY, const char key[]) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			const double diffX = x - prevX;
			const double diffY = y - prevY;

			acX = -0.5 * (1 - (diffX <= 0) - (diffX < 0));
			acY = 3.2;

			if (key[KEY_INPUT_LSHIFT]) {
				isToJump = false;
				if (key[KEY_INPUT_UP] && !isJumping) {
					acY = -40;
					PlaySoundMem(soundHandles["s_game_jump"], DX_PLAYTYPE_BACK, true);
					isJumping = true;
				}
				if (key[KEY_INPUT_RIGHT]) {
					acX = 1.5 * (diffX < 15);
					moveDirection = RIGHT;
				}
				if (key[KEY_INPUT_LEFT]) {
					acX = -1.5 * (diffX > -15);
					moveDirection = LEFT;
				}
			}
			else if (acX == 0 && !isJumping && !isToJump) {
				if (rand() % 10 == 0) {
					frameCount = 0;
					isToJump = true;
				}
				else {
					acX = rand() % 30 - 15 - 5 * (1 - ( x <= WIDTH/2 ) - ( x < WIDTH/2 ));
					moveDirection = acX > 0 ? RIGHT : acX < 0 ? LEFT : moveDirection;
				}

			}

			if (isToJump) {
				if (frameCount >= FPS*2 + FPS*3) {
					acY = -30;
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

		}

		// プレイヤーキャラクターの生死判定更新
		bool deathDecision(std::vector<std::shared_ptr<Enemy>> enemyList) {
			if (invincibleTime == 0 && characterState != OVER) {
				changeCharacterState(NORMAL);
				for (auto enemy : enemyList) {
					if (left() <= enemy->right() && top() <= enemy->bottom() &&
						right() >= enemy->left() && bottom() >= enemy->top()) {
						damageControl();
						invincibleTime = maxInvincibleTime;
						break;
					}
				}
			}
			else if (invincibleTime > 0) {
				invincibleTime--;
			}

			if (damage >= maxDamage) {
				die();
			}
			
			return Character::deathDecision();
		}

		int getPlayerDamage() {
			return damage;
		}

		void moveBecauseMarkerCollision(const std::vector<std::shared_ptr<Marker>> objectList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			for (auto marker : objectList) {
				marker->on();
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
							marker->off();
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
							marker->off();
						}
					}
					prevX = x;
					prevY = y;
				}
			}
		}

		void moveBecauseBlockCollision(const std::vector<std::shared_ptr<SingleGameBlockObject>> objectList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			for (auto block : objectList) {
				if (left() < block->right() && top() < block->bottom() &&
					right() > block->left() && bottom() > block->top()) {
					if (top() < block->top()) {
						y = block->top() - height;
						prevY = y;
						isJumping = false;
					}
					else if (left() < block->left()) {
						x = block->left() - width;
						prevX = x;
					}
					else if (right() > block->right()) {
						x = block->right();
						prevX = x;
					}
					else {
						y = block->top() - height;
						prevY = y;
						isJumping = false;
					}
					
				}
				
			}
		}

		void damageControl() {
			damage++;

			if (damage < maxDamage) {
				// HP残ってる
				changeCharacterState(DAMAGE);
			}
			else {
				die();
			}
		}
	};

	std::thread thread;
	std::shared_ptr<Player> player;

	std::vector<std::shared_ptr<SingleGameBlockObject>> blockList;
	std::vector<std::shared_ptr<Enemy>> enemyList;
	std::vector<std::shared_ptr<Enemy>> enemySubList;
	std::vector<std::shared_ptr<Marker>> markerList;

	const std::map<std::string, int> field = {
		{ "left", 0 - 500 },
		{ "right", WIDTH + 500 },
		{ "top", 0 - 500 },
		{ "bottom", HEIGHT + 500 }
	};

	std::shared_ptr<Teresa> makeTeresa(int x, int y, double size);
	std::shared_ptr<RocketWanwan> makeRocketWanwan(int x, int y, double size);
	std::shared_ptr<Inundation> makeInundation();
	std::shared_ptr<Switch> makeSwitch(int x, int y, double size);
	std::shared_ptr<Ufo> makeUfo(int x, int y, double size);
	std::shared_ptr<Ray> makeRay(int x, int y, double size);
	std::shared_ptr<Cloud> makeCloud(int x, int y, double size);
	std::shared_ptr<Drop> makeDrop(int x, int y, double size);
	std::shared_ptr<Eagle> makeEagle(int x, int y, double size);
	std::shared_ptr<Heiho> makeHeiho(int x, int y, double size);
	std::shared_ptr<Fire> makeFire(int x, int y, double size);

	void makeEffect(std::string effectHandleKey_, int x_ = 0, int y_ = 0, int width_ = WIDTH, int height_ = HEIGHT, bool willStay_ = false, int layer_ = 150, int framePerCount_ = 1, int counter_ = 0) {
		drawList.push_back(std::make_shared<Effect>(effectHandleKey_, x_, y_, width_, height_, willStay_, layer_, framePerCount_, counter_));
	}

	std::shared_ptr<BGM> bgm;
	const int maxTime = FPS * 60;
	const int maxPlayerDamage = 10;
	const int endBuffer = FPS * 5;

	enum Difficulty {
		EASY, HARD, NIGHTMARE,
	};

	int timer = maxTime;
	bool hasPlayerWon;

	Difficulty difficulty = EASY;

public:
	SinglePlayerGame() {
		thread = std::thread::thread(capture, std::ref(share));

		player = std::make_shared<Player>(WIDTH / 2 - 100 / 2, HEIGHT / 2 - 150 / 2, 75, 100, "s_game_player", maxPlayerDamage, *this);

		hasPlayerWon = true;
	}

	bool onStart();
	bool onUpdate();
	bool onFinish();
};
