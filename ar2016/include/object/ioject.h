#pragma once

#include "object/oject_layer.h"


template<LayerLabel layerLabel>
class IOject
{
public:
    const virtual bool draw() = 0;
    virtual ~IOject() {}
};
