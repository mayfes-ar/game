#include "game/menu.h"

std::shared_ptr<Game> startMenu() {

	SetDrawBright(255, 255, 255);
	std::shared_ptr<Game> gameType;
	Menu menu(gameType);

	if(!menu.onStart()){
		return nullptr;
	}

	while (menu.onUpdate()) {
		if (ProcessMessage() == -1) {
			return nullptr;
		}
	}

	menu.onFinish();

	return gameType;

}

// メニュー画面に表示するゲーム情報の登録
Menu::SelectGame::SelectGame(Menu& menu_) : menu(menu_) {
	gameList = std::vector<GameDescription>{
		GameDescription(
			imgHandles["menu_p"],
			imgHandles["menu_p_d"],
			imgHandles["menu_blue"],
			40,
			120,
			421/2,
			131/2,
			[]() -> std::shared_ptr<Game> {return std::make_shared<PuzzleGame>(); }),
		GameDescription(
			imgHandles["menu_s"],
			imgHandles["menu_s_d"],
			imgHandles["menu_orange"],
			40,
			320,
			414/2,
			134/2,
			[]() -> std::shared_ptr<Game> {return std::make_shared<SinglePlayerGame>(); }),
		GameDescription(
			imgHandles["menu_b"], // サムネイルのハンドル
			imgHandles["menu_b_d"],
			imgHandles["menu_green"],
			40,
			520,
			417/2,
			131/2,
			[]() -> std::shared_ptr<Game> {return std::make_shared<BreakoutGame>(); })
	};

	selectedGameIndex = 0;
	numOfGames = gameList.size();
	layer = 50;
}
