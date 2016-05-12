#pragma once
#include "breakout_object.h"

namespace Breakout
{
	class EnemyBase : public Object {
	public:
		EnemyBase() {}
		virtual ~EnemyBase() {}
		virtual std::shared_ptr<Fireball> makeFireball() = 0;
		
		// 何かしらのアクションでLifeを回復するメソッド。実装したくない場合はreturnすればよい
		virtual bool restoreEnemy() = 0;
		virtual void resetEnemy() = 0;
		
		virtual int giveDamage() {
			return 1;
		}

		virtual void attackOnTown(std::shared_ptr<House>& town) {
			//もししんでたら
			if (!town->isAlive() || !isAlive()) return;
			// もし接触していたら
			if (m_realm.isContacted(town->getRealm())) {
				town->damageTown(giveDamage());
				damageEnemy(town->giveDamage());
				if (!town->isAlive()) town->setDeadEffect("b_crawl", 5);
			}
			
			//if (!isAlive()) setDeadEffect("m_crawl", 5);
		}

		virtual void attackOnTown(std::shared_ptr<Resident>& town) {
			//もししんでたら
			if (!town->isAlive() || !isAlive()) return;
			// もし接触していたら
			if(m_realm.isContacted(town->getRealm())) {
				town->damageTown(giveDamage());
				damageEnemy(town->giveDamage());
				if (!town->isAlive()) town->setDeadEffect("b_crawl", 5);
			}
			//if (!isAlive()) setDeadEffect("m_crawl", 5);
		}

		virtual void updatePosition() {
			m_moving->updatePoistion(m_realm.start_point);
		}

		virtual void changeBehavior(std::shared_ptr<MovingBehavior>& behavior) {
			m_moving->setBehavior(behavior);
		}

		std::shared_ptr<Fireball> makeFireball(Eigen::Vector2f velocity, FireballKind kind) {
			auto moving = std::make_shared<Moving>(1.0f, std::make_shared<NewtonBehavior>(), velocity, Eigen::Vector2f::Zero());
			auto fireball = std::make_shared<Fireball>(Shape::Circle(m_realm.getLeftBottomPoint(), FIREBALL_RADIUS), moving, kind);
			return fireball;
		};

		int getLifeNum() {
			return m_life.getLifeNum();
		}

		// mode が enemy じゃない fireball にあたったらLifeを減らすようなメソッド
		virtual bool damageEnemy(int amount) {
			return m_life.damage(amount);
		}

		// もしplayerのfireballにあたったら
		virtual void damageByPlayerFireball(std::shared_ptr<Fireball>& fireball, std::shared_ptr<FireballManager>& manager) {
			m_life.damage(fireball->giveDamage());
			//触れたら消える
			manager->destroy(fireball);
		}

		bool isAlive() {
			return m_life.isAlive();
		}

		Shape::Rectangle getRealm() {
			return m_realm;
		}

		// 子クラスで呼んでもらう
		virtual bool draw() override {
			//死んでいて
			if (!isAlive()) {
				//燃える処理が終わってないなら
				if (m_dead_count >= 0) {
					m_dead_effect.incrementCounterWhenDrawWithRealm(m_realm);
					m_dead_count--;
				}
			}
			return true;
		}

		bool isEffectContinuing() {
			return m_dead_count >= 0;
		}

		void setDeadEffect(std::string effect_name, int frames_per_scene, int dead_effect = 0) {
			m_dead_effect = Effect(effectHandles[effect_name], frames_per_scene, PRIORITY_DYNAMIC_OBJECT);
			if (dead_effect == 0) dead_effect = m_dead_effect.getCounter() * m_dead_effect.getFramesPerScene();
		}

	protected:
		Shape::Rectangle m_realm = Shape::Rectangle();
		Life m_life = Life();
		std::shared_ptr<Moving> m_moving = std::make_shared<Moving>(1.0f / 30.0f, std::make_shared<NewtonBehavior>());
		std::random_device rnd;
		std::mt19937 mt = std::mt19937(rnd());

