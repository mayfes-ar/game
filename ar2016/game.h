#pragma once

#include "image_process.h"
#include "fps.h"


// abstract class
// 個々のゲームはこれを継承して作る
// 子クラスの onStart, onUpdate は親の同名のものを return すること
// drawList に Object の std::shared_ptr を入れれば描画してくれる
class Game {
protected:
	// ゲーム内の段階（プレイ中、リザルトなど）を管理
	class Mode {
		int currentMode = -1;
		bool isGoNext = true;
		std::vector<std::function<void()>> funcs;
		std::vector<int> timers;
	public:
		int getMode() const {
			return currentMode;
		}

		void setMode(std::function<void()> func, int timer) {
			funcs.push_back(func);
			timers.push_back(timer);
		}

		// 強制的に次のモードへ
		void goNext() {
			isGoNext = true;
		}

		// モード移行を判定し、タイマーを進める
		bool update() {
			if (isGoNext) {
				isGoNext = false;
				currentMode++;
				if (currentMode < funcs.size()) {
					funcs[currentMode]();
				} else {
					return false;
				}
			}
			
			if (currentMode < funcs.size() && timers[currentMode] > 0) {
				if (--timers[currentMode] == 0) {
					isGoNext = true;
				}
			}
			

			return true;
		}

	} mode;

	// 自状態の param 管理もしている。定義はimage_process.h
	ShareData share;

	std::list<std::shared_ptr<Object>> drawList;

	char key[256];
	Fps fps;

	virtual ~Game() {
		drawList.clear();
	}

public:
	virtual bool onStart() {
		GetHitKeyStateAll(key); //（不必要かもしれないが）キー入力を初期化している。
		if (mode.update()) {
			return true;
		} else {
			share.isFinish = true;
			return false;
		}
		
	}

	// 子クラスのonUpdate内で return Game::onUpdate();
	virtual bool onUpdate() {
		fps.update(); //フレームレート計測
		drawList.sort([](const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) -> bool { return a->getLayer() < b->getLayer(); }); // Layerが下のものから描画

		share.drawMutex.lock();
		ClearDrawScreen();

		for ( auto& itr = drawList.begin(); itr != drawList.end();) {
			if ((*itr)->draw()) {
				++itr;
			} else {
				itr = drawList.erase(itr);
			}
		}

		fps.draw();
		ScreenFlip();
		share.drawMutex.unlock();

		fps.wait();

		if (!mode.update()) {
			share.isFinish = true;
		}
		GetHitKeyStateAll(key); // キー入力状態を取得
		return !share.isFinish;
	}

	virtual bool onFinish() = 0; // 純粋仮想関数を調べよう

};
