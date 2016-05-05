#include "game/breakout_game.h"
#include "util/breakout_params.h"

void BreakoutGame::updateFireball()
{
	bool is_game_continuing = (!isGameClear()) && (!m_components->info->isTimeOver());
	//吸い込んでいたら再表示させない
	for (const auto& fireball : m_components->fireball_manager->getFireballList()) {
		if (fireball->isDisappeared() && is_game_continuing && !m_components->pot->isInhared()) {
			fireball->appear(Shape::Circle{ Breakout::FIREBALL_STARTPOS, Breakout::FIREBALL_RADIUS });
		}
	}
}

void BreakoutGame::updateCollisionDetection()
{
	// FireBallの衝突判定
	{
		for (auto& fireball : m_components->fireball_manager->getFireballList()) {

			fireball->updatePosition();
			// 壁との衝突判定
			if (CollisionDetection::isOnLine(fireball->getRealm(),
				m_components->field->getRealm().getBottomLine())) {
				fireball->disappear();
				m_components->ship->damageShip(1);
				continue;
			}

			if (fireball->isCollided(m_components->field->getRealm(), -1)) {
				continue;
			}

			// Ship衝突判定
			if (m_components->ship->isAlive()) {
				if (fireball->isCollided(m_components->ship->getRealm(), 1, m_components->ship->getVelocity())) {
					if (fireball->getMode() == Breakout::FireballKind::EnemyStrong) {
						fireball->disappear();
						m_components->ship->damageShip(1);
					}
					else {
						fireball->changeModeToPlayer();
					}
					continue;
				}
			}

			// Block衝突判定
			// EnemyModeだったらBlockと衝突させない
			if (fireball->isEnemy()) {}
			else {
				for (int block_id = 0; block_id < Breakout::BLOCK_HEIGHT_NUM * Breakout::BLOCK_WIDTH_NUM; ++block_id) {
					if (m_components->block_list.at(block_id)->isDisappeared()) continue;
					if (fireball->isCollided(m_components->block_list.at(block_id)->getRealm())) {
						m_components->block_list.at(block_id)->disappear();
						m_components->block_list.at(block_id)->detachAllItems();
						break;
					}
				}
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

	// 船のライフが0のときは操作させない（させると落ちる）
	if (m_components->ship->getLifeNum() == 0) return;

	if (share.lives[1] > 0) {
		const int diff = (int)(share.rects[1].x - m_components->ship->left());
		m_components->ship->translate(diff / 10);
	}

	if (key[KEY_INPUT_LEFT]) {
		m_components->ship->translate(-10);
	}
	else if (key[KEY_INPUT_RIGHT]) {
		m_components->ship->translate(10);
	}
	else {
		m_components->ship->translate(0);
	}
}



void BreakoutGame::updateGameState()
{
	for (auto& fireball : m_components->fireball_manager->getFireballList()) {
		if (fireball->isDisappeared()) continue;
		fireball->updatePosition();
	}

	// 壁との衝突判定
	int kind = mode.getMode();

	enum Kind {
		Playing = 0, //!< Play画面
		Result = 1, //!< Result画面
	};
	switch (kind) {
	case Playing:
		if (m_components->info->isLastPhase()) {
			m_components->background->turnLastPhase();
		}
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
	for (auto& fireball : m_components->fireball_manager->getFireballList()) {
		// pot が使用済みならもう更新しない。
		if (!m_components->pot->isAvailable()) return;

		if (share.lives[1] > 0) {
			// potを出現させる
			if (share.rects[1].width < 180) {
				m_components->pot->appear();
			}
			// potが出現していたら
			else if (!m_components->pot->isDisappeared()) {
				// Phase: 吸い込んでから吐き出すまで

				// potをマーカーの位置に移動させる
				const Eigen::Vector2i diff_dist = Eigen::Vector2i((int)share.rects[1].x - m_components->pot->getRealm().left(), (int)share.rects[1].y - m_components->pot->getRealm().top());
				m_components->pot->translate(diff_dist / 10);

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
					else if (share.rects[1].width > 250) {
						m_components->pot->exhareFireball();
					}
				}

				//　すでにfireballを吸い込んでいたら衝突判定はしない
				// もしfireballが見えなくなっている(消えている)状態だったら衝突判定はしない
				// すでに放出していたら衝突判定はしない
				if (fireball->isDisappeared() ||
					m_components->pot->isExhared() ||
					m_components->pot->hasFireball()) return;


				// Phase: 出現してから吸い込むまで
				auto rotatedTopLine = m_components->pot->getRealm().getTopLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());
				auto rotatedLeftLine = m_components->pot->getRealm().getLeftLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());
				auto rotatedRightLine = m_components->pot->getRealm().getRightLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());
				auto rotatedBottomLine = m_components->pot->getRealm().getBottomLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());

				// fireballがpotの口の辺上にあったらそれを吸い込む
				if (CollisionDetection::isOnLine(fireball->getRealm(), rotatedTopLine)) {
					m_components->pot->inhareFireball(fireball);
				}
				// ほかの辺にぶつかったらfireballを吸い込まずに消す
				else if ((CollisionDetection::isOnLine(fireball->getRealm(), rotatedLeftLine)) ||
					(CollisionDetection::isOnLine(fireball->getRealm(), rotatedRightLine)) ||
					(CollisionDetection::isOnLine(fireball->getRealm(), rotatedBottomLine))) {
					fireball->disappear();
					//
					m_components->fireball_manager->destroy(fireball);
				}
			}
		}
	}
}