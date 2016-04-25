#include "moving/moving.h"

void Moving::updateVelocity()
{
	m_velocity += m_time_step * m_accel;
}


void Moving::updatePosition(Eigen::Vector2i& position)
{
	updateVelocity();
	Eigen::Vector2d pos = position.cast<double>();
	pos += m_time_step * m_velocity + 1.0 / 2.0 * std::pow(m_time_step, 2.0) * m_accel;
	position = pos.cast<int>();
}