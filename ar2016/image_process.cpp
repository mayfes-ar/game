#include "image_process.h"

using namespace cv;

void detectFromMarkers(cv::Mat& src, ShareData& share);

void capture(ShareData & share) {
	cv::Mat capImage;
	BASEIMAGE baseImage;

	cv::VideoCapture cap(0);

	memset(&baseImage, 0, sizeof(BASEIMAGE));
	CreateFullColorData(&baseImage.ColorData);
	baseImage.MipMapCount = 0;

	while (ProcessMessage() == 0) {
		if (!cap.read(capImage) || share.isFinish) {
			break;
		}

		detectFromMarkers(capImage, share);

		baseImage.GraphData = capImage.data;
		if (share.handle == -1) {
			baseImage.Width = capImage.cols;
			baseImage.Height = capImage.rows;
			baseImage.Pitch = capImage.step;

			share.drawMutex.lock();
			share.handle = CreateGraphFromBaseImage(&baseImage);
			share.drawMutex.unlock();
		}
		else {
			share.drawMutex.lock();
			ReCreateGraphFromGraphImage(&baseImage, share.handle);
			share.drawMutex.unlock();
		}
	}

	cap.release();
}

void detectFromMarkers(cv::Mat& src, ShareData& share) {
	using namespace cv;
	using namespace std;

	double rate = 1.5;

	static aruco::Dictionary dictionary =
		aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(0));
	static aruco::DetectorParameters detectorParams;
	detectorParams.doCornerRefinement = true; // do corner refinement in markers

	static vector<int> ids;
	// vector<vector<Point2f>> corners;
	//auto* corners = new vector<vector<Point2f>>;
	static vector<vector<Point2f>> corners;
	//auto *rejected = new vector<vector<Point2f>>;

	// detect markers and estimate pose
	aruco::detectMarkers(src, dictionary, corners, ids, detectorParams/*, *rejected*/);
	 
	// make Rectans
	vector<Rectan> rectans;

	// draw results
	if (ids.size() > 0) {
		for (int i = 0; i < ids.size(); i++) {
			Rectan rectan;
			
			rectan.id = ids[i];
			Point2f arrow = (corners)[i][1] - (corners)[i][0];
			int size = (int)sqrt(arrow.x*arrow.x + arrow.y*arrow.y);
			rectan.width = size;
			rectan.height = size;

			Point2f center(0,0);
			for (auto point : (corners)[i]) {
				center += point;
			}
			center /= 4.0;
			rectan.x = center.x - size*0.5;
			rectan.y = center.y - size*0.5;

			float theta = atan2f(arrow.y, arrow.x) * 180.0 / M_PI;
			if (-5 < theta && theta < 5) rectan.direction = 0;
			else if (85 < theta && theta < 95) rectan.direction = 1;
			else if (175 < theta || theta < -175) rectan.direction = 2;
			else if (-95 < theta && theta < -85) rectan.direction = 3;

			rectan.scale(rate);
			rectan.translate(160, 0);

			if(rectan.direction != -1) rectans.push_back(rectan);
		}
	}
	share.rectMutex.lock();
	share.rects = rectans;
	share.rectMutex.unlock();

	//delete corners;
	//delete rejected;
}