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

	class Explanation : public Object {
	public:
		bool draw() {
			DrawGraph(0, 0, imgHandles["p_explain"], false);
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

			bool onTop = false;
			bool onBottom = false;
			bool onLeft = false;
			bool onRight = false;

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
			static const int offset = rect.height/10;
			DrawExtendGraph(left() - offset, top() - offset, right() + offset, bottom() + offset, imgHandles["wanwan"], true);
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
			static const int offset = rect.width / 10;
			DrawExtendGraph(left() - offset, top() - offset, right() + offset, bottom() + offset, effectHandles["smog"][counter%13], true);
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
	std::vector<std::shared_ptr<BlockObject>> markerBlockList;
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
		player = std::make_shared<Player>(x, y);
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

	bool onStart() {
		using namespace std;
		fps.isShow = true;

		// mode 0: opening
		mode.setMode([this]() {
			drawList.push_back(std::make_shared<Explanation>());
		}, -1);

		// mode 1
		mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, -300);
			setGoal(1100, 600);
			
			setBlock(400, 300, 300, 500, true);

		}, -1);

		// mode 2
		mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, -300);
			setGoal(1100, 600);
		}, -1);

		// mode 3
		mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, -300);
			setGoal(1100, 100);
			for (int i = 0; i < 120; i++) {
				setSmog();
			}
		}, -1);

		// mode 4
	/*	mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, 100);
			setGoal(1000, 600);
			setNeedle(400, 400, 100, 0, 0);
		}, -1);*/

		// mode 5
		/*mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, 100);
			setGoal(1000, 600);
			setNeedle(400, 400, 100, 0, 0);
		}, -1);*/

		// mode 6
	/*	mode.setMode([this]() {
			makeStageBase();
			setPlayer(100, 100);
			setGoal(1000, 600);
			setNeedle(400, 400, 100, 0, 0);
		}, -1);*/

		// result
		mode.setMode([this](){
			drawList.clear();
			
		}, -1);

		return Game::onStart();
	}

	bool onUpdate() {
		timer++;
		if (timer == 3600) {
			timer = 0;
		}

		// modeに応じて
		switch (mode.getMode()) {
		case 0: { // explain
			if (key[KEY_INPUT_RETURN]) {
				mode.goNext();
			}
			return Game::onUpdate();
		}
		case 1: {
			
			break;
		}
		case 2: {
			if (timer % (FPS/2) == 0) {
				setNeedle(300, -100, 100, 0, 7);
				setNeedle(600, -100, 100, 0, 7);
				setNeedle(900, -100, 100, 0, 7);
			}
			break;
		}
		case 3: {
			if (timer % (FPS) == 0 && smogList.size() < 150) {
				setSmog();
			}
			break;
		}
		default: // result
			if (key[KEY_INPUT_RETURN]) {
				share.willFinish = true;
			}
			return Game::onUpdate();
		}

		if (player->isContacted(goal)) {
			mode.goNext();
		}

		// 全ステージ共通
		std::vector<std::shared_ptr<BlockObject>> allBlockList = stageBlockList;
		markerBlockList.clear();
		share.rectMutex.lock();
		for (auto rect : share.rects) {
			auto block = std::make_shared<BlockObject>(rect, false);
			markerBlockList.push_back(block);
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

		// smog
		for (auto& itr = smogList.begin(); itr != smogList.end();) {
			bool hit = false;
			for (auto mBlock : markerBlockList) {
				if ((*itr)->isContacted(mBlock)) {
					(*itr)->vanish();
					hit = true;
					itr = smogList.erase(itr);
					break;
				}
			}
			if (!hit) {
				itr++;
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
