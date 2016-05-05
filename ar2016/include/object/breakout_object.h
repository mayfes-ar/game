/*
 * BreakoutGameで使うオブジェクト一覧
 */

#pragma once

#include <Eigen/Core>
#include <memory>
#include <array>
#include <list>
#include "object/object.h"
#include "object/shape.h"
#include "moving/moving.h"
#include "math/collision_detection.h"
#include "math/math_util.h"
#include "util/util.h"
#include "util/breakout_params.h"
#include "util/timer.h"
#include "util/score.h"
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
	ItemReceiverBase(const Shape::Rectangle& realm) : m_realm(realm) {}
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
	Shape::Rectangle m_realm = Shape::Rectangle(Eigen::Vector2i(0,0), BLOCK_WIDTH/2, BLOCK_HEIGHT);
};


// 縦方向に選択ボタンを配置するセレクト画面。
// 一方向のみ対応
/*
template <typename Mode, Hash = std::hash<Mode> >
class Select : public Object
{
public:
	Select(const std::list<Mode>& mode_list, 
		   const Shape::Rectangle& realm) 
		:m_mode_list(mode_list), m_realm(realm)
	{
		Object::layer = PRIORITY_SECTION;
		m_now_mode = m_mode_list.begin();
	}

	enum class Move : uint8_t {
		Up,
		Down,
		None
	};

	bool draw() override {
		int image_num = static_cast<int>(m_mode_list.size());
		int image_width = m_realm.width() / 2;
		int image_height = m_realm.height() / (2 * image_num);

		int id = 1;
		for (auto it = m_mode_list.begin(); it != m_mode_list.end(); ++it) {
			DrawExendGraph(m_realm.left(), m_realm.top() + image_height * (2 * i - 1), m_realm.left(), m_realm.top() + image_height * (2 * i),
				);
			id++;
		}
	}


	void move(const Move& move)
	{
		switch (move) {
		case Mode::None:
			break;
		case Mode::Up:
			if (m_mode_list.size() =< 1) {
				return;
			}
			else if (m_now_mode == m_mode_list.begin()) {
				return;
			}
			m_now_mode++;

		case Mode::Down:
			if (m_mode_list.size() = < 1) {
				return;
			}
			else if (m_now_mode == m_mode_list.begin()) {
				return;
			}
			m_now_mode--;
			break;
		}
	}

	void select() {
		m_selected = true;
	}

	bool selected() const {
		return m_selected;
	}

	Mode getMode() const {
		return *m_now_mode;
	}

private:
	std::list<Mode> m_mode_list;
	std::list<Mode>::iterator m_now_mode;
	Shape::Rectangle m_realm = Shape::Rectangle();
	bool m_selected = false;
};
*/


// 背景画像
// 溶岩の絵がほしい
class Background : public Object
{
public:
    Background()
    {
        Object::layer = PRIORITY_BACKGROUND;
    }

	void init() {
		m_is_last_phase = false;
		m_forest_saturation = 50;
		m_magma_saturation = 50;
	}

    bool draw() override {
		if (m_is_last_phase) {
			SetDrawBright(m_forest_saturation, m_forest_saturation, m_forest_saturation);
			if (m_forest_saturation < m_saturation_max) {
				m_forest_saturation++;
			}

			DrawExtendGraph(0, 0, WIDTH, HEIGHT, 
					imgHandles["b_magma"], true);
			SetDrawBright(255, 255, 255);
			return true;
		}
		else {
			if (m_magma_saturation < m_saturation_max) {
				m_magma_saturation++;
			}
			SetDrawBright(m_magma_saturation, m_magma_saturation, m_magma_saturation);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT, 
					imgHandles["b_hill"], true);
			SetDrawBright(255, 255, 255);
			return true;
		}
    }

	void turnLastPhase() {
		m_is_last_phase = true;
	}

private:
	int m_forest_saturation = 50;
	int m_magma_saturation = 50;
	bool m_is_last_phase = false;
	const int m_saturation_max = 100;
};

