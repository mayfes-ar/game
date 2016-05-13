#include "game/single_player_game.h"
#include <typeinfo.h>

std::shared_ptr<SinglePlayerGame::Teresa> SinglePlayerGame::makeTeresa(int x = 0, int y = 0, double size = 1.0) {
	if (x == 0 && y == 0) {
		x = rand() % (WIDTH - 100) + 50;
		y = rand() % (HEIGHT - 100) + 50;
	}
	auto enemy = std::make_shared<Teresa>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Teresa>(x, y - 200, *this, size);
	}
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::RocketWanwan> SinglePlayerGame::makeRocketWanwan(int x, int y, double size = 1.0) {
auto enemy = std::make_shared<RocketWanwan>(x, y, *this, size);
if (player->isContacted(enemy)) {
enemy = std::make_shared<RocketWanwan>(x, y - 200, *this, size);
}
enemySubList.push_back(enemy);
drawList.push_back(enemy);
return enemy;
}

std::shared_ptr<SinglePlayerGame::Inundation> SinglePlayerGame::makeInundation() {
	auto enemy = std::make_shared<Inundation>(0, HEIGHT+200, *this, 1);

	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Ghorst> SinglePlayerGame::makeGhorst(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Ghorst>(x, y, *this, size);

	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Switch> SinglePlayerGame::makeSwitch(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Switch>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Switch>(x, y - 200, *this, size);
	}
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Ufo> SinglePlayerGame::makeUfo(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Ufo>(x, y, *this, size);
	
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Ray> SinglePlayerGame::makeRay(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Ray>(x, y, *this, size);
	
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Cloud> SinglePlayerGame::makeCloud(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Cloud>(x, y, *this, size);
	
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Drop> SinglePlayerGame::makeDrop(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Drop>(x, y, *this, size);

	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Eagle> SinglePlayerGame::makeEagle(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Eagle>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Eagle>(x, y - 200, *this, size);
	}
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Heiho> SinglePlayerGame::makeHeiho(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Heiho>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Heiho>(x, y - 200, *this, size);
	}
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Fire> SinglePlayerGame::makeFire(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Fire>(x, y, *this, size);
	
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}


std::shared_ptr<SinglePlayerGame::tutoHeiho> SinglePlayerGame::maketutoHeiho(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<tutoHeiho>(x, y, *this, size);
	if (tutoplayer->isContacted(enemy)) {
		enemy = std::make_shared<tutoHeiho>(x, y - 200, *this, size);
	}
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}
	
std::shared_ptr<SinglePlayerGame::tutoFire> SinglePlayerGame::maketutoFire(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<tutoFire>(x, y, *this, size);

	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}



bool willFinishMode = false;
bool isChangingMode = false;

bool SinglePlayerGame::onStart() {
	using namespace std;
	fps.isShow = true;

	srand((unsigned int)time(NULL));

	// INTRO
	mode.setMode([this]() {
		class Title : public Object {
			Difficulty& difficulty;
		public:
			Title(Difficulty& difficulty_) : difficulty(difficulty_) {
				layer = 50;
			}

			bool draw() {
				DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_background"], true);
				// DrawExtendGraph(WIDTH / 2 - 429 / 2, 30, WIDTH / 2 + 429 / 2, 30 + 47, imgHandles["s_game_op_title"], true);
				// DrawExtendGraph(WIDTH/2-50, 400, WIDTH/2+50, 400+150, imgHandles["s_game_player"], true);

				SetFontSize(32);
				SetFontThickness(9);
				DrawString(200, 150, "EASY", difficulty == EASY ? GetColor(255,0,0) : GetColor(0, 0, 0));
				DrawString(200, 250, "HARD", difficulty == HARD ? GetColor(255, 0, 0) : GetColor(0, 0, 0));
				DrawString(200, 350, "NIGHTMARE", difficulty == NIGHTMARE ? GetColor(255, 0, 0) : GetColor(0, 0, 0));
				SetFontSize(24);
				SetFontThickness(6);

				switch (difficulty)
				{
				case EASY: {
					DrawString(450, 150, "←おすすめ", GetColor(255, 0, 0));
					break;
				}
				case HARD: {
					DrawString(450, 250, "←べてらん向け", GetColor(255, 0, 0));
					break;
				}
				case NIGHTMARE: {
					DrawString(450, 350, "←じごく", GetColor(255, 0, 0));
					break;
				}
				default:
					break;
				}

				DrawExtendGraph(760, 200, 760+Player::width*3, 200+Player::height*3, imgHandles[difficulty == EASY ? "s_game_player" : difficulty == HARD ? "s_game_player_damage" : "s_game_player_over"], true);

				return true;
			}
		};

		drawList.clear();
		drawList.push_back(make_shared<Title>(difficulty));
		drawList.push_back(make_shared<CurtainObject>(true));


		bgm = make_shared<BGM>(0);
		bgm->start();
	}, -1);

	// TUTORIAL
	mode.setMode([this]() {
		drawList.clear();
		// maxPlayerDamage = difficulty == EASY ? 5 : difficulty == HARD ? 10 : 20;
		tutoplayer = std::make_shared<tutoPlayer>(WIDTH / 2 - 100 / 2, HEIGHT / 2 - 150 / 2, tutoPlayer::width, tutoPlayer::height, "s_game_player", maxPlayerDamage, *this);


		auto makeBlock = [this](int x, int y, int width, int height) {
			auto block = make_shared<SingleGameBlockObject>(x, y, width, height, true);
			blockList.push_back(block);
			drawList.push_back(block);
		};
		makeBlock(0 - 200, 650, 200, 200);
		makeBlock(0, 650, 200, 200);
		makeBlock(0 + 200, 650, 200, 200);
		makeBlock(0 + 400, 650, 200, 200);
		makeBlock(0 + 600, 650, 200, 200);
		makeBlock(0 + 800, 650, 200, 200);
		makeBlock(0 + 1000, 650, 200, 200);
		makeBlock(0 + 1200, 650, 200, 200);
		makeBlock(0 + 1400, 650, 200, 200);
		makeBlock(0 - 400, 0, 250, HEIGHT);
		makeBlock(WIDTH + 150, 0, 250, HEIGHT);


		drawList.push_back(tutoplayer);
		drawList.push_back(make_shared<Background>(share.handle));
		drawList.push_back(make_shared<CurtainObject>(true));


		share.rectMutex.lock();
		markerList.clear();
		markerList.shrink_to_fit();
		for (int i = 0; i < share.rects.size(); i++) {
			auto marker = std::make_shared<Marker>(share.rects[i], false, i, *this);
			markerList.push_back(marker);
			drawList.push_back(marker);
		}
		share.rectMutex.unlock();

		class Title : public Object {
			bool start = false;
			int timer = 0;
			std::shared_ptr<SinglePlayerGame::tutoHeiho> tutoenemy;
			Tutorial *tutorial;
			std::string tutosen1 = "あ、危ない！！";
			std::string tutosen2 = "お姫様に火の玉が当たりそうです！！";
			std::string tutosen3 = "お姫様を守るために盾を使ってみましょう";
			std::string tutosen4 = "マーカーを火の玉に当ててみてください";
			std::string tutosen5 = "剣を使えば敵を倒すこともできます";
			std::string tutosen6 = "盾と剣を使ってお姫様を敵から守りましょう！！";
			std::string tutosen7 = "これでチュートリアルを終了します";
			std::string tutosen0 = "チュートリアルをスキップするにはSを押してください";


		public:
			Title(std::shared_ptr<SinglePlayerGame::tutoHeiho> &tutoenemy_, SinglePlayerGame& game_) {
				layer = 50;
				tutoenemy = tutoenemy_;
				tutorial = &game_.tutorial;
			}

			void update() {

				if (tutoenemy->childfire != NULL) {
					if (tutoenemy->childfire->getIsFreezed() && start == false) {
						start = true;
					}
					if (start == true) {
						timer++;
					}
				}
			}

			bool draw() {
				update();
				UINT w, h;
				getPngSize("img/s_game/tuto0.png",&w,&h);
				DrawExtendGraph(WIDTH - w/2, 30, WIDTH, 30 + h/2, imgHandles["s_game_tuto0"], true);
				//DrawString(WIDTH - 700, 30, std::to_string(w).c_str(), GetColor(0, 0, 0));

				if (timer <= FPS * 2/3) {


				}
				else if (timer <= FPS * 2) {
					getPngSize("img/s_game/tuto1.png", &w, &h);
					//DrawString(550, 350, tutosen1.c_str(), GetColor(0, 0, 0));
					DrawGraph(WIDTH/2-w/2, HEIGHT/2-h/2, imgHandles["s_game_tuto1"], true);
				}
				else if (timer <= FPS * 4) {
					getPngSize("img/s_game/tuto2.png", &w, &h);
					//DrawString(550, 350, tutosen2.c_str(), GetColor(0, 0, 0));
					DrawGraph(WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_tuto2"], true);
				}
				else if (timer <= FPS * 6) {
					getPngSize("img/s_game/tuto3.png", &w, &h);
					//DrawString(550, 350, tutosen3.c_str(), GetColor(0, 0, 0));
					DrawGraph(WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_tuto3"], true);
				}
				else if (timer <= FPS * 8) {
					*tutorial = BEATFIRE;
					getPngSize("img/s_game/tuto4.png", &w, &h);
					//DrawString(550, 350, tutosen4.c_str(), GetColor(0, 0, 0));
					DrawGraph(WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_tuto4"], true);
					if (timer == FPS * 8 && tutoenemy->childfire->getIsAlive()) {
						timer--;
					}
				}
				else if (timer <= FPS * 10) {
					*tutorial = BEATHEIHO;
					getPngSize("img/s_game/tuto5.png", &w, &h);
					//DrawString(550, 350, tutosen5.c_str(), GetColor(0, 0, 0));
					DrawGraph(WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_tuto5"], true);
					if (timer == FPS * 10 && tutoenemy->getIsAlive()) {
						timer--;
					}
				}
				else if (timer <= FPS * 12) {
					getPngSize("img/s_game/tuto6.png", &w, &h);
					//DrawString(550, 350, tutosen6.c_str(), GetColor(0, 0, 0));
					DrawGraph(WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_tuto6"], true);
				}
				else if (timer <= FPS * 14) {
					getPngSize("img/s_game/tuto7.png", &w, &h);
					//DrawString(550, 350, tutosen7.c_str(), GetColor(0, 0, 0));
					DrawGraph(WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_tuto7"], true);
				}
				else {
					*tutorial = END;

				}
				return true;
			}



			/**
			*	PNGファイルの画像サイズを取得する
			*/
			bool getPngSize(const char* path, UINT* width, UINT* height)
			{
				FILE* f;
				fopen_s(&f, path,"rb");
				if (!f) return false;

				BYTE header[8];// PNGファイルシグネチャ 
				if (fread(header, sizeof(BYTE), 8, f) < 8) { fclose(f); return false; }

				const static BYTE png[] = { 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a };
				if (memcmp(header, png, 8) != 0) { fclose(f); return false; }

				BYTE ihdr[25];// IHDRチャンク(イメージヘッダ) 
				if (fread(ihdr, sizeof(BYTE), 25, f) < 25) { fclose(f); return false; }

				// length = 13 (0x0D) 
				const static BYTE length[] = { 0x00, 0x00, 0x00, 0x0D };
				if (memcmp(ihdr, length, 4) != 0) { fclose(f); return false; }

				// IHDR 
				if (memcmp(ihdr + 4, "IHDR", 4) != 0) { fclose(f); return false; }

				BYTE* p;

				DWORD w;
				p = (BYTE*)&w;
				p[0] = ihdr[8 + 3];
				p[1] = ihdr[8 + 2];
				p[2] = ihdr[8 + 1];
				p[3] = ihdr[8 + 0];

				DWORD h;
				p = (BYTE*)&h;
				p[0] = ihdr[12 + 3];
				p[1] = ihdr[12 + 2];
				p[2] = ihdr[12 + 1];
				p[3] = ihdr[12 + 0];

				*width = (UINT)w;
				*height = (UINT)h;

				fclose(f);
				return true;
			}



			/*typedef struct { int w, h; } Size;

			Size GetJpegSize(const char *png)
			{
				Size ret = { 0, 0 };
				unsigned char buf[8];
				FILE *f;
				fopen_s(&f, png, "rb");
				while (f && fread(buf, 1, 2, f) == 2 && buf[0] == 0xff)
				{
					if (buf[1] == 0xc0 && fread(buf, 1, 7, f) == 7)
					{
						ret.h = buf[3] * 256 + buf[4];
						ret.w = buf[5] * 256 + buf[6];
					}
					else if (buf[1] == 0xd8 || (fread(buf, 1, 2, f) == 2 &&
						!fseek(f, buf[0] * 256 + buf[1] - 2, SEEK_CUR))) continue;
					break;
				}
				if (f) fclose(f);
				return ret;
			}*/

					
		};
		if (tutorial == END) {

		}
		tutoenemy = maketutoHeiho(WIDTH, 300, 1);
		drawList.push_back(make_shared<Title>(tutoenemy, *this));


	}, -1);

	// GAME
	mode.setMode([this]() {
		drawList.clear();
		drawList.push_back(make_shared<CurtainObject>(true));

		maxPlayerDamage = difficulty == EASY ? 10 : difficulty == HARD ? 10 : 20;
		player = std::make_shared<Player>(WIDTH / 2 - 100 / 2, HEIGHT / 2 - 150 / 2, Player::width, Player::height, "s_game_player", maxPlayerDamage, *this);




		auto makeBlock = [this](int x, int y, int width, int height) {
			auto block = make_shared<SingleGameBlockObject>(x, y, width, height, true);
			blockList.push_back(block);
			drawList.push_back(block);
		};
		makeBlock(0 - 200, 650, 200, 200);
		makeBlock(0, 650, 200, 200);
		makeBlock(0 + 200, 650, 200, 200);
		makeBlock(0 + 400, 650, 200, 200);
		makeBlock(0 + 600, 650, 200, 200);
		makeBlock(0 + 800, 650, 200, 200);
		makeBlock(0 + 1000, 650, 200, 200);
		makeBlock(0 + 1200, 650, 200, 200);
		makeBlock(0 + 1400, 650, 200, 200);
		makeBlock(0 - 400, 0, 250, HEIGHT);
		makeBlock(WIDTH + 150, 0, 250, HEIGHT);

		drawList.push_back(player);
		drawList.push_back(make_shared<Background>(share.handle));
		/*
		makeEffect("s_game_coin", 200, 200, 50, 50, true);
		makeEffect("s_game_coin", 250, 200, 50, 50, true, 150, 1, 3);
		makeEffect("s_game_coin", 300, 200, 50, 50, true, 150, 2);
		makeEffect("s_game_coin", 350, 200, 50, 50, true, 150, 2, 3);
		makeEffect("s_game_koumori", 400, 200, 50, 50, true);
		makeEffect("s_game_koumori", 450, 200, 50, 50, true, 150, 2);
		makeEffect("s_game_koumori", 500, 200, 50, 50, true, 150, 3);
		makeEffect("s_game_koumori", 550, 200, 50, 50, true, 150, 4);
		makeEffect("s_game_hit", 600, 200, 50, 50, true, 150, 2);
		makeEffect("s_game_enemy_over", 650, 200, 50, 50, true, 150, 3);
		makeEffect("s_game_sword", 700, 200, 50, 50, true, 150, 2);
		*/

		share.rectMutex.lock();
		markerList.clear();
		markerList.shrink_to_fit();
		for (int i = 0; i < share.rects.size(); i++) {
			auto marker = std::make_shared<Marker>(share.rects[i], false, i, *this);
			markerList.push_back(marker);
			drawList.push_back(marker);
		}
		share.rectMutex.unlock();


		bgm = make_shared<BGM>(1);
		bgm->start();
		


		class Title : public Object {
			bool start = false;
			int timer = 0;
			int *gametimer;

		public:
			Title(int *gametimer_,SinglePlayerGame& game_) {
				layer = 50;
				gametimer = gametimer_;
			}

			void update() {
						timer++;
						(*gametimer)++;
			}

			bool draw() {
				UINT w, h;
				if (timer <= FPS * 6) {
					update();
				}
				if (timer <= FPS * 2 / 3) {

				}
				else if (timer <= FPS * 2) {
					getPngSize("img/s_game/countdown_3.png", &w, &h);
					DrawGraph(WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_countdown3"], true);
				}
				else if (timer <= FPS * 2 + FPS * 4/3) {
					getPngSize("img/s_game/countdown_2.png", &w, &h);
					DrawGraph(WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_countdown2"], true);
				}
				else if (timer <= FPS * 2 + FPS * 8/3) {
					getPngSize("img/s_game/countdown_1.png", &w, &h);
					DrawGraph(WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_countdown1"], true);
				}
				else if (timer <= FPS * 6) {
					getPngSize("img/s_game/s.png", &w, &h);
					DrawGraph(WIDTH / 2 - 5 * w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_s"], true);
					DrawGraph(WIDTH / 2 - 3 * w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_t"], true);
					DrawGraph(WIDTH / 2 - w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_a"], true);
					DrawGraph(WIDTH / 2 + w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_r"], true);
					DrawGraph(WIDTH / 2 + 3 * w / 2, HEIGHT / 2 - h / 2, imgHandles["s_game_t"], true);
				}
				return true;
			}

			/**
			*	PNGファイルの画像サイズを取得する
			*/
			bool getPngSize(const char* path, UINT* width, UINT* height)
			{
				FILE* f;
				fopen_s(&f, path, "rb");
				if (!f) return false;

				BYTE header[8];// PNGファイルシグネチャ 
				if (fread(header, sizeof(BYTE), 8, f) < 8) { fclose(f); return false; }

				const static BYTE png[] = { 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a };
				if (memcmp(header, png, 8) != 0) { fclose(f); return false; }

				BYTE ihdr[25];// IHDRチャンク(イメージヘッダ) 
				if (fread(ihdr, sizeof(BYTE), 25, f) < 25) { fclose(f); return false; }

				// length = 13 (0x0D) 
				const static BYTE length[] = { 0x00, 0x00, 0x00, 0x0D };
				if (memcmp(ihdr, length, 4) != 0) { fclose(f); return false; }

				// IHDR 
				if (memcmp(ihdr + 4, "IHDR", 4) != 0) { fclose(f); return false; }

				BYTE* p;

				DWORD w;
				p = (BYTE*)&w;
				p[0] = ihdr[8 + 3];
				p[1] = ihdr[8 + 2];
				p[2] = ihdr[8 + 1];
				p[3] = ihdr[8 + 0];

				DWORD h;
				p = (BYTE*)&h;
				p[0] = ihdr[12 + 3];
				p[1] = ihdr[12 + 2];
				p[2] = ihdr[12 + 1];
				p[3] = ihdr[12 + 0];

				*width = (UINT)w;
				*height = (UINT)h;

				fclose(f);
				return true;
			}
		};
		drawList.push_back(make_shared<Title>(&timer,*this));


	}, -1);

	// RESULT
	mode.setMode([this]() {
		
	
		drawList.clear();	
		drawList.push_back(make_shared<CurtainObject>(true));


		class Title : public Object {
			bool hasPlayerWon = true;
			std::shared_ptr<SinglePlayerGame::Player> player;
			int maxTime;
			int timer;
			
		public:
			Title(bool hasPlayerWon_, std::shared_ptr<SinglePlayerGame::Player> player_, int maxTime_, int timer_) {
				layer = 50;
				hasPlayerWon = hasPlayerWon_;
				player = player_;
				maxTime = maxTime_;		
				timer = timer_;
			}

			//クリアの文
			const std::string clear1 = "おめでとうございます。";
			const std::string clear2 = "見事お姫様を守り抜くことができました。";
			const std::string clear3 = "彼女はあなたを”好き”になったようです。";
			const std::string clear4 = "あなたはどうですか。";
			const std::string clear5 = "姫を守るために取った”剣”の重さ";
			const std::string clear6 = "どうか覚えていてくださいね。";
			const std::string clear7 = "遊んでくれてありがとうございました。";

			//ゲームオーバーの文
			const std::string dead1 = "「姫」を守り切ることはできませんでした。";
			const std::string dead2 = "この悲劇は、たかが「ゲーム」でしょうか。";
			const std::string dead3 = "いいえ、「姫」は確かにいました。";
			const std::string dead4 = "「あなた」という現実、「姫」という物語";
			const std::string dead5 = "二人は、いつでも会うことができるのです。";
			const std::string dead6 = "次からは、ちゃんと守ってくださいね。";
			const std::string dead7 = "遊んでくれてありがとうございました。";


			bool draw() {
				
				if (HEIGHT - rect.y > 60) {
					rect.y += 1;//文章スクロール用
				}

				if (hasPlayerWon) {

					//********勝利画面
					//画像
					DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_background1"], true);
					DrawExtendGraph(650, 450, 650 + 320, 450 + 311, imgHandles["s_game_result_castle"], true);
					DrawExtendGraph(900, 170, WIDTH-80, 380, imgHandles["s_game_result_frame_star"], true);					
					DrawExtendGraph(820, 20, WIDTH, 230, imgHandles["s_game_result_rainbow"], true);
					DrawExtendGraph(1100, 1046 / 3 + 100, 1100 + 621 / 3, 1046 / 3 + 1046 / 3 + 100, imgHandles["s_game_player"], true);
					DrawExtendGraph(10, 20, WIDTH/2-20, HEIGHT-10, imgHandles["s_game_result_background_gameover1"], true);
					//スクロール文章
					
					DrawExtendGraph(50, HEIGHT-rect.y, WIDTH / 2 - 40, HEIGHT - rect.y +HEIGHT -60, imgHandles["s_game_result_clear1"], true);
/*
					DrawString(50, HEIGHT - rect.y, clear1.c_str(), GetColor(25, 25, 25));
					DrawString(50, HEIGHT + 150 - rect.y, clear2.c_str(), GetColor(25, 25, 25));
					DrawString(50, HEIGHT + 300 - rect.y, clear3.c_str(), GetColor(25, 25, 25));
					DrawString(50, HEIGHT + 450 - rect.y, clear4.c_str(), GetColor(25, 25, 25));
					DrawString(50, HEIGHT + 600 - rect.y, clear5.c_str(), GetColor(25, 25, 25));
					DrawString(50, HEIGHT + 750 - rect.y, clear6.c_str(), GetColor(25, 25, 25));
					DrawString(50, HEIGHT + 900 - rect.y, clear7.c_str(), GetColor(25, 25, 25));
					*/
					//スコア
					std::string clearScore = "得点 : " + std::to_string(maxTime + (player->getMaxDamage() - player->getPlayerDamage()) * 50);
					DrawString(940, 230, clearScore.c_str(), GetColor(225, 225, 225));
					std::string damage = ("受けたダメージ : " + std::to_string(player->getPlayerDamage()));
					DrawString(940, 300, damage.c_str(), GetColor(255, 225, 225));

					
					
				}

				else {

					//******ゲームオーバー画面

					//画像・ゲームオーバー
					DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_background2"], true);
					DrawExtendGraph(170, 300, 170 + 1600 / 5, HEIGHT, imgHandles["s_game_result_sketch"], true);
					DrawExtendGraph(621 / 3 - 50, 450, 621 / 3 + 621 / 4 - 50, HEIGHT, imgHandles["s_game_player_drowned"], true);
					DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_stage1"], true);
					DrawExtendGraph(220, 100, 520, 320, imgHandles["s_game_result_frame_blackstar"], true);

					DrawExtendGraph(550, HEIGHT - rect.y, WIDTH - 140, HEIGHT - rect.y + HEIGHT - 10, imgHandles["s_game_result_dead1"], true);
					/*

					//スクロール文章・ゲームオーバー
					DrawString(600, HEIGHT - rect.y, dead1.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT+150 - rect.y, dead2.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT+300 - rect.y, dead3.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT+450 - rect.y, dead4.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT+600 - rect.y, dead5.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT + 750 - rect.y, dead6.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT + 900 - rect.y, dead7.c_str(), GetColor(225, 225, 225));
					*/

					//スコア
					std::string deadScore = "得点 : " + std::to_string(maxTime - timer);
					std::string playTime = "記録 : " + std::to_string((maxTime - timer) / 30) + "秒";
					DrawString(270, 170, deadScore.c_str(), GetColor(225, 225, 225));
					DrawString(270, 230, playTime.c_str(), GetColor(225, 225, 225));
				}
							
				return true;
			}
		};

		drawList.push_back(make_shared<Title>(hasPlayerWon, player, maxTime, timer));

		//yu
		if (hasPlayerWon) {
			//勝利画面のエフェクト・リザルト
			makeEffect("s_game_result_hanabi", 400, 100, 500, 500, true, 50, 3, 8);
			makeEffect("s_game_result_hanabi", 700, 50, 300, 300, true, 50, 3, 0);

			}
		else {
			//ゲームオーバーのエフェクト・リザルト	
			makeEffect("s_game_result_kirakira", 150, 270, 500, 500, false, 150, 1, 0);
			
		}

		bgm->stop();
		bgm = make_shared<BGM>(2, hasPlayerWon);
		bgm->start();


	}, -1);

	return Game::onStart();
}

bool SinglePlayerGame::onUpdate() {
	willFinishMode = false;
	funcTimer.update();

	switch (mode.getMode()) {
	case INTRO: { // イントロダクション
		static int counterForWait = 5;
		if (counterForWait == 0) {
			counterForWait = 5;
			if (key[KEY_INPUT_RETURN]) {
				willFinishMode = true;
				drawList.clear();
			}
			else if (key[KEY_INPUT_1]) {
				difficulty = EASY;
			}
			else if (key[KEY_INPUT_2]) {
				difficulty = HARD;
			}
			else if (key[KEY_INPUT_3]) {
				difficulty = NIGHTMARE;
			}
			else if (key[KEY_INPUT_UP]) {
			}
			else if (key[KEY_INPUT_DOWN]) {
				difficulty = difficulty == EASY ? HARD : difficulty == HARD ? NIGHTMARE : EASY;
			}
			else {
				counterForWait = 0;
				
			}
		}
		else if (counterForWait > 0) {
			counterForWait--;
		}
		
		
		break;
	}
	case TUTORIAL: {

		if (tutorial != START) {
			// 認識したマーカーを描画
			share.rectMutex.lock();
			for (auto marker : markerList) {
				marker->setRect(share.rects[marker->getIndex()]);
			}
			share.rectMutex.unlock();
			
		}
		
		switch (tutorial) {
		case START: {
			if (heihoFreezeTimeRemain >= 0) {					
				heihoFreezeTimeRemain--;
				
			}
			if(heihoFreezeTimeRemain == FPS*4 +5){
			}

			if (heihoFreezeTimeRemain == 0) {
				tutoenemy->freeze();
				tutoenemy->childfire->freeze();

			}
			break;
		}
		case BEATFIRE: {//Titleクラス
			if (tutoenemy->childfire->getIsAlive()) {
				tutoenemy->childfire->deathDecision();
			}
			break;
		}
		case BEATHEIHO: {//Titleクラス
			if (tutoenemy->getIsAlive()) {
				tutoenemy->deathDecision();
			}
			break;
		}
		case END: {//Titleクラス
			willFinishMode = true;
			//makeEffect("s_game_curtain_close", 0, 0, WIDTH, HEIGHT, false, 150, 2, 1);
			break;
		}
		}


		if (key[KEY_INPUT_S]) {
			willFinishMode = true;
		}


		for (auto enemy : enemyList) {
			enemy->update();
		}
		for (auto enemy : enemySubList) {
			enemyList.push_back(enemy);
		}
		enemySubList.clear();
		enemySubList.shrink_to_fit();
		tutoplayer->update(key);

		break;
	}
	case GAME: { // playing


		tutoenemy->setIsDead();
		if (tutoenemy->childfire != NULL) {
			tutoenemy->childfire->setIsDead();
		}
		timer -= 1;
		
		if (timer <= 0) {
			willFinishMode = true;
		}

		// 認識したマーカーを描画
		share.rectMutex.lock();
	
		for (auto marker : markerList) {
			marker->setRect(share.rects[marker->getIndex()]);
		}
		share.rectMutex.unlock();

		player->update(key);

		for (auto enemy : enemyList) {
			enemy->deathDecision();
			enemy->update();
		}
		for (auto enemy : enemySubList) {
			enemyList.push_back(enemy);
		}
		enemySubList.clear();
		enemySubList.shrink_to_fit();

		if (hasPlayerWon && player->deathDecision(enemyList)) {
			
			hasPlayerWon = false;
			bgm->stop();
			funcTimer.set([this]() {
				willFinishMode = true;
			}, FPS*3);


		}

		
		// 敵の出現を管理する
		switch (difficulty) {
		case EASY: {
			switch (maxTime - timer) {
		
			case 100: {
				makeEagle(0, 0, 1);
				break;
			}
			case 600: {
				makeRocketWanwan(-RocketWanwan::width / 2, HEIGHT / 2 + 50);
				break;
			}
			case 800: {
				makeCloud(0, 50, 1);
				break;
			}
			case 1000: {
				makeInundation();
			}
			case 2000: {
				makeHeiho(WIDTH, 300, 1);
				break;
			}
			case 2300: {
				makeUfo(0, 50, 1);
				break;
			}
			case 2800: {
				makeCloud(0, 50, 1);
				break;
			}

			case 3400: {
				makeHeiho(WIDTH, 300, 1);
				break;
		    }
			default: {
			}
			}
			// 定期的に実行する場合など
			if (timer < 2500 && (maxTime - timer) % 50 == 0) {
				makeTeresa();
			}
			break;
		}
		case HARD: {
			switch (maxTime - timer) {
			case 100: {
				makeRocketWanwan(-RocketWanwan::width / 2, HEIGHT / 2 + 50);
				break;
			}
			case 200: {
				makeInundation();
				makeEagle(0, 0, 1);
				makeEagle(200, 0, 1);
				makeEagle(400, 0, 1);
				makeEagle(600, 0, 1);
				break;
			}
			case 300: {
				makeUfo(0, 50, 1);
				makeRocketWanwan(WIDTH - RocketWanwan::width / 2, HEIGHT / 2 + 50);
				makeRocketWanwan(WIDTH - RocketWanwan::width / 2, HEIGHT / 2 + 50 + RocketWanwan::height);
				break;
			}				  
			case 600: {
				makeEagle(0, 0, 1);
				makeEagle(200, 0, 1);
				makeEagle(400, 0, 1);
				makeEagle(600, 0, 1);
				makeCloud(0, 50, 1);
				makeCloud(200, 50, 1);
				makeCloud(800, 50, 1);
				break;
			}
			case 900: {
				makeInundation();
				makeUfo(0, 50, 1);
				makeRocketWanwan(-RocketWanwan::width / 2, HEIGHT / 2 + 50);
				makeRocketWanwan(-RocketWanwan::width / 2, HEIGHT / 2 + 50 + RocketWanwan::height);
				break;
			}
			case 1200: {
				makeEagle(0, 0, 1);
				makeEagle(200, 0, 1);
				makeHeiho(WIDTH, 300, 1);
				break;
			}
			case 1500: {
				makeCloud(0, 50, 1);
				makeCloud(200, 50, 1);
				makeCloud(800, 50, 1);
				makeRocketWanwan(WIDTH - RocketWanwan::width / 2, HEIGHT / 2 + 50);
				makeHeiho(WIDTH, 300, 1);
				break;
			}	
			case 1600: {
				makeUfo(0, 50, 1);
				break;
			}
			case 1800: {
				makeInundation();
				makeEagle(0, 0, 1);
				makeEagle(200, 0, 1);
				makeEagle(400, 0, 1);
				makeEagle(600, 0, 1);
				break;
			}
			case 2100: {
				makeUfo(0, 50, 1);
				makeRocketWanwan(-RocketWanwan::width, HEIGHT / 2 + 50);
				makeEagle(0, 0, 1);
				makeCloud(200, 50, 1);
				break;
			}
			case 2400: {
				makeHeiho(WIDTH, 300, 1);
				break;
			}
			case 2700: {
				makeCloud(0, 50, 1);
				makeCloud(200, 50, 1);
				makeCloud(800, 50, 1);
				break;
			}
			case 3000: {
				makeInundation();
				makeEagle(0, 0, 1);
				makeEagle(200, 0, 1);
				break;
			}
			case 3300: {
				makeUfo(0, 50, 1);
				makeRocketWanwan(WIDTH - RocketWanwan::width / 2, HEIGHT / 2 + 50);
				makeRocketWanwan(WIDTH - RocketWanwan::width / 2, HEIGHT / 2 + 50 + RocketWanwan::height);
				break;
			}
			default: {
			}
			}

			// 定期的に実行する場合など
			if (timer < 150 && (maxTime - timer) % 5 == 0) {
				makeTeresa();
			}
			break;
		}
		case NIGHTMARE: {
			switch (maxTime - timer) {
			case 400:
			case 700:
			case 1000:
			case 1200:
			case 1400:
			case 100: {
				makeInundation();
			}
			default: {
			}
			}

			// 定期的に実行する場合など
			if (timer < 150 && (maxTime - timer) % 3 == 0) {
				makeTeresa();
			}
			else if ((maxTime - timer) % 30 == 0){
				makeTeresa();
			}
			if ((maxTime - timer) % 10 == 0 && rand()%2 == 0) {
				makeHeiho(WIDTH, 250 + rand()%100, 1);
			}
			if ((maxTime - timer) % 30 == 0 && rand() % 3 == 0) {
				makeRocketWanwan(rand()%2 == 0 ? -RocketWanwan::width : WIDTH+RocketWanwan::width, HEIGHT / 2 + 50);
			}
			if ((maxTime - timer) % 30 == 0 && rand() % 3 == 0) {
				if (rand() % 10 == 0) {
					makeEagle(-1000, -1000, 10);
				}
				else {
					makeEagle(-600, 0, 1);
					makeEagle(-400, 0, 1);
					makeEagle(-200, 0, 1);
					makeEagle(0, 0, 1);
					makeEagle(200, 0, 1);
					makeEagle(400, 0, 1);
					makeEagle(600, 0, 1);
				}
			}
			if ((maxTime - timer) % 100 == 0 && rand() % 2 == 0) {
				makeCloud(0, 50, 1);
			}
			if ((maxTime - timer) % 100 == 0 && rand() % 2 == 0) {
				makeUfo(WIDTH, 100, 1);
			}
			break;
		}
		}
		
		break;
	}
	case RESULT: { // リザルト画面

		
		result_timer -= 1;

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
	static int waitNCounter = FPS / 3;
	if (key[KEY_INPUT_N] && waitNCounter == 0) {
		willFinishMode = true;
		waitNCounter = FPS / 3;
	}
	if (waitNCounter > 0) { waitNCounter--; }


	for (auto& itr = enemyList.begin(); itr != enemyList.end();) {
		if ((*itr)->getIsAlive()) {
			++itr;
		}
		else {
			itr = enemyList.erase(itr);
		}
	}


	if (willFinishMode && !isChangingMode) {
		isChangingMode = true;
		drawList.push_back(make_shared<CurtainObject>(false));
		funcTimer.set([this]() {
			isChangingMode = false;
			mode.goNext();
		}, FPS);
	}

	return Game::onUpdate();

}

bool SinglePlayerGame::onFinish() {
	bgm->stop();

	thread.join();
	return true;
}

