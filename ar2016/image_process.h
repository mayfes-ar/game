#pragma once

#include "units.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <mutex>
#include <thread>

#define _USE_MATH_DEFINES
#include <math.h>

// スレッド間で共有するデータ
class ShareData {
public:
	// trueにすればonFinishに移行
	bool isFinish = false;

	std::mutex drawMutex;
	int handle = -1;

	//rects が認識結果（座標と大きさ）
	std::mutex rectMutex;
	std::vector<Rectan> rects;
};

void capture(ShareData& share);
