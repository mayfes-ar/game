#pragma once

#include <algorithm>
#include <cmath>


// ４桁に制限している
class Score {
public:
	Score() {}

	void addPoint(int score) {
		int now_score = m_score;
		now_score += score;
		m_score = std::max<int>(0, std::min<int>(now_score, m_max_score));
	}

	void setScore(int score) {
		m_score = std::max<int>(0, std::min<int>(score, m_max_score));
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
