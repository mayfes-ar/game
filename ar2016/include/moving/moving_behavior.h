#pragma once

#include <Eigen/Core>


class MovingBehavior {
public:
	virtual ~MovingBehavior() {}

	virtual void update(float time_step, Eigen::Vector2f& pos, Eigen::Vector2f& vel, Eigen::Vector2f& accel) = 0;
};
