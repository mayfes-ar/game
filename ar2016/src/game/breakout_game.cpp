#include "game/breakout_game.h"
#include "util/breakout_params.h"

void BreakoutGame::updateFireball()
{
	bool is_game_continuing = (!isGameClear()) && (!m_components->info->isTimeOver());
	//吸い込んでいたら再表示させない
	if (m_components->fireball->isDisappeared() && is_game_continuing && !m_components->pot->isInhared()) {
		m_components->fireball->appear(Shape::Circle{Breakout::FIREBALL_STARTPOS, Breakout::FIREBALL_RADIUS});
	}
}

void BreakoutGame::updateCollisionDetection()
{
	// FireBallの衝突判定
	{
		m_components->fireball->updatePosition();
		// 壁との衝突判定
		if (CollisionDetection::isOnLine(m_components->fireball->getRealm(),
			m_components->field->getRealm().getBottomLine())) {
			m_components->fireball->disappear();
			m_components->ship->damageShip(1);
			return;
		}

		if (m_components->fireball->isCollided(m_components->field->getRealm(), -1)) {
			return;
		}

		// Ship衝突判定
		if (m_components->ship->isAlive())
			if (m_components->fireball->isCollided(m_components->ship->getRealm(), 1, m_components->ship->getVelocity())) {
				return;
			}

		// Block衝突判定
		for (int block_id = 0; block_id < Breakout::BLOCK_HEIGHT_NUM * Breakout::BLOCK_WIDTH_NUM; ++block_id) {
			if (m_components->block_list.at(block_id)->isDisappeared()) continue;
			if (m_components->fireball->isCollided(m_components->block_list.at(block_id)->getRealm())) {
				m_components->block_list.at(block_id)->disappear();
				m_components->block_list.at(block_id)->detachAllItems();
				return;
			}
		}
	}
	//Itemの衝突判定
	{
		for (auto &item : m_components->item_list) {
			item->updateStatus();
			if (item->isAttached()) continue;
			item->getRealm();
			m_components->ship->top();


			if (item->getRealm().bottom() > m_components->ship->top() && item->getRealm().right() > m_components->ship->left() && item->getRealm().left() < m_components->ship->right() && item->getRealm().top() < m_components->ship->bottom()) {
				m_components->ship->attachItem(item);
			}
		}
	}
}

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
		m_components->ship->translate(-10);
	}
	else if (key[KEY_INPUT_RIGHT]) {
		m_components->ship->translate(10);
	} else {
		m_components->ship->translate(0);
	}
}



void BreakoutGame::updateGameState()
{
	if (m_components->fireball->isDisappeared()) return;

	m_components->fireball->updatePosition();
	// 壁との衝突判定
	int kind = mode.getMode();

	enum Kind {
		Playing = 0,
		Result = 1
	};
	switch (kind) {
	case Playing:
		if (isGameClear()) {
			m_components->result->clearGame();
			mode.goNext();
			return;
		}

		if (m_components->info->isTimeOver() ||
			!m_components->ship->isAlive()) {
			mode.goNext();
			return;
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

bool BreakoutGame::isGameClear() const
{
	for (const auto& block : m_components->block_list) {
		if (!block->isDisappeared()) {
			return false;
		}
	}

	return true;
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
		const float diff_rot = (float)share.rects[1].rotate - m_components->pot->getRotation();
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