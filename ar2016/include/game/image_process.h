#pragma once

#include "object/units.h"
#include "game/marker_dictionary.h"

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

	std::mutex rectMutex;
	//マーカーが認識されなかったときにこの値が正ならマーカーが認識されなくなっても値分のフレームだけ画面に表示される。
	std::array<int, MARKER_NUM> lives{0,0,0,0,0};
	//rects が認識結果（座標と大きさ）
	std::array<Rectan, MARKER_NUM>  rects;
	aruco::Dictionary dictionary = MarkerDictionary().dictionary;
};

void capture(ShareData& share);
