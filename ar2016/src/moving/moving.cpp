#include "moving/moving.h"

void Moving::updateVelocity()
{
	m_velocity += m_time_step * m_accel;
}


void Moving::updatePosition(Eigen::Vector2i& position)
{
	updateVelocity();
	Eigen::Vector2f pos = position.cast<float>();
	pos += m_time_step * m_velocity + (std::pow(m_time_step, 2.0f) * m_accel) / 2.0f;
	position = pos.cast<int>();
}