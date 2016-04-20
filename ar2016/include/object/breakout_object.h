/*
 * BreakoutGameで使うオブジェクト一覧
 */

#pragma once

#include "object/object.h"

namespace Breakout {
// 描画の優先順位
const int PRIORITY_BACKGROUND = 0;
const int PRIORITY_STATIC_OBJECT = 1;
const int PRIORITY_DYNAMIC_OBJECT = 2;
const int PRIORITY_CHARACTER = 3;


class Background : public Object
{
public:
    Background(){
        Object::layer = PRIORITY_BACKGROUND;
    }

    bool draw() {
        SetDrawBright(40, 40, 40);
        DrawExtendGraph(0, 0, WIDTH, HEIGHT, 
                imgHandles["breakout-background"], true);
        SetDrawBright(255, 255, 255);
        return true;
    }
};

} // namespace Breakout
