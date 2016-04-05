#include "game.h"


void capture(std::mutex& mutex, int& handle, bool& isFinish) {

	cv::Mat capImage;
	BASEIMAGE baseImage;

	cv::VideoCapture cap(0);

	memset(&baseImage, 0, sizeof(BASEIMAGE));
	CreateFullColorData(&baseImage.ColorData);
	baseImage.MipMapCount = 0;

	while (ProcessMessage() == 0) {
		if (!cap.read(capImage) || isFinish) {
			break;
		}

		baseImage.GraphData = capImage.data;
			if (handle == -1) {
				baseImage.Width = capImage.cols;
				baseImage.Height = capImage.rows;
				baseImage.Pitch = capImage.step;

				mutex.lock();
				handle = CreateGraphFromBaseImage(&baseImage);
				mutex.unlock();
			} else {
				mutex.lock();
				ReCreateGraphFromGraphImage(&baseImage, handle);
				mutex.unlock();
			}
	}

	cap.release();
}