﻿#include "game/breakout_game.h"
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

	if (m_components->fireball->isDisappeared()) return;

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

void BreakoutGame::updatePotStatus() {
	// pot が使用済みならもう更新しない。
	if (!m_components->pot->isAvailable()) return;

	if (share.lives[1] > 0) {
		// potを出現させる
		m_components->pot->appear();

		// potをマーカーの位置に移動させる
		const Eigen::Vector2i diff_dist = Eigen::Vector2i((int)share.rects[1].x - m_components->pot->getRealm().left(), (int)share.rects[1].y - m_components->pot->getRealm().top());
		m_components->pot->translate(diff_dist/10);

		// potをマーカーの回転にあわせる
		const double diff_rot = share.rects[1].rotate - m_components->pot->getRotation();
		m_components->pot->rotate(diff_rot / 10.0);
		
		if (m_components->pot->hasFireball()) {
			//カウントを減らす
			m_components->pot->countDown();
			//吐き出すかどうか判断
			if (m_components->pot->isExhared()) {
				// 吐き出す
				m_components->pot->exhareFireball();
			}
		}

		// もしfireballが見えなくなっている(消えている)状態だったら衝突判定はしない
		// すでに放出していたら衝突判定はしない
		if (m_components->fireball->isDisappeared() || m_components->pot->isExhared()) return;

		auto rotatedTopLine = m_components->pot->getRealm().getTopLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());
		auto rotatedLeftLine = m_components->pot->getRealm().getLeftLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());
		auto rotatedRightLine = m_components->pot->getRealm().getRightLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());
		auto rotatedBottomLine = m_components->pot->getRealm().getBottomLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());

		// fireballがpotの口の辺上にあったらそれを吸い込む
		if (CollisionDetection::isOnLine(m_components->fireball->getRealm(), rotatedTopLine)) {
			m_components->pot->inhareFireball(m_components->fireball);
		}
		// ほかの辺にぶつかったらfireballを吸い込まずに消す
		else if ((CollisionDetection::isOnLine(m_components->fireball->getRealm(), rotatedLeftLine)) ||
			(CollisionDetection::isOnLine(m_components->fireball->getRealm(), rotatedRightLine)) ||
			(CollisionDetection::isOnLine(m_components->fireball->getRealm(), rotatedBottomLine))) {
			m_components->fireball->disappear();
		}
	}
}