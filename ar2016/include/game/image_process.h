﻿#pragma once

#include "object/units.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <mutex>
#include <thread>

#define _USE_MATH_DEFINES
#include <math.h>
#include <array>

// スレッド間で共有するデータ
class ShareData {
public:
	// trueにすればonFinishに移行
	bool willFinish = false;

	std::mutex drawMutex;
	int handle = -1;

	//rects が認識結果（座標と大きさ）
	std::mutex rectMutex;
	std::array<Rectan, 7>  rects;
};

void capture(ShareData& share);
