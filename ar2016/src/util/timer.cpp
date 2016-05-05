#include "util/timer.h"
#include "Dxlib.h"

void Timer::start()
{
	if (m_is_initialized) {
		return;
	}

	m_start_time = GetNowCount();
	m_is_initialized = true;
}

void Timer::reset()
{
	m_start_time = GetNowCount();
}

bool Timer::isTimerEnd() 
{
	m_now_time = GetNowCount() - m_start_time;

	if (m_now_time > m_max_time) {
		return true;
	}

	return false;
}



Timer::minSecMSec Timer::getNowTime()
{
	if (m_is_initialized) {
		throw std::logic_error("Timer is not initialized");
	}

	m_now_time = GetNowCount() - m_start_time;

	int now_time = m_now_time;
	int miniute = now_time / (60 * 1000);
	now_time -= miniute * 60 * 1000;
	int sec = now_time / 1000;
	now_time -= sec * 1000;
	int msec = now_time;

	return minSecMSec{ miniute, sec, msec };
}

Timer::minSecMSec Timer::getLeftedTime()
{
	if (!m_is_initialized) {
		throw std::logic_error("Timer is not initialized");
	}

	m_now_time = GetNowCount() - m_start_time;

	int left_time = m_max_time - m_now_time;
	if (left_time < 0) {
		return minSecMSec{ 0, 0, 0 };
	}

	int minute = left_time / (60 * 1000);
	left_time -= minute * 60 * 1000;
	int sec = left_time / 1000;
	left_time -= sec * 1000;
	int msec = left_time;

	return minSecMSec{minute, sec, msec};
}

