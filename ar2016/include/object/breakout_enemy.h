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
				m_is_damaged = 30;
			}
		}

		virtual void attackOnTown(std::shared_ptr<Resident>& town) {
			//もししんでたら
			if (!town->isAlive() || !isAlive()) return;
			// もし接触していたら
			if(m_realm.isContacted(town->getRealm())) {
				town->damageTown(giveDamage());
				damageEnemy(town->giveDamage());
				if (!town->isAlive()) town->setDeadEffect("b_crawl", 5);
				m_is_damaged = 30;
			}
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
			// 顔の真下から出す
			if (kind == EnemyStrong) {
				fireball->setPosition(m_realm.getCenterPoint() + Eigen::Vector2i(0, m_realm.height));
				fireball->setVelocity(Eigen::Vector2f::Zero());
				fireball->setRadius(2);
			}
			return fireball;
		};

		int getLifeNum() {
			return m_life.getLifeNum();
		}

		// mode が enemy じゃない fireball にあたったらLifeを減らすようなメソッド
		virtual bool damageEnemy(int amount) {
			m_is_damaged = 30;
			return m_life.damage(amount);
		}

		// もしplayerのfireballにあたったら
		virtual void damageByPlayerFireball(std::shared_ptr<Fireball>& fireball, std::shared_ptr<FireballManager>& manager) {
			m_life.damage(fireball->giveDamage());
			//触れたら消える
			manager->destroy(fireball);
			m_is_damaged = 30;
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
				if (isDamagedEffectContinuing()) {
					int monotonedHandle, width, height;
					GetGraphSize(imgHandles[m_enemy_img_name], &width, &height);
					monotonedHandle = MakeScreen(width, height, TRUE);
					GraphFilterBlt(imgHandles[m_enemy_img_name], monotonedHandle, DX_GRAPH_FILTER_MONO, -60, 7);
					DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), monotonedHandle, TRUE);
				}
				if (m_dead_count >= 0) {
					m_dead_effect.incrementCounterWhenDrawWithRealm(m_realm);
					m_dead_count--;
				}
			} else {
				if (isDamagedEffectContinuing()) {
					int monotonedHandle, width, height;
					GetGraphSize(imgHandles[m_enemy_img_name], &width, &height);
					monotonedHandle = MakeScreen(width, height, TRUE);
					GraphFilterBlt(imgHandles[m_enemy_img_name], monotonedHandle, DX_GRAPH_FILTER_MONO, -60, 7);
					DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), monotonedHandle, TRUE);
					return true;
				}
				DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles[m_enemy_img_name], TRUE);
			}
			return true;
		}

		bool isEffectContinuing() {
			return m_dead_count >= 0;
		}

		void setDeadEffect(std::string effect_name, int frames_per_scene, int dead_count = 0) {
			m_dead_effect = Effect(effectHandles[effect_name], frames_per_scene, PRIORITY_DYNAMIC_OBJECT);
			if (dead_count == 0) dead_count = m_dead_effect.getCountMax() * m_dead_effect.getFramesPerScene();
			m_dead_count = dead_count;
		}

		//ダメージを受けたframeに判断する
		bool isDamaged() {
			return m_is_damaged == 30;
		}

		bool getIsOnShip() { return m_is_on_ship; }

		void setIsOnShip(std::shared_ptr<Ship>& ship, bool isOrNot) { 
			m_is_on_ship = isOrNot;
			m_ship = ship;
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
		// ダメージを受けたら30になって、毎フレーム減っていく。draw()に影響を与える
		int m_is_damaged = 0;
		bool m_is_on_ship = false;
		int on_ship_duration = 0;
		std::shared_ptr<Breakout::Ship> m_ship = nullptr;

		//ダメージを受けた後の効果が継続しているかどうか
		bool isDamagedEffectContinuing() {
			m_is_damaged--;
			return m_is_damaged > 0;
		}

		std::string m_enemy_img_name;

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
			m_enemy_img_name = "b_snake_weak";
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}
		SnakeEnemy(Shape::Rectangle realm, std::shared_ptr<MovingBehavior> behavior, Eigen::Vector2f& velocity , Life life) {
			m_life = life;
			m_realm = realm;
			m_moving->setBehavior(behavior);
			m_moving->setVelocity(velocity);
			m_enemy_img_name = "b_snake_weak";
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
			//もし死んでいたら動かないようにする。
			if (!isAlive()) return;
			//船の上に60frameいたら死ぬ
			if (on_ship_duration > 60) {
				damageEnemy(3);
				setDeadEffect("b_burning", 3, 35);
				on_ship_duration = 0;
			}

			if (getIsOnShip()) {
				//もし船が強化されていたら
				if (m_ship->isEnhanced()) {
					damageEnemy(3);
					setDeadEffect("b_burning", 3, 35);
					on_ship_duration = 0;
				}
				std::shared_ptr<MovingBehavior> rnd_behavior = std::make_shared<RandomBehavior>(
					m_ship->getRealm().left(),
					m_ship->getRealm().right() - SNAKE_WIDTH,
					m_ship->getRealm().top() - SNAKE_HEIGHT + 2,
					m_ship->getRealm().top() - SNAKE_HEIGHT + 2);
				m_moving->setBehavior(rnd_behavior);
				if (m_realm.left() > m_ship->getRealm().right()) {
					std::shared_ptr<MovingBehavior> newton = std::make_shared<NewtonBehavior>();
					m_moving->setBehavior(newton);
					m_moving->setVelocity(Eigen::Vector2f(30.0f, -20.0f));
					m_moving->setAccel(Eigen::Vector2f(0, 50.0f));
					std::shared_ptr<Ship> null_ship = nullptr;
					setIsOnShip(null_ship, false);
				} else if(m_realm.right() < m_ship->getRealm().left()) {
					std::shared_ptr<MovingBehavior> newton = std::make_shared<NewtonBehavior>();
					m_moving->setBehavior(newton);
					m_moving->setVelocity(Eigen::Vector2f(-30.0f, -20.0f));
					m_moving->setAccel(Eigen::Vector2f(0, 100.0f));
					std::shared_ptr<Ship> null_ship = nullptr;
					setIsOnShip(null_ship, false);
				}
				on_ship_duration++;
			}
			else if (m_realm.bottom() >= FIELD_START_POS.y() + FIELD_HEIGHT) {
				std::shared_ptr<MovingBehavior> rnd_behavior = std::make_shared<RandomBehavior>(FIELD_START_POS.x(),
					FIELD_START_POS.x() + FIELD_WIDTH - SNAKE_WIDTH,
					// ここで-2することでもう一度randomがセットされるのを防いでいる
					FIELD_START_POS.y() + FIELD_HEIGHT - SNAKE_HEIGHT - 2,
					FIELD_START_POS.y() + FIELD_HEIGHT - SNAKE_HEIGHT - 2);
				m_moving->setBehavior(rnd_behavior);
			}
			else if (m_realm.left() <= FIELD_START_POS.x() || m_realm.right() >= FIELD_START_POS.x() + FIELD_WIDTH) {
				if(m_moving->getAccel().y() == 0)
					m_moving->setVelocity(Eigen::Vector2f(0.0f, 30.0f));
				m_moving->setVelocity(Eigen::Vector2f(0.0f, m_moving->getVelocity().y()));
				m_moving->setAccel(Eigen::Vector2f(0.0f, 50.0f));
			}
			
			m_moving->updatePoistion(m_realm.start_point);
		}
	};



	class EnemyLeftHand : public EnemyBase {
	public:
		EnemyLeftHand(Shape::Rectangle realm, std::shared_ptr<MovingBehavior> behavior, Life life) {
			m_life = life;
			m_realm = realm;
			m_moving->setBehavior(behavior);
			m_enemy_img_name = "b_left_hand";
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}
		std::shared_ptr<Fireball> makeFireball() override {
			Eigen::Vector2f base_vel(velocity_generator(mt), velocity_generator(mt));
			FireballKind kind = FireballKind::EnemyWeak;

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

	private:

	};

	class EnemyRightHand : public EnemyBase {
	public:
		EnemyRightHand(Shape::Rectangle realm, std::shared_ptr<MovingBehavior> behavior, Life life) {
			m_life = life;
			m_realm = realm;
			m_moving->setBehavior(behavior);
			m_enemy_img_name = "b_right_hand";
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}
		std::shared_ptr<Fireball> makeFireball() override {
			Eigen::Vector2f base_vel(velocity_generator(mt), velocity_generator(mt));
			FireballKind kind = FireballKind::EnemyWeak;

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
			m_enemy_img_name = "b_donald";
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}

		EnemyHead(Shape::Rectangle realm, std::shared_ptr<MovingBehavior>& behavior, Life life)
		{
			m_life = life;
			m_realm = realm;
			m_moving->setBehavior(behavior);
			m_enemy_img_name = "b_donald";
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

		bool damageEnemy(int amount) override {
			m_boss_damaged_sound.start();
			m_is_damaged = 30;
			return m_life.damage(amount);
		}

		bool draw() override {
			if (!isAlive()) {
				EnemyBase::draw();
				return true;
			}
			if (hasLeft()) m_left_hand->draw();
			if (hasRight()) m_right_hand->draw();
			EnemyBase::draw();
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
		Sound m_boss_damaged_sound = Sound("b_boss_damaged");
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
				m_generate_enemy_num++;
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
		int getGeneratedEnemy() const {
			return m_generate_enemy_num;
		}

	private:
		std::vector<std::shared_ptr<EnemyBase>> m_enemy_list = {};
		int m_max_num = 0;
		int m_generate_enemy_num = 0;
	};
}