		std::uniform_real_distribution<double> velocity_generator = std::uniform_real_distribution<double>(FIREBALL_STARTVEL.x(), FIREBALL_STARTVEL.y());
		std::uniform_real_distribution<double> fireball_mode_generator = std::uniform_real_distribution<double>(0.0, 1.0);
		std::uniform_real_distribution<double> enemy_kind_generator = std::uniform_real_distribution<double>(0.0, 1.0);

	private:
		Effect m_dead_effect = Effect(effectHandles["b_explosion"], 5, PRIORITY_DYNAMIC_OBJECT);
		int m_dead_count = 35;
	};

	// 人や住居とぶつかったら爆発して死ぬ。相手にもダメージを与える
	class SnakeEnemy : public EnemyBase {
	public:
		SnakeEnemy(Shape::Rectangle realm, std::shared_ptr<MovingBehavior> behavior, Life life) {
			m_life = life;
			m_realm = realm;
			m_moving->setBehavior(behavior);
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}
		SnakeEnemy(Shape::Rectangle realm, std::shared_ptr<MovingBehavior> behavior, Eigen::Vector2f& velocity , Life life) {
			m_life = life;
			m_realm = realm;
			m_moving->setBehavior(behavior);
			m_moving->setVelocity(velocity);
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}

		// 別にやらないので
		std::shared_ptr<Fireball> makeFireball() { return std::make_shared<Fireball>(); };

		virtual bool restoreEnemy() {
			return m_life.restore(1);
		}
		void resetEnemy() override {
			m_life.resetLife();
		}

		// 壁に沿って降りてから横にランダムに移動する。
		void updatePosition() override {
			if (m_realm.bottom() >= FIELD_START_POS.y() + FIELD_HEIGHT) {
				std::shared_ptr<MovingBehavior> rnd_behavior = std::make_shared<RandomBehavior>(FIELD_START_POS.x(),
					FIELD_START_POS.x() + FIELD_WIDTH - SNAKE_WIDTH,
					// ここで-2することでもう一度randomがセットされるのを防いでいる
					FIELD_START_POS.y() + FIELD_HEIGHT - SNAKE_HEIGHT - 2,
					FIELD_START_POS.y() + FIELD_HEIGHT - SNAKE_HEIGHT - 2);
				m_moving->setBehavior(rnd_behavior);
			}
			else if (m_realm.left() <= FIELD_START_POS.x() || m_realm.right() >= FIELD_START_POS.x() + FIELD_WIDTH) {
				m_moving->setVelocity(Eigen::Vector2f(0.0f, 60.0f));
			}
			
			m_moving->updatePoistion(m_realm.start_point);
		}

		bool draw() override {
			if (!isAlive()) {
				if (isEffectContinuing()) {
					DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_snake_weak"], TRUE);
					EnemyBase::draw();
				}
				return true;
			}
			DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_snake_weak"], TRUE);
			return true;
		}
	};



	class EnemyLeftHand : public EnemyBase {
	public:
		EnemyLeftHand(Shape::Rectangle realm, std::shared_ptr<MovingBehavior> behavior, Life life) {
			m_life = life;
			m_realm = realm;
			m_moving->setBehavior(behavior);
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}
		std::shared_ptr<Fireball> makeFireball() override {
			Eigen::Vector2f base_vel(velocity_generator(mt), velocity_generator(mt));
			FireballKind kind;
			float fireball_kind_prob = 0.8;

			if (fireball_mode_generator(mt) > fireball_kind_prob) kind = FireballKind::EnemyStrong;
			else kind = FireballKind::EnemyWeak;

			return EnemyBase::makeFireball(base_vel, kind);
		}
		std::shared_ptr<EnemyBase> makeEnemy() {
			// 横下に投げ出す
			Eigen::Vector2f base_vel(-100.0f, 30.0f);
			
			//if (enemy_kind_generator(mt) > 0.6) {
				std::shared_ptr<SnakeEnemy> snake = std::make_shared<SnakeEnemy>(Shape::Rectangle(m_realm.getLeftBottomPoint(), SNAKE_WIDTH, SNAKE_HEIGHT), std::make_shared<NewtonBehavior>(), base_vel, Life(SNAKE_ENEMY_LIFE, SNAKE_ENEMY_LIFE));
				return snake;
			//} else {
				//return nullptr;
			//}
		}


