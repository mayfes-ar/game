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
		int diff = (int)(share.rects[0].x - m_components->ship->left());
		if (std::abs((double)diff) / 5.0 < 1.0) { // チャッタリング対策
			diff = 0;
		}
		else if (std::abs((double)diff) / 5.0 < 2.0) {
			m_components->ship->translate(diff / 6);
		}
		m_components->ship->translate(diff / 3);
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
		TutorialShip,
		TutorialPot,
		CountDown,
		Playing, //!< Play画面
		Result, //!< Result画面
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
				m_components->increaseBlock(0.0);
				break;
			case Breakout::Mode::Normal:
				m_components->increaseBlock(0.15);
				break;
			case Breakout::Mode::Hard:
				m_components->increaseBlock(0.3);
				m_components->fireball_manager->changeMaximumFireballNum(Breakout::MAX_FIREBALL_NUM_ON_HARD);
				break;
			}
			// 次の画面の準備
			mode.goNext();
			return;
		}

		static bool is_chattering = false;
		if (key[KEY_INPUT_UP] && !is_chattering) {
			is_chattering = true;
			m_components->select->move(Breakout::Move::Down);
		}
		else if (key[KEY_INPUT_DOWN] && !is_chattering) {
			is_chattering = true;
			m_components->select->move(Breakout::Move::Up);
		}
		else if (!key[KEY_INPUT_DOWN] && !key[KEY_INPUT_UP]) {
			is_chattering = false;
		}

		if (key[KEY_INPUT_RETURN]) {
			m_components->select->select();
		}
		break;
	}
	case TutorialShip: {
		if (!tutorial_ship() || (key[KEY_INPUT_S] && key[KEY_INPUT_1])) {
			drawList.clear();
			mode.goNext();
			
		}
		break;
	}
	case TutorialPot: {
		if (key[KEY_INPUT_R]) {
			setup_tutorial_pot();
		}
		if (!tutorial_pot() || (key[KEY_INPUT_S] && key[KEY_INPUT_2])) {
			drawList.clear();
			mode.goNext();
			
		}
		break;
	}
	case CountDown: {
		if (!m_components->count_down->isCountdowning()) {
			m_components->info->init();
			m_components->background->init();
			mode.goNext();
		}
		break;
	}
	case Playing:
		updateCollisionDetection();
		updateFireballPosition();
		moveShip();
		updateBlockStatus();
		updatePotStatus();
		updateEnemy();
		updateTown();
		shipVSEnemy();
		EnemyVSTown();

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
			(m_components->house_list.size() == 0 /*&&
				m_components->resident_list.size() == 0*/)) {
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
		if (key[KEY_INPUT_RETURN]) {
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
		if (!m_components->pot->isAvailable()) {
			m_components->pot->resetStatus();
		}

		if (share.lives[0] > 0) {
			// potを出現させる
			if (share.rects[0].width < 180) {
				m_components->pot->appear();
			}
			// potが出現していたら
			else if (!m_components->pot->isDisappeared()) {
				// Phase: 吸い込んでから吐き出すまで

				// potをマーカーの位置に移動させる
				const Eigen::Vector2i diff_dist = Eigen::Vector2i((int)share.rects[0].x + share.rects[0].width / 2 - m_components->pot->getRealm().left(), (int)share.rects[0].y + share.rects[0].height / 2 - m_components->pot->getRealm().top());
				m_components->pot->translate(diff_dist / 5);

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
		/*for (auto& resident : m_components->resident_list) {
			enemy->attackOnTown(resident);
		}*/
	}
}

void BreakoutGame::updateInfo() {

}

bool BreakoutGame::tutorial_ship() {

	moveShip();

	
	if (!m_components->himes[0]->hasFukidashi()) {
		m_components->himes.erase(m_components->himes.begin());
		if (m_components->himes.size() == 0) {
			return false;
		}
		drawList.push_back(m_components->himes[0]);
	}

	if (m_components->himes.size() <= 6) {
		m_components->fireball->updatePosition();
	}

	auto fireball_effect = m_components->fireball->returnFireballReflect(m_components->ship->getRealm(), 1, m_components->ship->getVelocity());
	if (fireball_effect->isCollide() && m_components->himes.size() == 5) {
		//跳ね返すときは反射effectを入れる
		drawList.push_back(fireball_effect);
		m_components->fireball->changeModeToPlayer();

		//跳ね返りを確認したら次の言葉へ
		m_components->himes[0]->finishDraw();
		m_components->himes.erase(m_components->himes.begin());
		drawList.push_back(m_components->himes[0]);
	}

	//エネミーだったらとおりぬける
	if (!m_components->fireball->isEnemy()) {
		if (m_components->fireball->isCollided(m_components->block->getRealm())) {
			//跳ね返りを確認したら次の言葉へ
			m_components->himes[0]->finishDraw();
			m_components->himes.erase(m_components->himes.begin());
			drawList.push_back(m_components->himes[0]);
			m_components->block->disappear();
		}
	}
	return true;
}

void BreakoutGame::setup_tutorial_ship() {
	using namespace Breakout;

	drawList.clear();

	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  空に\nファイアーボール\nが見えるかしら？", 60));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  家に当たったら\n燃えてしまう\nから、私たちで\n守らなきゃ！", 80));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  町の防衛システム\nである`SHIP`\nを使って、\n跳ね返してみて！", 60));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  マーカーを\n左右に動かす\nことで\n操作できるわ！", 100));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  ブロックを\nすり抜けたわ", 1000));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  跳ね返ったわ！\n色が変わって\n緑になったわね",1000));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  今度はブロックに\nあたったわ。", 70));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  緑のときは\n敵とブロック\nにもあたる\nようになるの", 70));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  それでは次に行きましょう！", 60));

	const auto circle = Shape::Circle(FIREBALL_STARTPOS + Eigen::Vector2i(0, 20), FIREBALL_RADIUS);

	const Eigen::Vector2f start_vel = Eigen::Vector2f(0, 100.0f);
	const Eigen::Vector2f start_accel = Eigen::Vector2f::Zero();
	auto moving = std::make_shared<Moving>(0.03f, std::make_shared<NewtonBehavior>());
	m_components->fireball = std::make_shared<Breakout::Fireball>(circle, moving, EnemyWeak);
	m_components->fireball->setVelocity(start_vel);
	m_components->block = std::make_shared<NormalBlock>(Shape::Rectangle(FIREBALL_STARTPOS + Eigen::Vector2i(0, 150), BLOCK_WIDTH, BLOCK_HEIGHT), Breakout::Block::Color::Green);
	m_components->block->appear();

	drawList.push_back(m_components->himes[0]);
	drawList.push_back(m_components->fireball);
	drawList.push_back(m_components->background);
	drawList.push_back(m_components->ship);
	drawList.push_back(m_components->block);
}

