#include "game/puzzle_game.h"


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

		setDamage(600, 300, 200);
		setBlock(400, 300, 300, 200, true);

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


//void PuzzleGame::Player::update() {
//
//}
