#pragma once


class Game {
protected:
	bool isNormal = true;
public:
	virtual bool onStart() = 0;
	virtual bool onUpdate() = 0;
	virtual bool onFinish() = 0;
};