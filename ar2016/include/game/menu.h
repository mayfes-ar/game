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

	// 背景処理
	class BackEffect : public Object {
		int counter = 0;
		const int countMax = 2 * effectHandles["effect1"].size();

	public:
		BackEffect(){
			layer = 0;
		}
		bool draw() {
			DrawExtendGraph(0, 0, WIDTH, HEIGHT, effectHandles["effect1"][counter/2], true);
			counter++;
			if (counter == countMax) { counter = 0; }
			return true;
		}
	};

	class BackGround : public Object {
	public:
		BackGround(){
			layer = 1;
		}
		bool draw() {
			SetDrawBright(40, 40, 40);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["ar2016_logo"], true);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["menu"], true);
			SetDrawBright(255, 255, 255);
			return true;
		}
	};

	// タイトルなど静的オブジェクトの処理
	class Title : public Object {
	public:
		Title() {
			layer = 50;

		}

		bool draw() {
			// SetFontSize(80);
			// ChangeFont("メイリオ");
			// SetFontThickness(9);
			// DrawString(100, 50, "TITLE", GetColor(255, 255, 255));
			DrawExtendGraph(640-192, 0, 640+192, 0+180, imgHandles["menu_title"], true);
			return true;
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
	class SelectGame : public Object {
		// 個々のゲーム情報を持つクラス
		class GameDescription :public Object {
		public :
			std::string title;
			std::string description;
			int imageHandle;
			int imageWidth;
			int imageHeight;
			std::function<std::shared_ptr<Game>()> gameFunc;

			GameDescription(std::string title_, std::string description_, int imageHandle_, int imageWidth_, int imageHeight_,  std::function<std::shared_ptr<Game>()> gameFunc_) :
				title(title_),
				description(description_),
				imageHandle(imageHandle_),
				imageWidth(imageWidth_),
				imageHeight(imageHeight_),
				gameFunc(gameFunc_){}

			bool draw(){
				ChangeFont("メイリオ");
				SetFontSize(40);
				SetFontThickness(9);
				DrawString(400, 220, title.c_str(), GetColor(250, 250, 250));
				SetFontSize(24);
				SetFontThickness(6);
				DrawString(400, 300, description.c_str(), GetColor(250, 250, 250));
				DrawExtendGraph(350-imageWidth, 500-imageHeight, 350, 500, imageHandle, true);
				return true;
			}
		};
		int numOfGames;
		int selectedGameIndex;
		std::vector<GameDescription> gameList;

		public :
		SelectGame();

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
		}

		// ゲーム選択の描画
		bool draw() {
			gameList[selectedGameIndex].draw();
			return true;
		}
	};

	std::shared_ptr<SelectGame> games;
	std::shared_ptr<Game>&  gameType;

	std::shared_ptr<BGM> bgm;

	// Menu クラスのループ処理
public:
	Menu(std::shared_ptr<Game>& gameType_) : gameType(gameType_) {
		games = std::make_shared<SelectGame>();
	}

	bool onStart() {
		using namespace std;
		fps.isShow = true;

		mode.setMode([this]() {
			drawList.push_back(make_shared<Title>());
			drawList.push_back(make_shared<BackEffect>());
			drawList.push_back(make_shared<BackGround>());
			drawList.push_back(games);
		}, -1);

		bgm = make_shared<BGM>();
		bgm->start();

		return Game::onStart();
	}

	bool onUpdate() {

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
		if (counterForWaiting == 0 && key[KEY_INPUT_RIGHT]) {
			games->setNextGame(1);
			counterForWaiting = 5;
		}
		if (counterForWaiting == 0 && key[KEY_INPUT_LEFT]) {
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