		// 何かしらのアクションでLifeを回復するメソッド。実装したくない場合はreturnすればよい
		bool restoreEnemy() override {
			return m_life.restore(1);
		}
		void resetEnemy() override {
			m_life.resetLife();
		}

		bool draw() override {
			if (!isAlive()) {
				if (isEffectContinuing()) {
					DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_left_hand"], TRUE);
					EnemyBase::draw();
				}
				return true;
			}
			DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_left_hand"], TRUE);
			return true;
		}
	private:

	};

	class EnemyRightHand : public EnemyBase {
	public:
		EnemyRightHand(Shape::Rectangle realm, std::shared_ptr<MovingBehavior> behavior, Life life) {
			m_life = life;
			m_realm = realm;
			m_moving->setBehavior(behavior);
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}
		std::shared_ptr<Fireball> makeFireball() override {
			Eigen::Vector2f base_vel(velocity_generator(mt), velocity_generator(mt));
			FireballKind kind;
			float fireball_kind_prob = 0.8;

			if (fireball_mode_generator(mt) > fireball_kind_prob) kind = FireballKind::EnemyStrong;
			else kind = FireballKind::EnemyWeak;

			return EnemyBase::makeFireball(base_vel, kind);
		}
		std::shared_ptr<EnemyBase> makeEnemy() {
			// 横下に投げ出す
			Eigen::Vector2f base_vel(100.0f, 30.0f);

			//if (enemy_kind_generator(mt) > 0.6) {
				std::shared_ptr<SnakeEnemy> snake = std::make_shared<SnakeEnemy>(Shape::Rectangle(m_realm.getRightBottomPoint(), SNAKE_WIDTH, SNAKE_HEIGHT), std::make_shared<NewtonBehavior>(), base_vel, Life(SNAKE_ENEMY_LIFE, SNAKE_ENEMY_LIFE));
				return snake;
			//} else {
				//return nullptr;
			//}
		}
		bool damageEnemy(int amount) override {
			// TODO mode (mode == プレイヤーだったら)
			// TODO fireballはあたったら消えるからmanagerから消す

			return m_life.damage(amount);
		}
		void updatePosition() override {
			m_moving->updatePoistion(m_realm.start_point);
		}

		// 何かしらのアクションでLifeを回復するメソッド。実装したくない場合はreturnすればよい
		bool restoreEnemy() override {
			return m_life.restore(1);
		}
		void resetEnemy() override {
			m_life.resetLife();
		}

		bool draw() override {
			if (!isAlive()) {
				if (isEffectContinuing()) {
					DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_right_hand"], TRUE);
					EnemyBase::draw();
				}
				return true;
			}
			DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_right_hand"], TRUE);
			return true;
		}
	private:

	};


	class EnemyHead : public EnemyBase {
	public:
		EnemyHead(Shape::Rectangle realm, std::shared_ptr<MovingBehavior>& behavior, Life life, std::shared_ptr<EnemyLeftHand>& left_hand, std::shared_ptr<EnemyRightHand>& right_hand)
			: m_left_hand(left_hand), m_right_hand(right_hand)
		{
			m_life = life;
			m_realm = realm;
			m_moving->setBehavior(behavior);
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}

		EnemyHead(Shape::Rectangle realm, std::shared_ptr<MovingBehavior>& behavior, Life life)
		{
			m_life = life;
			m_realm = realm;
			m_moving->setBehavior(behavior);
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}

		std::shared_ptr<Fireball> makeFireball() override {
			std::uniform_real_distribution<double> select_enemy_generator = std::uniform_real_distribution<double>(0.0, 1.0);
			double ratio = select_enemy_generator(mt);
			if (ratio > 0.7 && hasLeft()) {
				return m_left_hand->makeFireball();
			}
			else if (ratio > 0.4 && hasRight()) {
				return m_right_hand->makeFireball();
			}

			Eigen::Vector2f base_vel(velocity_generator(mt), velocity_generator(mt));
			FireballKind kind;
			float fireball_kind_prob;
			if (hasLeft() && hasRight()) fireball_kind_prob = 0.8;
			else if (hasLeft() || hasRight()) fireball_kind_prob = 0.6;
			else fireball_kind_prob = 0.4;

			if (fireball_mode_generator(mt) > fireball_kind_prob) kind = FireballKind::EnemyStrong;
			else kind = FireballKind::EnemyWeak;

			return EnemyBase::makeFireball(base_vel, kind);
		}

