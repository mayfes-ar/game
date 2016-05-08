#pragma once
#include "breakout_object.h"

namespace Breakout
{
	class EnemyBase {
	public:
		EnemyBase() {}
		virtual ~EnemyBase() {}
		virtual std::shared_ptr<Fireball> makeFireball() = 0;
		// mode が enemy じゃない fireball にあたったらLifeを減らすようなメソッド
		virtual bool damageEnemy(int amount) = 0;
		// 何かしらのアクションでLifeを回復するメソッド。実装したくない場合はreturnすればよい
		virtual bool restoreEnemy() = 0;
		virtual void resetEnemy() = 0;
		virtual void updatePosition() = 0;

		std::shared_ptr<Fireball> makeFireball(Eigen::Vector2f velocity, FireballKind kind) {
			auto moving = std::make_shared<Moving>(1.0f, std::make_shared<NewtonBehavior>(), velocity, Eigen::Vector2f::Zero());
			auto fireball = std::make_shared<Fireball>(Shape::Circle(m_realm.getLeftBottomPoint(), FIREBALL_RADIUS), moving, kind);
			return fireball;
		};

		int getLifeNum() {
			return m_life.getLifeNum();
		}
		bool isAlive() const {
			return m_life.isAlive();
		}
		Shape::Rectangle getRealm() {
			return m_realm;
		}
	protected:
		Shape::Rectangle m_realm = Shape::Rectangle();
		Life m_life = Life();
		std::shared_ptr<Moving> m_moving = nullptr;
		std::random_device rnd;
		std::mt19937 mt = std::mt19937(rnd());

		std::uniform_real_distribution<double> velocity_generator = std::uniform_real_distribution<double>(FIREBALL_STARTVEL.x(), FIREBALL_STARTVEL.y());
		std::uniform_real_distribution<double> fireball_mode_generator = std::uniform_real_distribution<double>(0.0, 1.0);
};


class EnemyLeftHand : public Object, public EnemyBase {
public:
	EnemyLeftHand(Shape::Rectangle realm, std::shared_ptr<Moving> moving, Life life) {
		m_life = life;
		m_realm = realm;
		m_moving = moving;
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
		if (!isAlive()) return true;
		DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_left_hand"], TRUE);
		return true;
	}
private:

};

class EnemyRightHand : public Object, public EnemyBase {
public:
	EnemyRightHand(Shape::Rectangle realm, std::shared_ptr<Moving> moving, Life life) {
		m_life = life;
		m_realm = realm;
		m_moving = moving;
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
		if (!isAlive()) return true;
		DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_right_hand"], TRUE);
		return true;
	}
private:

};


class EnemyHead : public Object, public EnemyBase {
public:
	EnemyHead(Shape::Rectangle realm, std::shared_ptr<Moving>& moving, Life life, std::shared_ptr<EnemyLeftHand>& left_hand, std::shared_ptr<EnemyRightHand>& right_hand)
		: m_left_hand(left_hand), m_right_hand(right_hand)
	{
		m_life = life;
		m_realm = realm;
		m_moving = moving;
		Object::layer = PRIORITY_DYNAMIC_OBJECT;
	}

	EnemyHead(Shape::Rectangle realm, std::shared_ptr<Moving>& moving, Life life)
	{
		m_life = life;
		m_realm = realm;
		m_moving = moving;
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

	bool damageEnemy(int amount) override {
		// TODO mode (mode == プレイヤーだったら)
		// TODO fireballはあたったら消えるからmanagerから消す

		return m_life.damage(amount);
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
		if (!isAlive()) return true;
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