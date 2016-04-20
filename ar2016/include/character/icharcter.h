#pragma once


#include "object/iobject.h"


class ICharacter : public IObject<LayerLabel::Character>
{
    virtual bool isAlive() = 0;

    virtual ~ICharacter() {}
};