		std::shared_ptr<EnemyBase> makeEnemy() {
			// 右手と左手どちらから投げるかを決める
			std::uniform_real_distribution<double> leftOrRight = std::uniform_real_distribution<double>(0.0, 1.0);
			double ratio = leftOrRight(mt);
			if (ratio > 0.5 && hasLeft()) {
				return m_left_hand->makeEnemy();
			}
			else if(ratio > 0.0 && hasRight()) {
				return m_right_hand->makeEnemy();
			}
		}

		void updatePosition() override {
			m_moving->updatePoistion(m_realm.start_point);
			if (hasLeft()) m_left_hand->updatePosition();
			if (hasRight()) m_right_hand->updatePosition();
		}

		// 何かしらのアクションでLifeを回復するメソッド。実装したくない場合はreturnすればよい
		bool restoreEnemy() override {
			return m_life.restore(1);
		}
		void resetEnemy() override {
			m_life.resetLife();
		}

		bool draw() override {
			if (!isAlive()) {
				if (isEffectContinuing()) {
					DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_donald"], TRUE);
					EnemyBase::draw();
				}
				return true;
			}
			DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_donald"], TRUE);

			if (hasLeft()) m_left_hand->draw();
			if (hasRight()) m_right_hand->draw();
			return true;
		}

		bool hasLeft() {
			return m_left_hand != nullptr ? true : false;
		}

		bool hasRight() {
			return m_right_hand != nullptr ? true : false;
		}

		std::shared_ptr<EnemyLeftHand> getLeftHand() {
			return m_left_hand;
		}
		std::shared_ptr<EnemyRightHand> getRightHand() {
			return m_right_hand;
		}
	private:
		std::shared_ptr<EnemyLeftHand> m_left_hand = nullptr;
		std::shared_ptr<EnemyRightHand> m_right_hand = nullptr;
	};

	class EnemyManager : public Object {
	public:
		EnemyManager(int max_num) : m_max_num(max_num) {
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}

		bool isMax() {
			return m_enemy_list.size() == m_max_num;
		}

		bool add(std::shared_ptr<EnemyBase>& enemy) {
			if (enemy == nullptr) return false;
			if (m_enemy_list.size() == m_max_num) return false;
			m_enemy_list.push_back(enemy);
			return true;
		}
		// index を渡して消す
		bool destroy(int index) {
			if (m_enemy_list.size() <= index + 1) return false;
			m_enemy_list.erase(m_enemy_list.begin() + index);
			return true;
		}

		bool destroy(std::shared_ptr<EnemyBase>& enemy) {
			for (int i = 0; i < m_enemy_list.size(); i++) {
				if (m_enemy_list[i] != enemy) continue;
				m_enemy_list.erase(m_enemy_list.begin() + i);
				return true;
			}
			return false;
		}

		void destroyIfEffectIsFinished() {
			for (auto& itr = m_enemy_list.begin(); itr != m_enemy_list.end();) {
				if ((*itr)->isEffectContinuing()) {
					(*itr)->updatePosition();
					++itr;
				}
				else {
					itr = m_enemy_list.erase(itr);
				}
			}
		}

		void updatePosition() {
			for (auto& enemy : m_enemy_list) {
				enemy->updatePosition();
			}
		}

		std::vector<std::shared_ptr<EnemyBase>> getEnemyList() {
			return m_enemy_list;
		}

		bool draw() override {
			bool isSuccess = true;
			for (auto& enemy : m_enemy_list) {
				isSuccess &= enemy->draw();
			}
			return isSuccess;
		}

	private:
		std::vector<std::shared_ptr<EnemyBase>> m_enemy_list = {};
		int m_max_num = 0;
	};
}