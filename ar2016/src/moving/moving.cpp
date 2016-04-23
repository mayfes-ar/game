#include "moving/moving.h"

void Moving::updateAccel()
{
}

void Moving::updateVelocity()
{
	m_velocity += m_time_step * m_accel;
}


void Moving::updatePosition()
{
	m_position += m_time_step * m_velocity;
}