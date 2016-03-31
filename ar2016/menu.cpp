#include "menu.h"

std::shared_ptr<Game> menu() {
	char key[256];
	Fps fps;
	fps.isShow = true;

	while (true) {
		if (ProcessMessage() == -1) {
			return nullptr;
		}
		ClearDrawScreen();

		GetHitKeyStateAll(key);
		if (key[KEY_INPUT_RETURN]) {
			break;
		} else if (key[KEY_INPUT_ESCAPE]) {
			return nullptr;
		}

		fps.update();
		fps.wait();

		SetFontSize(64);
		DrawString(400, 200, "MENU", GetColor(255, 255, 255));
		fps.draw();
		ScreenFlip();

		
	}

	return std::make_shared<FirstGame>();
	
}
