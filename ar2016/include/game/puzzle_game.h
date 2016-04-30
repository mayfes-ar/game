#pragma once

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
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["block"], true);
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

	class ScoreObject : public Object {
	public:
		int score = 0;

		ScoreObject() {
			layer = 200;
		}
		bool draw() {
			DrawFormatString(400, 0, GetColor(65, 205, 63), "SCORE: %d", score);
			return true;
		}
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
		Player(int x_, int y_, PuzzleGame& game_) : initX(x_), initY(y_), game(game_) {
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = 75;
			rect.height =100;
			layer = 100;
		}

		bool draw() {
			if (isRightDirection) {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["p_girl"], true);
			} else {	
				DrawExtendGraph(right(), top(), left(), bottom(), imgHandles["p_girl"], true);
			}
			return true;
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
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["p_goal"], true);
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
				DrawExtendGraph(left() - margin, top() - margin, right() + margin, bottom() + margin, effectHandles["p_ball"][counter], true);
				counter++;
				if (counter == countMax) {
					counter = 0;
				}
			} else {
				DrawExtendGraph(left() - margin, top() - margin, right() + margin, bottom() + margin, imgHandles["s_game_dead"], true);
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
			DrawExtendGraph(left() - margin, top() - margin, right() + margin, bottom() + margin, effectHandles["p_coin"][counter], true);
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

	};

	class WarpGimmick : public Gimmick {

	};

	class WindGimmick : public Gimmick {

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
