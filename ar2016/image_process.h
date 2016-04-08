#pragma once

#include "units.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <thread>

class ShareData {
public:
	// trueにすればonFinishに移行
	bool isFinish = false;

	std::mutex drawMutex;
	int handle = -1;
};

void capture(ShareData& share);
void detect(cv::Mat& src);
