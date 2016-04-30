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

		StageBlock(Rectan rect_) {
			rect = rect_;
			willStay = true;
		}
		bool draw() {
			if (canHit) {
				DrawBox(left(), top(), right(), bottom(), GetColor(240, 117, 50), true);
			} else {
				DrawBox(left(), top(), right(), bottom(), GetColor(240, 117, 50), false);
			}
			return willStay;
		}
	};
	//
	class MarkerBlock : public BlockObject {
	public:
		MarkerBlock(Rectan rect_) {
			rect = rect_;
			willStay = true;
		}

		bool draw() {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["block"], true);
			return willStay;
		}
		
	};

	// gimmick
	class Gimmick : public Object {
		bool willExist = true;
		const PuzzleGame& game;
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

		void update(const std::vector<std::shared_ptr<BlockObject>> blockList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			const double diffX = x - prevX;
			const double diffY = y - prevY;

			double acX = -0.3 * (1 - (diffX <= 0) - (diffX < 0));
			double acY = 2;

			bool onTop = false;
			bool onBottom = false;
			bool onLeft = false;
			bool onRight = false;

			if (game.key[KEY_INPUT_UP] && !isJumping) {
				acY = -25;
			}
			isJumping = true;

			if (game.key[KEY_INPUT_RIGHT]) {
				isRightDirection = true;
				acX = 0.8 * (diffX < 8);
			}
			if (game.key[KEY_INPUT_LEFT]) {
				isRightDirection = false;
				acX = -0.8 * (diffX > -8);
			}

			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;

			for (auto block : blockList) {
				if (isContacted(block)) {

					if (prevY < block->bottomHit() && prevY + height > block->topHit()) {
						if (prevX >= block->rightHit()) {
							x = prevX = block->right();
							onLeft = true;
						} else if (prevX + width <= block->leftHit()) {
							x = prevX = block->left() - width;
							onRight = true;
						} else {
							// TODO
							init();
							break;
						}
					} else {
						if (prevY >= block->bottomHit()) {
							y = prevY = block->bottom();
							onTop = true;
						} else if (prevY + height <= block->topHit()) {
							y = prevY = block->top() - height;
							isJumping = false;
							onBottom = true;
						} else {
							// TODO
							init();
							break;
						}
					}
				}
			}

			if ((onTop && onBottom) || (onLeft && onRight)) {
				init();
			}
		}

		void warp(int x, int y) {
			rect.x = prevX = x;
			rect.y = prevY = y;
		}
		void init() {
			rect.x = prevX = initX;
			rect.y = prevY = initY;
		}
	};

	// Goal
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

	class NeedleObject : public Object {
		const double vx;
		const double vy;
		bool willExist = true;
		int counter = 0;

	public:
		NeedleObject(int x, int y, int size, double vx_, double vy_) : vx(vx_), vy(vy_){
			rect = Rectan(x, y, size, size);
			layer = 50;
		}
		bool draw() {
			static const int margin = rect.height/3;
			DrawExtendGraph(left() - margin, top() - margin, right() + margin, bottom() + margin, effectHandles["p_ball"][counter], true);
			counter++;
			if (counter == 30) {
				counter = 0;
			}
			return willExist;
		}
		bool update(std::vector<std::shared_ptr<BlockObject>> blockList) {
			rect.x += vx;
			rect.y += vy;
			for (auto block : blockList) {
				if (isContacted(block)) {
					if (!(vx == 0 && vy == 0)) { willExist = false; }
				}
			}

			return willExist;
		}
	};

	class SmogObject : public Object {
		int counter = 0;
		bool willFinish = false;
	public:
		SmogObject(int x, int y, int size) {
			rect = Rectan(x, y, size, size);
			layer = 120;
		}
		bool draw() {
			static const int margin = rect.width / 10;
			DrawExtendGraph(left() - margin, top() - margin, right() + margin, bottom() + margin, effectHandles["p_smog"][counter%13], true);
			counter++;
			
			if (willFinish) {
				return false;
			}
			return true;
		}
		void vanish() {
			willFinish = true;
		}
	};

	std::thread thread;
	std::shared_ptr<Player> player;
	std::shared_ptr<GoalObject> goal;

	std::vector<std::shared_ptr<BlockObject>> stageBlockList;
	std::shared_ptr<BlockObject> markerBlock;

	std::vector<std::shared_ptr<NeedleObject>> needleList;
	std::vector<std::shared_ptr<SmogObject>> smogList;

	std::mt19937 mt;
	std::uniform_int_distribution<> rand100 = std::uniform_int_distribution<>(0, 99);

	int timer = 0;

	void setBlock(int x, int y, int width, int height, bool willStay) {
		auto block = std::make_shared<BlockObject>(x, y, width, height, willStay);
		stageBlockList.push_back(block);
		drawList.push_back(block);
	}
	void setNeedle(int x, int y, int size, double vx, double vy) {
		auto needle = std::make_shared<NeedleObject>(x, y, size, vx, vy);
		needleList.push_back(needle);
		drawList.push_back(needle);
	}
	void setSmog() {
		auto smog = std::make_shared<SmogObject>(rand100(mt) * 11, rand100(mt) * 6, 3 * rand100(mt) + 50);
		smogList.push_back(smog);
		drawList.push_back(smog);
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
		stageBlockList.clear();
		needleList.clear();
		smogList.clear();

		drawList.push_back(std::make_shared<Background>(share.handle));

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
	}

	bool onStart();
	bool onUpdate();
	bool onFinish() {
		thread.join();
		return true;
	}
};