class Explanation : public Object
{
public:
	explicit Explanation(const Shape::Rectangle& realm)
		: m_realm(realm)
	{}
	
	bool draw() override {
		DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), 
				imgHandles["b_explanation"], TRUE);
		return true;
	}

private:
	Shape::Rectangle m_realm = Shape::Rectangle();
};

// 時間やスコアを表示する
class Info : public Object
{
public:
	explicit Info(const Shape::Rectangle realm, const std::shared_ptr<Timer>& timer)
		: m_realm(realm), m_timer(timer)
	{
		Object::layer = PRIORITY_SECTION;
		m_score = std::make_shared<Score>();
	}

	void init() {
		m_timer->start();
		m_score->reset();
	}

	bool draw() {
        SetDrawBright(100, 100, 100);
        DrawBox(m_realm.left(), m_realm.top(), 
                m_realm.right(), m_realm.bottom(), 
                GetColor(50, 50, 50), TRUE);
        SetDrawBright(255, 255, 255);
		// Timer の描画
		const auto time = m_timer->getLeftedTime();
		std::array<int, 6> time_num;
		const int minute = std::get<0>(time);
		const int sec = std::get<1>(time);
		const int msec = std::get<2>(time);
		if (minute < 10) {
			time_num.at(0) = 0;
			time_num.at(1) = minute;
		}
		else {
			time_num.at(0) = minute / 10;
			time_num.at(1) = minute - (minute / 10) * 10;
		}

		if (sec < 10) {
			time_num.at(2) = 0;
			time_num.at(3) = sec;
		}
		else {
			time_num.at(2) = sec / 10;
			time_num.at(3) = sec - (sec / 10) * 10;
		}
		if (msec < 100) {
			time_num.at(4) = 0;
			//time_num.at(5) = msec / 10;
		}
		else {
			time_num.at(4) = msec / 100;
			//time_num.at(5) = (msec - (msec / 100) * 100) / 10;
		}

		const int num_width = (m_realm.right() - m_realm.left()) / 8;
		const int num_height = (m_realm.bottom() - m_realm.top()) / 8;
		for (int i = 0; i < 5; ++i) {
			std::ostringstream key;
			if (i < 2) {
				key << "red_" << time_num.at(i);

			}
			else if (i < 4) {
				key << "blue_" << time_num.at(i);
			}
			else {
				key << "green_" << time_num.at(i);
			}

			DrawExtendGraph((i+1) * num_width, num_height, (i + 2)* num_width, 2 * num_height, 
					imgHandles[key.str()], TRUE);
		}

		m_score->addPoint(1);
		auto score = m_score->getPoint();
		std::array<int, 4> score_num;
		score_num.at(0) = score / 1000;
		score -= (score / 1000) * 1000;
		score_num.at(1) = score / 100;
		score -= (score / 100) * 100;
		score_num.at(2) = score / 10;
		score -= (score / 10) * 10;
		score_num.at(3) = score;

		const int score_width = m_realm.width / 6;
		const int score_height = m_realm.height / 12;
		for (int i = 0; i < 4; ++i) {
			std::ostringstream key;
			key << "yellow_" << score_num.at(i);
			DrawExtendGraph((i + 1) * score_width, 2 * num_height + 50/*きもち*/, (i + 2)* score_width, 2 * num_height + 50 + score_width,  
					imgHandles[key.str()], TRUE);
		}
		// Score の描画
		return true;
	}

	bool isTimeOver() const {
		return m_timer->isTimerEnd();
	}

	bool isLastPhase() {
		float ratio = m_timer->getRatio();
		return ratio < 0.3f;
	}

	// Scoreをいじるときはこの関数から直接scoreたたいてください
	void addScore(int score) {
		m_score->addPoint(score);
	}

private:
	Shape::Rectangle m_realm = Shape::Rectangle();
	std::shared_ptr<Timer> m_timer = nullptr;
	std::shared_ptr<Score> m_score = nullptr;
};

