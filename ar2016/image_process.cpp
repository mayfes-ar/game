#include "image_process.h"

using namespace cv;

void detect(cv::Mat& src, ShareData& share);
double angle(Point pt1, Point pt2, Point pt0);

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
	using namespace std;

	vector<Rectan> rect;
	Mat gray, edges;

	cvtColor(src, gray, COLOR_BGR2GRAY);
	blur(gray, gray, Size(3, 3));

	const int thresh = 100;
	Canny(gray, edges, thresh, 2 * thresh);
	dilate(edges, edges, Mat());

	auto *contours = new vector<vector<Point>>;
	Mat hierarchy;
	findContours(edges, *contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	static vector<Point> approx;
	auto *squares = new vector<vector<Point>>;

	for (auto contour : *contours) {
		approxPolyDP(Mat(contour), approx, arcLength(Mat(contour), true) * 0.02, true);

		// 4頂点　面積一定以上　凸状
		const int areaThresh = 500;
		if ((approx).size() == 4
			&& fabs(contourArea(Mat(approx))) > areaThresh
			&& isContourConvex(Mat(approx))) {
			
			double maxCos = 0;
			for (int i = 2; i < 5; i++) {
				const double cos = fabs(angle((approx)[i % 4], (approx)[i - 2], (approx)[i - 1]));
				maxCos = MAX(maxCos, cos);
			}
			
			if (maxCos < 0.3) {
				(*squares).push_back(approx);
			}
		}
	}

	// squares >>> rects
	vector<Rectan> rects;
	const double allowedSlope = 0.1;
	const double rate = 1.5;
	for (auto square : *squares) {
		/*const Point* p = &(square)[0];
		const int n = (int)(square).size();
		polylines(src, &p, &n, 1, true, Scalar(240, 117, 28), 3, 4);*/
		
		Rectan rect;

		const double xDiff = fabs(square[0].x - square[1].x);
		const double yDiff = fabs(square[0].y - square[1].y);

		if (xDiff > yDiff) {
			if (yDiff / xDiff > allowedSlope) { continue; }
			rect.width = xDiff;
			rect.height = fabs(square[2].y - square[1].y);
			rect.x = MIN(square[0].x, square[1].x);
			rect.y = MIN(square[1].y, square[2].y);
		} else {
			if (xDiff / yDiff > allowedSlope) { continue; }
			rect.height = yDiff;
			rect.width = fabs(square[2].x - square[1].x);
			rect.x = MIN(square[2].x, square[1].x);
			rect.y = MIN(square[0].y, square[1].y);
		}
		rect.scale(rate);
		rect.translate(160, 0);
		rects.push_back(rect);
	}

	share.rectMutex.lock();
	share.rects = rects;
	share.rectMutex.unlock();

	delete contours;
	delete squares;
}

double angle(Point pt1, Point pt2, Point pt0)
{
	const double dx1 = pt1.x - pt0.x;
	const double dy1 = pt1.y - pt0.y;
	const double dx2 = pt2.x - pt0.x;
	const double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}