﻿#include "image_process.h"

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

	static aruco::Dictionary dictionary;
	// markers of 6x6 bits
	dictionary.markerSize = 7;
	// maximum number of bit corrections
	dictionary.maxCorrectionBits = 1;
	// lets create a dictionary of 100 markers

	unsigned char markers[6][49] = {
		{
			0,0,0,1,0,0,0,
			0,0,1,0,0,0,0,
			0,1,0,0,0,0,0,
			1,0,1,1,1,1,1,
			0,1,0,0,0,0,0,
			0,0,1,0,0,0,0,
			0,0,0,1,0,0,0
		},
		{
			0,0,1,1,1,0,0,
			0,1,0,0,0,1,0,
			0,1,0,0,0,1,0,
			0,0,1,1,0,0,0,
			0,0,0,1,0,0,0,
			0,0,0,0,0,0,0,
			0,0,0,1,0,0,0
		},
		{
			0,0,0,0,0,0,0,
			0,1,1,1,1,1,0,
			0,1,0,0,0,1,0,
			0,1,0,0,0,1,0,
			0,1,0,0,0,1,0,
			0,1,1,1,1,1,0,
			0,0,0,0,0,0,0
		},
		{
			0,0,0,0,0,0,0,
			0,1,1,1,1,1,0,
			0,1,0,0,0,1,0,
			1,1,0,0,0,1,0,
			0,1,0,0,0,1,0,
			0,1,1,1,1,1,0,
			0,0,0,0,0,0,0
		},
		{
			1,1,1,1,1,1,1,
			0,1,1,1,1,1,0,
			0,0,0,0,0,0,0,
			0,1,1,1,1,1,0,
			0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,
			1,1,1,1,1,1,1
		},
		{
			0,0,0,0,0,0,0,
			1,1,1,1,1,0,0,
			1,1,1,1,0,1,0,
			1,1,1,1,1,1,1,
			1,1,1,1,1,1,0,
			1,1,1,1,1,0,0,
			0,0,0,0,0,0,0
		}
	};

	for (int i = 0; i<sizeof(markers) / sizeof(markers[0]); i++) {
		// assume generateMarkerBits() generate a new marker in binary format, so that
		// markerBits is a 5x5 matrix of CV_8UC1 type, only containing 0s and 1s

		cv::Mat markerBits(7, 7, CV_8UC1, markers[i]);//generateMarkerBits();

		cv::Mat markerCompressed = aruco::Dictionary::getByteListFromBits(markerBits);
		// add the marker as a new row
		dictionary.bytesList.push_back(markerCompressed);
	}

	//static aruco::Dictionary dictionary =
	//	aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(0));

	static aruco::DetectorParameters detectorParams;
	detectorParams.doCornerRefinement = true; // do corner refinement in markers

	static vector<int> ids;
	// vector<vector<Point2f>> corners;
	//auto* corners = new vector<vector<Point2f>>;
	static vector<vector<Point2f>> corners;
	//auto *rejected = new vector<vector<Point2f>>;

	// detect markers and estimate pose
	aruco::detectMarkers(src, dictionary, corners, ids, detectorParams/*, *rejected*/);
	 
	vector<Rectan> rects;

	// set results
	for (int i = 0; i < ids.size(); i++) {
		Rectan rect;

		rect.id = ids[i];
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

		const double theta = atan2f(arrow.y, arrow.x);

		rect.scale(CAP2IMG_RATE);
		rect.translate(CAP2IMG_SHIFT_X, CAP2IMG_SHIFT_Y);

		rects.push_back(rect);
	}
	
	share.rectMutex.lock();
	share.rects = rects;
	share.rectMutex.unlock();
}