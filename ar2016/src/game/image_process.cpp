﻿#include "game/image_process.h"

using namespace cv;

void detect(cv::Mat& src, ShareData& share);

void capture(ShareData & share) {
	cv::Mat capImage;
	BASEIMAGE baseImage;

	cv::VideoCapture cap(0);

	memset(&baseImage, 0, sizeof(BASEIMAGE));
	CreateFullColorData(&baseImage.ColorData);
	baseImage.MipMapCount = 0;

	while (ProcessMessage() == 0) {
		if (!cap.read(capImage) || share.willFinish) {
			break;
		}

		flip(capImage, capImage, 1);
		detect(capImage, share);

		baseImage.GraphData = capImage.data;
		if (share.handle == -1) {
			baseImage.Width = capImage.cols;
			baseImage.Height = capImage.rows;
			baseImage.Pitch = capImage.step;

			share.drawMutex.lock();
			share.handle = CreateGraphFromBaseImage(&baseImage);
			share.drawMutex.unlock();
		} else {
			share.drawMutex.lock();
			ReCreateGraphFromGraphImage(&baseImage, share.handle);
			share.drawMutex.unlock();
		}
	}

	cap.release();
}

void detect(cv::Mat& src, ShareData& share) {
	using namespace cv;
	using namespace std;

	static aruco::Dictionary dictionary = share.dictionary;
	// markers of 6x6 bits
	

	// if use predefined dictionaries, use below
	//static aruco::Dictionary dictionary =
	//	aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(0));

	static aruco::DetectorParameters detectorParams;
	detectorParams.doCornerRefinement = true; // do corner refinement in markers
 
	static vector<int> ids;
	static vector<vector<Point2f>> corners;

	// detect markers and estimate pose
	aruco::detectMarkers(src, dictionary, corners, ids, detectorParams);

	//array<Rectan, 5> rects;
	array<bool, MARKER_NUM> isDetected = {false, false, false, false};

	// set results
	for (int i = 0; i < ids.size(); i++) {
		Rectan rect;

		isDetected[ids[i]] = true;
		share.lives[ids[i]] = KEEP_FRAME;
		Point2f arrow = (corners)[i][1] - (corners)[i][0];
		const int size = (int)sqrt(arrow.x*arrow.x + arrow.y*arrow.y);
		rect.width = size;
		rect.height = size;

		Point2f center(0, 0);
		for (auto point : (corners)[i]) {
			center += point;
		}
		center /= 4.0;
		rect.x = center.x - size*0.5;
		rect.y = center.y - size*0.5;

		rect.rotate = atan2f(arrow.y, arrow.x);

		rect.scale(CAP2IMG_RATE);
		rect.translate(CAP2IMG_SHIFT_X, CAP2IMG_SHIFT_Y);

		share.rectMutex.lock();
		share.rects[ids[i]] = rect;
		share.rectMutex.unlock();
	}

	for (int id = 0; id < MARKER_NUM; id++) {

		if (isDetected[id]) continue;
		share.rectMutex.lock();
		share.lives[id]--;
		if (share.lives[id] < 0) {
			share.rects[id].width = 0;
			share.rects[id].height = 0;
		}
		share.rectMutex.unlock();
	}
}
