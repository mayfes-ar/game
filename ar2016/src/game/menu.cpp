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
		GameDescription("バトルネットワーク ロックマンエグゼ3",
		"大人気のデータアクションRPG「ロックマンエグゼ」シリーズの第3弾。\n主人公の熱斗が現実世界で暴走する電気機器をさがし、\nロックマンが電脳世界でウィルスを退治するというように、\n協力して事件を解決していく。\nロックマンエグゼ！トランスミッション！！",
			imgHandles["rockman"],
			150,
			270,
			[]() -> std::shared_ptr<Game> {return std::make_shared<PuzzleGame>(); }),
		GameDescription("オタサーの姫ゲーム",
						"宇宙のどこかの星に美しいお姫様がいて、ずっとかわいがられてきました。\n今、国王様がお姫様の婚約者を全宇宙から募集しようとしています。\n迫りくる試練からお姫様を守り切った人こそ、婚約者にふさわしい。\n勇気あるあなた、チャレンジしてみませんか？",
						imgHandles["s_game_player_drowned"],
						621 / 4,
						1046 / 4,
						[]() -> std::shared_ptr<Game> {return std::make_shared<SinglePlayerGame>(); }),
		GameDescription("New スーパーマリオブラザーズ", // ゲームタイトル
			"「誰でもできる、新しいマリオ。」\n跳んで走って、投げて踏んで、たたいて壊して、探して集める…。\nいろんな動きをカンタンにこなし、マリオがゴールを目指す。\n海や森、氷や砂の世界など、まったく新しいキノコ王国を舞台に、\nマリオの新しい冒険が始まる。", // 説明文
			imgHandles["player"], // サムネイルのハンドル
			150,
			225, // 起動するゲーム
			[]() -> std::shared_ptr<Game> {return std::make_shared<BreakoutGame>(); })
	};

	selectedGameIndex = 0;
	numOfGames = gameList.size();
	layer = 50;
}
