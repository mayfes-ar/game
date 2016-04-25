#include "game/breakout_game.h"

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
	m_components->fireball->Fireball::updatePosition();
}

void BreakoutGame::updateBlockStatus() {}
