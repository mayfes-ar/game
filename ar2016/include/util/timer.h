/*
	Timer 
	時間を測る機能と
*/

#pragma once

#include <tuple>
#include <string>
#include <thread>


class Timer {
public:
	// 分、秒, ミリ秒を持つ
	using minSecMSec = std::tuple<int, int, int>;

	Timer() {}

	Timer(int max_time)
		: m_max_time(max_time) {}

	Timer(int min, int sec, int msec)
		: m_max_time(min * 60 * 1000 + sec * 1000 + msec) {}
	
	// Timerのカウントを始める
	void start();

	void reset();

	bool isTimerEnd();

	minSecMSec getNowTime();

	minSecMSec getLeftedTime();

	float getRatio() {
		return static_cast<float>(m_max_time - m_now_time) / static_cast<float>(m_max_time);
	}

private:
	int m_start_time = 0;
	int m_now_time = 0;
	int m_max_time = -1;

	bool m_is_initialized = false;
};
