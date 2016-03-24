#pragma once

#include "DxLib.h"

class Fps {
	int baseTime;
	int count;
	float fps;
	static const int N = 30;

public:
	Fps() : baseTime(0), count(0), fps(0) {}

	void update() {
		if (count == 0) {
			baseTime = GetNowCount();
		} else if (count == N) {
			int nowTime = GetNowCount();
			fps = float(N) * 1000.f / (nowTime - baseTime);
			count = 0;
			baseTime = nowTime;
		}
		
		count++;
	}

	void draw() {
		DrawFormatString(0, 0, GetColor(255, 255, 255), _T("%.1f"), fps);
	}
};
