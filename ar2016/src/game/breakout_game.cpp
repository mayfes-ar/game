#include "game/breakout_game.h"

void BreakoutGame::moveShip()
{
	// マーカーから船を移動
	// 今はキーボードの左右で移動
	// 0晩マーカーの位置で船を制御
	if(share.lives[0] > 0)
		m_components->ship->setLeft((int)share.rects[0].x);

	if (share.lives[0] > 0) {
		const int diff = (int)(share.rects[0].x - m_components->ship->left());
		m_components->ship->translate(diff/10);
	}

	if (key[KEY_INPUT_LEFT]) {
		m_components->ship->translate(-5);
	}
	else if (key[KEY_INPUT_RIGHT]) {
		m_components->ship->translate(5);
	}
}

void BreakoutGame::moveFireBall()
{
	m_components->fireball->updatePosition();
	auto realm = m_components->fireball->getRealm();
	auto moving = m_components->fireball->getMoving();
	// Ship衝突判定
	if (CollisionDetection::isOnRectangle(realm, m_components->ship->getRealm())) {
			auto vel = moving->getVelocity();
			moving->setVelocity(Eigen::Vector2d{vel.x(), -vel.y()});
	}

	// Block衝突判定
/*
	for (const auto& block : m_components->block_list) {
		if (CollisionDetection::isOnLine(realm, block->getRealm().getLeftLine())) {
			block->disappear();
			auto vel = moving->getVelocity();
			moving->setVelocity(Eigen::Vector2d{-vel.x(), vel.y() });
			break;
		}
		else if (CollisionDetection::isOnLine(realm, block->getRealm().getRightLine())) {
			block->disappear();
			auto vel = moving->getVelocity();
			moving->setVelocity(Eigen::Vector2d{ -vel.x(), vel.y() });
			break;
		}
		else if (CollisionDetection::isOnLine(realm, block->getRealm().getTopLine())) {
			block->disappear();
			auto vel = moving->getVelocity();
			moving->setVelocity(Eigen::Vector2d{ vel.x(), -vel.y() });
			break;
		}
		else if (CollisionDetection::isOnLine(realm, block->getRealm().getBottomLine())) {
			block->disappear();
			auto vel = moving->getVelocity();
			moving->setVelocity(Eigen::Vector2d{ vel.x(), -vel.y() });
			break;
		}
	}
*/
}

void BreakoutGame::updateBlockStatus() {}
