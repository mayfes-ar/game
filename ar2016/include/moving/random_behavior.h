#pragma once

#include "moving/moving_behavior.h"
#include <cmath>
#include <random>

// 目的地を適当に決めてそこについたらまた目的地を決める
class RandomBehavior : public MovingBehavior
{
public:
	RandomBehavior(int min_x, int max_x, int min_y, int max_y) 
		: m_min_x(min_x), m_max_x(max_x), m_min_y(min_y), m_max_y(max_y)
	{}

	void update(float time_step, Eigen::Vector2f& pos, Eigen::Vector2f& vel, Eigen::Vector2f& accel) override
	{
		auto diff = dest_pos - pos;
		// 目的地との差が小さかったら新たな目的地を決める
		if (diff.norm() < 20) {
			dest_pos = Eigen::Vector2f((float)dest_x(m_mt), (float)dest_y(m_mt));
		}
		// 違いとtime_stepをかけたものを足す。つまり追従する
		pos += time_step * diff;
	}
private:
	std::random_device m_rnd;
	std::mt19937 m_mt = std::mt19937(m_rnd());

	int m_min_x = 0;
	int m_max_x = 0;
	int m_min_y = 0;
	int m_max_y = 0;

	std::uniform_int_distribution<int> dest_x = std::uniform_int_distribution<int>(m_min_x, m_max_x);
	std::uniform_int_distribution<int> dest_y = std::uniform_int_distribution<int>(m_min_y, m_max_y);
	Eigen::Vector2f dest_pos;
};