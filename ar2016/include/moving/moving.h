﻿#pragma once

#include <Eigen/Core>

class Moving {
public:
	Moving() {}

	Moving(double time_step, const Eigen::Vector2d& accel, const Eigen::Vector2d& velocity)
		: m_time_step(time_step), m_accel(accel), m_velocity(velocity)
	{}

	void updatePosition(Eigen::Vector2i& position);

	void setAccel(const Eigen::Vector2d& accel) {
		m_accel = accel;
	}

	void setVelocity(const Eigen::Vector2d& velocity) {
		m_velocity = velocity;
	}

	Eigen::Vector2d getAccel() const
	{
		return m_accel;
	}

	Eigen::Vector2d getVelocity() const
	{
		return m_velocity;
	}

private:
	double m_time_step = 0.1;
	Eigen::Vector2d m_accel = Eigen::Vector2d::Zero();
	Eigen::Vector2d m_velocity = Eigen::Vector2d::Zero(); 

	void updateVelocity();

};
