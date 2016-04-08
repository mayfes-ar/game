#pragma once

#include "image_process.h"
#include "fps.h"


// abstract class
// 個々のゲームはこれを継承して作る
// 子クラスの onStart, onUpdate は親の同名のものを return すること
// drawList に Object の std::shared_ptr を入れれば描画してくれる
class Game {
protected:
	ShareData share;

	std::list<std::shared_ptr<Object>> drawList;

	char key[256];
	Fps fps;

	virtual ~Game() {
		drawList.clear();
	}

public:
	virtual bool onStart() {
		GetHitKeyStateAll(key);
		return true;
	}

	// 子クラスのonUpdate内で return Game::onUpdate();
	virtual bool onUpdate() {
		fps.update();
		drawList.sort([](const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) -> bool { return a->getLayer() < b->getLayer(); });

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
		GetHitKeyStateAll(key);
		return !share.isFinish;
	}

	virtual bool onFinish() = 0;

};

	

