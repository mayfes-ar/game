#pragma once

#include "moving/moving_behavior.h"
#include <cmath>

// v += at , x = vt + 1 / 2 * a * t^2
class NewtonBehavior : public MovingBehavior
{
public:
	NewtonBehavior() {}

	void update(float time_step, Eigen::Vector2f& pos, Eigen::Vector2f& vel, Eigen::Vector2f& accel) override
	{
		vel += time_step * accel;
		pos += time_step * vel + 1.0f / 2.0f * std::pow(time_step, 2.0f) * accel;
	}
};



