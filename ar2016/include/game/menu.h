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
				/*
				ChangeFont("メイリオ");
				SetFontSize(40);
				SetFontThickness(9);
				DrawString(400, 220, title.c_str(), GetColor(250, 250, 250));
				SetFontSize(24);
				SetFontThickness(6);
				DrawString(400, 300, description.c_str(), GetColor(250, 250, 250));
				*/
				// DrawExtendGraph(0, 0, 410, HEIGHT, imgHandles["s_game_block"], true);
				DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_water"], true);

				// DrawExtendGraph(500, 100, WIDTH - 90, HEIGHT - 100, imageHandle, true);
				DrawExtendGraph(350, 50, WIDTH-50, HEIGHT - 50, imgHandles["s_game_op"], true);

				DrawExtendGraph(100, 120, 250, 210, imgHandles["p_perfect"], true);
				DrawExtendGraph(100, 320, 250, 410, imgHandles["s_game_cake"], true);
				DrawExtendGraph(100, 520, 250, 610, imgHandles["b_donald"], true);
				

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
			drawList.push_back(games);
		}, -1);

		bgm = make_shared<BGM>();
		bgm->start();
		ChangeFont("メイリオ");

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
