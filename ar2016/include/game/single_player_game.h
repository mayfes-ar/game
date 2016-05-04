﻿#pragma once

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
		bool draw() {
			return true;
		}
		double rightHit() const { return right() - rect.width / 3; }
		double leftHit() const { return left() + rect.width / 3; }
		double topHit() const { return top() + rect.height / 3; }
		double bottomHit() const { return bottom() - rect.height / 3; }
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
			layer = 20;
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
				drawWithRect(imgHandles["s_game_marker"]);
				// DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_marker"], true);
				break;
			}
			case 1: {
				drawWithRect(imgHandles["p_goal"]);
				// DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["p_goal"], true);
				break;
			}
			default:
				drawWithRect(imgHandles["block"]);
				// DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["block"], true);
				break;
			}
			return false;
		}
	};

	class Character : public SingleGameObject {
	public:
		const int imgHandle;
		const int maxDamage;
		std::vector<std::string> imgHandleC;
		

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

		void updateCoordinate(double acX, double acY) {
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

		virtual void setAc(double* acX, double* acY) {
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
			setAc(&acX, &acY);
			updateCoordinate(acX, acY);
			moveBecauseBlockCollision(game.blockList);
			moveBecauseMarkerCollision(game.markerList);

			moveDirection = acX > 0 ? RIGHT : acX < 0 ? LEFT : NOMOVE;
		}
		
		virtual void setAc(double* acX, double* acY) {
			
		}

		// 死亡判定。マーカーで殴られたら死ぬ。
		bool deathDecision() {
			// この場合、ブロックに当たったら死ぬ、的な

			if (invincibleTime == 0) {
				for (auto marker : game.markerList) {
					if (left() < marker->right() && top() < marker->bottom() &&
						right() > marker->left() && bottom() > marker->top()) {
						if (isAlive) {
							PlaySoundMem(soundHandles["s_game_attack"], DX_PLAYTYPE_BACK, true);
						}
						isAlive = damageControl();
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
		static const int width = 100;
		static const int height = 100;

	public:
		Teresa(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 2, std::string imgHandleKey_ = "s_game_teresa") : Enemy(x_, y_, width*size, height*size, imgHandleKey_, maxDamage_, game_) {
			layer = 101;
		}

		void setAc(double* acX, double* acY) {
			*acX = rand() % 5 - 2;
			*acY = rand() % 5 - 2;
		}
		
	};

	class RocketWanwan : public Enemy {
		static const int width = 200;
		static const int height = 200;
	public:
		RocketWanwan(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 10, std::string imgHandleKey_ = "s_game_wanwan") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			layer = 101;
			moveDirection = x_ < WIDTH/2 ? RIGHT : LEFT ;
		}

		void update(){
			double acX = 0;
			double acY = 0;
			setAc(&acX, &acY);
			updateCoordinate(acX, acY);
			//moveBecauseBlockCollision(game.blockList);
			moveBecauseMarkerCollision(game.markerList);
		}

		void setAc(double* acX, double* acY) {
			*acY = -0.5 * (1 - (rect.y - prevY <= 0) - (rect.y - prevY< 0));
			switch (moveDirection) {
			case LEFT: {
				*acX = -10;
				break;
			}
			case RIGHT: {
				*acX = 10;
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
		Switch(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 1, std::string imgHandleKey_ = "s_game_switch") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			layer = 100;
		}

		void setAc(double* acX, double* acY) {
			*acX = 0;
			*acY = 0;
		}
	};

	class Inundation : public Enemy {
		static const int width = 1280 + 200;
		static const int height = 720;
		bool isWaterUp = true;
		std::shared_ptr<Switch> button;
	public:
		Inundation(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = -1, std::string imgHandleKey_ = "s_game_water") : Enemy(x_, y_, width * size, height * size, imgHandleKey_, maxDamage_, game_) {
			layer = 101;
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
			double acX = 0;
			double acY = 0;
			setAc(&acX, &acY);
			updateCoordinate(acX, acY);
		}

		void setAc(double* acX, double* acY) {
			*acX = 0;
			*acY = isWaterUp ? -0.005 : 0.02;
		}
	};

	class Player : public Character {

	public:
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
			setAc(&acX, &acY, key);
			updateCoordinate(acX, acY);
			moveBecauseBlockCollision(game.blockList);
			//moveBecauseMarkerCollision(game.markerList);
		}

		void updateCoordinate(double acX, double acY) {
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
		void setAc(double* acX, double* acY, const char key[]) {
			const double diffX = rect.x - prevX;
			*acX = -0.5 * (1 - (diffX <= 0) - (diffX < 0));
			*acY = 4;

			if (key[KEY_INPUT_UP] && !isJumping) {
				*acY = -40;
				PlaySoundMem(soundHandles["s_game_jump"], DX_PLAYTYPE_BACK, true);
			}
			isJumping = true;

			if (key[KEY_INPUT_RIGHT]) {
				*acX = 1.0 * (diffX < 10);
				moveDirection = RIGHT;
			}
			if (key[KEY_INPUT_LEFT]) {
				*acX = -1.0 * (diffX > -10);
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
			/*
			if (damage > 5) {
			characterState = OVER;
			isAlive = false;
			}
			*/
			return Character::deathDecision();
		}
	};

	std::thread thread;
	std::shared_ptr<Player> player;

	std::vector<std::shared_ptr<SingleGameBlockObject>> blockList;
	std::vector<std::shared_ptr<Enemy>> enemyList;
	std::vector<std::shared_ptr<Marker>> markerList;

	const std::map<std::string, int> field = {
		{ "left", 0 - 200 },
		{ "right", WIDTH + 200 },
		{ "top", 0 - 200 },
		{ "bottom", HEIGHT + 200 }
	};

	std::shared_ptr<Teresa> makeTeresa(int x, int y, double size);
	std::shared_ptr<RocketWanwan> makeRocketWanwan(int x, int y, double size);
	std::shared_ptr<Inundation> makeInundation();
	std::shared_ptr<Switch> makeSwitch(int x, int y, double size);

	std::shared_ptr<BGM> bgm;
	const int maxTime = 30 * 40;
	int timer = maxTime;
	bool hasPlayerWon;

public:
	SinglePlayerGame() {
		thread = std::thread::thread(capture, std::ref(share));
		
			player = std::make_shared<Player>(WIDTH / 2 - 100 / 2, HEIGHT / 2 - 150 / 2, 90, 140, "s_game_player", 5, *this);
		
		
		hasPlayerWon = true;
	}

	bool onStart();
	bool onUpdate();
	bool onFinish();
};
