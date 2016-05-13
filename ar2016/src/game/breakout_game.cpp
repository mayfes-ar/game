#include "game/breakout_game.h"
#include "util/breakout_params.h"
#include "object/breakout_object.h"

void BreakoutGame::updateCollisionDetection()
{
	// FireBallの衝突判定
	{
		for (auto& fireball : m_components->fireball_manager->getFireballList()) {
			bool continue_loop = false;

			// 下壁との衝突判定
			if (CollisionDetection::isOnLine(fireball->getRealm(),
				m_components->field->getRealm().getBottomLine())) {
				//fireballを消す
				m_components->fireball_manager->destroy(fireball);
				//Ship へのダメージじゃなくて自分が守ってる町へのダメージというイメージ
				//m_components->ship->damageShip(1);
				continue;
			}

			auto fireball_effect = fireball->returnFireballReflect(m_components->field->getRealm(), -1);
			if (fireball_effect->isCollide()) {
				drawList.push_back(fireball_effect);
				continue;
			}

			// Ship衝突判定
			if (m_components->ship->isAlive()) {
				auto fireball_effect = fireball->returnFireballReflect(m_components->ship->getRealm(), 1, m_components->ship->getVelocity());
				if (fireball_effect->isCollide()) {
					// EnemyStrongであり かつ 無敵状態じゃなかったら
					if (fireball->getMode() == Breakout::FireballKind::EnemyStrong && !m_components->ship->isEnhanced()) {
						//fireballを消す
						m_components->fireball_manager->destroy(fireball);
						m_components->ship->damageShip(1);
					}
					else {
						//跳ね返すときは反射effectを入れる
						drawList.push_back(fireball_effect);
						fireball->changeModeToPlayer();
					}
					continue;
				}
			}

			// Block衝突判定
			// EnemyModeだったらBlockと衝突させない
			if (!fireball->isEnemy()) {
				for (int block_id = 0; block_id < Breakout::BLOCK_HEIGHT_NUM * Breakout::BLOCK_WIDTH_NUM; ++block_id) {
					if (m_components->block_list.at(block_id)->isDisappeared()) continue;
					auto fireball_effect = fireball->returnFireballReflect(m_components->block_list.at(block_id)->getRealm());
					if (fireball_effect->isCollide()) {
						m_components->block_list.at(block_id)->damageBlock(fireball->giveDamage());
						drawList.push_back(fireball_effect);
						continue_loop = true;
						break;
					}
				}
			}
			if (continue_loop) continue;

			// Enemy衝突判定
			//エネミーモードじゃなかったら
			if (!fireball->isEnemy()) {
				if (m_components->enemy->isAlive()) {
					if (fireball->isCollided(m_components->enemy->getRealm())) {
						m_components->enemy->damageByPlayerFireball(fireball, m_components->fireball_manager);
						continue;
					}

					if (m_components->enemy->getLeftHand()->isAlive()) {
						if (fireball->isCollided(m_components->enemy->getLeftHand()->getRealm())) {
							m_components->enemy->getLeftHand()->damageByPlayerFireball(fireball, m_components->fireball_manager);
							continue;
						}
					}
					if (m_components->enemy->getRightHand()->isAlive()) {
						if (fireball->isCollided(m_components->enemy->getRightHand()->getRealm())) {
							m_components->enemy->getRightHand()->damageByPlayerFireball(fireball, m_components->fireball_manager);
							continue;
						}
					}
				}


				for (auto& enemy : m_components->enemy_manager->getEnemyList()) {
					if (enemy->isAlive()) {
						if (fireball->isCollided(enemy->getRealm())) {
							enemy->damageByPlayerFireball(fireball, m_components->fireball_manager);
							continue_loop = true;
							break;
						}
					}
				}
			}
			if (continue_loop) continue;

			// Town の衝突判定
			for (auto& house : m_components->house_list) {
				if (house->isAlive()) {
					if (fireball->isCollided(house->getRealm())) {
						house->damageByFireball(fireball, m_components->fireball_manager);
						continue_loop = true;
						break;
					}
				}
			}
			if (continue_loop) continue;

			for (auto& resident : m_components->resident_list) {
				if (resident->isAlive()) {
					if (fireball->isCollided(resident->getRealm())) {
						resident->damageByFireball(fireball, m_components->fireball_manager);
						continue_loop = true;
						break;
					}
				}
			}
			if (continue_loop) continue;

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

	if (share.lives[0] > 0) {
		const int diff = (int)(share.rects[0].x - m_components->ship->left());
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

void BreakoutGame::updateFireballPosition()
{
	for (auto& fireball : m_components->fireball_manager->getFireballList()) {
		if (fireball->isDisappeared()) continue;
		fireball->updatePosition();
	}
}

void BreakoutGame::updateGameState()
{

	// 壁との衝突判定
	int kind = mode.getMode();

	enum Kind {
		Selecting = 0,
		Playing = 1, //!< Play画面
		Result = 2, //!< Result画面
	};
	switch (kind) {
	case Selecting:
	{
		static int initilized_cnt = 0;
		if (initilized_cnt < 10 && !m_is_mode_selected) {
			initilized_cnt++;
			return;
		}
		else {
			m_is_mode_selected = true;
			initilized_cnt = 0;
		}

		if (m_components->select->selected()) {
			const auto game_mode = m_components->select->getMode();
			switch (game_mode) {
			case Breakout::Mode::Easy:
				break;
			case Breakout::Mode::Hard:
				break;
			}
			m_components->info->init();
			mode.goNext();
			return;
		}

		static bool is_chattering = false;
		if (key[KEY_INPUT_UP] && !is_chattering) {
			m_components->select->move(Breakout::Move::Down);
			is_chattering = true;
		}
		else if (key[KEY_INPUT_DOWN] && !is_chattering) {
			m_components->select->move(Breakout::Move::Up);
			is_chattering = true;
		}
		else if (key[KEY_INPUT_RETURN]){
			m_components->select->select();
		}
		else {
			is_chattering = false;
		}
		break;
	}
	case Playing:
		m_components->info->addScoreAll();
		if (m_components->info->isLastPhase()) {
			m_components->background->turnLastPhase();
		}
		if (isGameClear()) {
			m_components->result->clearGame();
			mode.goNext();
			m_components->result->setFinalScore(m_components->info->getScore());
			return;
		}

		if (m_components->info->isTimeOver() ||
			!m_components->ship->isAlive() ||
			(m_components->house_list.size() == 0 &&
				m_components->resident_list.size() == 0)) {
			m_components->result->setFinalScore(m_components->info->getScore());
			mode.goNext();
			return;
		}

		if (key[KEY_INPUT_ESCAPE]) {
			m_components->result->setFinalScore(m_components->info->getScore());
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
	if (m_components->enemy->isAlive()) return false;
	for (auto& enemy : m_components->enemy_manager->getEnemyList()) {
		if (enemy->isAlive()) return false;
	}
	//for (const auto& block : m_components->block_list) {
	//	//壊せないブロックだったら勘定に入れない
	//	if (block->getBlockKind() == "UnbreakableBlock") continue;
	//	if (!block->isDisappeared()) {
	//		return false;
	//	}
	//}

	return true;
}

void BreakoutGame::updateBlockStatus() {}

void BreakoutGame::updatePotStatus() {
	for (auto& fireball : m_components->fireball_manager->getFireballList()) {
		// pot が使用済みならもう更新しない。
		if (!m_components->pot->isAvailable()) return;

		if (share.lives[0] > 0) {
			// potを出現させる
			if (share.rects[0].width < 180) {
				m_components->pot->appear();
			}
			// potが出現していたら
			else if (!m_components->pot->isDisappeared()) {
				// Phase: 吸い込んでから吐き出すまで

				// potをマーカーの位置に移動させる
				const Eigen::Vector2i diff_dist = Eigen::Vector2i((int)share.rects[0].x - m_components->pot->getRealm().left(), (int)share.rects[0].y - m_components->pot->getRealm().top());
				m_components->pot->translate(diff_dist / 10);

				// potをマーカーの回転にあわせる
				const float diff_rot = (float)share.rects[0].rotate - m_components->pot->getRotation();
				m_components->pot->rotate(diff_rot / 10.0);

				// fireball を中に持っていたら
				if (m_components->pot->hasFireball()) {
					//カウントを減らす
					m_components->pot->countDown();
					//吐き出すかどうか判断
					if (m_components->pot->isExhared()) {
						// 吐き出す
						m_components->pot->exhareFireball();
					}
					else if (share.rects[0].width > 250) {
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

					m_components->fireball_manager->destroy(fireball);
				}
			}
		}
	}
}

void BreakoutGame::updateEnemy() {
	m_components->enemy->updatePosition();
	m_components->enemy_manager->updatePosition();

	m_components->enemy_manager->destroyIfEffectIsFinished();

	// Enemyがファイアーボールをはく
	while (!m_components->fireball_manager->isMax()) {
		m_components->fireball_manager->add(m_components->enemy->makeFireball());
	}
	while (!m_components->enemy_manager->isMax()) {
		m_components->enemy_manager->add(m_components->enemy->makeEnemy());
	}

}

void BreakoutGame::updateTown() {
	for (auto& itr = m_components->house_list.begin(); itr != m_components->house_list.end();) {
		if ((*itr)->isEffectContinuing()) {
			(*itr)->updatePosition();
			++itr;
		}
		else {
			(*itr)->exhareResident();
			itr = m_components->house_list.erase(itr);
		}
	}

	for (auto& itr = m_components->resident_list.begin(); itr != m_components->resident_list.end();) {
		if ((*itr)->isEffectContinuing()) {
			//消えていたら
			if ((*itr)->isDisappeared()) {
				++itr;
				continue;
			}
			// fireballがでたら逃げ込む
			if (m_components->fireball_manager->getFireballList().size() != 0) {
				for (auto& house : m_components->house_list) {
					if (house->ifNullThenSetResident((*itr))) break;
				}
			}
			(*itr)->updatePosition();
			++itr;
		}
		else {
			itr = m_components->resident_list.erase(itr);
		}
	}
	
}

void BreakoutGame::shipVSEnemy() {
	for (auto& enemy : m_components->enemy_manager->getEnemyList()) {
		if (!enemy->isAlive()) continue;
		auto ship_realm = m_components->ship->getRealm();
		auto enemy_realm = enemy->getRealm();
		if (enemy_realm.isContacted(ship_realm)) {
			// もしエネミーの下がshipの上から1/3よりも上にあったら
			if (enemy_realm.bottom() < ship_realm.top() + ship_realm.height / 3) {
				//shipの上に乗っていると判断
				enemy->setIsOnShip(m_components->ship, true);
			}
			else {
				enemy->damageEnemy(2);
				//このダメージで死んだら
				if (!enemy->isAlive()) {
					enemy->setDeadEffect("b_enemy_vanish", 5);
				}
			}
		}
	}
}

void BreakoutGame::EnemyVSTown() {
	for (auto& enemy : m_components->enemy_manager->getEnemyList()) {
		for (auto& house : m_components->house_list) {
			enemy->attackOnTown(house);
		}
		for (auto& resident : m_components->resident_list) {
			enemy->attackOnTown(resident);
		}
	}
}