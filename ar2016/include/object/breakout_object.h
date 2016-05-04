/*
 * BreakoutGameで使うオブジェクト一覧
 */

#pragma once

#include <Eigen/Core>
#include <memory>
#include "object/object.h"
#include "object/shape.h"
#include "moving/moving.h"
#include "math/collision_detection.h"
#include "math/math_util.h"
#include "util/util.h"
#include "util/breakout_params.h"
#include "util/timer.h"
#include "util/color_palette.h"


namespace Breakout {
	// 描画の優先順位
	constexpr int PRIORITY_BACKGROUND = 0; // 背景画像
	constexpr int PRIORITY_SECTION = 1; // レイアウト（ゲームフィールド、スコア表示、デバッグ表示等）
	constexpr int PRIORITY_STATIC_OBJECT = 2; // 静的オブジェクト (Block)
	constexpr int PRIORITY_DYNAMIC_OBJECT = 3; // 動的オブジェクト(Ship, Fireball)
	constexpr int PRIORITY_CHARACTER = 4; // キャラクター(マリオ)
	constexpr int PRIORITY_INFO = 5; // インフォメーション

	class ItemReceiverBase;

	enum ItemKind {
		RestoreShip,
		DamageShip
	};

	//アイテムクラス
	class ItemBase
	{
	public:
		virtual ~ItemBase() {}
		virtual void updateStatus() = 0;

		void attachItemReceiver(std::shared_ptr<ItemReceiverBase> item_receiver) {
			m_item_receiver = item_receiver;
			updateStatus();
		}
		void detachItemReceiver() {
			m_item_receiver = nullptr;
			updateStatus();
		}

		bool isAttached() {
			return m_item_receiver == nullptr ? false : true;
		}

		ItemKind getItemKind() {
			return m_item_kind;
		}

	protected:
		std::shared_ptr<ItemReceiverBase> m_item_receiver = nullptr;
		ItemKind m_item_kind;
	};

	//アイテムレシーバー基礎クラス
	class ItemReceiverBase
	{
	public:
		ItemReceiverBase() {}
		virtual ~ItemReceiverBase() {}
		//本当は純粋仮想にしたかったけどそうするとこのクラスのインスタンスthisがつくれないからこのようにした
		virtual void applyItems() {}
		virtual const Shape::Rectangle getRealm() { return m_realm; }

		void detachItem(std::shared_ptr<ItemBase> item) {
			item->detachItemReceiver();
			deleteItem(item);
		}

		void detachAllItems() {
			for (auto item : m_items) {
				item->detachItemReceiver();
			}
			m_items.clear();
		}

		void attachItem(std::shared_ptr<ItemBase> item) {
			m_items.push_back(item);
			item->attachItemReceiver(std::make_shared<ItemReceiverBase>(*this));
			applyItems();
		}

		//使用済みのアイテムを消す
		void deleteItem(std::shared_ptr<ItemBase> item) {
			for (int i = 0; i < m_items.size(); i++) {
				if (m_items[i] != item) continue;
				m_items.erase(m_items.begin() + i);
			}
		}

	protected:
		std::vector<std::shared_ptr<ItemBase>> m_items = {};
		Shape::Rectangle m_realm = Shape::Rectangle();
	};

	class Item : public Object, public ItemBase
	{
	public:
		virtual ~Item() {}
		Item(ItemKind item_kind) {
			m_item_kind = item_kind;
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}

		Shape::Rectangle getRealm() const {
			return m_realm;
		}

		void updateStatus() override {
			if (m_item_receiver != nullptr) {
				m_realm.start_point = m_item_receiver->getRealm().start_point;// + Eigen::Vector2i((m_item_receiver->getRealm().width - m_realm.width) / 2, 0);
			}
			else {
				m_realm.start_point += Eigen::Vector2i(0, 20);
			}
		}

		bool draw() override {
			//誰にかに保持されていたら描画しない
			if (m_item_receiver != nullptr) return true;

			switch (m_item_kind) {
			case RestoreShip:
				DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["restore_ship"], TRUE);
				break;
			case DamageShip:
				DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["damage_ship"], TRUE);
				break;
			}

