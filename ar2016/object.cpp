#include "object.h"

using namespace std;

map<string, int> Object::imgHandles;

bool Object::load() {

	auto loadImage = [](string key, string name) -> bool {
		const string path = "img/" + name + ".png";
		const int handle = LoadGraph(path.c_str());
		if (handle == -1) {
			return false;
		} else {
			Object::imgHandles[key] = handle;
			return true;
		}
	};

	bool isSuccess = true;

	isSuccess &= loadImage("player", "mario");


	return isSuccess;
}
