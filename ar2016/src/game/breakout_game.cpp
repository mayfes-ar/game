#include "game/breakout_game.h"

void BreakoutGame::moveShip()
{
	// マーカーから船を移動
	// 今はキーボードの左右で移動
	// 0晩マーカーの位置で船を制御
	if(share.lives[0] > 0)
		m_components->ship->setLeft((int)share.rects[0].x);


	/*if (key[KEY_INPUT_LEFT]) {
		m_components->ship->translate(-5);
	}
	else if (key[KEY_INPUT_RIGHT]) {
		m_components->ship->translate(5);
	}*/
}

void BreakoutGame::moveFireBall()
{
}

void BreakoutGame::updateBlockStatus() {}
