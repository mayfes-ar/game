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
Menu::GameList::GameList() {
	gameList = std::vector<GameDescription>{
		GameDescription("First Game", // ゲームタイトル
						"fuga fuga", // 説明文
						"nyan nyan", // サムネイルのハンドル
						[]() -> std::shared_ptr<Game> {return std::make_shared<FirstGame>(); }), // 起動するゲーム
		GameDescription("Second Game",
						"hoge hoge",
						"wan wan",
						[]() -> std::shared_ptr<Game> {return nullptr; })
	};

	numOfGames = gameList.size();
}
