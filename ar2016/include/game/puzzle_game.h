#pragma once

#include "game/game.h"

class PuzzleGame : public Game {

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

	class Player : public Object {
		double prevX;
		double prevY;
		const double initX;
		const double initY;

		bool isJumping = true;

	public:
		Player(int x_, int y_) : initX(x_), initY(y_) {
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = 50;
			rect.height =75;
			layer = 100;
		}

		bool draw() {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["player"], true);
			return true;
		}

		void update(const char key[], const std::vector<std::shared_ptr<BlockObject>> blockList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			const double diffX = x - prevX;
			const double diffY = y - prevY;

			double acX = -0.3 * (1 - (diffX <= 0) - (diffX < 0));
			double acY = 2;

			if (key[KEY_INPUT_UP] && !isJumping) {
				acY = -25;
			}
			isJumping = true;

			if (key[KEY_INPUT_RIGHT]) {
				acX = 0.8 * (diffX < 8);
			}
			if (key[KEY_INPUT_LEFT]) {
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

		void init() {
			rect.x = prevX = initX;
			rect.y = prevY = initY;
		}
	};

	class GoalObject : public Object {
	public:
		GoalObject(int x_, int y_) {
			rect = Rectan(x_, y_, 100, 100);
			layer = 100;
		}

		bool draw() {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["luigi"], true);
			return true;
		}
	};

	class NeedleObject : public Object {
		const double vx;
		const double vy;
		bool willExist = true;

	public:
		NeedleObject(int x, int y, int size, double vx_, double vy_) : vx(vx_), vy(vy_){
			rect = Rectan(x, y, size, size);
			layer = 50;
		}
		bool draw() {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["wanwan"], true);
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

	std::thread thread;
	std::shared_ptr<Player> player;
	std::shared_ptr<GoalObject> goal;
	std::vector<std::shared_ptr<BlockObject>> blockList;
	std::vector<std::shared_ptr<NeedleObject>> needleList;

	void setBlock(int x, int y, int width, int height, bool willStay) {
		auto block = std::make_shared<BlockObject>(x, y, width, height, willStay);
		blockList.push_back(block);
		drawList.push_back(block);
	}
	void setNeedle(int x, int y, int size, double vx, double vy) {
		auto needle = std::make_shared<NeedleObject>(x, y, size, vx, vy);
		needleList.push_back(needle);
		drawList.push_back(needle);
	}
	void setPlayer(int x, int y) {
		player = std::make_shared<Player>(x, y);
		drawList.push_back(player);
	}
	void setGoal(int x, int y) {
		goal = std::make_shared<GoalObject>(x, y);
		drawList.push_back(goal);
	}

	void makeStageBase() {
		drawList.clear();
		blockList.clear();
		needleList.clear();

		drawList.push_back(std::make_shared<Background>(share.handle));

		setBlock(0, 700, 1280, 100, true);
		setBlock(-50, 0, 100, 720, true);
		setBlock(1230, 0, 100, 720, true);
		setBlock(0, -50, 1280, 100, true);
	}

public:
	PuzzleGame() {
		thread = std::thread::thread(capture, std::ref(share));
	}

	bool onStart() {
		using namespace std;
		fps.isShow = true;

		// mode 0: opening
		mode.setMode([this]() {
			setBlock(300, 300, 200, 200, true);
		}, 3 * FPS);

		// mode 1
		mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, 400);
			setGoal(50, 100);
			setBlock(0, 360, 1000, 50, true);//mannaka
			setBlock(1100, 0, 50, 720, true);//migi

			setBlock(1165, 100, 50, 50, true);//switch
			setBlock(1165, 250, 50, 50, true);
			setBlock(1165, 400, 50, 50, true);
			setBlock(1165, 550, 50, 50, true);

			setBlock(180, 600, 100, 100, true);//hidaridaiza
			setBlock(1000, 600, 100, 100, true);//migidaiza

			setBlock(900, 250, 100, 50, true);//migiuedaiza

			setBlock(50, 200, 150, 100, true);//goaldaiza

			setBlock(600, 600, 100, 50, true);//shitanaka

			setBlock(550, 250, 100, 50, true);//uenaka

			//if (switchA) {
			setBlock(400, 600, 100, 50, true);//shitahidari

			setBlock(890, 100, 10, 200, true);//bariamigi
			//}
			//if (switchB) {
			setBlock(800, 600, 100, 50, true);//shitamigi

			setBlock(750, 200, 100, 50, true);//uemigi
			//}
			//if (switchC) {
			setBlock(540, 100, 10, 200, true);//barianaka
			setNeedle(900, 500, 100, -6, 0);
			//}
			//if (switchD) {
			setBlock(300, 200, 100, 50, true);//uehidari
			setNeedle(850, 450, 150, -4, 0);
			//}
			//while(onUpdate() == true){
				setNeedle(400, 650, 50, 0, -3);
			//}
			int i;
			for (i = 6; i < 20; i++) {
				setNeedle(50*i, 650, 50, 0, 0);
			}
			for (i = 1; i < 20; i++) {
				setNeedle(50 * i, 300, 50, 0, 0);
			}

			//if () {
				setNeedle(50, 150, 50, 5,0);
				setNeedle(550, 650, 50, 0, -5);
				setNeedle(750, 410, 50, 0, 5);
			//}
			
			
		}, -1);

		// mode 2
		mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, 100);
			setGoal(1000, 600);
			setNeedle(400, 400, 100, 0, 0);
		}, -1);

		// mode 3
		mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, 100);
			setGoal(1000, 600);
			setNeedle(400, 400, 100, 0, 0);
		}, -1);

		// mode 4
		mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, 100);
			setGoal(1000, 600);
			setNeedle(400, 400, 100, 0, 0);
		}, -1);

		// mode 5
		mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, 100);
			setGoal(1000, 600);
			setNeedle(400, 400, 100, 0, 0);
		}, -1);

		// mode 6
		mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, 100);
			setGoal(1000, 600);
			setNeedle(400, 400, 100, 0, 0);
		}, -1);

		// result
		mode.setMode([this](){
			drawList.clear();
		}, -1);

		return Game::onStart();
	}

	bool onUpdate() {
		// modeに応じて
		switch (mode.getMode()) {
		case 0: { 
			return Game::onUpdate();
		}
		case 1: {

			break;
		}

		default:
			if (key[KEY_INPUT_RETURN]) {
				share.willFinish = true;
			}
			return Game::onUpdate();
		}

		if (player->isContacted(goal)) {
			mode.goNext();
		}

		// 全ステージ共通
		std::vector<std::shared_ptr<BlockObject>> allBlockList = blockList;
		share.rectMutex.lock();
		for (auto rect : share.rects) {
			auto block = std::make_shared<BlockObject>(rect, false);
			allBlockList.push_back(block);
			drawList.push_back(block);
		}
		share.rectMutex.unlock();

		// needleList
		for (auto& itr = needleList.begin(); itr != needleList.end();) {
			if ((*itr)->update(allBlockList)) {
				++itr;
			} else {
				itr = needleList.erase(itr);
			}
		}
		for (auto needle : needleList) {
			if (needle->isContacted(player)) {
				player->init();
			}
		}

		player->update(key, allBlockList);

		if (key[KEY_INPUT_ESCAPE]) {
			share.willFinish = true;
		}

		return Game::onUpdate();
	}

	bool onFinish() {
		thread.join();
		return true;
	}
};
