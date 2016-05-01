﻿#pragma once

#include "game/game.h"

class PuzzleGame : public Game {
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
				} else {
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

	//
	class StageBlock : public BlockObject {
	public:
		const int kind = 0;

		StageBlock(Rectan rect_, bool canHit_=true) {
			rect = rect_;
			willStay = true;
			layer = 20;
			canHit = canHit_;
		}
		bool draw() {
			if (canHit) {
				DrawBox(left(), top(), right(), bottom(), GetColor(240, 117, 50), true);
			} else {
				DrawBox(left(), top(), right(), bottom(), GetColor(40, 117, 50), false);
			}
			return willStay;
		}
	};
	//
	class MarkerBlock : public BlockObject {
	public:
		MarkerBlock(Rectan rect_) {
			rect = rect_;
			willStay = false;
			layer = 25;
		}

		bool draw() {
			drawWithRect(imgHandles["block"]);
			return willStay;
		}
		
	};

	// gimmick共通クラス
	// updateの返り値で存続判定
	class Gimmick : public Object {
	protected:
		bool willExist = true;
		const PuzzleGame& game;
		Gimmick(PuzzleGame& game_) : game(game_){}
	public:
		virtual bool update() = 0;
	};


	class Background : public Object {
		int& handle;
	public:
		Background(int& handle_) : handle(handle_) {
			layer = 0;
		}
		bool draw() {
			DrawExtendGraph(CAP2IMG_SHIFT_X, CAP2IMG_SHIFT_Y, CAP2IMG_SHIFT_X + CAP2IMG_RATE*CAP_WIDTH, CAP2IMG_SHIFT_Y + CAP2IMG_RATE*CAP_HEIGHT, handle, FALSE);
			return true;
		}
	};

	class Explanation : public Object {
	public:
		bool draw() {
			DrawGraph(0, 0, imgHandles["p_explain"], false);
			return true;
		}
	};

	// スコアを管理・表示
	class ScoreObject : public Object {
		bool isPlaying = true;
	public:
		int score = 0;

		ScoreObject() {
			layer = 200;
		}
		bool draw() {
			if (isPlaying) {
				DrawFormatString(400, 0, GetColor(65, 205, 63), "SCORE: %d", score);
			} else {

			}
			return true;
		}
		void setResultDraw() { isPlaying = false; }
	};

	class Player : public Object {
		const PuzzleGame& game;

		double prevX;
		double prevY;
		const double initX;
		const double initY;

		bool isRightDirection = true;

		bool isJumping = true;

	public:
		double exForceX = 0;
		double exForceY = 0;

		Player(int x_, int y_, PuzzleGame& game_) : initX(x_), initY(y_), game(game_) {
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = 75;
			rect.height =100;
			layer = 100;
		}

		bool draw() {
			drawWithRect(imgHandles["p_girl"], 0, !isRightDirection);
			return true;
		}
		void preUpdate() {
			exForceX = exForceY = 0;
		}
		void update();
		void warp(int x, int y) {
			rect.x = prevX = x;
			rect.y = prevY = y;
		}
		void init() {
			rect.x = prevX = initX;
			rect.y = prevY = initY;
		}
	};

	class GoalObject : public Object {
		bool isReached = false;
	public:
		GoalObject(int x_, int y_) {
			rect = Rectan(x_, y_, 100, 150);
			layer = 100;
		}
		bool draw() {
			drawWithRect(imgHandles["p_goal"]);
			return true;
		}
		bool check(std::shared_ptr<Player>& player) {
			if (isContacted(player) && !isReached) {
				isReached = true;
				return true;
			} else {
				return false;
			}
		}

	};

	// gimmicks
	class DamageGimmick : public Gimmick {
		const bool canVanish;
		const double vx;
		const double vy;
		int counter = 0;
		const int countMax = effectHandles["p_ball"].size();
	public:
		DamageGimmick(int x, int y, int size, double vx_, double vy_, PuzzleGame& game_) : canVanish(vx_*vx_ + vy_*vy_ > 0), vx(vx_), vy(vy_), Gimmick(game_) {
			rect = Rectan(x, y, size, size);
			layer = 50;
		}
		bool draw() {
			const int margin = rect.height / 3;
			if (canVanish) {
				drawWithRect(effectHandles["p_ball"][counter], margin);
				counter++;
				if (counter == countMax) {
					counter = 0;
				}
			} else {
				drawWithRect(imgHandles["s_game_dead"], margin);
			}
			
			return willExist;
		}
		bool update() {
			if (canVanish) {
				rect.x += vx;
				rect.y += vy;
				
				for (auto block : game.allBlocks) {
					if (isContacted(block)) {
						willExist = false;
					}
				}
			}

			if (isContacted(game.player)) {
				game.player->init();
			}
			return willExist;
		}
	};

	class SmogGimmick : public Gimmick {
		int counter = 0;
		const int countMax = effectHandles["p_smog"].size();
	public:
		SmogGimmick(int x, int y, int size, PuzzleGame& game_) : Gimmick(game_) {
			rect = Rectan(x, y, size, size);
			layer = 120;
		}
		bool draw() {
			const int margin = rect.width / 10;
			DrawExtendGraph(left() - margin, top() - margin, right() + margin, bottom() + margin, effectHandles["p_smog"][counter], true);
			counter++;
			if (counter == countMax) { counter = 0; }
			return willExist;
		}
		bool update() {
			if (isContacted(game.markerBlock)) { willExist = false; }
			return willExist;
		}
	};
	
	class CoinGimmick : public Gimmick {
		const int value;
		int counter = 0;
		const int countMax = effectHandles["p_coin"].size();
	public:
		CoinGimmick(int x, int y, int size, int value_, PuzzleGame& game_) : value(value_), Gimmick(game_) {
			rect = Rectan(x, y, size, size);
			layer = 40;
		}
		bool draw() {
			const int margin = rect.width / 10;
			drawWithRect(effectHandles["p_coin"][counter], margin);
			counter++;
			if (counter == countMax) { counter = 0; }
			return willExist;
		}
		bool update() {
			if (isContacted(game.player)) {
				game.score->score += value;
				willExist = false;
			}
			return willExist;
		}
	};

	class SwitchGimmick : public Gimmick {
		bool isOn;
		const bool isReverse;
		bool& targetBool;
		void setSwitch(bool isOn_) {
			isOn = isOn_;
			if (isReverse) {
				targetBool = !isOn_;
			} else {
				targetBool = isOn_;
			}
		}
	public:
		SwitchGimmick(int x, int y, int size, bool& target, bool isReverse_, PuzzleGame& game_) : targetBool(target), isReverse(isReverse_), Gimmick(game_) {
			rect = Rectan(x, y, size, size);
			layer = 50;
			setSwitch(false);
		}
		bool draw() {
			if (isOn) {
				drawWithRect(imgHandles["p_on"]);
			} else {
				drawWithRect(imgHandles["p_off"]);
			}
			return willExist;
		}
		bool update() {
			setSwitch(false);
			if (isContacted(game.player) || isContacted(game.markerBlock)) {
				setSwitch(true);
			}

			return willExist;
		}
	};

	class WarpGimmick : public Gimmick {
		const double posX;
		const double posY;

	public:
		WarpGimmick(Rectan rect_, double posX_, double posY_, PuzzleGame& game_) : posX(posX_), posY(posY_), Gimmick(game_) {
			rect = rect_;
			layer = 70;
		}
		bool draw() {
			DrawBox(left(), top(), right(), bottom(), GetColor(238, 46, 213), false);
			return willExist;
		}
		bool update() {
			if (isContacted(game.player)) {
				game.player->warp(posX, posY);
			}
			return willExist;
		}
	};

	class WindGimmick : public Gimmick {
		const double windX;
		const double windY;
	public:
		WindGimmick(Rectan rect_, double windX_, double windY_, PuzzleGame& game_) :windX(windX_), windY(windY_), Gimmick(game_) {
			rect = rect_;
			layer = 80;
		}
		bool draw() {
			DrawBox(left(), top(), right(), bottom(), GetColor(125, 224, 227), false);
			return willExist;
		}
		bool update() {
			if (isContacted(game.player)) {
				game.player->exForceX += windX;
				game.player->exForceY += windY;
			}
			return willExist;
		}
	};

	std::thread thread;
	std::shared_ptr<Player> player;
	std::shared_ptr<GoalObject> goal;
	std::shared_ptr<ScoreObject> score;

	std::vector<std::shared_ptr<BlockObject>> stageBlocks;
	std::shared_ptr<BlockObject> markerBlock;
	std::vector<std::shared_ptr<BlockObject>> allBlocks;
	std::vector<std::shared_ptr<Gimmick>> gimmicks;

	std::mt19937 mt;
	std::uniform_int_distribution<> rand100 = std::uniform_int_distribution<>(0, 99);

	int timer = 0;

	std::shared_ptr<StageBlock> setBlock(int x, int y, int width, int height, bool canHit=true) {
		auto block = std::make_shared<StageBlock>(Rectan(x, y, width, height), canHit);
		stageBlocks.push_back(block);
		drawList.push_back(block);
		return block;
	}
	
	void setDamage(int x, int y, int size, double vx = 0, double vy = 0) {
		auto damage = std::make_shared<DamageGimmick>(x, y, size, vx, vy, *this);
		gimmicks.push_back(damage);
		drawList.push_back(damage);
	}
	void setSmog() {
		auto smog = std::make_shared<SmogGimmick>(rand100(mt) * 11, rand100(mt) * 6, 3 * rand100(mt) + 50, *this);
		gimmicks.push_back(smog);
		drawList.push_back(smog);
	}
	void setCoin(int x, int y, int size=60, int value=100) {
		auto coin = std::make_shared<CoinGimmick>(x, y, size, value, *this);
		gimmicks.push_back(coin);
		drawList.push_back(coin);
	}
	void setSwitch(int x, int y, int size, bool& target, bool isReverse = false) {
		auto switch_ = std::make_shared<SwitchGimmick>(x, y, size, target, isReverse, *this);
		gimmicks.push_back(switch_);
		drawList.push_back(switch_);
	}
	void setWarp(int x, int y, int width, int height, int posX, int posY) {
		auto warp = std::make_shared<WarpGimmick>(Rectan(x, y, width, height), posX, posY, *this);
		gimmicks.push_back(warp);
		drawList.push_back(warp);
	}
	void setWind(int x, int y, int width, int height, double windX, double windY) {
		auto wind = std::make_shared<WindGimmick>(Rectan(x, y, width, height), windX, windY, *this);
		gimmicks.push_back(wind);
		drawList.push_back(wind);
	}

	void setPlayer(int x, int y) {
		player = std::make_shared<Player>(x, y, *this);
		drawList.push_back(player);
	}
	void setGoal(int x, int y) {
		goal = std::make_shared<GoalObject>(x, y);
		drawList.push_back(goal);
	}

	void makeStageBase() {
		drawList.clear();
		stageBlocks.clear();
		gimmicks.clear();

		drawList.push_back(std::make_shared<Background>(share.handle));
		drawList.push_back(score);

		setBlock(0, 700, 1280, 100, true);
		setBlock(-50, -720, 100, 720*2, true);
		setBlock(1230, -720, 100, 720*2, true);
		setBlock(200, -720, 100, 620, true);
	}

public:
	PuzzleGame() {
		thread = std::thread::thread(capture, std::ref(share));
		std::random_device rnd;
		mt = std::mt19937(rnd());
		score = std::make_shared<ScoreObject>();
	}

	bool onStart();
	bool onUpdate();
	bool onFinish() {
		thread.join();
		return true;
	}
};
