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
		makeStageBase( );
		setPlayer(100, -300);
		setGoal(1100, 550);

		setBlock(0, 520, 440, 200);
		setBlock(440, 200, 270, 500);
		setBlock(1000, 220, 300, 20);
		setCoin(1030, 100);
		setCoin(1110, 100);

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
		setBlock(0, 500, 200, 50);
		setSwitch(500, 0, 50, setBlock(200, 500, 50, 200)->canHit, true);
		setWarp(100, 570, 50, 50, 1100, -100);
	}, -1);

	// mode 4
	mode.setMode([this]() {
		makeStageBase(false);
		setPlayer(100, -100);
		setGoal(1100, 100);

		setWarp(-500, 800, 2500, 600, 100, -100);
		setWind(100, 200, 100, 400, 0, -3);
		setWind(400, 200, 100, 400, 0, -3);
		setWind(650, -200, 200, 1000, 0, 3);
		setWind(1000, 200, 100, 400, 0, -3);

	}, -1);

	
	// mode 5
	mode.setMode([this]() {
		makeStageBase(false);
		setPlayer(100, 90);
		setGoal(1000, 500);

		for (int i = 0; i * 50 <= 1200; i++) {
			setLoop(i * 50, 770, 50, 10, (i - 1) * 50, 0);
			setLoop(i * 50, -10, 50, 10, (i - 1) * 50, 670);
		}

		setDamage(900, 450, 60);
		setDamage(960, 450, 60);
		setDamage(1020, 450, 60);
		setDamage(1080, 450, 60);
		setDamage(1140, 450, 60);
		setDamage(1200, 450, 60);
		setDamage(900, 500, 50);
		setDamage(900, 550, 50);
		setDamage(900, 600, 50);
		setDamage(900, 650, 50);
		setDamage(900, 700, 50);
		setDamage(1150, 500, 50);
		setDamage(1150, 550, 50);
		setDamage(1150, 600, 50);
		setDamage(1150, 650, 50);
		setDamage(1150, 700, 50);
		setDamage(800, 100, 50);
		setBlock(0, 200, 400, 20);
		setBlock(200, 150, 70, 70);
		setBlock(440, 400, 24, 240);
		setBlock(700, 400, 24, 240);
		setBlock(464, 400, 236, 24);
		setBlock(464, 616, 236, 24);
		setWarp(680, 424, 20, 20, 600, 300);
		setWarp(1220, 680, 20, 20, 530, 450);
		setCoin(40, 240);
		setCoin(120, 240);
		setCoin(200, 240);
		setCoin(280, 240);
		setCoin(320, 500);
		setCoin(550, 430, 60, 500);

		setWarp(-500, 1000, 2500, 600, 100, -100);
		setBlock(-500, 1600, 2500, 600);

	}, -1);

	// mode 6
		mode.setMode([this]() {
		makeStageBase(false);
		setPlayer(100, -100);
		setGoal(1100, 100);

		setWarp(-500, 800, 2500, 600, 100, -100);
		setWind(100, 200, 100, 400, 0, -3);
		setWind(400, 200, 100, 400, 0, -3);
		setWind(650, -200, 200, 1000, 0, 3);
		setWind(1000, 200, 100, 400, 0, -3);

	}, -1);

	// result
	mode.setMode([this]() {
		drawList.clear();
		gimmicks.clear();

		score->setResultDraw();

		drawList.push_back(score);

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
	// <---各ステージ--->
	switch (mode.getMode()) {
	case 0: { // explain
		if (key[KEY_INPUT_UP]) {
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
	case 4: {
		break;
	}
	default: // result
		if (key[KEY_INPUT_RETURN]) {
			share.willFinish = true;
		}
		return Game::onUpdate();
	}

	// <---全ステージ共通--->
	

	// 初めてゴールに触れた時だけ
	if (goal->check(player)) {
		funcTimer.set([this]() { mode.goNext(); }, FPS * 2);
	}

	allBlocks = stageBlocks;
	
	share.rectMutex.lock();
	markerBlock = std::make_shared<MarkerBlock>(share.rects[0]);
	allBlocks.push_back(markerBlock);
	drawList.push_back(markerBlock);
	share.rectMutex.unlock();

	// gimmicks作用前の準備
	player->preUpdate();

	// gimmick
	for (auto& itr = gimmicks.begin(); itr != gimmicks.end();) {
		if ((*itr)->update()) {
			++itr;
		} else {
			itr = gimmicks.erase(itr);
		}
	}

	// gimmicks作用後の位置計算
	player->update();

	if (key[KEY_INPUT_ESCAPE]) {
		share.willFinish = true;
	}

	return Game::onUpdate();
}


void PuzzleGame::Player::update() {
	if (isReached) { return; }
	updateFunc();
	updateFunc = []() {};

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