bool BreakoutGame::tutorial_pot() {
	using namespace Breakout;

	//shipを動かす。
	moveShip();


	if (!m_components->himes[0]->hasFukidashi()) {
		m_components->himes.erase(m_components->himes.begin());
		if (m_components->himes.size() == 0) {
			return false;
		}
		drawList.push_back(m_components->himes[0]);
	}

	if (m_components->himes.size() == 15) {
		m_components->fireball->setVelocity(Eigen::Vector2f(0, 100.0f));
		m_components->fireball->updatePosition();
		if (m_components->fireball->isCollided(m_components->ship->getRealm(), 1, m_components->ship->getVelocity())) {
			
			m_components->fireball->disappear();
			m_components->ship->damageShip(1);

			m_components->himes[0]->finishDraw();
			m_components->himes.erase(m_components->himes.begin());
			drawList.push_back(m_components->himes[0]);
		}
	}
	else if (m_components->himes.size() == 11) {
		
		//potが出現したら
		if (share.lives[0] > 0) {
			// potを出現させる
			if (share.rects[0].width < POT_APPEAR_DEPTH) {
				m_components->pot->appear();

				m_components->himes[0]->finishDraw();
				m_components->himes.erase(m_components->himes.begin());
				drawList.push_back(m_components->himes[0]);
			}
		}
	}
	else if (m_components->himes.size() == 9) {
		m_components->fireball->setPosition(FIREBALL_STARTPOS);
		m_components->fireball->setVelocity(Eigen::Vector2f(0, 100.0f));
		m_components->fireball->appear();

		m_components->himes[0]->finishDraw();
		m_components->himes.erase(m_components->himes.begin());
		drawList.push_back(m_components->himes[0]);
	}
	else if (m_components->himes.size() == 8) {
		m_components->fireball->updatePosition();
		if (share.lives[0] > 0) {
			// potを出現させる
			if (share.rects[0].width < POT_APPEAR_DEPTH) {
				m_components->pot->appear();
			}
			// potが出現していたら
			else if (!m_components->pot->isDisappeared()) {
				// Phase: 吸い込んでから吐き出すまで

				// potをマーカーの位置に移動させる
				const Eigen::Vector2i diff_dist = Eigen::Vector2i((int)share.rects[0].x + share.rects[0].width / 2 - m_components->pot->getRealm().left(), (int)share.rects[0].y + share.rects[0].height / 2 - m_components->pot->getRealm().top());
				m_components->pot->translate(diff_dist / 10);

				// potをマーカーの回転にあわせる
				const float diff_rot = (float)share.rects[0].rotate - m_components->pot->getRotation();
				m_components->pot->rotate(diff_rot / 10.0);

				//// fireball を中に持っていたら
				//if (m_components->pot->hasFireball()) {
				//	//カウントを減らす
				//	m_components->pot->countDown();
				//	//吐き出すかどうか判断
				//	if (m_components->pot->isExhared()) {
				//		// 吐き出す
				//		m_components->pot->exhareFireball();
				//	}
				//	else if (share.rects[0].width > POT_EXHARE_DEPTH) {
				//		m_components->pot->exhareFireball();
				//	}
				//}

				////　すでにfireballを吸い込んでいたら衝突判定はしない
				//// もしfireballが見えなくなっている(消えている)状態だったら衝突判定はしない
				//// すでに放出していたら衝突判定はしない
				//if (m_components->fireball->isDisappeared() ||
				//	m_components->pot->isExhared() ||
				//	m_components->pot->hasFireball()) return;


				// Phase: 出現してから吸い込むまで
				auto rotatedTopLine = m_components->pot->getRealm().getTopLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());
				auto rotatedLeftLine = m_components->pot->getRealm().getLeftLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());
				auto rotatedRightLine = m_components->pot->getRealm().getRightLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());
				auto rotatedBottomLine = m_components->pot->getRealm().getBottomLine().getRotatedLine(m_components->pot->getRealm().getCenterPoint(), m_components->pot->getRotation());

				// fireballがpotの口の辺上にあったらそれを吸い込む
				// 次に進む
				if (CollisionDetection::isOnLine(m_components->fireball->getRealm(), rotatedTopLine)) {
					m_components->pot->inhareFireball(m_components->fireball);
						
					m_components->himes[0]->finishDraw();
					m_components->himes.erase(m_components->himes.begin());
					drawList.push_back(m_components->himes[0]);
				}
				// ほかの辺にぶつかったらfireballを吸い込まずに消す
				else if ((CollisionDetection::isOnLine(m_components->fireball->getRealm(), rotatedLeftLine)) ||
					(CollisionDetection::isOnLine(m_components->fireball->getRealm(), rotatedRightLine)) ||
					(CollisionDetection::isOnLine(m_components->fireball->getRealm(), rotatedBottomLine))) {

				}
			}
		}
	}
	else if (m_components->himes.size() == 6) {
		m_components->fireball->updatePosition();
		if (m_components->pot->hasFireball()) {
			//カウントを減らす
			m_components->pot->countDown();
			//吐き出すかどうか判断
			if (m_components->pot->isExhared()) {
				// 吐き出す
				m_components->pot->exhareFireball();

				m_components->himes[0]->finishDraw();
				m_components->himes.erase(m_components->himes.begin());
				drawList.push_back(m_components->himes[0]);
			}
			else if (share.rects[0].width > POT_EXHARE_DEPTH) {
				m_components->pot->exhareFireball();
			
				m_components->himes[0]->finishDraw();
				m_components->himes.erase(m_components->himes.begin());
				drawList.push_back(m_components->himes[0]);
			}
		}
	}
	else if(m_components->himes.size() <= 5) {
		m_components->fireball->updatePosition();
	}

	//auto fireball_effect = m_components->fireball->returnFireballReflect(m_components->ship->getRealm(), 1, m_components->ship->getVelocity());
	//if (fireball_effect->isCollide()) {
	//	//跳ね返すときは反射effectを入れる
	//	drawList.push_back(fireball_effect);
	//	m_components->fireball->changeModeToPlayer();

	//	//跳ね返りを確認したら次の言葉へ
	//	m_components->himes[0]->finishDraw();
	//	m_components->himes.erase(m_components->himes.begin());
	//	drawList.push_back(m_components->himes[0]);
	//}
	return true;
}

