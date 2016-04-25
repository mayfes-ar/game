

#pragma once

#include <list>
#include <memory>
#include <vector>
#include "object/breakout_object.h"
#include "const.h"
#include <Eigen/Core>


class BreakoutComponents
{
public:
    BreakoutComponents() {}

	void setup();


    std::shared_ptr<Breakout::Info> info = nullptr;
    std::shared_ptr<Breakout::Info> debug = nullptr;
    std::shared_ptr<Breakout::Field> field = nullptr;

	std::vector<std::shared_ptr<Breakout::Block>> block_list;

    std::shared_ptr<Breakout::Fireball> fireball = nullptr;

    std::shared_ptr<Breakout::Ship> ship = nullptr;
};
