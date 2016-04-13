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
	// GameのonStart内でそれぞれのモードに移行したときに呼ばれる関数と制限時間をsetModeで登録する
	class Mode {
		int currentMode = -1;
		bool isGoNext = true;
		std::vector<std::function<void()>> funcs;
		std::vector<int> timers;
	public:
		int getMode() const {
			return currentMode;
		}

		// 例）setMode([this](){ drawList.clean(); }, -1);
		void setMode(std::function<void()> func, int timer) {
			funcs.push_back(func);
			timers.push_back(timer);
		}

		// 強制的に次のモードへ
		void goNext() {
			isGoNext = true;
		}

		// モード移行を判定し、タイマーを進める
		// 次に移るモードが存在しなかった時 return false;
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

	// Objectを継承したクラスのオブジェクトへのstd::shared_ptrを突っ込めば毎ループ（Game::onUpdateで）描画してくれる
	// 例）drawList.push_back(std::make_shared<HogeObject>(arg...));
	std::list<std::shared_ptr<Object>> drawList;

	// key[KEY_INPUT_*] でそのキーが押されているかわかる
	char key[256];
	Fps fps;

	virtual ~Game() {
		drawList.clear();
	}

public:
	// 子クラスのonStart内で return Game::onStart();
	virtual bool onStart() {
		GetHitKeyStateAll(key); //（不必要かもしれないが）キー入力を初期化している。
		if (mode.update()) {
			return true;
		} else {
			share.willFinish = true;
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
			share.willFinish = true;
		}
		GetHitKeyStateAll(key); // キー入力状態を取得
		return !share.willFinish;
	}

	virtual bool onFinish() = 0; // 純粋仮想関数を調べよう

};
