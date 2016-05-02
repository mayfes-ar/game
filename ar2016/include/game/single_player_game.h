#pragma once

#include "game/game.h"

class SinglePlayerGame : public Game {

	class Background : public Object {
		int& handle;

	public:
		Background(int& handle_) : handle(handle_) {
			layer = 0;
		}

		bool draw() {
			// DrawExtendGraph(CAP2IMG_SHIFT_X, CAP2IMG_SHIFT_Y, CAP2IMG_SHIFT_X + CAP2IMG_RATE*CAP_WIDTH, CAP2IMG_SHIFT_Y + CAP2IMG_RATE*CAP_HEIGHT, handle, FALSE);
			SetDrawBright(40, 40, 40);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["ar2016_logo"], true);
			SetDrawBright(255, 255, 255);
			return true;
		}
	};

	// BGM の処理
	class BGM : public Object {
		int mode;
		bool hasPlayerWon = true;
	public:
		BGM(int mode_) {
			mode = mode_;
		}
		BGM(int mode_, bool hasPlayerWon_) {
			mode = mode_;
			hasPlayerWon = hasPlayerWon_;
		}

		bool draw() {
			// 曲名でも画面のどこかに表示する？
			return true;
		}

		void start() {
			switch (mode) {
			case 0: {
				break;
			}
			case 1: {
				PlaySoundMem(soundHandles["s_game_bgm"], DX_PLAYTYPE_LOOP, true);
				break;
			}
			case 2: {
				if (hasPlayerWon) {

				}
				else {
					PlaySoundMem(soundHandles["s_game_over"], DX_PLAYTYPE_BACK, true);
				}
				break;
			}
			default: {
				break;
			}
			}
		}

		void stop() {
			switch (mode) {
			case 0: {
				break;
			}
			case 1: {
				StopSoundMem(soundHandles["s_game_bgm"]);
				break;
			}
			case 2: {
				StopSoundMem(soundHandles["s_game_over"]);
				break;
			}
			default: {
				break;
			}
			}
		}

		// プレイヤーが死んだときの効果音
		void playDeadSound() {
			PlaySoundMem(soundHandles["s_game_dead"], DX_PLAYTYPE_NORMAL, true);
		}
	};

	class SingleGameObject : public Object {
	public:
		bool draw() {
			return true;
		}
		double rightHit() const { return right() - rect.width / 3; }
		double leftHit() const { return left() + rect.width / 3; }
		double topHit() const { return top() + rect.height / 3; }
		double bottomHit() const { return bottom() - rect.height / 3; }
	};

	class SingleGameBlockObject : public SingleGameObject{
	protected:
		bool willStay;
	public:
		bool canHit = true;

		SingleGameBlockObject() {}
		SingleGameBlockObject(int x_, int y_, int width_, int height_, bool willStay_) {
			rect.x = x_, rect.y = y_, rect.width = width_, rect.height = height_;
			willStay = willStay_;
			layer = 20;
		}

		SingleGameBlockObject(Rectan rect_, bool willStay_) {
			rect = rect_;
			willStay = willStay_;
			layer = 20;
		}

		virtual bool draw() {
			if (willStay) {
				DrawBox(left(), top(), right(), bottom(), GetColor(240, 117, 50), true);
			}
			else {
				DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["block"], true);
			}
			return willStay;
		}


	};

	// 認識したマーカーの扱い
	class Marker : public SingleGameObject {
		int index = 0;

	public:
		Marker(int x_, int y_, int width_, int height_, bool willStay_) {
			rect.x = x_, rect.y = y_, rect.width = width_, rect.height = height_;
			layer = 101;
		}

		Marker(Rectan rect_, bool willStay_, int index_) {
			rect = rect_;
			index = index_;
			if (rect.width > 0 || rect.height > 0) {
				rect.width = 100;
				rect.height = 100;
			}
			else {
				rect.x = rect.y = -300;
			}
			layer = 101;
		}

		bool draw() {
			switch (index)
			{
			case 0: {
				drawWithRect(imgHandles["s_game_marker"]);
				// DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_marker"], true);
				break;
			}
			case 1: {
				drawWithRect(imgHandles["p_goal"]);
				// DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["p_goal"], true);
				break;
			}
			default:
				drawWithRect(imgHandles["block"]);
				// DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["block"], true);
				break;
			}
			return false;
		}
	};

	class Character : public SingleGameObject {
	public:
		const int imgHandle;
		const int maxDamage;

		double prevX;
		double prevY;
		bool isJumping = true;

		enum MoveDirection // x軸方向のみ
		{
			LEFT, NOMOVE, RIGHT,
		};
		MoveDirection moveDirection = NOMOVE;

		bool isAlive = true;
		int damage = 0;
		int invincibleTime = 0;

		const SinglePlayerGame& game;

		Character(int x_, int y_, int width_, int height_, std::string imgHandleKey_, int maxDamage_, SinglePlayerGame& game_) :imgHandle(imgHandles[imgHandleKey_]), maxDamage(maxDamage_),game(game_) {
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = width_;
			rect.height = height_;
		}

		bool draw() {
			switch (moveDirection)
			{
			case LEFT: {
				drawWithRect(imgHandle, 0, true);
				break;
			}
			case NOMOVE: {
				drawWithRect(imgHandle);
				break;
			}
			case RIGHT: {
				drawWithRect(imgHandle);
				break;
			}
			default:
				break;
			}
			return true;
		}

		virtual void update() {
			updateCoordinate(0, 0);
			moveBecauseBlockCollision(game.blockList);
			moveBecauseMarkerCollision(game.markerList);
		}

		void updateCoordinate(double acX, double acY) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			const double diffX = 40 * tanh((x - prevX) / 40);
			const double diffY = 40 * tanh((y - prevY) / 40);


			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;
		}

		virtual void setAc(double* acX, double* acY) {
			*acX = -0.5 * (1 - (rect.x - prevX <= 0) - (rect.x - prevX < 0));
			*acY = 3.2;
		}

		void moveBecauseBlockCollision(const std::vector<std::shared_ptr<SingleGameBlockObject>> objectList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			for (auto block : objectList) {
				if (left() < block->right() && top() < block->bottom() &&
					right() > block->left() && bottom() > block->top()) {

					if (prevY < block->bottomHit() && prevY + height > block->topHit()) {
						if (prevX >= block->rightHit()) {
							x = block->right();
						}
						else if (prevX + width <= block->leftHit()) {
							x = block->left() - width;
						}
						else {
							// TODO
						}
					}
					else {
						if (prevY >= block->bottomHit()) {
							y = block->bottom();
						}
						else if (prevY + height <= block->topHit()) {
							y = block->top() - height;
							isJumping = false;
						}
						else {
							// TODO
						}
					}
				}
			}
		}

		void moveBecauseMarkerCollision(const std::vector<std::shared_ptr<Marker>> objectList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			for (auto block : objectList) {
				if (left() < block->right() && top() < block->bottom() &&
					right() > block->left() && bottom() > block->top()) {

					if (prevY < block->bottomHit() && prevY + height > block->topHit()) {
						if (prevX >= block->rightHit()) {
							x = block->right();
						}
						else if (prevX + width <= block->leftHit()) {
							x = block->left() - width;
						}
						else {
							// TODO
						}
					}
					else {
						if (prevY >= block->bottomHit()) {
							y = block->bottom();
						}
						else if (prevY + height <= block->topHit()) {
							y = block->top() - height;
							isJumping = false;
						}
						else {
							// TODO
						}
					}
				}
			}
		}

		bool deathDecision() {
			
			if (rect.x < game.field.at("left") || rect.x > game.field.at("right")) {
				isAlive = false;
			}
			if (rect.y < game.field.at("top") || rect.y > game.field.at("bottom")) {
				isAlive = false;
			}
			
			return !isAlive;
		}
	};

	// 敵キャラクター
	class Enemy : public Character {
	public:
		Enemy(int x_, int y_, int width_, int height_, std::string imgHandleKey_, int maxDamage_, SinglePlayerGame& game_) : Character(x_,y_,width_,height_,imgHandleKey_,maxDamage_,game_) {
			layer = 101;
		}

		virtual void update() {
			double acX = 0;
			double acY = 0;
			setAc(&acX, &acY);
			updateCoordinate(acX, acY);
			moveBecauseBlockCollision(game.blockList);
			moveBecauseMarkerCollision(game.markerList);

			moveDirection = acX > 0 ? RIGHT : acX < 0 ? LEFT : NOMOVE;
		}
		
		virtual void setAc(double* acX, double* acY) {
			
		}

		// 死亡判定。マーカーで殴られたら死ぬ。
		bool deathDecision() {
			// この場合、ブロックに当たったら死ぬ、的な

			if (invincibleTime == 0) {
				for (auto marker : game.markerList) {
					if (left() < marker->right() && top() < marker->bottom() &&
						right() > marker->left() && bottom() > marker->top()) {
						if (isAlive) {
							PlaySoundMem(soundHandles["s_game_attack"], DX_PLAYTYPE_BACK, true);
						}
						isAlive = damageControl();
						invincibleTime = 15;

						return !isAlive;

					}
				}

			}
			else if (invincibleTime > 0) {
				invincibleTime--;
			}

			return Character::deathDecision();
		}

		bool getIsAlive() {
			return isAlive;
		}

		// ダメージが何点以上で死亡とするか。返り値は 死：false, 生：true
		bool damageControl() {
			damage++;
			return damage < maxDamage;
		}
	};

	class Teresa : public Enemy {
		int width = 100;
		int height = 100;

	public:
		Teresa(int x_, int y_, SinglePlayerGame& game_, double size, int maxDamage_ = 2, std::string imgHandleKey_ = "s_game_teresa") : Enemy(x_, y_, 100*size, 100*size, imgHandleKey_, maxDamage_, game_) {
			layer = 101;
		}

		void setAc(double* acX, double* acY) {
			*acX = rand() % 5 - 2;
			*acY = rand() % 5 - 2;
		}
		
	};

	class Player : public Character {

	public:
		Player(int x_, int y_, int width_, int height_, std::string imgHandleKey_, int maxDamage_, SinglePlayerGame& game_) : Character(x_, y_, width_, height_, imgHandleKey_, maxDamage_, game_) {
			layer = 100;
		}

		bool draw() {
			DrawString(50, 50, std::to_string(damage).c_str(), GetColor(255, 255, 255));
			return Character::draw();
		}

		void update(const char key[]) {
			double acX = 0;
			double acY = 0;
			setAc(&acX, &acY, key);
			updateCoordinate(acX, acY);
			moveBecauseBlockCollision(game.blockList);
			moveBecauseMarkerCollision(game.markerList);
		}

		void updateCoordinate(double acX, double acY) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			const double diffX = x - prevX;
			const double diffY = y - prevY;


			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;
		}

		// プレイヤーキャラクターの座標更新
		void setAc(double* acX, double* acY, const char key[]) {
			const double diffX = rect.x - prevX;
			*acX = -0.5 * (1 - (diffX <= 0) - (diffX < 0));
			*acY = 4;

			if (key[KEY_INPUT_UP] && !isJumping) {
				*acY = -40;
				PlaySoundMem(soundHandles["s_game_jump"], DX_PLAYTYPE_BACK, true);
			}
			isJumping = true;

			if (key[KEY_INPUT_RIGHT]) {
				*acX = 1.0 * (diffX < 10);
				moveDirection = RIGHT;
			}
			if (key[KEY_INPUT_LEFT]) {
				*acX = -1.0 * (diffX > -10);
				moveDirection = LEFT;
			}
		}

		// プレイヤーキャラクターの生死判定更新
		bool deathDecision(std::vector<std::shared_ptr<Enemy>> enemyList) {
			if (invincibleTime == 0) {
				for (auto enemy : enemyList) {
					if (left() <= enemy->right() && top() <= enemy->bottom() &&
						right() >= enemy->left() && bottom() >= enemy->top()) {
						damage += 1;
						invincibleTime = 50;
						break;
					}
				}
			}
			else if (invincibleTime > 0) {
				invincibleTime--;
			}
			/*
			if (damage > 5) {
			isAlive = false;
			}
			*/
			return Character::deathDecision();
		}
	};

	std::thread thread;
	std::shared_ptr<Player> player;

	std::vector<std::shared_ptr<SingleGameBlockObject>> blockList;
	std::vector<std::shared_ptr<Enemy>> enemyList;
	std::vector<std::shared_ptr<Marker>> markerList;

	const std::map<std::string, int> field = {
		{ "left", 0 - 200 },
		{ "right", WIDTH + 200 },
		{ "top", 0 - 200 },
		{ "bottom", HEIGHT + 200 }
	};

	std::shared_ptr<Teresa> makeTeresa(int x = 0, int y = 0, double size = 1.0) {
		if (x == 0 && y == 0) {
			x = rand() % (WIDTH-100) + 50;
			y = rand() % (HEIGHT-100) + 50;
		}
		auto enemy = std::make_shared<Teresa>(x, y, *this, size);
		if (player->isContacted(enemy)) {
			enemy = std::make_shared<Teresa>(x, y - 100, *this, size);
		}
		enemyList.push_back(enemy);
		drawList.push_back(enemy);
		return enemy;
	}

	std::shared_ptr<BGM> bgm;
	const int MAX_TIME = 30 * 40;
	int timer = MAX_TIME;
	bool hasPlayerWon;

