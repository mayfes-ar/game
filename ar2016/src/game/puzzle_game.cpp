#include "game/puzzle_game.h"


bool PuzzleGame::onStart() {
	using namespace std;
	fps.isShow = true;
	
	// mode 0: opening
	mode.setMode([this]() {
		setBlock(0, 700, 1280, 100, true);
		setBlock(-50, -720, 100, 720 * 2, true);
		setBlock(1230, -720, 100, 720 * 2, true);

		auto explanation = std::make_shared<Explanation>();
		drawList.push_back(explanation);
		setSwitch(500, 550, 100, explanation->isFirst, true);
		setPlayer(100, 600);
		setGoal(1100, 555);
	}, -1);
	
	// mode 1
	mode.setMode([this]() {
		makeStageBase();
		drawList.push_back(std::make_shared<CurtainObject>(true));
		timer->startPlayingBGM(2);

		setPlayer(100, -100);
		setGoal(1080, 555);

		setBlock(0, 520, 540, 200);
		setBlock(540, 200, 170, 500);
		setBlock(1000, 220, 300, 100);
		setCoin(1050, 130);
		setCoin(1130, 130);
		setCoin(1090, 50);

	}, -1);
	
	// mode 2
	mode.setMode([this]() {
		makeStageBase();
		setPlayer(100, -300);
		setGoal(1080, 550);

		setBlock(50, 300, 400, 100);
		setSwitch(1000, 200, 80, setBlock(450, 300, 400, 100)->canHit, true);
		setBlock(850, 300, 400, 100);
		setCoin(1010, 80);
		setCoin(60, 420);
		setCoin(160, 420);
		setCoin(260, 420);
		setCoin(340, 420);

		setWarp(50, 620, 100, 100, 1150, 0, true);

		for (int i = 0; i < 40; i++){
			setSmog();
		}

	}, -1);
	
	// mode 3
	mode.setMode([this]() {
		makeStageBase();

		setPlayer(50, 580);
		setGoal(100, 80);
		
		setWind(0, 240, 840, 80, 3, 0);
		setWind(440, 560, 540, 120, -3, 0);
		setBlock(0, 300, 1100, 40);
		setBlock(200, 560, 290, 220);

		for (int i = 580; i < 700; i += 20) {
			setDamage(482, i, 20);
		}

		setBlock(700, 0, 40, 190);
		for (int i = 0; i < 180; i += 20) {
			setDamage(692, i, 20);
		}
		setCoin(600, 40);

		setBlock(880, 540, 240, 20);
		setBlock(620, 540, 120, 20);
		setDamage(780, 320, 40);
		setBlock(1050, 340, 50, 200);
		setCoin(500, 600);
		setCoin(900, 440);
		setCoin(980, 440);
		setCoin(900, 370);
		setCoin(980, 370);


		setWind(1040, 150, 140, 51, -2, 0);
		setCoin(1120, 10);

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

		setCoin(420, 500);
		setCoin(1020, 450);
		setCoin(1020, 370);
		setCoin(1020, 290);
		setCoin(670, 550);
		setCoin(770, 550);
		setCoin(670, 50);
		setCoin(770, 50);

	}, -1);

	// mode 5
	mode.setMode([this]() {
		makeStageBase(false);
		setPlayer(100, 90);
		setGoal(950, 500);

		for (int i = 0; i * 50 <= WIDTH; i++) {
			setLoop(i * 50, 780, 50, 10, (i - 1) * 50, 0);
			setLoop(i * 50, -10, 50, 10, (i - 1) * 50, 670);
		}

		setDamage(905, 450, 30);
		setDamage(965, 450, 30);
		setDamage(1025, 450, 30);
		setDamage(1085, 450, 30);
		setDamage(1145, 450, 30);
		setDamage(900, 500, 30);
		setDamage(900, 550, 30);
		setDamage(900, 600, 30);
		setDamage(900, 650, 30);
		setDamage(900, 700, 30);
		setDamage(1150, 500, 30);
		setDamage(1150, 550, 30);
		setDamage(1150, 600, 30);
		setDamage(1150, 650, 30);
		setDamage(1150, 700, 30);
		setBlock(20, 200, 400, 20);
		setBlock(200, 150, 70, 70);
		setBlock(440, 400, 24, 240);
		setBlock(700, 400, 24, 240);
		setBlock(464, 400, 236, 24);
		setBlock(464, 616, 236, 24);
		setWarp(620, 424, 60, 60, 600, 300, true);
		setWarp(1200, 650, 60, 60, 520, 500, true);
		setCoin(40, 240);
		setCoin(120, 240);
		setCoin(200, 240);
		setCoin(280, 240);
		setCoin(320, 420);
		setCoin(550, 430, 60, 500);

		setWarp(-500, 1000, 2500, 600, 100, -100);
		setBlock(-500, 1600, 2500, 600);

	}, -1);
	
	// mode 6
	mode.setMode([this]() {
		makeStageBase(false);
		setPlayer(10, 401);
		setGoal(1100, 250);

		setBlock(0, 0, 450, 150, true);//hidariue

		setBlock(100, 390, 100, 400, true);//hidarishita
		setBlock(0, 540, 200, 400, true);

		setBlock(800, 10, 700, 140, true);//migiue
		setBlock(1300, -500, 50, 500, true);

		setBlock(300, 390, 220, 10, true);
		setBlock(300, 400, 500, 100, true);//shita
		setBlock(650, 500, 50, 300, true);
		setBlock(320, 660, 350, 150, true);

		setBlock(900, 400, 400, 100, true);//migishita
		setBlock(1250, 500, 300, 400, true);

		setCoin(350, 590);//shita
		setCoin(450, 590);
		setCoin(550, 590);

		setCoin(900, -58);//migiue
		setCoin(1000, -58);
		setCoin(1100, -58);

		setCoin(820, 420);

		setSwitch(900, 550, 100, setBlock(900, 150, 25, 250, false)->canHit, true);
		setSwitch(1150, 550, 100, setBlock(925, 150, 25, 250, false)->canHit, true);

		setFuncSwitch(700, 550, 100, [this]() {funcTimer.set([this]() { setDamage(700, -100, 100, 0, 2); }, 1); });
		setFuncSwitch(1150, 550, 100, [this]() {funcTimer.set([this]() {setDamage(-100, 150, 240, 3, 0); }, 1); });

		setWarp(-1000, 1000, 3000, 100, 10, 400);//shita
	}, -1);

	// result
	mode.setMode([this]() {
		drawList.clear();
		gimmicks.clear();
		timer->stopPlayingBGM();
		score->setResultDraw();

		drawList.push_back(std::make_shared<CurtainObject>(true));
		drawList.push_back(score);
		drawList.push_back(std::make_shared<ResultObject>(*this));

	}, -1);

	return Game::onStart();
}


