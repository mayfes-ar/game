#include "main.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	using namespace std;

	cv::Mat image;
	BASEIMAGE baseImage;
	int handle = -1;

	Fps fps;
	
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		return -1;
	}
	//cap.set(cv::CAP_PROP_FPS, 15);
	//cap.set(cv::CAP_PROP_FRAME_WIDTH, 1000);
	//cap.set(cv::CAP_PROP_FRAME_HEIGHT, 320);

	//ChangeWindowMode(true);
	SetGraphMode(1280, 720, 32);
	//SetWindowInitPosition(0, 0);

	if (DxLib_Init() == -1) {
		cout << "dxlib init" << endl;
		return -1;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	memset(&baseImage, 0, sizeof(BASEIMAGE));
	CreateFullColorData(&baseImage.ColorData);
	baseImage.MipMapCount = 0;
	
	int wid, hei;
	char key[256];
	Player player(400, 400, 30, 30);

	while (ProcessMessage() == 0) {
		if (!cap.read(image)) {
			break;
		}
		
		ClearDrawScreen();
		GetHitKeyStateAll(key);

		baseImage.GraphData = image.data;
		if (handle == -1) {
			baseImage.Width = image.cols;
			baseImage.Height = image.rows;
			baseImage.Pitch = image.step;
			handle = CreateGraphFromBaseImage(&baseImage);
			GetGraphSize(handle, &wid, &hei);
		} else {
			ReCreateGraphFromGraphImage(&baseImage, handle);
		}

		fps.update();
		player.action(key);
		if (key[KEY_INPUT_ESCAPE]) {
			break;
		}
		
		DrawExtendGraph(0, 0, 960, 720, handle, false);
		//DrawGraph(0, 0, handle, false);
		player.draw();
		fps.draw();
		//DrawFormatString(0, 100, GetColor(255, 255, 255), _T( "%d, %d"), wid, hei);

		ScreenFlip();
	}

	cap.release();
	DxLib::DxLib_End();
	return 0;
}
