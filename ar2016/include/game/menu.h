#pragma once

//  #include "***_game.h"
#include "game/first_game.h"
#include "game/single_player_game.h"
#include "game/breakout_game.h"
#include "game/puzzle_game.h"


std::shared_ptr<Game> startMenu();

// メニュー画面
// gameType = std::make_shared<起動するGameの子クラス>();
class Menu : Game {

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
				}
				else {
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

	class MenuObject : public Object {
		int phase = 0;

	protected:
		void drawImage(int handle, int imageWidth, int imageHeight, int shiftX = 0, int shiftY = 0, bool isReverse = false) const {
			if (isReverse) {
				DrawExtendGraph(left() - shiftX + imageWidth, top() - shiftY, left() - shiftX, top() - shiftY + imageHeight, handle, true);
			}
			else {
				DrawExtendGraph(left() - shiftX, top() - shiftY, left() - shiftX + imageWidth, top() - shiftY + imageHeight, handle, true);
			}
		}

		void drawDark(std::function<void()> func) {
			SetDrawBright(100, 100, 100);
			func();
			SetDrawBright(255, 255, 255);
		}

		void drawWithRotation(int imgHandle, double angle, bool turnFlag = false, double size = 1.0) {
			int imageWidth = 0;
			int imageHeight = 0;
			GetGraphSize(imgHandle, &imageWidth, &imageHeight);
			imageWidth = imageWidth > 0 ? imageWidth : 1;
			imageHeight = imageHeight > 0 ? imageHeight : 1;
			DrawRotaGraph3((left() + right()) / 2, (top() + bottom()) / 2, imageWidth / 2, imageHeight / 2, rect.width / (double)imageWidth *size, rect.height / (double)imageHeight*size, angle, imgHandle, true, turnFlag);
		}

		class PendulumCounter {
			const int amplitude;
			int phase;
			int step;
		public:
			PendulumCounter(int amplitude_, int phase_ = 0) : amplitude(amplitude_ > 0 ? amplitude_ : amplitude_ * -1) {
				phase = phase_;
				step = 1;
			}

			int get() {
				int nowPhase = phase;
				if (phase == amplitude) {
					step = -1;
				}
				if (phase == -amplitude) {
					step = 1;
				}
				phase += step;

				return nowPhase;
			}

			double getAngle(bool increment = true) {
				return increment ? M_PI / 180 * get() : M_PI / 180 * phase;
			}

			void reset() {
				phase = 0;
			}
		};

		std::shared_ptr<PendulumCounter> makePendulum(int amplitude, int phase = 0) {
			return std::make_shared<PendulumCounter>(amplitude, phase);
		}

	public:
		double rightHit() const { return right() - rect.width / 3; }
		double leftHit() const { return left() + rect.width / 3; }
		double topHit() const { return top() + rect.height / 3; }
		double bottomHit() const { return bottom() - rect.height / 3; }

	};

	class Effect : public MenuObject {
		const std::vector<int> effectHandle;
		const bool willStay;
		const int maxCount;
		const int framePerCount;
		int counter;
		bool isRunning;

	public:
		Effect(std::string effectHandleKey_, int x_ = 0, int y_ = 0, int width_ = WIDTH, int height_ = HEIGHT, bool willStay_ = false, int layer_ = 150, int framePerCount_ = 1, int counter_ = 0) : effectHandle(effectHandles[effectHandleKey_]), willStay(willStay_), maxCount(effectHandles[effectHandleKey_].size()*(framePerCount_ > 0 ? framePerCount_ : 1)), framePerCount((framePerCount_ > 0 ? framePerCount_ : 1)) {
			rect.x = x_;
			rect.y = y_;
			rect.width = width_;
			rect.height = height_;
			counter = counter_*(framePerCount_ > 0 ? framePerCount_ : 1);
			layer = layer_;
			isRunning = true;
		}

		void changeIsRunning() {
			isRunning = false;
		}

		bool draw() {
			if (willStay) {
				if (counter == maxCount) {
					counter = 0;
				}
			}


			drawWithRect(effectHandle[counter / framePerCount]);
			counter++;

			return (willStay || counter != maxCount) && isRunning;
		}
	};

	// BGM の処理
	class BGM : public Object {
	public:
		bool draw() {
			// 曲名でも画面のどこかに表示する？
			return true;
		}

		void start() {
			PlaySoundMem(soundHandles["menu_bgm"] , DX_PLAYTYPE_LOOP, true);
		}

		void stop() {
			StopSoundMem(soundHandles["menu_bgm"]);
		}
	};

	// ゲーム選択の仕組み
	class SelectGame : public MenuObject {
		// 個々のゲーム情報を持つクラス
		class GameDescription :public MenuObject {
		public :
			int imageHandle;
			int descHandle;
			int backgroundHandle;
			shared_ptr<PendulumCounter> p = make_shared<PendulumCounter>(FPS/4);
			std::function<std::shared_ptr<Game>()> gameFunc;

			GameDescription(int imageHandle_, int descHandle_, int backgroundHandle_, int x, int y, int imageWidth_, int imageHeight_,  std::function<std::shared_ptr<Game>()> gameFunc_) :
				imageHandle(imageHandle_),
				descHandle(descHandle_),
				backgroundHandle(backgroundHandle_),
				gameFunc(gameFunc_){
				rect.x = x;
				rect.y = y;
				rect.width = imageWidth_;
				rect.height = imageHeight_;
			}

			bool draw(bool isSelected) {
				/*
				ChangeFont("メイリオ");
				SetFontSize(40);
				SetFontThickness(9);
				DrawString(400, 220, title.c_str(), GetColor(250, 250, 250));
				SetFontSize(24);
				SetFontThickness(6);
				DrawString(400, 300, description.c_str(), GetColor(250, 250, 250));
				*/
				int margin = 25;
				if (isSelected) {
					DrawExtendGraph(WIDTH - margin - 1276 * (HEIGHT - 2*margin) / 892, margin, WIDTH -margin ,HEIGHT -margin, descHandle, true);
					
					drawWithRotation(imgHandles["menu_select"], 0, false, 1.5);
					drawWithRotation(imageHandle, p->getAngle(), false, 1.2);
				}
				else {
					p->reset();
					drawWithRotation(imgHandles["menu_select"], 0, false, 1.2);
					drawWithRotation(imageHandle, 0);

				}

				return true;
			}
			bool draw() { return true; }
		};

		int numOfGames;
		int selectedGameIndex = 0;
		std::vector<GameDescription> gameList;

		Menu& menu;

		public :
			SelectGame(Menu& menu_);

		std::shared_ptr<Game> startSelectedGame() {
			return gameList[selectedGameIndex].gameFunc();
		}

		void setNextGame(int step){
			selectedGameIndex += step;
			if (-1 < selectedGameIndex && selectedGameIndex < numOfGames) {
				// pass
			} else {
				selectedGameIndex = (selectedGameIndex + numOfGames) % numOfGames;
			}
			int x = gameList[selectedGameIndex].left();
			int y = gameList[selectedGameIndex].top();
			menu.makeEffect("menu_select", x - 50, y - 120, 200, 200, false, 150, 2);
		}

		// ゲーム選択の描画
		bool draw() {
			//SetDrawBright(200, 200, 200);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["menu_background"], true);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT, gameList[selectedGameIndex].backgroundHandle, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 150);
			//SetDrawBright(255, 255, 255);

			for (int i = 0; i < gameList.size(); i++) {
				gameList[i].draw(i == selectedGameIndex ? true : false);
			}

			return true;
		}
	};

	void makeEffect(std::string effectHandleKey_, int x_ = 0, int y_ = 0, int width_ = WIDTH, int height_ = HEIGHT, bool willStay_ = false, int layer_ = 150, int framePerCount_ = 1, int counter_ = 0) {
		drawList.push_back(std::make_shared<Effect>(effectHandleKey_, x_, y_, width_, height_, willStay_, layer_, framePerCount_, counter_));

	}

	std::shared_ptr<SelectGame> games;
	std::shared_ptr<Game>&  gameType;

	std::shared_ptr<BGM> bgm;

	// Menu クラスのループ処理
public:
	Menu(std::shared_ptr<Game>& gameType_) : gameType(gameType_) {
		games = std::make_shared<SelectGame>(*this);
	}

	bool onStart() {
		using namespace std;
		fps.isShow = true;

		mode.setMode([this]() {
			drawList.push_back(games);
		}, -1);

		bgm = make_shared<BGM>();
		bgm->start();
		ChangeFont("メイリオ");

		//drawList.push_back(make_shared<Background>(share.handle));

		return Game::onStart();
	}

	bool onUpdate() {
		funcTimer.update();

		if (key[KEY_INPUT_RETURN]) {
			// gameType = std::make_shared<FirstGame>();
			gameType = games->startSelectedGame();
			share.willFinish = true;
		}
		if (key[KEY_INPUT_ESCAPE]) {
			share.willFinish = true;
		}

		static int counterForWaiting = 3;
		if (counterForWaiting > 0){
			counterForWaiting--;
		}
		if (counterForWaiting == 0 && key[KEY_INPUT_DOWN]) {
			games->setNextGame(1);
			counterForWaiting = 5;
		}
		if (counterForWaiting == 0 && key[KEY_INPUT_UP]) {
			games->setNextGame(-1);
			counterForWaiting = 5;
		}


		return Game::onUpdate();
	}

	bool onFinish(){
		bgm->stop();
		return true;
	}

};