class Result : public Object
{
public:
	explicit Result(const Shape::Rectangle& m_realm)
		: m_realm(m_realm) {
		Object::layer = PRIORITY_INFO;
	}

	void init()
	{
		m_cnt = 40;
	}


	bool draw() override {
		if (m_cnt >= 255) {
			m_cnt = 255;
		}
		SetDrawBright(m_cnt, m_cnt, m_cnt);
		m_cnt += 2;

		if (m_is_game_clear) {
			DrawExtendGraph(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				imgHandles["b_game_clear"], TRUE);
				return true;
		}

			DrawExtendGraph(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				imgHandles["b_game_over"], TRUE);

		return true;
	}

	// ゲームをクリアしたかどうか
	void clearGame() {
		m_is_game_clear = true;
	}

private:
	Shape::Rectangle m_realm = Shape::Rectangle();
	bool m_is_game_clear = false;
	int m_cnt = 0;
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
	enum class Color : uint8_t {
		Green,
		Blue,
		Red
	};

    explicit Block(const Shape::Rectangle& realm, Color color) 
		: m_color(color), ItemReceiverBase(realm)
    {
        Object::layer = PRIORITY_STATIC_OBJECT;
    }

    bool draw() override {
        if (m_is_disappeared) {
            // 何も描画しない
            return true;
        }

		switch (m_color) {
		case Color::Green:
			DrawExtendGraph(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				imgHandles["block_green"], TRUE);
			break;
		case Color::Blue:
			DrawExtendGraph(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				imgHandles["block_blue"], TRUE);
			break;
		case Color::Red:
			DrawExtendGraph(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				imgHandles["block_red"], TRUE);
			break;
		}

        return true;
    }

	const Shape::Rectangle getRealm() override {
		return m_realm;
	}

	Color getColor() const {
		return m_color;
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
	Color m_color = Color::Green;
};


//Fireball の Modeを決定するためのもの
//EnemyかPlayerか、StrongかWeakかの4種類からなっている。
//それぞれ描画時に色を変える？
enum FireballKind {
	EnemyWeak,
	EnemyStrong,
	PlayerWeak,
	PlayerStrong
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
	Fireball(const Shape::Circle& realm, const std::shared_ptr<Moving>& moving, FireballKind mode)
		: m_realm(realm), m_moving(moving), m_mode(mode)
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
		switch (m_mode) {
		case EnemyWeak:
			DrawCircle(x, y, r, Color::RED, true);
			break;
		case EnemyStrong:
			DrawCircle(x, y, r, Color::BLACK, true);
			break;
		case PlayerWeak:
			DrawCircle(x, y, r, Color::BLUE, true);
			break;
		case PlayerStrong:
			DrawCircle(x, y, r, Color::GREEN, true);
			break;
		}

        return true;
    }

	void updatePosition() {
		m_moving->updatePoistion(m_realm.center);
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

	FireballKind getMode() {
		return m_mode;
	}

	void setMode(FireballKind mode) {
		m_mode = mode;
	}

	void changeModeToPlayer() {
		switch (m_mode) {
		case EnemyWeak:
			m_mode = PlayerWeak;
			break;
		case EnemyStrong:
			m_mode = PlayerStrong;
			break;
		}
	}

	bool isEnemy() {
		switch (m_mode) {
		case EnemyWeak:
		case EnemyStrong:
			return true;
		}
		return false;
	}

	bool isStrong() {
		switch (m_mode) {
		case EnemyStrong:
		case PlayerStrong:
			return true;
		}
		return false;
	}


private:
    bool m_is_disappeared = false; // 火の玉にあったかどうか
    Shape::Circle m_realm = Shape::Circle(); // Firebollの領域
	std::shared_ptr<Moving> m_moving = nullptr;
	FireballKind m_mode = EnemyWeak;
};

class FireballManager : public Object {
public:
	FireballManager(int max_num) : m_max_num(max_num) {
		Object::layer = PRIORITY_DYNAMIC_OBJECT;
	}

