#include "main.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		return -1;
	} else {
		cap.release();
	}
	//cap.set(cv::CAP_PROP_FPS, 15);
	//cap.set(cv::CAP_PROP_FRAME_WIDTH, 1000);
	//cap.set(cv::CAP_PROP_FRAME_HEIGHT, 320);

	//ChangeWindowMode(true);
	SetGraphMode(1280, 720, 32);
	//SetWindowInitPosition(0, 0);

	if (DxLib_Init() == -1) {
		return -1;
	}
	SetDrawScreen(DX_SCREEN_BACK);

	Fps fps;
	while (true) {

		const auto game = menu();
		if (game == nullptr) {
			break;
		}

		if (!game->onStart()) {
			break;
		}

		while (game->onUpdate()) {
			if (ProcessMessage() == -1) {
				DxLib_End();
				return -1;
			}

			fps.update();
			fps.wait();
		}

		if (!game->onFinish()) {
			break;
		}

	}


	DxLib_End();
	return 0;
}
