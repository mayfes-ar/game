#include "util/particle.h"
#include <array>
#include <random>

void PFilter::resample()
{
	//累積重み
	const int particle_num = static_cast<int>(m_particles.size());
	std::vector<float> weight;
	weight.reserve(particle_num);

	weight.at(0) = m_particles.at(0).weight;
	for (int i = 1; i < particle_num; ++i) {
		weight.at(i) = weight.at(i - 1) + m_particles.at(i).weight;
	}

	//パーティクルのコピー
	std::vector<Particle> particles(m_particles);

	//乱数を生成し、tmpから選ぶ
	static std::random_device rnd;
	static std::mt19937 mt(rnd());
	std::uniform_real_distribution<float> dist_rnd(0, static_cast<float>(m_realm.radius));

	for (int p_id = 0; p_id < particle_num; ++p_id) {
		float dist = dist_rnd(mt);

		for (int other_p_id = 0; other_p_id < particle_num; ++other_p_id) {
			if (dist > weight.at(other_p_id)) {
				continue;
			}
			else {
				m_particles.at(p_id) = particles.at(other_p_id);
				m_particles.at(p_id).weight = 0.0f;
				break;
			}
		}
	}
}