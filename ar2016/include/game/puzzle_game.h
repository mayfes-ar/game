﻿#pragma once

#include "game/game.h"

#define _USE_MATH_DEFINES
#include <math.h>

class PuzzleGame : public Game {
	// 指定フレーム後に、登録した関数を実行してくれる
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
				} else {
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

	//
	class StageBlock : public BlockObject {
	public:
		const int kind;

		StageBlock(Rectan rect_, bool canHit_=true, int kind_=0) : kind(kind_){
			rect = rect_;
			willStay = true;
			layer = 20;
			canHit = canHit_;
		}
		bool draw() {
			if (canHit) {
				switch (kind) {
				case 1: {
					
					break;
				}
				default: {
					DrawRectGraph(left(), top(), left(), top(), rect.width, rect.height, imgHandles["s_brick1"], false, false);
				}
				}
			} else {
				//DrawBox(left(), top(), right(), bottom(), GetColor(40, 117, 50), false);
			}
			return willStay;
		}
	};
	//
	class MarkerBlock : public BlockObject {
	public:
		MarkerBlock(Rectan rect_) {
			rect = rect_;
			willStay = false;
			layer = 25;
			if (rect.width == 0) {
				canHit = false;
			}
		}

		bool draw() {
			if (canHit) {
				drawWithRect(imgHandles["p_block"]);
			} else {
				drawWithRect(imgHandles["p_block_x"]);
			}
			return willStay;
		}
	};

	// gimmick共通クラス
	// updateの返り値で存続判定
	class Gimmick : public Object {
	protected:
		bool willExist = true;
		const PuzzleGame& game;
		Gimmick(PuzzleGame& game_) : game(game_){}
	public:
		virtual bool update() = 0;
	};


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

	class Explanation : public Object {
	public:
		int volume = 50;
		bool isIncreasing = true;
		Explanation() {
			PlaySoundMem(soundHandles["p_bgm1"], DX_PLAYTYPE_LOOP, true);
			ChangeVolumeSoundMem(volume, soundHandles["p_bgm1"]);
			layer = 2;
		}
		/*~Explanation() {
			isIncreasing = false;
			//StopSoundMem(soundHandles["p_bgm1"]);
		}*/
		bool isFirst = true;
		bool draw() {
			if (volume < 252 && isIncreasing == true) {
				volume = volume + 4;
				ChangeVolumeSoundMem(volume, soundHandles["p_bgm1"]);
			}

			/*if (isIncreasing == false) {
				if (volume - 4 >= 0) {
					volume = volume - 4;
				}
				else {
					volume = 0;
				}
				ChangeVolumeSoundMem(volume, soundHandles["p_bgm1"]);
			}
			*/
			if (isFirst) {
				DrawGraph(0, 0, imgHandles["p_explain1"], false);
			} else {
				DrawGraph(0, 0, imgHandles["p_explain2"], false);
			}
			return true;
		}
	};

	class ResultObject : public Object {
		PuzzleGame& game;
		const int handle = movieHandles["p_flower"];
		int volume = 0;
		bool isIncreasing = true;
	public:
		ResultObject(PuzzleGame& game_) : game(game_){
			layer = 0;
			SeekMovieToGraph(handle, 0);
			if (GetMovieStateToGraph(handle) == 0) {
				PlayMovieToGraph(handle);
			}
			StopSoundMem(soundHandles["p_bgm2"]);
			PlaySoundMem(soundHandles["p_bgm3"], DX_PLAYTYPE_LOOP, true);

			

			//ChangeVolumeSoundMem(50, soundHandles["p_bgm3"]);

		}
		~ResultObject() {
			StopSoundMem(soundHandles["p_bgm3"]);
		}
		bool draw() {
			DrawGraph(0, 0, handle, false);
			/*if (volume < 252 && isIncreasing == true) {
				volume = volume + 4;
				ChangeVolumeSoundMem(volume, soundHandles["p_bgm3"]);
			}*/
			if (GetMovieStateToGraph(handle) == 1) {
				return true;
			}
			game.share.willFinish = true;
			return false;
		}
	};

	// スコアを管理・表示
	class ScoreObject : public Object {
		bool isPlaying = true;
	public:
		int score = -1000;

		ScoreObject() {
			layer = 200;
		}
		bool draw() {
			
			if (isPlaying) {
				const int size = 60;
				DrawExtendGraph(400, 2, 400+size, 2+size, imgHandles["p_saihu"], true);
				drawNumber(400+size, 2, size, score, effectHandles["p_num"]);
				//DrawFormatString(400, 0, GetColor(65, 205, 63), "SCORE: %d", score);
			} else {
				if (score < 10000) {
					DrawExtendGraph(400, 50, 400 + 480, 50 + 200, imgHandles["p_score"], true);
					static const int size = 253;
					drawNumber(240, 320, size, score, effectHandles["p_num_2"]);
				} else {
					DrawGraph(150, 200, imgHandles["p_perfect"], true);
				}
				//DrawFormatString(400, 400, GetColor(165, 205, 163), "SCORE: %d", score);
			}
			return true;
		}
		void setResultDraw() { isPlaying = false; }
	};

	class TimerObject : public Object {
		static const int gameTime = 300;
		int time = gameTime * FPS;

	public:
		TimerObject() {
			layer = 200;
		}
		~TimerObject() {
			StopSoundMem(soundHandles["p_bgm1"]);
			StopSoundMem(soundHandles["p_bgm2"]);
			StopSoundMem(soundHandles["p_bgm3"]);
		}
		int volume = 90;
		int soundState = 0;  //soundState = 0:constant; 1:increasing; 2:decreasing;
		bool draw() {
			const int size = 60;

			if (volume < 252 && soundState == 1) {
				volume = volume + 4;
			}
			if (volume >= 0 && soundState == 2) {
				if (volume - 4 >= 0) {
					volume = volume - 4;
				}
				else {
					volume = 0;
				}
			}
			/*if (CheckSoundMem(soundHandles["p_bgm1"]) == 1) {
				ChangeVolumeSoundMem(volume, soundHandles["p_bgm1"]);
			}*/
			if (CheckSoundMem(soundHandles["p_bgm2"]) == 1) {
				ChangeVolumeSoundMem(volume, soundHandles["p_bgm2"]);
			}
			/*else if (CheckSoundMem(soundHandles["p_bgm3"]) == 1) {
				ChangeVolumeSoundMem(volume, soundHandles["p_bgm3"]);
			}*/

			DrawExtendGraph(800, 2, 800 + size, 2 + size, imgHandles["p_timer"], true);
			drawNumber(800 + size, 2, size, time/FPS, effectHandles["p_num"]);
			//DrawFormatString(800, 0, GetColor(165, 205, 163), "TIME: %d", time/FPS);
			return true;
		}
		bool update() {
			time--;
			return time == 0;
		}
		void startPlayingBGM(int n) {
			/*if (n == 1) {
				PlaySoundMem(soundHandles["p_bgm1"], DX_PLAYTYPE_LOOP, true);
				ChangeVolumeSoundMem(50, soundHandles["p_bgm1"]);
			}
			*/
			if (n == 2) {
				StopSoundMem(soundHandles["p_bgm1"]);
				PlaySoundMem(soundHandles["p_bgm2"], DX_PLAYTYPE_LOOP, true);
				ChangeVolumeSoundMem(volume, soundHandles["p_bgm2"]);
			}
			/*else if (n == 3) {
				StopSoundMem(soundHandles["p_bgm2"]);
				PlaySoundMem(soundHandles["p_bgm3"], DX_PLAYTYPE_LOOP, true);
				ChangeVolumeSoundMem(50, soundHandles["p_bgm3"]);
			}*/
			soundState = 1;
		}
		/*void stopPlayingBGM() {
			soundState = 2;
		}*/
		/*
		void startPlayingBGM() {
			PlaySoundMem(soundHandles["p_bgm2"], DX_PLAYTYPE_LOOP, true);
		}
		void stopPlayingBGM() {
			StopSoundMem(soundHandles["p_bgm2"]);
		}
		*/

	};

	class CurtainObject : public Object {
		const bool isOpen;
		int counter = 0;
		const int openCountMax = effectHandles["p_curtain_open"].size();
		const int closeCountMax = effectHandles["p_curtain_close"].size();
	public:
		CurtainObject(bool isOpen_) : isOpen(isOpen_) {
			layer = 300;
		}
		int volume = 255;
		bool draw() {
			const int handle = isOpen ? effectHandles["p_curtain_open"][counter] : effectHandles["p_curtain_close"][counter];
			DrawExtendGraph(0, 0, 1280, 720, handle, true);

			if (isOpen) {
				counter++;
				return !(counter == openCountMax);
			} else {
				if (counter < closeCountMax - 1) { counter++; }
				volume = volume - 4;
				if (CheckSoundMem(soundHandles["p_bgm1"]) == 1) {
					ChangeVolumeSoundMem(volume,soundHandles["p_bgm1"]);
				}
				else if(CheckSoundMem(soundHandles["p_bgm2"]) == 1) {
				ChangeVolumeSoundMem(volume, soundHandles["p_bgm2"]);
				}
				return true;
			}
		}
	};

	class Player : public Object {
		
		class InitEffect : public Object {
			const int countMax = effectHandles["p_init"].size() * 2;
			int counter = 0;
		public:
			InitEffect(int x, int y, int size) {
				rect = Rectan(x, y, size, size);
				layer = 80;
			}
			bool draw() {
				drawWithRect(effectHandles["p_init"][counter / 2]);
				counter++;
				return counter < countMax;
			}
		};

		PuzzleGame& game;

		double prevX;
		double prevY;
		const double initX;
		const double initY;

		bool isRightDirection = true;
		bool isJumping = true;
		bool isMovable = true;
		bool isDamaged = false;

		std::function<void()> updateFunc = []() {};

	public:
		double exForceX = 0;
		double exForceY = 0;

		Player(int x_, int y_, PuzzleGame& game_) : initX(x_), initY(y_), game(game_) {
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = 60;
			rect.height =100;
			layer = 100;
		}

		bool draw() {
			if (isDamaged) {
				drawWithRect(imgHandles["p_hime_damaged"], 0, !isRightDirection);
			}
			else {
				drawWithRect(imgHandles["p_hime"], 0, !isRightDirection);
			}
			return true;
		}
		void preUpdate() {
			exForceX = exForceY = 0;
		}
		void update();
		void warp(int x, int y) {
			updateFunc = [this, x, y]() {
				rect.x = prevX = x;
				rect.y = prevY = y;
				exForceX = exForceY = 0;
			};
		}
		void loop(int x, int y) {
			updateFunc = [this, x, y]() {
				const int diffX = x - rect.x;
				const int diffY = y - rect.y;
				rect.x = x; 
				rect.y = y;
				prevX += diffX;
				prevY += diffY;
				exForceX = exForceY = 0;
			};
		}
		void init(int soundNum=0) {
			if (!isMovable) { return; }
			game.drawList.push_back(std::make_shared<InitEffect>(rect.x - 62, rect.y - 50, 200));
			switch (soundNum)
			{
			case 1: {
				PlaySoundMem(soundHandles["ps_needle"], DX_PLAYTYPE_BACK);
				break;
			}
			case 2: {
				PlaySoundMem(soundHandles["ps_ball"], DX_PLAYTYPE_BACK);
				break;
			}
			default:
				break;
			}

			updateFunc = [this]() {
				isDamaged = true;
				isMovable = false;
				game.funcTimer.set([this]() {
					isDamaged = false;
					isMovable = true;
					rect.x = prevX = initX;
					rect.y = prevY = initY;
					exForceX = exForceY = 0;
				}, FPS);
			};
		}
		void goal() { 
			const int goalScore = 1000;
			game.score->score += goalScore;
			isMovable = false;
		}
		void markerCheck(std::shared_ptr<BlockObject> block) {
			if (top() < block->bottomHit() && bottom() > block->topHit() && right() > block->leftHit() && left() < block->rightHit()) {
				block->canHit = false;
			}
		}
	};

	class GoalObject : public Object {
		class CoinsEffect : public Object {
			const int countMax = effectHandles["p_coins"].size();
			int counter = 0;
		public:
			CoinsEffect(int x, int y, int size) {
				rect = Rectan(x, y, size, size);
				layer = 101;
			}
			bool draw() {
				if (counter % 2 == 0) {
					PlaySoundMem(soundHandles["ps_coin"], DX_PLAYTYPE_BACK);
				}
				drawWithRect(effectHandles["p_coins"][counter], 50);
				counter++;
				return counter < countMax;
			}
		};

		bool isReached = false;
		const int margin = 20;
	public:
		GoalObject(int x_, int y_) {
			rect = Rectan(x_ + margin, y_ + margin, 150 - margin*2, 150 - margin*2);
			layer = 100;
		}
		bool draw() {
			drawWithRect(imgHandles["p_castle"], margin);
			return true;
		}
		bool check(std::shared_ptr<Player>& player) {
			if (isContacted(player) && !isReached) {
				isReached = true;
				player->goal();
				return true;
			} else {
				return false;
			}
		}
		std::shared_ptr<Object> goalEffect() {
			return std::make_shared<CoinsEffect>(rect.x, rect.y, rect.width);
		}
	};

	// gimmicks
	class DamageGimmick : public Gimmick {
		const bool canVanish;
		const double vx;
		const double vy;
		int counter = 0;
		const int countMax = effectHandles["p_ball"].size();
	public:
		DamageGimmick(int x, int y, int size, double vx_, double vy_, PuzzleGame& game_) : canVanish(vx_*vx_ + vy_*vy_ > 0), vx(vx_), vy(vy_), Gimmick(game_) {
			rect = Rectan(x, y, size, size);
			layer = 50;
		}
		bool draw() {
			const int margin = rect.height / 3;
			if (canVanish) {
				drawWithRect(effectHandles["p_ball"][counter], margin);
				counter++;
				if (counter == countMax) {
					counter = 0;
				}
			} else {
				drawWithRect(imgHandles["p_needle"], margin);
			}
			
			return willExist;
		}
		bool update() {
			if (canVanish) {
				rect.x += vx;
				rect.y += vy;
				
				for (auto block : game.allBlocks) {
					if (isContacted(block) && block->canHit) {
						willExist = false;
					}
				}
			}

			if (isContacted(game.player)) {
				game.player->init(canVanish ? 2 : 1);
			}
			return willExist;
		}
	};

	class SmogGimmick : public Gimmick {

		int counter = 0;
		const int countMax = effectHandles["p_smog"].size();
	public:
		SmogGimmick(int x, int y, int size, PuzzleGame& game_) : Gimmick(game_) {
			rect = Rectan(x, y, size, size);
			layer = 120;
			counter = (int)(rect.x + rect.y) % countMax;
		}
		bool draw() {
			const int margin = rect.width / 10;
			DrawExtendGraph(left() - margin, top() - margin, right() + margin, bottom() + margin, effectHandles["p_smog"][counter], true);
			counter++;
			if (counter == countMax) { counter = 0; }
			return willExist;
		}
		bool update() {
			if (isContacted(game.markerBlock)) { willExist = false; }
			return willExist;
		}
	};
	
	class CoinGimmick : public Gimmick {
		const int value;
		int counter = 0;
		const int countMax = effectHandles["p_coin"].size();
	public:
		CoinGimmick(int x, int y, int size, int value_, PuzzleGame& game_) : value(value_), Gimmick(game_) {
			rect = Rectan(x, y, size, size);
			layer = 40;
		}
		bool draw() {
			const int margin = rect.width / 10;
			drawWithRect(effectHandles["p_coin"][counter], margin);
			counter++;
			if (counter == countMax) { counter = 0; }
			return willExist;
		}
		bool update() {
			if (isContacted(game.player)) {
				game.score->score += value;
				willExist = false;
				PlaySoundMem(soundHandles["ps_coin"], DX_PLAYTYPE_BACK);
			}
			return willExist;
		}
	};

	class SwitchGimmick : public Gimmick {
		bool isOn;
		const bool isReverse;
		bool& targetBool;
		int pusheffect = 0;
		int counter = 0;
		const int countMax = effectHandles["p_crystal1"].size() * 2;
		const int effectMax = effectHandles["p_push_switch"].size();
		void setSwitch(bool isOn_) {
			if (isOn == false && isOn_ == true) {
				pusheffect = 1;
				PlaySoundMem(soundHandles["ps_crystal"], DX_PLAYTYPE_BACK);
			}
			isOn = isOn_;
			if (isReverse) {
				targetBool = !isOn_;
			} else {
				targetBool = isOn_;
			}

		}
	public:
		SwitchGimmick(int x, int y, int size, bool& target, bool isReverse_, PuzzleGame& game_) : targetBool(target), isReverse(isReverse_), Gimmick(game_) {
			rect = Rectan(x, y, size, size);
			layer = 50;
			setSwitch(false);
		}
		bool draw() {
			const int margin = rect.width / 4;
			if (pusheffect) {
				drawWithRect(effectHandles["p_push_switch"][pusheffect], margin);
				pusheffect++;
				if (pusheffect == effectMax) { pusheffect = 0; }
			}
			if (isOn) {
				drawWithRect(effectHandles["p_crystal1"][counter/2], margin);
				//drawWithRect(imgHandles["p_on"]);
			} else {
				drawWithRect(effectHandles["p_crystal2"][counter/2], margin);
				//drawWithRect(imgHandles["p_off"]);
			}
			
			counter++;
			if (counter == countMax) { counter = 0; }
			return willExist;
		}
		bool update() {
			if (isContacted(game.player) || isContacted(game.markerBlock)) {
				setSwitch(true);
			}
			else {
				setSwitch(false);
			}

			return willExist;
		}
	};

	class FuncSwitchGimmick : public Gimmick {
		bool isOn;
		const std::function<void()> func;
		int counter = 0;
		int pusheffect = 0;
		const int countMax = effectHandles["p_crystal1"].size() * 2;
		const int effectMax = effectHandles["p_push_switch"].size();
		void setSwitch(bool isOn_) {
			if (isOn == false && isOn_ == true) {
				pusheffect = 1;
				PlaySoundMem(soundHandles["ps_crystal"], DX_PLAYTYPE_BACK);
			}
			if (isOn_ && !isOn) {
				func();
			}
			isOn = isOn_;
			
		}
	public:
		FuncSwitchGimmick(int x, int y, int size, std::function<void()> func_, PuzzleGame& game_) : func(func_), Gimmick(game_) {
			rect = Rectan(x, y, size, size);
			layer = 50;
			setSwitch(false);
		}
		bool draw() {
			const int margin = rect.width / 4;
			if (pusheffect) {
				drawWithRect(effectHandles["p_push_switch"][pusheffect], margin);
				pusheffect++;
				if (pusheffect == effectMax) { pusheffect = 0; }
			}
			if (isOn) {
				//drawWithRect(imgHandles["p_on"]);
				drawWithRect(effectHandles["p_crystal1"][counter / 2], margin);
			} else {
				//drawWithRect(imgHandles["p_off"]);
				drawWithRect(effectHandles["p_crystal2"][counter / 2], margin);
			}
			counter++;
			if (counter == countMax) { counter = 0; }
			return willExist;
		}
		bool update() {
			if (isContacted(game.player) || isContacted(game.markerBlock)) {
				setSwitch(true);
			} else {
				setSwitch(false);
			}
			return willExist;
		}
	};

	class WarpGimmick : public Gimmick {
		const double posX;
		const double posY;
		int counter = 0;
		const int countMax = effectHandles["p_warp"].size();
		const bool hasSound;

	public:
		WarpGimmick(Rectan rect_, double posX_, double posY_, PuzzleGame& game_, bool hasSound_) : posX(posX_), posY(posY_), Gimmick(game_), hasSound(hasSound_) {
			rect = rect_;
			layer = 70;
		}
		bool draw() {
			const int margin = rect.width / 10;
			drawWithRect(effectHandles["p_warp"][counter], margin);
			counter++;
			if (counter == countMax) {
				counter = 0;
			}
			//DrawBox(left(), top(), right(), bottom(), GetColor(238, 46, 213), false);
			return willExist;
		}
		bool update() {
			if (isContacted(game.player)) {
				game.player->warp(posX, posY);
				if (hasSound) {
					PlaySoundMem(soundHandles["ps_warp"], DX_PLAYTYPE_BACK);
				}
			}
			return willExist;
		}
	};

	class LoopGimmick : public Gimmick {
		const double posX;
		const double posY;

	public:
		LoopGimmick(Rectan rect_, double posX_, double posY_, PuzzleGame& game_) : posX(posX_), posY(posY_), Gimmick(game_) {
			rect = rect_;
			layer = 70;
		}
		bool draw() {
			DrawBox(left(), top(), right(), bottom(), GetColor(146, 138, 213), false);
			return willExist;
		}
		bool update() {
			if (isContacted(game.player)) {
				game.player->loop(posX, posY);
			}
			return willExist;
		}
	};

	class WindGimmick : public Gimmick {
		const double windX;
		const double windY;

		//0:up 1:right 2:up 3:down
		const int dirwind = windX*windX>windY*windY?1+2*(windX<0):2*(windY>0);
		
		int counter = 0;
		const int countMax = effectHandles["p_arrow"].size();
	public:
		WindGimmick(Rectan rect_, double windX_, double windY_, PuzzleGame& game_) :windX(windX_), windY(windY_), Gimmick(game_) {
			rect = rect_;
			layer = 80;
		}
		bool draw() {
			const int chipsize = 60;
			const int nx = 1>round(rect.width  / chipsize)?1: round(rect.width / chipsize);
			const int ny = 1>round(rect.height / chipsize)?1: round(rect.height / chipsize);
			const int mx = (rect.width - nx * chipsize)/2;
			const int my = (rect.height - ny * chipsize)/2;
			for (int i = 0; i < nx; i++) {
				for (int j = 0; j < ny; j++) {
					DrawRotaGraph(left() + mx + chipsize*(2*i+1)/2, top() + my + chipsize*(2*j+1)/2,1.0, dirwind*M_PI_2, effectHandles["p_arrow"][counter], true);
				}
			}
			counter++;
			if (counter == countMax) { counter = 0; }
			//DrawBox(left(), top(), right(), bottom(), GetColor(125, 224, 227), false);
			return willExist;
		}
		bool update() {
			if (isContacted(game.player)) {
				game.player->exForceX += windX;
				game.player->exForceY += windY;
			}
			return willExist;
		}
	};

	std::thread thread;
	std::shared_ptr<Player> player;
	std::shared_ptr<GoalObject> goal;
	std::shared_ptr<ScoreObject> score;
	std::shared_ptr<TimerObject> timer;

	std::vector<std::shared_ptr<BlockObject>> stageBlocks;
	std::shared_ptr<BlockObject> markerBlock;
	std::vector<std::shared_ptr<BlockObject>> allBlocks;
	std::vector<std::shared_ptr<Gimmick>> gimmicks;

	std::mt19937 mt;
	std::uniform_int_distribution<> rand100 = std::uniform_int_distribution<>(0, 99);

	int counter = 0;

	std::shared_ptr<StageBlock> setBlock(int x, int y, int width, int height, bool canHit=true, int kind=0) {
		auto block = std::make_shared<StageBlock>(Rectan(x, y, width, height), canHit, kind);
		stageBlocks.push_back(block);
		drawList.push_back(block);
		return block;
	}
	
	void setDamage(int x, int y, int size, double vx = 0, double vy = 0) {
		auto damage = std::make_shared<DamageGimmick>(x, y, size, vx, vy, *this);
		gimmicks.push_back(damage);
		drawList.push_back(damage);
	}
	void setSmog() {
		auto smog = std::make_shared<SmogGimmick>(rand100(mt) * 11, rand100(mt) * 6, 3 * rand100(mt) + 50, *this);
		gimmicks.push_back(smog);
		drawList.push_back(smog);
	}
	void setCoin(int x, int y, int size=60, int value=100) {
		auto coin = std::make_shared<CoinGimmick>(x, y, size, value, *this);
		gimmicks.push_back(coin);
		drawList.push_back(coin);
	}
	void setSwitch(int x, int y, int size, bool& target, bool isReverse = false) {
		auto switch_ = std::make_shared<SwitchGimmick>(x, y, size, target, isReverse, *this);
		gimmicks.push_back(switch_);
		drawList.push_back(switch_);
	}
	void setFuncSwitch(int x, int y, int size, std::function<void()> func) {
		auto switch_ = std::make_shared<FuncSwitchGimmick>(x, y, size, func,  *this);
		gimmicks.push_back(switch_);
		drawList.push_back(switch_);
	}
	void setWarp(int x, int y, int width, int height, int posX, int posY, bool hasSound=false) {
		auto warp = std::make_shared<WarpGimmick>(Rectan(x, y, width, height), posX, posY, *this, hasSound);
		gimmicks.push_back(warp);
		drawList.push_back(warp);
	}
	void setLoop(int x, int y, int width, int height, int posX, int posY) {
		auto loop = std::make_shared<LoopGimmick>(Rectan(x, y, width, height), posX, posY, *this);
		gimmicks.push_back(loop);
		drawList.push_back(loop);
	}
	void setWind(int x, int y, int width, int height, double windX, double windY) {
		auto wind = std::make_shared<WindGimmick>(Rectan(x, y, width, height), windX, windY, *this);
		gimmicks.push_back(wind);
		drawList.push_back(wind);
	}

	void setPlayer(int x, int y) {
		player = std::make_shared<Player>(x, y, *this);
		drawList.push_back(player);
	}
	void setGoal(int x, int y) {
		goal = std::make_shared<GoalObject>(x, y);
		drawList.push_back(goal);
	}

	// setMode内で最初に呼ぶこと
	// true -> under-700, left-50, right-1230 
	void makeStageBase(bool isSurrounded=true) {
		drawList.clear();
		stageBlocks.clear();
		gimmicks.clear();

		drawList.push_back(std::make_shared<Background>(share.handle));
		drawList.push_back(score);
		drawList.push_back(timer);

		if (isSurrounded) {
			setBlock(-100, 700, 1480, 200, true);
			setBlock(-250, -720, 300, 720 * 3, true);
			setBlock(1230, -720, 300, 720 * 3, true);
			setBlock(200, -720, 100, 620, true);
		}
	}

public:
	PuzzleGame() {
		thread = std::thread::thread(capture, std::ref(share));
		std::random_device rnd;
		mt = std::mt19937(rnd());
		score = std::make_shared<ScoreObject>();
		timer = std::make_shared<TimerObject>();
	}
	/*~PuzzleGame() {
		timer->stopPlayingBGM();
	}*/

	bool onStart();
	bool onUpdate();
	bool onFinish() {
		thread.join();
		return true;
	}
};
