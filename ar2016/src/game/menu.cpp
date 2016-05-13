#include "game/menu.h"

std::shared_ptr<Game> startMenu() {

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
Menu::SelectGame::SelectGame() {
	gameList = std::vector<GameDescription>{
		GameDescription(
			imgHandles["p_perfect"],
			imgHandles["s_game_op"],
			100,
			120,
			150,
			90,
			[]() -> std::shared_ptr<Game> {return std::make_shared<PuzzleGame>(); }),
		GameDescription(
			imgHandles["s_game_cake"],
			imgHandles["s_game_op"],
			100,
			320,
			150,
			90,
			[]() -> std::shared_ptr<Game> {return std::make_shared<SinglePlayerGame>(); }),
		GameDescription(
			imgHandles["b_donald"], // サムネイルのハンドル
			imgHandles["s_game_block"],
			100,
			520,
			150,
			90,
			[]() -> std::shared_ptr<Game> {return std::make_shared<BreakoutGame>(); })
	};

	selectedGameIndex = 0;
	numOfGames = gameList.size();
	layer = 50;
}
