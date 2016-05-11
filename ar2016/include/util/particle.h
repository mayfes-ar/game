#pragma once

#include <Eigen/Core>
#include <vector>
#include "object/shape.h"


struct Particle {
	Particle() {}
	Particle(const Eigen::Vector2f& pos, const Eigen::Vector2f& vel, float weight = 0.0f)
		: pos(pos), vel(vel), weight(weight) {}

	Eigen::Vector2f pos = Eigen::Vector2f::Zero();
	Eigen::Vector2f vel = Eigen::Vector2f::Zero();
	float weight = 0.0f;
};

class PFilter {
public:
private:
	std::vector<Particle> m_particles; //< 散らばす粒子
	Shape::Circle m_realm; //< 散らばす粒子の領域

	void resample();
};

