#include "image_process.h"

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

		detect(capImage);

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

void detect(cv::Mat& src) {
	using namespace cv;
	using namespace std;

	Mat gray, edges;

	cvtColor(src, gray, COLOR_BGR2GRAY);
	blur(gray, gray, Size(3, 3));

	const double thresh = 100;
	Canny(gray, edges, thresh, 2 * thresh);
	dilate(edges, edges, Mat());

	auto *contours = new vector<vector<Point>>;
	Mat hierarchy;
	findContours(edges, *contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	drawContours(src, *contours, -1, Scalar(240, 117, 28));

	delete contours;
}
