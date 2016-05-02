﻿#include "object/breakout_object.h"
#include <random>

namespace Breakout {

	// 衝突したかどうか
	// 反射の際に正確に反射するのではなく、方向と速度にノイズを載せる
	bool Fireball::isCollided(const Shape::Rectangle& parent) {

		const auto vel = m_moving->getVelocity();

		// 四角形の上側との衝突
		const int dist_center_top = MathUtil::distPointToLine(m_realm.center, parent.getTopLine());
		const int dist_center_right = MathUtil::distPointToLine(m_realm.center, parent.getRightLine());
		const int dist_center_left = MathUtil::distPointToLine(m_realm.center, parent.getLeftLine());
		const int dist_center_bottom = MathUtil::distPointToLine(m_realm.center, parent.getBottomLine());

		if (CollisionDetection::isOnLine(m_realm, parent.getTopLine())) {
			auto vel = m_moving->getVelocity();
			if (m_realm.center.x() < parent.left()) {
				if (dist_center_top < dist_center_left) {
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y() });
					return true;
				}
			} else if (m_realm.center.x() > parent.right()) {
				if (dist_center_top < dist_center_right) {
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y()});
					return true;
				}
			}
			m_moving->setVelocity(Eigen::Vector2f{ vel.x(), -vel.y()});
			return true;

		}
		// 四角形の下側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getBottomLine())) {
			auto vel = m_moving->getVelocity();
			if (m_realm.center.x() < parent.left()) {
				if (dist_center_bottom < dist_center_left) {
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y() });
					return true;
				}
			}
			else if (m_realm.center.x() > parent.right()) {
				if (dist_center_bottom < dist_center_right) {
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y()});
					return true;
				}
			}
			m_moving->setVelocity(Eigen::Vector2f{ vel.x(), -vel.y()});
			return true;
		}
		// 四角形の左側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getLeftLine())) {
			auto vel = m_moving->getVelocity();
			m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y()});
			return true;
		}
		// 四角形の右側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getRightLine())) {
			auto vel = m_moving->getVelocity();
			m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y()});
			return true;
		}
		
		return false;
	}
	bool Field::isOutOfField(const Shape::Rectangle& rect)
	{
		// 四角形がフィールドの中にいるかどうか
		// 四角形がフィールドの枠にいるかどうか
		return true;
	}

	bool Field::isOutOfField(const Shape::Circle& cir)
	{
		// 四角形がフィールドの中にいるかどうか
		// 四角形がフィールドの枠にいるかどうか
		return true;
	}

} // namespace BreakOut