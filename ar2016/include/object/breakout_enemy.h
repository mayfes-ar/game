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
				if (m_explosion_count >= 0) {
					m_explosion_effect.incrementCounterWhenDrawWithRealm(m_realm);
					m_explosion_count--;
				}
			}
			return true;
		}

		bool isEffectContinuing() {
			return m_explosion_count >= 0;
		}

	protected:
		Shape::Rectangle m_realm = Shape::Rectangle();
		Life m_life = Life();
		std::shared_ptr<Moving> m_moving = std::make_shared<Moving>(1.0f / 30.0f, std::make_shared<NewtonBehavior>());
		std::random_device rnd;
		std::mt19937 mt = std::mt19937(rnd());

		std::uniform_real_distribution<double> velocity_generator = std::uniform_real_distribution<double>(FIREBALL_STARTVEL.x(), FIREBALL_STARTVEL.y());
		std::uniform_real_distribution<double> fireball_mode_generator = std::uniform_real_distribution<double>(0.0, 1.0);

	private:
		int m_explosion_count = 35;
		Effect m_explosion_effect = Effect(effectHandles["b_explosion"], 5, PRIORITY_DYNAMIC_OBJECT);
	};


	class EnemyLeftHand : public Object, public EnemyBase {
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

	class EnemyRightHand : public Object, public EnemyBase {
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


	class EnemyHead : public Object, public EnemyBase {
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
}