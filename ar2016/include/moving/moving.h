#pragma once

#include <Eigen/Core>

class Moving {
public:
	Moving() {}

	Moving(float time_step, const Eigen::Vector2f& accel, const Eigen::Vector2f& velocity)
		: m_time_step(time_step), m_accel(accel), m_velocity(velocity)
	{}

	void updatePosition(Eigen::Vector2i& position);

	void setAccel(const Eigen::Vector2f& accel) {
		m_accel = accel;
	}

	void setVelocity(const Eigen::Vector2f& velocity) {
		m_velocity = velocity;
	}

	Eigen::Vector2f getAccel() const
	{
		return m_accel;
	}

	Eigen::Vector2f getVelocity() const
	{
		return m_velocity;
	}

private:
	float m_time_step = 0.1;
	Eigen::Vector2f m_accel = Eigen::Vector2f::Zero();
	Eigen::Vector2f m_velocity = Eigen::Vector2f::Zero(); 

	void updateVelocity();

};
