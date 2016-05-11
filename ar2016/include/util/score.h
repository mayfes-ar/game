#pragma once


// ４桁に制限している
class Score {
public:
	Score() {}

	void addPoint(int score) {
		m_score += score;
		if (m_score >= m_max_score) {
			m_score = m_max_score;
		}
	}

	int getPoint() {
		return m_score;
	}

	void reset() {
		m_score = 0;
	}
private:
	int m_score = 0;
	const int m_max_score = 10000;
};
