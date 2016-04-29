#include "game/breakout_game.h"
#include "util/breakout_params.h"

void BreakoutGame::moveShip()
{
	// マーカーから船を移動
	// 今はキーボードの左右で移動
	// 0晩マーカーの位置で船を制御
	//if(share.lives[0] > 0)
	//	m_components->ship->setLeft((int)share.rects[0].x);

	// 船のライフが0のときは操作させない（させると落ちる）
	if (m_components->ship->getLifeNum() == 0) return;

	if (share.lives[0] > 0) {
		const int diff = (int)(share.rects[0].x - m_components->ship->left());
		m_components->ship->translate(diff/10);
	}

	if (key[KEY_INPUT_LEFT]) {
		m_components->ship->translate(-5);
	}
	if (key[KEY_INPUT_RIGHT]) {
		m_components->ship->translate(5);
	}
}

void BreakoutGame::updateShipStatus()
{
	if (key[KEY_INPUT_R]) {
		m_components->ship->restoreShip(1);
	}
	if (key[KEY_INPUT_D]) {
		m_components->ship->damageShip(1);
	}
}

void BreakoutGame::moveFireBall()
{
	m_components->fireball->updatePosition();
	// 壁との衝突判定
	if (m_components->fireball->isCollided(m_components->field->getRealm())) {

		return;
	}

	// Ship衝突判定
	for (const auto& block: m_components->ship->getRealm()) {
		if (m_components->fireball->isCollided(block)) {
			m_components->ship->damageShip(1);
			return;
		}
	}

	// Block衝突判定
	for (int block_id = 0; block_id < Breakout::BLOCK_HEIGHT_NUM * Breakout::BLOCK_WIDTH_NUM; ++block_id) {
		if (m_components->block_list.at(block_id)->isDisappeared()) continue;
		if (m_components->fireball->isCollided(m_components->block_list.at(block_id)->getRealm())) {
			m_components->block_list.at(block_id)->disappear();
			// 
			return;
		}
	}

}

void BreakoutGame::updateBlockStatus() {}

void BreakoutGame::updateGameState()
{
	int kind = mode.getMode();

	enum Kind {
		Playing = 0,
		Result = 1
	};
	switch (kind) {
	case Playing:
		if (m_components->info->isTimeOver()) {
			mode.goNext();
		}

		if (key[KEY_INPUT_ESCAPE]) {
			mode.goNext();
		}
		break;
	case Result: {
		if (key[KEY_INPUT_ESCAPE]) {
			share.willFinish = true;
		}
		break;
	}
	default:
		break;
	}
}