	bool add(std::shared_ptr<Fireball>& fireball) {
		if (m_fireball_list.size() == m_max_num) return false;
		m_fireball_list.push_back(fireball);
		return true;
	}
	// index を渡して消す
	bool destroy(int index) {
		if (m_fireball_list.size() <= index + 1) return false;
		m_fireball_list.erase(m_fireball_list.begin() + index);
		return true;
	}

	bool destroy(std::shared_ptr<Fireball>& fireball) {
		for (int i = 0; i < m_fireball_list.size(); i++) {
			if (m_fireball_list[i] != fireball) continue;
			m_fireball_list.erase(m_fireball_list.begin() + i);
			return true;
		}
		return false;
	}

	std::vector<std::shared_ptr<Fireball>> getFireballList() {
		return m_fireball_list;
	}

	bool draw() override {
		bool isSuccess = true;
		for (auto& fireball : m_fireball_list) {
			isSuccess &= fireball->draw();
		}
		return isSuccess;
	}

private:
	std::vector<std::shared_ptr<Fireball>> m_fireball_list = {};
	int m_max_num = 0;
};



// キャラクタがのる船
// ライフをもつ
class Ship : public Object, public ItemReceiverBase
{
public:
    explicit Ship(const Eigen::Vector2i& start_point, const Life& life)
		: m_start_point(start_point) , m_life(life)
    {
        Object::layer = PRIORITY_DYNAMIC_OBJECT;
		for (int i = 0; i < m_life.getLifeNum(); i++) {
			m_blocks.push_back(Shape::Rectangle(start_point + Eigen::Vector2i{ BLOCK_WIDTH, 0 } * i, BLOCK_WIDTH, BLOCK_HEIGHT));
		}
    }

	//! @brief 舟を動かす
	bool translate(int translation) {	
		//範囲外なら移動させない
		//start_pointの更新
		m_start_point = m_blocks[0].getLeftTopPoint();
		if (m_blocks[m_blocks.size() - 1].right() + translation > FIELD_START_POS.x() + FIELD_WIDTH ||
			m_blocks[0].left() + translation < FIELD_START_POS.x()) return false;

		for (auto &block: m_blocks) {
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
	// StrongかWeakかを見たあとに、Player~にする
	void inhareFireball(std::shared_ptr<Fireball>& fireball) {
		m_fireball = fireball;
		m_fireball->disappear();
		m_fireball->setPosition(m_realm.getLeftTopPoint());
		m_initial_fireball_speed = m_fireball->getVelocity().norm();
		m_fireball->setVelocity(Eigen::Vector2f::Zero());
		m_fireball->changeModeToPlayer();
	}

	// fireballを吐き出す。吸い込んでから3秒後。
	void exhareFireball() {
		m_fireball->setVelocity(Eigen::Vector2f(- m_initial_fireball_speed * (float)cos(m_rotation+M_PI/2.0), - m_initial_fireball_speed * (float)sin(m_rotation+M_PI/2.0)));
		m_fireball->appear();
		m_fireball = nullptr;
		m_count = -1;
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
			(double)m_realm.width/150.0, m_rotation,
			imgHandles["pot"], TRUE, FALSE);


		DrawBox(m_realm.left(), m_realm.top(),
			m_realm.right(), m_realm.bottom(),
			GetColor(255, 0, 0), false);

		return true;
	}

	int getMarkerHeight() {
		return m_marker_height;
	}
	void setMarkerHeight(int h) {
		m_marker_height = h;
	}

private:
	bool m_is_disappered = true;
	float m_rotation = 0;
	std::shared_ptr<Fireball> m_fireball = nullptr;
	Shape::Rectangle m_realm = Shape::Rectangle();
	float m_initial_fireball_speed = 0;
	// 吸い込んでからどのくらいで打ち出すか(基本30frames/secだから3秒くらい)
	int m_count = 90;
	int m_marker_height = 0;

	void updateFireballPosition() {
		if (hasFireball()) {
			m_fireball->setPosition(m_realm.getLeftTopPoint());
		}
	}
};


} // namespace Breakout
