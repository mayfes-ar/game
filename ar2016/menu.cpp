#include "menu.h"

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
		GameDescription("First Game", // ゲームタイトル
						"めざせ！ポケモンマスター！", // 説明文
						imgHandles["player"], // サムネイルのハンドル
						[]() -> std::shared_ptr<Game> {return std::make_shared<FirstGame>(); }), // 起動するゲーム
		GameDescription("Second Game",
						"俺が、ガンダムだ！",
						imgHandles["block"],
						[]() -> std::shared_ptr<Game> {return std::make_shared<FirstGame>(); })
	};

	selectedGameIndex = 0;
	numOfGames = gameList.size();
	layer = 50;
}
