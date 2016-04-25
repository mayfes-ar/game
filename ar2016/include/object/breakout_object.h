/*
 * BreakoutGameで使うオブジェクト一覧
 */

#pragma once

#include "object/object.h"
#include "object/shape.h"
#include "moving/moving.h"
#include "math/collision_detection.h"
#include <Eigen/Core>
#include <memory>

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
	位置更新は、
	auto moving = fireball->getMoving();
	moving->updatePosition();
	// 範囲外かどうか
	fireball->syncPosition();
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

	std::shared_ptr<Moving> getMoving() const {
		return m_moving;
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
    explicit Ship(const Shape::Rectangle& realm)
		: m_realm(realm)
    {
        Object::layer = PRIORITY_DYNAMIC_OBJECT;
    }

	//! @brief 舟を動かす
	bool translate(int translation) {
		// 範囲外にあるかどうかのチェックを実装すべき
		m_realm.start_point.x() += translation;
		return true;
	}

	bool setLeft(int left) {
		m_realm.start_point.x() = left;
		return true;
	}

	double left() {
		return m_realm.left();
	}
	double right() {
		return m_realm.right();
	}

    bool draw() override {
        if (m_is_disappered) {
            // 何も描画しない
            return true;
        }

		
        DrawBox(m_realm.left(), m_realm.top(),
              m_realm.right(), m_realm.bottom(), 
            GetColor(0, 100, 100), TRUE);

        return true;
    }


    // Blockの消滅
    void disapper()
    {
        m_is_disappered = true;
    }

	Shape::Rectangle getRealm() const {
		return m_realm;
	}

private:
    bool m_is_disappered = false; 
    Shape::Rectangle m_realm = Shape::Rectangle();
    int life_num = 4; // defaultは４つ。火の玉に当たるごとに一つ減る
};


} // namespace Breakout
