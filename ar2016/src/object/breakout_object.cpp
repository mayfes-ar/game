#include "object/breakout_object.h"
#include <random>

namespace Breakout {

	// 衝突したかどうか
	bool Fireball::isCollided(const Shape::Rectangle& parent, const int inOrOut, const int parentVelocity) {

		const auto vel = m_moving->getVelocity();
		const float coe = 0.2;
		const float arranged_parent_vel = ((float)parentVelocity * coe);

		// 四角形の上側との衝突
		const int dist_center_top = MathUtil::distPointToLine(m_realm.center, parent.getTopLine());
		const int dist_center_right = MathUtil::distPointToLine(m_realm.center, parent.getRightLine());
		const int dist_center_left = MathUtil::distPointToLine(m_realm.center, parent.getLeftLine());
		const int dist_center_bottom = MathUtil::distPointToLine(m_realm.center, parent.getBottomLine());

		if (CollisionDetection::isOnLine(m_realm, parent.getTopLine()/*, m_moving->getVelocity().norm()*/)) {
			auto vel = m_moving->getVelocity();
			if (m_realm.center.x() < parent.left()) {
				if (dist_center_top < dist_center_left) {
					auto pos = Eigen::Vector2i(inOrOut * -1 * m_realm.radius + parent.left(), m_realm.center[1]);
					setPosition(pos);
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y() });
					return true;
				}
			} else if (m_realm.center.x() > parent.right()) {
				if (dist_center_top < dist_center_right) {
					auto pos = Eigen::Vector2i(inOrOut * m_realm.radius + parent.right(), m_realm.center[1]);
					setPosition(pos);
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y()});
					return true;
				}
			}
			auto pos = Eigen::Vector2i(m_realm.center[0], inOrOut * -1 * m_realm.radius + parent.top());
			setPosition(pos);
			auto new_vel = Eigen::Vector2f{ vel.x() + arranged_parent_vel, -vel.y() };
			auto degree = atan2f(-inOrOut * new_vel.y(), new_vel.x());
			if (degree < M_PI/9.0) {
				m_moving->setVelocity(Eigen::Vector2f(cos(M_PI / 9), -inOrOut * sin(M_PI / 9)) * vel.norm());
				return true;
			} else if(degree > M_PI * 8.0 / 9.0) {
				m_moving->setVelocity(Eigen::Vector2f(cos(M_PI * 8 / 9), -inOrOut * sin(M_PI * 8 / 9)) * vel.norm());
				return true;
			}
			m_moving->setVelocity(new_vel * vel.norm() / new_vel.norm());
			return true;

		}
		// 四角形の下側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getBottomLine())) {
			auto vel = m_moving->getVelocity();
			if (m_realm.center.x() < parent.left()) {
				if (dist_center_bottom < dist_center_left) {
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y() });
					auto pos = Eigen::Vector2i(inOrOut * -1 * m_realm.radius + parent.left(), m_realm.center[1]);
					setPosition(pos);
					return true;
				}
			}
			else if (m_realm.center.x() > parent.right()) {
				if (dist_center_bottom < dist_center_right) {
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y()});
					auto pos = Eigen::Vector2i(inOrOut * m_realm.radius + parent.right(), m_realm.center[1]);
					setPosition(pos);
					return true;
				}
			}
			
			auto pos = Eigen::Vector2i(m_realm.center[0], inOrOut * m_realm.radius + parent.bottom());
			setPosition(pos);
			auto new_vel = Eigen::Vector2f{ vel.x() + arranged_parent_vel, -vel.y() };
			auto degree = atan2f(-inOrOut * new_vel.y(), new_vel.x());
			if (degree > -M_PI / 9.0) {
				m_moving->setVelocity(Eigen::Vector2f(cos(M_PI / 9), -inOrOut * sin(M_PI / 9)) * vel.norm());
				return true;
			}
			else if (degree < -M_PI * 8.0 / 9.0) {
				m_moving->setVelocity(Eigen::Vector2f(cos(-M_PI * 8.0 / 9.0), -inOrOut * sin(-M_PI*8.0 / 9.0)) * vel.norm());
				return true;
			}
			m_moving->setVelocity(new_vel * vel.norm() / new_vel.norm());
			return true;
		}
		// 四角形の左側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getLeftLine())) {
			auto vel = m_moving->getVelocity();
			m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y()});
			auto pos = Eigen::Vector2i(inOrOut * -1 * m_realm.radius + parent.left(), m_realm.center[1]);
			setPosition(pos);
			return true;
		}
		// 四角形の右側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getRightLine())) {
			auto vel = m_moving->getVelocity();
			m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y()});
			auto pos = Eigen::Vector2i(inOrOut * m_realm.radius + parent.right(), m_realm.center[1]);
			setPosition(pos);
			return true;
		}
		
		return false;
	}


	std::shared_ptr<FireballReflect> Fireball::returnFireballReflect(const Shape::Rectangle& parent, const int inOrOut, const int parentVelocity) {

		const auto vel = m_moving->getVelocity();
		const float coe = 0.2;
		const float arranged_parent_vel = ((float)parentVelocity * coe);

		// 四角形の上側との衝突
		const int dist_center_top = MathUtil::distPointToLine(m_realm.center, parent.getTopLine());
		const int dist_center_right = MathUtil::distPointToLine(m_realm.center, parent.getRightLine());
		const int dist_center_left = MathUtil::distPointToLine(m_realm.center, parent.getLeftLine());
		const int dist_center_bottom = MathUtil::distPointToLine(m_realm.center, parent.getBottomLine());

		if (CollisionDetection::isOnLine(m_realm, parent.getTopLine()/*, m_moving->getVelocity().norm()*/)) {
			auto vel = m_moving->getVelocity();
			if (m_realm.center.x() < parent.left()) {
				if (dist_center_top < dist_center_left) {
					auto pos = Eigen::Vector2i(inOrOut * -1 * m_realm.radius + parent.left(), m_realm.center[1]);
					setPosition(pos);
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y() });
					//outだったら
					if (inOrOut == 1) {
						Eigen::Vector2i hit_point = pos + Eigen::Vector2i(m_realm.radius, 0);
						Direction dir = Left;
						return std::make_shared<FireballReflect>(hit_point, dir);
					} else {
						Eigen::Vector2i hit_point = pos + Eigen::Vector2i(-m_realm.radius, 0);
						Direction dir = Right;
						return std::make_shared<FireballReflect>(hit_point, dir);
					}
				}
			}
			else if (m_realm.center.x() > parent.right()) {
				if (dist_center_top < dist_center_right) {
					auto pos = Eigen::Vector2i(inOrOut * m_realm.radius + parent.right(), m_realm.center[1]);
					setPosition(pos);
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y() });
					//outだったら
					if (inOrOut == 1) {
						Eigen::Vector2i hit_point = pos + Eigen::Vector2i(-m_realm.radius, 0);
						Direction dir = Right;
						return std::make_shared<FireballReflect>(hit_point, dir);
					}
					else {
						Eigen::Vector2i hit_point = pos + Eigen::Vector2i(m_realm.radius, 0);
						Direction dir = Left;
						return std::make_shared<FireballReflect>(hit_point, dir);
					}
				}
			}
			auto pos = Eigen::Vector2i(m_realm.center[0], inOrOut * -1 * m_realm.radius + parent.top());
			setPosition(pos);
			auto new_vel = Eigen::Vector2f{ vel.x() + arranged_parent_vel, -vel.y() };
			auto degree = atan2f(-inOrOut * new_vel.y(), new_vel.x());
			if (degree < M_PI / 9.0) {
				m_moving->setVelocity(Eigen::Vector2f(cos(M_PI / 9), -inOrOut * sin(M_PI / 9)) * vel.norm());
				//outだったら
				if (inOrOut == 1) {
					Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, m_realm.radius);
					Direction dir = Top;
					return std::make_shared<FireballReflect>(hit_point, dir);
				}
				else {
					Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, -m_realm.radius);
					Direction dir = Bottom;
					return std::make_shared<FireballReflect>(hit_point, dir);
				}

			}
			else if (degree > M_PI * 8.0 / 9.0) {
				m_moving->setVelocity(Eigen::Vector2f(cos(M_PI * 8 / 9), -inOrOut * sin(M_PI * 8 / 9)) * vel.norm());
				//outだったら
				if (inOrOut == 1) {
					Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, m_realm.radius);
					Direction dir = Top;
					return std::make_shared<FireballReflect>(hit_point, dir);
				}
				else {
					Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, -m_realm.radius);
					Direction dir = Bottom;
					return std::make_shared<FireballReflect>(hit_point, dir);
				}
			}
			m_moving->setVelocity(new_vel * vel.norm() / new_vel.norm());
			//outだったら
			if (inOrOut == 1) {
				Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, m_realm.radius);
				Direction dir = Top;
				return std::make_shared<FireballReflect>(hit_point, dir);
			}
			else {
				Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, -m_realm.radius);
				Direction dir = Bottom;
				return std::make_shared<FireballReflect>(hit_point, dir);
			}

		}
		// 四角形の下側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getBottomLine())) {
			auto vel = m_moving->getVelocity();
			if (m_realm.center.x() < parent.left()) {
				if (dist_center_bottom < dist_center_left) {
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y() });
					auto pos = Eigen::Vector2i(inOrOut * -1 * m_realm.radius + parent.left(), m_realm.center[1]);
					setPosition(pos);
					
					//outなら
					if (inOrOut == 1) {
						Eigen::Vector2i hit_point = pos + Eigen::Vector2i(m_realm.radius, 0);
						Direction dir = Left;
						return std::make_shared<FireballReflect>(hit_point, dir);
					}
					else {
						Eigen::Vector2i hit_point = pos + Eigen::Vector2i(-m_realm.radius, 0);
						Direction dir = Right;
						return std::make_shared<FireballReflect>(hit_point, dir);
					}
				}
			}
			else if (m_realm.center.x() > parent.right()) {
				if (dist_center_bottom < dist_center_right) {
					m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y() });
					auto pos = Eigen::Vector2i(inOrOut * m_realm.radius + parent.right(), m_realm.center[1]);
					setPosition(pos);
					//outだったら
					if (inOrOut == 1) {
						Eigen::Vector2i hit_point = pos + Eigen::Vector2i(-m_realm.radius, 0);
						Direction dir = Right;
						return std::make_shared<FireballReflect>(hit_point, dir);
					}
					else {
						Eigen::Vector2i hit_point = pos + Eigen::Vector2i(m_realm.radius, 0);
						Direction dir = Left;
						return std::make_shared<FireballReflect>(hit_point, dir);
					}

				}
			}

			auto pos = Eigen::Vector2i(m_realm.center[0], inOrOut * m_realm.radius + parent.bottom());
			setPosition(pos);
			auto new_vel = Eigen::Vector2f{ vel.x() + arranged_parent_vel, -vel.y() };
			auto degree = atan2f(-inOrOut * new_vel.y(), new_vel.x());
			if (degree > -M_PI / 9.0) {
				m_moving->setVelocity(Eigen::Vector2f(cos(M_PI / 9), -inOrOut * sin(M_PI / 9)) * vel.norm());
				//outだったら
				if (inOrOut == 1) {
					Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, -m_realm.radius);
					Direction dir = Bottom;
					return std::make_shared<FireballReflect>(hit_point, dir);
				}
				else {
					Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, m_realm.radius);
					Direction dir = Top;
					return std::make_shared<FireballReflect>(hit_point, dir);
				}
			}
			else if (degree < -M_PI * 8.0 / 9.0) {
				m_moving->setVelocity(Eigen::Vector2f(cos(-M_PI * 8.0 / 9.0), -inOrOut * sin(-M_PI*8.0 / 9.0)) * vel.norm());
				//outだったら
				if (inOrOut == 1) {
					Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, -m_realm.radius);
					Direction dir = Bottom;
					return std::make_shared<FireballReflect>(hit_point, dir);
				}
				else {
					Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, m_realm.radius);
					Direction dir = Top;
					return std::make_shared<FireballReflect>(hit_point, dir);
				}
			}
			m_moving->setVelocity(new_vel * vel.norm() / new_vel.norm());
			//outだったら
			if (inOrOut == 1) {
				Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, -m_realm.radius);
				Direction dir = Bottom;
				return std::make_shared<FireballReflect>(hit_point, dir);
			}
			else {
				Eigen::Vector2i hit_point = pos + Eigen::Vector2i(0, m_realm.radius);
				Direction dir = Top;
				return std::make_shared<FireballReflect>(hit_point, dir);
			}
		}
		// 四角形の左側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getLeftLine())) {
			auto vel = m_moving->getVelocity();
			m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y() });
			auto pos = Eigen::Vector2i(inOrOut * -1 * m_realm.radius + parent.left(), m_realm.center[1]);
			setPosition(pos);
			//outなら
			if (inOrOut == 1) {
				Eigen::Vector2i hit_point = pos + Eigen::Vector2i(m_realm.radius, 0);
				Direction dir = Left;
				return std::make_shared<FireballReflect>(hit_point, dir);
			}
			else {
				Eigen::Vector2i hit_point = pos + Eigen::Vector2i(-m_realm.radius, 0);
				Direction dir = Right;
				return std::make_shared<FireballReflect>(hit_point, dir);
			}
		}
		// 四角形の右側との衝突
		else if (CollisionDetection::isOnLine(m_realm, parent.getRightLine())) {
			auto vel = m_moving->getVelocity();
			m_moving->setVelocity(Eigen::Vector2f{ -vel.x(), vel.y() });
			auto pos = Eigen::Vector2i(inOrOut * m_realm.radius + parent.right(), m_realm.center[1]);
			setPosition(pos);
			//outなら
			if (inOrOut == 1) {
				Eigen::Vector2i hit_point = pos + Eigen::Vector2i(-m_realm.radius, 0);
				Direction dir = Right;
				return std::make_shared<FireballReflect>(hit_point, dir);
			}
			else {
				Eigen::Vector2i hit_point = pos + Eigen::Vector2i(m_realm.radius, 0);
				Direction dir = Left;
				return std::make_shared<FireballReflect>(hit_point, dir);
			}
		}

		return std::make_shared<FireballReflect>();
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