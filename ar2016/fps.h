#pragma once

#include "DxLib.h"

class Fps {
	int baseTime;
	int count;
	float fps;
	const int standard;

public:
	bool isShow = false;

	Fps() : baseTime(0), count(0), fps(0), standard(30) {}

	void update() {
		if (count == 0) {
			baseTime = GetNowCount();
		} else if (count == standard) {
			const int nowTime = GetNowCount();
			fps = float(standard) * 1000.f / (nowTime - baseTime);
			count = 0;
			baseTime = nowTime;
		}
		
		count++;
	}

	void draw() {
		if (isShow) {
			DrawFormatString(0, 0, GetColor(255, 255, 255), _T("%.1f"), fps);
		}
	}

	void wait() {
		const int tookTime = GetNowCount() - baseTime;
		const int waitTime = count * 1000 / standard - tookTime;
		if (waitTime > 0) {
			Sleep(waitTime);
		}
	}
};
