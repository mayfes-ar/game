/*
 * BreakoutGameで使うオブジェクト一覧
 */

#pragma once

#include "object/object.h"
#include "object/shape.h"
#include <Eigen/Core>

namespace Breakout {
// 描画の優先順位
constexpr int PRIORITY_BACKGROUND = 0;
constexpr int PRIORITY_STATIC_OBJECT = 1;
constexpr int PRIORITY_DYNAMIC_OBJECT = 2;
constexpr int PRIORITY_CHARACTER = 3;

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
        if (m_is_disappered) {
            // 何も描画しない
            return true;
        }
        DrawBox(m_realm.left(), m_realm.top(), 
                m_realm.right(), m_realm.bottom(), 
                GetColor(255, 0, 0), false);

        return true;
    }

    // Blockの消滅
    void disapper()
    {
        m_is_disappered = true;
    }

    void appear()
    {
        m_is_disappered = false;
    }
private:
    bool m_is_disappered = false; // 火の玉にあったかどうか
    Shape::Rectangle m_realm = Shape::Rectangle();
};


;

// Block崩しに使われるBlock
// Firebollにぶつかると消える
// 初期位置に固定という意味で静的オブジェクト
class Fireball : public Object
{
public:
    explicit Fireball(const Shape::Circle& realm) 
        : m_realm(realm)
    {
        Object::layer = PRIORITY_DYNAMIC_OBJECT;
    }

    bool draw() {
        if (m_is_disappered) {
            // 何も描画しない
            return true;
        }
        int x = m_realm.center.x();
        int y = m_realm.center.y();
        int r = m_realm.radius;
        DrawCircle(x, y, r, GetColor(0, 255, 0), true);

        return true;
    }

	// Blockと衝突しているかどうかを判断し、衝突した場合は反射する。

    // Firebollの消滅
    void disapper()
    {
        m_is_disappered = true;
    }
private:
    bool m_is_disappered = false; // 火の玉にあったかどうか
    Shape::Circle m_realm = Shape::Circle(); // Firebollの領域
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
private:
    bool m_is_disappered = false; 
    Shape::Rectangle m_realm = Shape::Rectangle();
    int life_num = 4; // defaultは４つ。火の玉に当たるごとに一つ減る
};


} // namespace Breakout
