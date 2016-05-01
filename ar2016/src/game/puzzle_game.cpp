﻿#include "game/puzzle_game.h"


bool PuzzleGame::onStart() {
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
		setGoal(1100, 550);

		setCoin(800, 100);
		setCoin(900, 100);
		setDamage(800, 300, 100);
		setBlock(400, 300, 300, 200);
		setBlock(450, 550, 100, 100, false);
		setSwitch(1000, 200, 60, setBlock(650, 550, 100, 100, false)->canHit);

	}, -1);

	// mode 2
	mode.setMode([this]() {
		makeStageBase();
		setPlayer(100, -300);
		setGoal(1100, 550);
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

	// result
	mode.setMode([this]() {
		drawList.clear();
		gimmicks.clear();

	}, -1);

	return Game::onStart();
}


bool PuzzleGame::onUpdate() {
	funcTimer.update();
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
		if (timer % (FPS / 2) == 0) {
			setDamage(300, -100, 100, 0, 10);
			setDamage(600, -100, 100, 0, 11);
			setDamage(900, -100, 100, 0, 12);
		}
		break;
	}
	case 3: {
		if (timer % (FPS) == 0 && gimmicks.size() < 150) {
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

	// 初めてゴールに触れた時だけ
	if (goal->check(player)) {
		funcTimer.set([this]() { mode.goNext(); }, FPS * 2);
	}

	// 全ステージ共通
	allBlocks = stageBlocks;
	
	share.rectMutex.lock();
	markerBlock = std::make_shared<MarkerBlock>(share.rects[0]);
	allBlocks.push_back(markerBlock);
	drawList.push_back(markerBlock);
	share.rectMutex.unlock();

	player->preUpdate();

	// gimmick
	for (auto& itr = gimmicks.begin(); itr != gimmicks.end();) {
		if ((*itr)->update()) {
			++itr;
		} else {
			itr = gimmicks.erase(itr);
		}
	}

	player->update();

	if (key[KEY_INPUT_ESCAPE]) {
		share.willFinish = true;
	}

	return Game::onUpdate();
}


void PuzzleGame::Player::update() {
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
	x += diffX + acX + exForceX;
	prevX = tempX;
	const double tempY = y;
	y += diffY + acY + exForceY;
	prevY = tempY;

	for (auto block : game.allBlocks) {
		if (block->canHit && isContacted(block)) {

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
