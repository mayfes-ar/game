#pragma once

class Life {
private:
	int m_life = 0;
	int m_max_life = 0;
	int m_origin_life = 0;

public:
	Life() {}
	Life(int life, int max_life) : m_origin_life(life), m_life(life), m_max_life(max_life) {}
	Life(int life, int origin_life, int max_life): m_origin_life(origin_life), m_life(life), m_max_life(max_life) {}
	int getLifeNum() {
		return m_life;
	}
	void resetLife() {
		m_life = m_origin_life;
	}
	bool damage(int amount) {
		if (m_life == 0) return false;
		m_life -= amount;
		if (m_life < 0) m_life = 0;
		return true;
	}
	bool restore(int amount) {
		if (m_life == m_max_life) return false;
		m_life += amount;
		if (m_life > m_max_life) m_life = m_max_life;
		return true;
	}

	bool isAlive() const {
		return m_life != 0;
	}

	float getRatio() {
		return (float)m_life / (float)m_max_life;
	}

};