			//DrawBox(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), GetColor(255, 255, 0), TRUE);
			return true;
		}

	private:
		Shape::Rectangle m_realm = Shape::Rectangle(Eigen::Vector2i(0, 0), BLOCK_WIDTH / 2, BLOCK_HEIGHT);
	};


	// 背景画像
	// 溶岩の絵がほしい
	class Background : public Object
	{
	public:
		Background(int& handle)
			: m_handle(handle)
		{
			Object::layer = PRIORITY_BACKGROUND;
		}

		bool draw() override {
			SetDrawBright(40, 40, 40);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT,
				imgHandles["ar2016_logo"], true);
			SetDrawBright(255, 255, 255);
			return true;
		}

		int& m_handle;
	};

	// 時間やスコアを表示する
	class Info : public Object
	{
	public:
		explicit Info(const Shape::Rectangle realm, const std::shared_ptr<Timer>& timer)
			: m_realm(realm), m_timer(timer)
		{
			Object::layer = PRIORITY_SECTION;
		}

		void init() {
			m_timer->start();
		}

		bool draw() {
			SetDrawBright(100, 100, 100);
			DrawBox(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				GetColor(50, 50, 50), TRUE);
			SetDrawBright(255, 255, 255);
			// Timer の描画
			const auto time = m_timer->getLeftedTime();
			DrawFormatString(m_realm.left(), m_realm.top() + 50, Color::WHITE, "%02d:%02d:%02d",
				std::get<0>(time), std::get<1>(time), std::get<2>(time));
			// Score の描画
			return true;
		}

		bool isTimeOver() const {
			return m_timer->isTimerEnd();
		}

	private:
		Shape::Rectangle m_realm = Shape::Rectangle();
		std::shared_ptr<Timer> m_timer = nullptr;
	};

	class Result : public Object
	{
	public:
		explicit Result(const Shape::Point& start_point)
			: m_start_point(start_point) {
			Object::layer = PRIORITY_INFO;
		}

		void init() {
			SetFontSize(80);
			m_is_initialized = true;
		}

		bool draw() override {
			if (!m_is_initialized) {
				return false;
			}

			if (m_is_game_clear) {
				DrawString(m_start_point.x(), m_start_point.y(),
					"Game Clear", Color::BLUE);
				return true;
			}

			DrawString(m_start_point.x(), m_start_point.y(),
				"Game Over", Color::RED);

			return true;
		}

		// ゲームをクリアしたかどうか
		void clearGame() {
			m_is_game_clear = true;
		}

	private:
		Shape::Point m_start_point = Shape::Point();
		bool m_is_initialized = false;
		bool m_is_game_clear = false;
	};

	// フィールド 
	class Field : public Object
	{
	public:
		explicit Field(const Shape::Rectangle& realm)
			: m_realm(realm)
		{
			Object::layer = PRIORITY_SECTION;
		}

		bool draw() override {
			SetDrawBright(100, 100, 100);
			DrawBox(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				Color::WHITE, false);
			SetDrawBright(255, 255, 255);
			return true;
		}

		Shape::Rectangle getRealm() const {
			return m_realm;
		}

		// フィールド外にいるかどうか
		bool isOutOfField(const Shape::Rectangle& rect);
		bool isOutOfField(const Shape::Circle& cir);

	private:
		Shape::Rectangle m_realm = Shape::Rectangle();
	};

	// Block崩しに使われるBlock
	// Firebollにぶつかると消える
	// 初期位置に固定という意味で静的オブジェクト
	class Block : public Object, public ItemReceiverBase
	{
	public:
		explicit Block(const Shape::Rectangle& realm)
		{
			m_realm = realm;
			Object::layer = PRIORITY_STATIC_OBJECT;
		}

		bool draw() override {
			if (m_is_disappeared) {
				// 何も描画しない
				return true;
			}
			DrawBox(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				Color::GREEN, false);

			return true;
		}

		const Shape::Rectangle getRealm() override {
			return m_realm;
		}

		void applyItems() override {
			return;
		}

		bool isDisappeared() const {
			return m_is_disappeared;
		}

		// Blockの消滅
		void disappear()
		{
			m_is_disappeared = true;
		}

		void appear()
		{
			m_is_disappeared = false;
		}
	private:
		bool m_is_disappeared = false; // 火の玉にあったかどうか
		//Shape::Rectangle m_realm = Shape::Rectangle();
	};


	// Block崩しに使われるBlock
	// Firebollにぶつかると消える
	// 初期位置に固定という意味で静的オブジェクト
	/*
	*/
	class Fireball : public Object
	{
	public:
		Fireball(const Shape::Circle& realm, const std::shared_ptr<Moving>& moving)
			: m_realm(realm), m_moving(moving)
		{
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}

		bool draw() {
			if (m_is_disappeared) {
				// 何も描画しない
				return true;
			}
			int x = m_realm.center.x();
			int y = m_realm.center.y();
			int r = m_realm.radius;
			DrawCircle(x, y, r, Color::RED, true);

			return true;
		}

		void updatePosition() {
			m_moving->updatePosition(m_realm.center);
		}

		void setPosition(const Eigen::Vector2i& pos) {
			m_realm.center = pos;
		}

		void setVelocity(const Eigen::Vector2f& velocity) {
			m_moving->setVelocity(velocity);
		}

		Eigen::Vector2f getVelocity() {
			return m_moving->getVelocity();
		}

		bool isDisappeared() const {
			return m_is_disappeared;
		}

		void appear(const Shape::Circle& start_realm) {
			m_realm = start_realm;
			m_is_disappeared = false;
		}

		// Fireballの消滅
		void disappear()
		{
			m_is_disappeared = true;
		}

		void appear()
		{
			m_is_disappeared = false;
		}

		Shape::Circle getRealm() const
		{
			return m_realm;
		}
		// if in then -1, out then 1, default is 1
		bool isCollided(const Shape::Rectangle& parent, const int inOrOut = 1, const int parentVelocity = 0);





	private:
		bool m_is_disappeared = false; // 火の玉にあったかどうか
		Shape::Circle m_realm = Shape::Circle(); // Firebollの領域
		std::shared_ptr<Moving> m_moving = nullptr;
	};

	// キャラクタがのる船
	// ライフをもつ
	class Ship : public Object, public ItemReceiverBase
	{
	public:
		explicit Ship(const Eigen::Vector2i& start_point, const Life& life)
			: m_start_point(start_point), m_life(life)
		{
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
			for (int i = 0; i < m_life.getLifeNum(); i++) {
				m_blocks.push_back(Shape::Rectangle(start_point + Eigen::Vector2i{ BLOCK_WIDTH, 0 } *i, BLOCK_WIDTH, BLOCK_HEIGHT));
			}
		}

		//! @brief 舟を動かす
		bool translate(int translation) {
			//範囲外なら移動させない
			//start_pointの更新
			m_start_point = m_blocks[0].getLeftTopPoint();
			if (m_blocks[m_blocks.size() - 1].right() + translation > FIELD_START_POS.x() + FIELD_WIDTH ||
				m_blocks[0].left() + translation < FIELD_START_POS.x()) return false;

			for (auto &block : m_blocks) {
				block.start_point.x() += translation;
			}
			// m_velocityのセット
			m_velocity = translation;
			return true;
		}

		bool restoreShip(int amount) {
			if (!m_life.restore(amount)) return false;
			if (m_blocks.empty()) {
				m_blocks.push_back(Shape::Rectangle(m_start_point, BLOCK_WIDTH, BLOCK_HEIGHT));
				return true;
			}
			for (int i = 0; i < amount; i++) {
				if (m_blocks[m_blocks.size() - 1].getRightBottomPoint().x() + BLOCK_WIDTH > FIELD_START_POS.x() + FIELD_WIDTH) {
					m_blocks.insert(m_blocks.begin(), Shape::Rectangle(m_blocks[0].getLeftTopPoint() - Eigen::Vector2i(BLOCK_WIDTH, 0), BLOCK_WIDTH, BLOCK_HEIGHT));
				}
				else
					m_blocks.push_back(Shape::Rectangle(m_blocks[m_blocks.size() - 1].getRightTopPoint(), BLOCK_WIDTH, BLOCK_HEIGHT));
			}
			return true;
		}

		bool damageShip(int amount) {
			if (!m_life.damage(amount)) return false;
			for (int i = 0; i < amount; i++) {
				m_blocks.pop_back();
			}
			return true;
		}

		void resetShip() {
			m_blocks.clear();
			m_life.resetLife();
			for (int i = 0; i < m_life.getLifeNum(); i++) {
				m_blocks.push_back(Shape::Rectangle(m_start_point + Eigen::Vector2i{ BLOCK_WIDTH, 0 } *i, BLOCK_WIDTH, BLOCK_HEIGHT));
			}
		}

		int getLifeNum() {
			return m_life.getLifeNum();
		}

		bool isAlive() const {
			return m_life.isAlive();
		}

		int left() {
			return m_start_point[0];
		}
		int right() {
			return m_start_point[0] + m_blocks.size() * BLOCK_WIDTH;
		}
		int top() {
			return m_start_point[1];
		}
		int bottom() {
			return m_start_point[1] + BLOCK_HEIGHT;
		}
		int getVelocity() {
			return m_velocity;
		}

		bool draw() override {
			if (m_is_disappered) {
				// 何も描画しない
				return true;
			}

			for (auto block : m_blocks) {
				DrawExtendGraph(block.left(), block.top(),
					block.right(), block.bottom(),
					imgHandles["ship_block"], TRUE);
			}

			return true;
		}


		// Blockの消滅
		void disapper()
		{
			m_is_disappered = true;
		}

		const Shape::Rectangle getRealm() override {
			return Shape::Rectangle(m_start_point, m_blocks.size() * BLOCK_WIDTH, BLOCK_HEIGHT);
		}

		void applyItems() override {
			for (int i = 0; i < m_items.size(); i++) {

				switch (m_items[i]->getItemKind()) {
					//継続的な効果を持たないものは適用したら消す
				case RestoreShip:
					restoreShip(1);
					deleteItem(m_items[i]);
					//消したのでその分indexを減らす
					i--;
					break;
				case DamageShip:
					damageShip(1);
					deleteItem(m_items[i]);
					i--;
					break;
				}
			}
		}

	private:
		bool m_is_disappered = false;
		std::vector<Shape::Rectangle> m_blocks = {};
		Eigen::Vector2i m_start_point = Eigen::Vector2i::Zero();
		int m_velocity = 0; // マーカーを認識しているときにtranslateで渡されるもの。実際の速度的な概念とは違う。
		Life m_life = Life(); // defaultは４つ。火の玉に当たるごとに一つ減る
	};


	class Pot : public Object
	{
	public:
		explicit Pot(const Shape::Rectangle& realm)
			: m_realm(realm)
		{
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}

		bool translate(const Eigen::Vector2i& translation) {
			//範囲外なら移動させない
			//realmの更新
			if (m_realm.right() + translation.x() > FIELD_START_POS.x() + FIELD_WIDTH ||
				m_realm.left() + translation.x() < FIELD_START_POS.x() ||
				m_realm.top() + translation.y() < FIELD_START_POS.y() ||
				m_realm.bottom() + translation.y() > FIELD_START_POS.y() + FIELD_HEIGHT) return false;

			m_realm.start_point += translation;
			updateFireballPosition();
			return true;
		}

		bool rotate(float rotation) {
			m_rotation += rotation;
			return true;
		}

		// マーカーを認識したらis_disappearedはonにする。吸い込んでから3病後に発射。このときポットにカウントダウンを表示する。0になったとき発射不可能な場所にいた場合は発射失敗。

		// fireballを吸い込む（特定のfireballのほうがいいかも？）
		void inhareFireball(std::shared_ptr<Fireball>& fireball) {
			m_fireball = fireball;
			m_fireball->disappear();
			m_fireball->setPosition(m_realm.getLeftTopPoint());
			m_initial_fireball_speed = m_fireball->getVelocity().norm();
			m_fireball->setVelocity(Eigen::Vector2f::Zero());
		}

		// fireballを吐き出す。吸い込んでから3秒後。
		void exhareFireball() {
			m_fireball->setVelocity(Eigen::Vector2f(-m_initial_fireball_speed * (float)cos(m_rotation + M_PI / 2.0), -m_initial_fireball_speed * (float)sin(m_rotation + M_PI / 2.0)));
			m_fireball->appear();
			m_fireball = nullptr;
		}

		Shape::Rectangle getRealm() const {
			return m_realm;
		}

		float getRotation() const {
			return m_rotation;
		}

		bool hasFireball() {
			return m_fireball == nullptr ? false : true;
		}

		void appear() {
			m_is_disappered = false;
		}

		void disappear() {
			m_is_disappered = true;
		}

		bool isDisappeared() {
			return m_is_disappered;
		}

		bool isAvailable() {
			return !isExhared();
		}

		// 吐き出したかどうか（countをみる)
		bool isExhared() {
			return m_count >= 0 ? false : true;
		}

		bool isInhared() {
			return m_fireball == nullptr ? false : true;
		}

		void countDown() {
			m_count--;
		}

		bool draw() override {
			if (m_is_disappered || !isAvailable()) {
				// 何も描画しない
				return true;
			}

			auto rotatedTopLine = getRealm().getTopLine().getRotatedLine(m_realm.getCenterPoint(), getRotation());
			auto rotatedLeftLine = getRealm().getLeftLine().getRotatedLine(getRealm().getCenterPoint(), getRotation());
			auto rotatedRightLine = getRealm().getRightLine().getRotatedLine(getRealm().getCenterPoint(), getRotation());
			auto rotatedBottomLine = getRealm().getBottomLine().getRotatedLine(getRealm().getCenterPoint(), getRotation());


			DrawLine(rotatedTopLine.point.x(), rotatedTopLine.point.y(), (rotatedTopLine.point + rotatedTopLine.dir).x(), (rotatedTopLine.point + rotatedTopLine.dir).y(), GetColor(0, 255, 0), 3);
			DrawLine(rotatedLeftLine.point.x(), rotatedLeftLine.point.y(), (rotatedLeftLine.point + rotatedLeftLine.dir).x(), (rotatedLeftLine.point + rotatedLeftLine.dir).y(), GetColor(200, 255, 0), 3);
			DrawLine(rotatedRightLine.point.x(), rotatedRightLine.point.y(), (rotatedRightLine.point + rotatedRightLine.dir).x(), (rotatedRightLine.point + rotatedRightLine.dir).y(), GetColor(0, 255, 200), 3);
			DrawLine(rotatedBottomLine.point.x(), rotatedBottomLine.point.y(), (rotatedBottomLine.point + rotatedBottomLine.dir).x(), (rotatedBottomLine.point + rotatedBottomLine.dir).y(), GetColor(200, 0, 200), 3);

			DrawRotaFormatString(m_realm.left(), m_realm.top(),
				1.0, 1.0,
				m_realm.width / 2, m_realm.height / 2,
				m_rotation, GetColor(255, 0, 0), 0, FALSE, "%d", m_count);

			DrawRotaGraph(m_realm.getCenterPoint().x(), m_realm.getCenterPoint().y(),
				(double)m_realm.width / 150.0, m_rotation,
				imgHandles["pot"], TRUE, FALSE);


			DrawBox(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				GetColor(255, 0, 0), false);

			return true;
		}

	private:
		bool m_is_disappered = true;
		float m_rotation = 0;
		std::shared_ptr<Fireball> m_fireball = nullptr;
		Shape::Rectangle m_realm = Shape::Rectangle();
		float m_initial_fireball_speed = 0;
		// 吸い込んでからどのくらいで打ち出すか(基本30frames/secだから3秒くらい)
		int m_count = 90;

		void updateFireballPosition() {
			if (hasFireball()) {
				m_fireball->setPosition(m_realm.getLeftTopPoint());
			}
		}
	};

	class EnemyBase {
	public:
		EnemyBase() {}
		virtual ~EnemyBase() {}
		// 今はmodeだけどそのうちfireballのモードenumに置き換わるはず
		// ここでvelocityを設定するかどうか迷う
		virtual std::shared_ptr<Fireball> makeFireball(int mode, Eigen::Vector2f velocity) = 0;
		// mode が enemy じゃない fireball にあたったらLifeを減らすようなメソッド
		virtual bool damageEnemy(std::shared_ptr<Fireball>& fireball) = 0;
		// 何かしらのアクションでLifeを回復するメソッド。実装したくない場合はreturnすればよい
		virtual bool restoreEnemy() = 0;
		virtual void resetEnemy() = 0;

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
		std::shared_ptr<Moving> m_moving = std::make_shared<Moving>();
	};

	class EnemyHead : public Object, public EnemyBase {
	public:
		EnemyHead(Shape::Rectangle realm) {
			m_life = Life(4, 4);
			m_realm = realm;
			m_moving->setVelocity(Eigen::Vector2f(8, 0));
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}
		std::shared_ptr<Fireball> makeFireball(int mode, Eigen::Vector2f velocity) override {
			// TODO mode
			auto fireball = std::make_shared<Fireball>(Shape::Circle(m_realm.getLeftBottomPoint(), FIREBALL_RADIUS), std::make_shared<Moving>());
			fireball->setVelocity(velocity);
			return fireball;
		}
		bool damageEnemy(std::shared_ptr<Fireball>& fireball) override {
			// TODO mode (mode == プレイヤーだったら)
			// TODO fireballはあたったら消えるからmanagerから消す
			return m_life.damage(1);
		}
		void updatePosition() {
			//単振動させる
			/*if (m_moving->getVelocity().x() >= 0) {
				m_moving->setAccel(m_moving->getAccel() - ENEMY_ACCEL);
			}
			else {
				m_moving->setAccel(m_moving->getAccel() + ENEMY_ACCEL);
			}*/
			m_moving->updatePosition(m_realm.start_point);
		}
		// 何かしらのアクションでLifeを回復するメソッド。実装したくない場合はreturnすればよい
		bool restoreEnemy() override {
			return m_life.restore(1);
		}
		void resetEnemy() override {
			m_life.resetLife();
		}

		bool draw() override {
			DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["kuppa_head"], TRUE);
			return true;
		}
	private:

	};

} // namespace Breakout