void BreakoutGame::setup_tutorial_pot() {
	using namespace Breakout;

	drawList.clear();
	m_components->himes.clear();

	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  空から\nより大きな\nファイアボール、\nいや、サンダーボール\nが落ちてくるわ！", 1000, "b_hime_odoroki"));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  あぁ、Shipが\n削られてしまったわ", 60));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  サンダーボールは\n通常のShip\nでは跳ね返せないの", 80));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  でも安心して！\n私たちには\n`Pot`という\nもうひとつの\n防衛システム\nがあるの", 120));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  マーカーを\n後ろに引いてみて", 1000));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  その調子よ！もう一度\nサンダーボールが\n来るわ！", 70));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("あ", 10));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  Potの口を\nサンダーボールに\nあわせることで\n吸い込むこと\nができるわ", 1000));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  吸い込んだら\n吐き出す！", 70));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  マーカーを前に\n押し出すか、\nカウントが0\nになるまで\n待つと、\n吐き出せるの", 1000));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  サンダーボール\nを吐き出したものは\n大きなダメージ\nを敵に与えられるわ", 70));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  それでは次に行きましょう", 70));

	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  ほかにも\nブロックからは\nアイテムが落ちたり、\n", 70));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  するのだけど、\n時間がもうないから\n実戦で身に\n着けてもらうわ", 80));
	m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  それでは防衛開始よ！！", 80));

	const auto circle = Shape::Circle(FIREBALL_STARTPOS + Eigen::Vector2i(0, 20), FIREBALL_RADIUS);

	const Eigen::Vector2f start_vel = Eigen::Vector2f(0, 60.0f);
	const Eigen::Vector2f start_accel = Eigen::Vector2f::Zero();
	auto moving = std::make_shared<Moving>(0.03f, std::make_shared<NewtonBehavior>());
	m_components->fireball = std::make_shared<Breakout::Fireball>(circle, moving, EnemyStrong);
	m_components->fireball->setVelocity(start_vel);

	drawList.push_back(m_components->himes[0]);
	drawList.push_back(m_components->fireball);
	drawList.push_back(m_components->background);
	drawList.push_back(m_components->ship);
	drawList.push_back(m_components->pot);
}