bool PuzzleGame::onUpdate() {
	if (key[KEY_INPUT_ESCAPE]) {
		share.willFinish = true;
	}
	funcTimer.update();
	counter++;
	if (counter == 3600) {
		counter = 0;
	}

	// modeに応じて
	// <---各ステージ--->
	switch (mode.getMode()) {
	case 0: { // explain
		
		break;
	}
	case 1: {
		
		break;
	}
	case 2: {
		if (counter % (FPS * 3) == 0) {
			setSmog();
		}
		
		break;
	}
	case 3: {
		/*if (counter % (FPS * 3) == 0) {
			setDamage(300, -100, 50, 5, 14);
		}
		if (2 * counter % (FPS * 6) == FPS * 3) {
			setDamage(400, -100, 50, -5, 14);
		}*/
		break;
	}
	case 4: {
		break;
	}
	case 5: {
		if (counter % (FPS * 2) == 0) {//Stage 5のギミック
			setDamage(800, -100, 50, 0, 10);
		}
		break;
	}
	case 6: {
		if (counter % (FPS / 2) == 0) {
			setDamage(225, 800, 50, 0, -5);
			setDamage(450, -100, 70, 0, 10);
		}
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
		if (mode.getMode() == 0 || mode.getMode() == 6) {
			drawList.push_back(std::make_shared<CurtainObject>(false));
			timer->stopPlayingBGM();
		}
		if (mode.getMode() > 0) {
			drawList.push_back(goal->goalEffect());
		}
		funcTimer.set([this]() { mode.goNext(); }, FPS * 2);
	}

	if (mode.getMode()) {
		if (timer->update()) {
			drawList.push_back(std::make_shared<CurtainObject>(false));
			funcTimer.set([this]() {mode.goLast();}, FPS);
		}
	}
	

	allBlocks = stageBlocks;
	
	share.rectMutex.lock();
	markerBlock = std::make_shared<MarkerBlock>(share.rects[1]);
	allBlocks.push_back(markerBlock);
	drawList.push_back(markerBlock);
	share.rectMutex.unlock();


	// gimmicks作用前の準備
	player->preUpdate();
	player->markerCheck(markerBlock);

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

	return Game::onUpdate();
}


void PuzzleGame::Player::update() {
	if (!isMovable) { return; }
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
		PlaySoundMem(soundHandles["ps_jump"], DX_PLAYTYPE_BACK);
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
	const double tempNextX = x;
	const double tempNextY = y;

	bool isSecond = false;
	while (true) {

		for (auto block : game.allBlocks) {
			if (block->canHit && isContacted(block)) {

				if (prevY < block->bottomHit() && prevY + height > block->topHit()) {
					if (prevX >= block->rightHit()) {
						x = prevX = block->right();
						onLeft = true;
					} else if(prevX + width <= block-> leftHit()) {
						x = prevX = block->left() - width;
						onRight = true;
					} else {
						init();
					}
				} else {
					if (prevY >= block->bottomHit()) {
						y = prevY = block->bottom();
						onTop = true;
					} else if(prevY + height <= block->topHit()) {
						y = prevY = block->top() - height;
						isJumping = false;
						onBottom = true;
					} else {
						init();
					}
				}
			}
		}

		if (isSecond) { break; }

		bool isFailure = false;
		for (auto block : game.stageBlocks) {
			if (isContacted(block) && block->canHit) { isFailure = true; }
		}

		if ((onTop && onBottom) || (onLeft && onRight) || isFailure) {
			isSecond = true;
			game.markerBlock->canHit = false;
			prevX = tempX; prevY = tempY;
			x = tempNextX; y = tempNextY;
			onTop = false; onBottom = false; onLeft = false; onRight = false;
		} else {
			break;
		}
	}

	if (y > 2000) {
		init();
	}
}
