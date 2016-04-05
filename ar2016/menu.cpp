#include "menu.h"

std::shared_ptr<Game> startMenu() {
	using namespace std;

	shared_ptr<Game> gameType; 
	Menu menu(gameType);

	menu.onStart();

	while (menu.onUpdate()) {
		if (ProcessMessage() == -1) {
			return nullptr;
		}
	}

	menu.onFinish();

	return gameType;
	
}