public:
	SinglePlayerGame() {
		thread = std::thread::thread(capture, std::ref(share));
		player = std::make_shared<Player>(WIDTH / 2 - 100 / 2, HEIGHT / 2 - 150 / 2, 75, 100, "s_game_player", 5, *this);
		hasPlayerWon = true;
	}

	bool onStart() {
		using namespace std;
		fps.isShow = true;

		srand((unsigned int)time(NULL));

		// mode 0
		mode.setMode([this]() {
			class Title : public Object {
			public:
				Title() {
					layer = 50;
				}

				bool draw() {
					DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_op"], true);
					DrawExtendGraph(WIDTH / 2 - 429 / 2, 30, WIDTH / 2 + 429 / 2, 30 + 47, imgHandles["s_game_op_title"], true);
					// DrawExtendGraph(WIDTH/2-50, 400, WIDTH/2+50, 400+150, imgHandles["s_game_player"], true);
					return true;
				}
			};

			drawList.clear();
			drawList.push_back(make_shared<Title>());

			bgm = make_shared<BGM>(0);
			bgm->start();

			for (int i = 0; i < 20; i++);
		}, 150);

		// mode 1
		mode.setMode([this]() {
			drawList.clear();
			auto makeBlock = [this](int x, int y, int width, int height) {
				auto block = make_shared<SingleGameBlockObject>(x, y, width, height, true);
				blockList.push_back(block);
				drawList.push_back(block);
			};
			makeBlock(0 - 200, 600, WIDTH + 200 + 200, 50);



			drawList.push_back(player);
			drawList.push_back(make_shared<Background>(share.handle));

			bgm = make_shared<BGM>(1);
			bgm->start();

		}, MAX_TIME);

		// mode 2
		mode.setMode([this]() {
			class Title : public Object {
				bool hasPlayerWon = true;
			public:
				Title(bool hasPlayerWon_) {
					layer = 50;
					hasPlayerWon = hasPlayerWon_;
				}

				bool draw() {
					if (hasPlayerWon) {
						DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_clear"], true);
					}
					else {
						DrawExtendGraph(WIDTH / 2 - 400, 30, WIDTH / 2 + 400, 30 + 296, imgHandles["s_game_result_dead"], true);
						DrawExtendGraph(WIDTH / 2 - 75, 400, WIDTH / 2 + 75, 400 + 150, imgHandles["s_game_dead"], true);
					}
					return true;
				}
			};

			drawList.clear();
			drawList.push_back(make_shared<Title>(hasPlayerWon));

			bgm->stop();
			bgm = make_shared<BGM>(2, hasPlayerWon);
			bgm->start();
		}, -1);

		return Game::onStart();
	}

	bool onUpdate() {
		bool willFinishMode = false;


		switch (mode.getMode()) {
		case 0: { // イントロダクション
			static int counterForWaiting = 10;
			if (key[KEY_INPUT_RETURN] && counterForWaiting == 0) {
				willFinishMode = true;
			}
			if (counterForWaiting > 0) {
				counterForWaiting--;
			}
			break;
		}
		case 1: { // playing
			timer -= 1;
			if (timer <= 0) {
				willFinishMode = true;
			}


			// 認識したマーカーを描画
			share.rectMutex.lock();
			markerList.clear();
			markerList.shrink_to_fit();
			for (int i = 0; i < share.rects.size(); i++) {
				auto marker = std::make_shared<Marker>(share.rects[i], false, i);
				markerList.push_back(marker);
				drawList.push_back(marker);
			}
			share.rectMutex.unlock();

			player->update(key);


			for (auto enemy : enemyList) {
				enemy->deathDecision();
				enemy->update();
			}


			if (player->deathDecision(enemyList)) {
				bgm->stop();
				bgm->playDeadSound();
				hasPlayerWon = false;
				willFinishMode = true;
			}

			// 敵の出現を管理する
			switch (MAX_TIME - timer) {
			case 30:
			case 60:
			case 90:
			case 1: {
				makeTeresa(WIDTH / 2, HEIGHT / 2);
				break;
			}
			default: {

				break;
			}

			}

			// 定期的に実行する場合など
			
			break;

		}

		case 2: { // リザルト画面
			if (key[KEY_INPUT_RETURN]) {
				willFinishMode = true;
			}
			break;
		}

		default:

			break;
		}


		if (key[KEY_INPUT_ESCAPE]) {
			share.willFinish = true;
		}
		if (key[KEY_INPUT_N]) {
			willFinishMode = true;
		}


		for (auto& itr = enemyList.begin(); itr != enemyList.end();) {
			if ((*itr)->getIsAlive()) {
				++itr;
			}
			else {
				itr = enemyList.erase(itr);
			}
		}

		if (willFinishMode) {

			mode.goNext();
		}

		return Game::onUpdate();
	}

	bool onFinish() {
		bgm->stop();

		thread.join();
		return true;
	}
};
