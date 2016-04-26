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
#include "util/util.h"
#include "util/breakout_params.h"


namespace Breakout {
// 描画の優先順位
constexpr int PRIORITY_BACKGROUND = 0; // 背景画像
constexpr int PRIORITY_SECTION = 1; // レイアウト（ゲームフィールド、スコア表示、デバッグ表示等）
constexpr int PRIORITY_STATIC_OBJECT = 2; // 静的オブジェクト (Block)
constexpr int PRIORITY_DYNAMIC_OBJECT = 3; // 動的オブジェクト(Ship, Fireball)
constexpr int PRIORITY_CHARACTER = 4; // キャラクター(マリオ)

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
	explicit Info(const Shape::Rectangle& realm)
		: m_realm(realm)
	{
		Object::layer = PRIORITY_SECTION;
	}

	bool draw() {
        SetDrawBright(100, 100, 100);
        DrawBox(m_realm.left(), m_realm.top(), 
                m_realm.right(), m_realm.bottom(), 
                GetColor(50, 50, 50), TRUE);
        SetDrawBright(255, 255, 255);
		return true;
	}

private:
	Shape::Rectangle m_realm = Shape::Rectangle();
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
                GetColor(255, 255, 255), false);
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
class Block : public Object
{
public:
    explicit Block(const Shape::Rectangle& realm) 
        : m_realm(realm)
    {
        Object::layer = PRIORITY_STATIC_OBJECT;
    }

    bool draw() override {
        if (m_is_disappeared) {
            // 何も描画しない
            return true;
        }
        DrawBox(m_realm.left(), m_realm.top(), 
                m_realm.right(), m_realm.bottom(), 
                GetColor(255, 0, 0), false);

        return true;
    }

	Shape::Rectangle getRealm() const {
		return m_realm;
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
    Shape::Rectangle m_realm = Shape::Rectangle();
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
        DrawCircle(x, y, r, GetColor(0, 255, 0), true);

        return true;
    }

	void updatePosition() {
		m_moving->updatePosition(m_realm.center);
	}

	void setPosition(const Eigen::Vector2i& pos) {
		m_realm.center = pos;
	}

	bool isDisappeared() const {
		m_is_disappeared;
	}

    // Firebollの消滅
    void disappear()
    {
        m_is_disappeared = true;
    }

	Shape::Circle getRealm() const
	{
		return m_realm;
	}

	// 衝突したかどうか
	bool isCollided(const Shape::Rectangle& parent) {
		// 四角形の上側との衝突
		if (CollisionDetection::isOnLine(m_realm, parent.getTopLine())) {
			auto vel = m_moving->getVelocity();
			m_moving->setVelocity(Eigen::Vector2d{ vel.x(), -vel.y() });
			return true;
		}
		// 四角形の下側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getBottomLine())) {
			auto vel = m_moving->getVelocity();
			m_moving->setVelocity(Eigen::Vector2d{ vel.x(), -vel.y() });
			return true;
		}
		// 四角形の左側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getLeftLine())) {
			auto vel = m_moving->getVelocity();
			m_moving->setVelocity(Eigen::Vector2d{ -vel.x(), vel.y() });
			return true;
		}
		// 四角形の右側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getRightLine())) {
			auto vel = m_moving->getVelocity();
			m_moving->setVelocity(Eigen::Vector2d{ -vel.x(), vel.y() });
			return true;
		}
		

		return false;
	}


private:
    bool m_is_disappeared = false; // 火の玉にあったかどうか
    Shape::Circle m_realm = Shape::Circle(); // Firebollの領域
	std::shared_ptr<Moving> m_moving = nullptr;
};

// キャラクタがのる船
// ライフをもつ
class Ship : public Object
{
public:
    explicit Ship(const Eigen::Vector2i start_point, const Life& life)
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

	double left() {
		return m_blocks[0].left();
	}
	double right() {
		return m_blocks[m_blocks.size() - 1].right();
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

	std::vector<Shape::Rectangle> getRealm() const {
		return m_blocks;
	}

private:
    bool m_is_disappered = false;
	std::vector<Shape::Rectangle> m_blocks = {};
	Eigen::Vector2i m_start_point = Eigen::Vector2i::Zero();
    Life m_life = Life(); // defaultは４つ。火の玉に当たるごとに一つ減る
};


} // namespace Breakout
