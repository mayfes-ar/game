#pragma once

#include "game/game.h"

class SinglePlayerGame : public Game {
	/* Layer の基準
		Player : 100
		Marker : 200
		Enemy :
			RocketWanWan : 165
			Teresa : 160
			Switch : 155
			Inundation : 300
		Effect : 周りの描画に影響しそうな大きなエフェクト
			??? :
		Block : 50
		背景 : 0
				
	*/


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
			PlaySoundMem(soundHandles["s_game_dead"], DX_PLAYTYPE_NORMAL, true);
		}
	};

	class SingleGameObject : public Object {
	public:
		virtual bool draw() {
			return true;
		}

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
	};

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
			// TODO: willStayがfalseのとき、framePerCountでの描画がうまくいかない

			return willStay || counter != maxCount;
		}
	};

	class SingleGameBlockObject : public SingleGameObject{
	protected:
		bool willStay;
	public:
		bool canHit = true;

		SingleGameBlockObject() {}
		SingleGameBlockObject(int x_, int y_, int width_, int height_, bool willStay_) {
			rect.x = x_, rect.y = y_, rect.width = width_, rect.height = height_;
			willStay = willStay_;
			layer = 20;
		}

		SingleGameBlockObject(Rectan rect_, bool willStay_) {
			rect = rect_;
			willStay = willStay_;
			layer = 50;
		}

		virtual bool draw() {
			if (willStay) {
				DrawBox(left(), top(), right(), bottom(), GetColor(240, 117, 50), true);
			}
			else {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["block"], true);
			}
			return willStay;
		}


	};

	// 認識したマーカーの扱い
	class Marker : public SingleGameObject {
		SinglePlayerGame& game;
		const int index;
		int imgHandle;
		Rectan prevRect;
		enum MoveDirection {
			LEFT,RIGHT,
		};
		MoveDirection moveDirection = LEFT;

		void setImgHandle() {
			switch (index)
			{
			case 0:
			case 1:
			case 2: {
				imgHandle = imgHandles["s_game_piyo"];
				break;
			}
			case 3:
			case 4:
			case 5: {
				imgHandle = imgHandles["s_game_sword"];
				break;
			}
			case 6:
			case 7:
			default: {
				imgHandle = imgHandles["s_game_shield"];
				break;
			}
			}
		}

		void modifyRect() {
			if (rect.width > 0 || rect.height > 0) {
				rect.width = 100;
				rect.height = 100;
			}
			else {
				rect.x = rect.y = -300;
			}
		}

	public:
		Marker(Rectan rect_, bool willStay_, int index_, SinglePlayerGame& game_) :index(index_), game(game_){
			rect = rect_;
			modifyRect();
			prevRect = rect;
			layer = 300;
			setImgHandle();
		}

		bool draw() {
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
	};

	class Character : public SingleGameObject {
	public:
		const int imgHandle;
		const int maxDamage;
		std::vector<std::string> imgHandleC;
		

		int getMaxDamage() {
			return maxDamage;
		}

		double prevX;
		double prevY;
		bool isJumping = true;

		enum MoveDirection // x軸方向のみ
		{
			LEFT, NOMOVE, RIGHT,
		};
		
		enum CharacterState
		{
			NORMAL, DAMAGE, OVER,
		};

		MoveDirection moveDirection = NOMOVE;
		CharacterState characterState = NORMAL;

		bool isAlive = true;
		int damage = 0;
		int invincibleTime = 0;

		const SinglePlayerGame& game;

		Character(int x_, int y_, int width_, int height_, std::string imgHandleKey_, int maxDamage_, SinglePlayerGame& game_) :imgHandle(imgHandles[imgHandleKey_]), maxDamage(maxDamage_), game(game_) {
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = width_;
			rect.height = height_;
		}



		virtual bool draw() {
			switch (moveDirection)
			{
			case RIGHT: {
				drawWithRect(imgHandle, 0, true);
				break;
			}

			case NOMOVE:
			case LEFT: {
				drawWithRect(imgHandle);
				break;
			}
			default:
				break;
			}
			return getIsAlive();
		}

		virtual void update() {
		}

		void updateCoordinate(double& acX, double& acY) {
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

		virtual void setAc(double& acX, double& acY) {
		}

		void moveBecauseBlockCollision(const std::vector<std::shared_ptr<SingleGameBlockObject>> objectList) {
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

		void moveBecauseMarkerCollision(const std::vector<std::shared_ptr<Marker>> objectList) {
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

		bool deathDecision() {
			
			if (rect.x < game.field.at("left") || rect.x > game.field.at("right")) {
				isAlive = false;
			}
			if (rect.y < game.field.at("top") || rect.y > game.field.at("bottom")) {
				isAlive = false;
			}
			
			return !isAlive;
		}
		
		bool getIsAlive() {
			return isAlive;
		}
}	;

	// 敵キャラクター
	class Enemy : public Character {
	public:
		Enemy(int x_, int y_, int width_, int height_, std::string imgHandleKey_, int maxDamage_, SinglePlayerGame& game_) : Character(x_,y_,width_,height_,imgHandleKey_,maxDamage_,game_) {
			layer = 101;
		}
		
		virtual void update() {
			double acX = 0;
			double acY = 0;
			setAc(acX, acY);
			updateCoordinate(acX, acY);
			moveBecauseBlockCollision(game.blockList);
			moveBecauseMarkerCollision(game.markerList);

			moveDirection = acX > 0 ? RIGHT : acX < 0 ? LEFT : NOMOVE;
		}
		
		virtual void setAc(double& acX, double& acY) {
			
		}

		// 死亡判定。マーカーで殴られたら死ぬ。
		bool deathDecision() {

			if (invincibleTime == 0) {
				for (auto marker : game.markerList) {
					if (left() < marker->right() && top() < marker->bottom() &&
						right() > marker->left() && bottom() > marker->top()) {
						if (isAlive) {
							PlaySoundMem(soundHandles["s_game_attack"], DX_PLAYTYPE_BACK, true);
						}
						isAlive = damageControl();
						marker->effectHit();
						invincibleTime = 15;

						return !isAlive;

					}
				}

			}
			else if (invincibleTime > 0) {
				invincibleTime--;
			}

			return Character::deathDecision();
		}

		// ダメージが何点以上で死亡とするか。返り値は 死：false, 生：true
		bool damageControl() {
			damage++;
			
			if (maxDamage < 0) {
				return true;
			}
			return damage < maxDamage;
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

		RocketWanwan(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 10, std::string imgHandleKey_ = "s_game_wanwan") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			layer = 165;
			moveDirection = x_ < WIDTH/2 ? RIGHT : LEFT ;
		}

		void update(){
			double acX = 0;
			double acY = 0;
			setAc(acX, acY);
			updateCoordinate(acX, acY);
			//moveBecauseBlockCollision(game.blockList);
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
	public:
		static const int height = 100;
		Switch(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 1, std::string imgHandleKey_ = "s_game_switch") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			layer = 155;
		}

		void setAc(double& acX, double& acY) {
			acX = 0;
			acY = 0;
		}
	};

	class Inundation : public Enemy {
		public: 
		static const int width = 1280 + 200;
		static const int height = 720;
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
			static int inundationCounter = 400;
			if (!button->getIsAlive() || inundationCounter == 0) {
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
			acY = isWaterUp ? -0.005 : 0.02;
		}
	};

	class Ufo : public Enemy {
		static const int width = 225 / 2;
		static const int height = 225 / 2;
	public:
		Ufo(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 10, std::string imgHandleKey_ = "s_game_ufo") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
		
		};
	};

	class Cloud : public Enemy {
		static const int width = 304 / 3;
		static const int height = 166 / 3;
	public:
		Cloud(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 10, std::string imgHandleKey_ = "s_game_cloud") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			
		};

		void updateCoordinate(double acX, double acY) {
			double& x = rect.x;
			double& y = rect.y;

			const double diffX = 5;
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

	class Drop : public Enemy {
		static const int width = 200 / 8;
		static const int height = 200 / 8;
	public:
		Drop(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 10, std::string imgHandleKey_ = "s_game_drop") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
		};
		void setAc(double* acX, double* acY) {
			*acX = 0;
			*acY = 3;
		}
	};

	class Eagle : public Enemy {
		static const int width = 430 / 4;
		static const int height = 263 / 4;
	public:
		Eagle(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 10, std::string imgHandleKey_ = "s_game_eagle") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
		};

		void updateCoordinate(double acX, double acY) {
			double& x = rect.x;
			double& y = rect.y;

			const double diffX = 5;
			const double diffY = 3;


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
		int frameCounter = 100;
	public:
		Heiho(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 10, std::string imgHandleKey_ = "s_game_heiho") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			moveDirection = LEFT;
		}

		void updateCoordinate(double acX, double acY) {
			double& x = rect.x;
			double& y = rect.y;
			
			double diffX;
			if (frameCounter >= 0) {
				diffX = -5;
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
		static const int width = 280 / 3;
		static const int height = 194 / 3;
	public:
		Fire(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 10, std::string imgHandleKey_ = "s_game_fire") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			moveDirection = LEFT;
		}
		void updateCoordinate(double acX, double acY) {
			double& x = rect.x;
			double& y = rect.y;

			const double diffX = -5;
			const double diffY = 0;


			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;
		}
		void setAc(double* acX, double* acY) {
			*acX = -2;
			*acY = 0;
		}
	};

	class Player : public Character {

	public:
		static const int width = 75;
		static const int height = 100;
		Player(int x_, int y_, int width_, int height_, std::string imgHandleKey_, int maxDamage_, SinglePlayerGame& game_) : Character(x_, y_, width_, height_, imgHandleKey_, maxDamage_, game_) {
			layer = 100;			
		}

		bool draw() {
			DrawString(50, 50, std::to_string(damage).c_str(), GetColor(255, 255, 255));
			if (characterState == DAMAGE) {
				switch (moveDirection)
			{
			case RIGHT: {
				drawWithRect(imgHandles["s_game_player_damage"], 0, true);
				break;
			}

			case NOMOVE:
			case LEFT: {
				drawWithRect(imgHandles["s_game_player_damage"]);
				break;
			}
			default:
				break;
			}
			return getIsAlive();

			}
			else {
				return Character::draw();
			}
			
		}

		void update(const char key[]) {
			double acX = 0;
			double acY = 0;
			setAc(acX, acY, key);
			updateCoordinate(acX, acY);
			moveBecauseBlockCollision(game.blockList);
			//moveBecauseMarkerCollision(game.markerList);
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
			const double diffX = rect.x - prevX;
			acX = -0.5 * (1 - (diffX <= 0) - (diffX < 0));
			acY = 4;

			if (key[KEY_INPUT_UP] && !isJumping) {
				acY = -40;
				PlaySoundMem(soundHandles["s_game_jump"], DX_PLAYTYPE_BACK, true);
			}
			isJumping = true;

			if (key[KEY_INPUT_RIGHT]) {
				acX = 1.0 * (diffX < 10);
				moveDirection = RIGHT;
			}
			if (key[KEY_INPUT_LEFT]) {
				acX = -1.0 * (diffX > -10);
				moveDirection = LEFT;
			}
		}

		// プレイヤーキャラクターの生死判定更新
		bool deathDecision(std::vector<std::shared_ptr<Enemy>> enemyList) {
			if (invincibleTime == 0) {
				for (auto enemy : enemyList) {
					if (left() <= enemy->right() && top() <= enemy->bottom() &&
						right() >= enemy->left() && bottom() >= enemy->top()) {
						damage += 1;
						characterState = DAMAGE;
						invincibleTime = 50;
						break;
					}
				}
			}
			else if (invincibleTime == 40) {
				characterState = NORMAL;
				invincibleTime--;
			}
			else if (invincibleTime > 0) {
				invincibleTime--;
			}

			
			if (damage >= maxDamage) {
				isAlive = false;

			}
			
			return Character::deathDecision();
		}

		int getPlayerDamage() {
			return damage;
		}

	};

	std::thread thread;
	std::shared_ptr<Player> player;

	std::vector<std::shared_ptr<SingleGameBlockObject>> blockList;
	std::vector<std::shared_ptr<Enemy>> enemyList;
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
	std::shared_ptr<Ufo> makeUfo();
	std::shared_ptr<Cloud> makeCloud();
	std::shared_ptr<Drop> makeDrop();
	std::shared_ptr<Eagle> makeEagle();
	std::shared_ptr<Heiho> makeHeiho();
	std::shared_ptr<Fire> makeFire();

	void makeEffect(std::string effectHandleKey_, int x_ = 0, int y_ = 0, int width_ = WIDTH, int height_ = HEIGHT, bool willStay_ = false, int layer_ = 150, int framePerCount_ = 1, int counter_ = 0) {
		drawList.push_back(std::make_shared<Effect>(effectHandleKey_, x_, y_, width_, height_, willStay_, layer_, framePerCount_, counter_));
	}

	std::shared_ptr<BGM> bgm;
	const int maxTime = 30 * 40;
	const int maxPlayerDamage = 20;
	int timer = maxTime;
	bool hasPlayerWon;

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
