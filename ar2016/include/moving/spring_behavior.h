#pragma once

#include "moving/moving_behavior.h"
#include <cmath>


class StringBehavior : public MovingBehavior
{
public:
	explicit StringBehavior(const Eigen::Vector2f& amp, float freq)
		: m_amp(amp), m_freq(freq) {}

	void update(float time_step, Eigen::Vector2f& pos, Eigen::Vector2f& vel, Eigen::Vector2f& accel) override
	{
		static constexpr float pi = 3.141592f;
		if (!m_is_origin_pos_init) {
			m_origin_pos = Eigen::Vector2f(pos);
			m_is_origin_pos_init = true;
		}
		const float omega = 2.0f * pi * m_freq;
		pos = m_origin_pos + m_amp * std::sin(omega * m_time_cnt);
		vel = m_amp * omega * std::cos(omega * m_time_cnt);
		accel = -m_amp * std::pow(omega, 2.0f) * std::sin(omega * m_time_cnt);
		m_time_cnt += time_step;
	}

private:
	float m_time_cnt = 0.0f;
	Eigen::Vector2f m_amp = Eigen::Vector2f::Zero();
	float m_freq = 0.0f;
	bool m_is_origin_pos_init = false;
	Eigen::Vector2f m_origin_pos;
};
