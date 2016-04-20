#pragma once


class IGame
{
public:
    virtual bool onStart() = 0;
    virtual bool onFinish() = 0;
    virtual bool onUpdate() = 0;
    virtual ~IGame() {}
};
