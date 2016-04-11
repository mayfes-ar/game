#include "main.h"

// ここからスタート
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	// カメラの起動確認
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		return -1;
	} else {
		cap.release();
	}
	

	//cap.set(cv::CAP_PROP_FPS, 15);
	//cap.set(cv::CAP_PROP_FRAME_WIDTH, 1000);
	//cap.set(cv::CAP_PROP_FRAME_HEIGHT, 320);

	// 画面サイズは 1280x720
	SetGraphMode(1280, 720, 32);
#ifdef _DEBUG
	ChangeWindowMode(true);
	SetMainWindowText("DEBUG MODE");
#endif 

	if (DxLib_Init() == -1) {
		return -1;
	}
	SetDrawScreen(DX_SCREEN_BACK);

	if (!Object::load()) {
		DxLib_End();
		return -1;
	}

	while (true) {

		// メニューからゲームを選択
		const auto game = startMenu(); //game には選択されたゲームが入る（例: std::shared_ptr<FirstGame>()）

		if (game == nullptr) {
			break;
		}

		if (!game->onStart()) { //onStart の返り値はただのエラーチェック
			break;
		}

		// ゲームのループ部分
		while (game->onUpdate()) {
			if (ProcessMessage() == -1) {
				DxLib_End();
				return -1;
			}
		}

		if (!game->onFinish() || true) { //onFinish の返り値でMenuに戻る(true)orゲームが終了する(false)
			break;
		}

	}


	DxLib_End();
	return 0;
}
