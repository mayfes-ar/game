#pragma once

#include <Eigen/Core>
#include <memory>
#include "moving/moving_behavior.h"


// 位置を渡し、予め位置、速度、加速度更新関数を登録し、
class Moving {
public:
	Moving(float time_step, const std::shared_ptr<MovingBehavior>& moving_behavior)
		: m_time_step(time_step), m_moving_behavior(moving_behavior)
	{}

	Moving(float time_step, const std::shared_ptr<MovingBehavior>& moving_behavior,
		const Eigen::Vector2f& vel, const Eigen::Vector2f& accel)
		: m_time_step(time_step), m_moving_behavior(moving_behavior),
		  m_vel(vel), m_accel(accel)
	{}

	void updatePoistion(Eigen::Vector2i& pos) {
		Eigen::Vector2f pos_f = pos.cast<float>();
		m_moving_behavior->update(m_time_step, pos_f, m_vel, m_accel);
		pos = pos_f.cast<int>();
	}

	void setAccel(const Eigen::Vector2f& accel) {
		m_accel = accel;
	}

	void setVelocity(const Eigen::Vector2f& velocity) {
		m_vel = velocity;
	}

	void setBehavior(std::shared_ptr<MovingBehavior>& behavior) {
		m_moving_behavior = behavior;
	}

	Eigen::Vector2f getAccel() const
	{
		return m_accel;
	}

	Eigen::Vector2f getVelocity() const
	{
		return m_vel;
	}
private:
	float m_time_step = 0.0f;
	std::shared_ptr<MovingBehavior> m_moving_behavior = nullptr;
	Eigen::Vector2f m_vel = Eigen::Vector2f::Zero();
	Eigen::Vector2f m_accel = Eigen::Vector2f::Zero();
};