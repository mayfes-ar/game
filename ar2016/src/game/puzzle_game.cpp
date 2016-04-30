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
			setNeedle(300, -100, 100, 0, 10);
			setNeedle(600, -100, 100, 0, 11);
			setNeedle(900, -100, 100, 0, 12);
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

	// 初めてゴールに触れた時だけ
	if (goal->check(player)) {
		funcTimer.set([this]() { mode.goNext(); }, FPS * 2);
	}

	// 全ステージ共通
	std::vector<std::shared_ptr<BlockObject>> allBlockList = stageBlockList;
	
	share.rectMutex.lock();
	markerBlock = std::make_shared<BlockObject>(share.rects[0], false);
	allBlockList.push_back(markerBlock);
	drawList.push_back(markerBlock);
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
		if ((*itr)->isContacted(markerBlock)) {
			(*itr)->vanish();
			hit = true;
			itr = smogList.erase(itr);
			break;
		}
		if (!hit) {
			itr++;
		}
	}

	player->update(allBlockList);

	if (key[KEY_INPUT_ESCAPE]) {
		share.willFinish = true;
	}

	return Game::onUpdate();
}


//void PuzzleGame::Player::update() {
//
//}
