#pragma once

#include "image_process.h"
#include "fps.h"


// abstract class
// 個々のゲームはこれを継承して作る
// 子クラスの onStart, onUpdate は親の同名のものを return すること
// drawList に Object の std::shared_ptr を入れれば描画してくれる
class Game {
protected:
	class Mode {
		int currentMode = -1;
		bool isGoNext = true;
		std::vector<std::function<void()>> funcs;
		std::vector<int> timers;
	public:
		void setMode(std::function<void()> func, int timer) {
			funcs.push_back(func);
			timers.push_back(timer);
		}

		void goNext() {
			isGoNext = true;
		}

		bool update() {
			if (isGoNext) {
				currentMode++;
				if (currentMode + 1 >= funcs.size()) {
					return false;
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
		return true;
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
		GetHitKeyStateAll(key); // キー入力状態を取得
		return !share.isFinish;
	}

	virtual bool onFinish() = 0; // 純粋仮想関数を調べよう

};
