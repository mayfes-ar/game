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
		GameDescription("New スーパーマリオブラザーズ", // ゲームタイトル
						"「誰でもできる、新しいマリオ。」\n跳んで走って、投げて踏んで、たたいて壊して、探して集める…。\nいろんな動きをカンタンにこなし、マリオがゴールを目指す。\n海や森、氷や砂の世界など、まったく新しいキノコ王国を舞台に、\nマリオの新しい冒険が始まる。", // 説明文
						imgHandles["player"], // サムネイルのハンドル
						150,
						225, // 起動するゲーム
						[]() -> std::shared_ptr<Game> {return std::make_shared<FirstGame>(); }),
		GameDescription("モンスターハンター4",
						"進化したのは、モンスターか、ハンターか。\n■広大なフィールドに生息するモンスター達\n■生まれ変わるハンティングアクション\n■進化したオトモアイルー\n■広がる世界、個性豊かなキャラクター\n■Wi-Fi通信無料\n  待望のインターネット通信によるマルチプレイ対応！",
						imgHandles["monhan"],
						320,
						200,
						[]() -> std::shared_ptr<Game> {return std::make_shared<SinglePlayerGame>(); }),
		GameDescription("バトルネットワーク ロックマンエグゼ3",
						"大人気のデータアクションRPG「ロックマンエグゼ」シリーズの第3弾。\n主人公の熱斗が現実世界で暴走する電気機器をさがし、\nロックマンが電脳世界でウィルスを退治するというように、\n協力して事件を解決していく。\nロックマンエグゼ！トランスミッション！！",
						imgHandles["rockman"],
						150,
						270,
						[]() -> std::shared_ptr<Game> {return nullptr; })
	};

	selectedGameIndex = 0;
	numOfGames = gameList.size();
	layer = 50;
}
