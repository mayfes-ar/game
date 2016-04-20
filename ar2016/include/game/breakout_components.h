/*
 * BreakoutGameクラスが持つフィールド上のオブジェクト一覧
 */

#pragma once

#include <list>
#include "object/breakout_object.h"


class BreakoutComponents
{
public:
    BreakoutComponents() {}

    void setup()
    {
        // block_listの初期化
        // fireballの初期化
        // shipの初期化
        // Todo Player
    }

    std::vector<std::shared_ptr<Breakout::Block>> block_list;

    std::shared_ptr<Breakout::Fireball> fireball;

    std::shared_ptr<Breakout::Ship> ship;
};
