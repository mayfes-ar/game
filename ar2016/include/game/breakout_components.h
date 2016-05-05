

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

    std::shared_ptr<Breakout::Background> background = nullptr;
    std::shared_ptr<Breakout::Info> info = nullptr;
	std::shared_ptr<Breakout::Explanation> explanation = nullptr;
    std::shared_ptr<Breakout::Result> result = nullptr;
    std::shared_ptr<Breakout::Field> field = nullptr;

	std::vector<std::shared_ptr<Breakout::Block>> block_list;

	std::shared_ptr<Breakout::FireballManager> fireball_manager = nullptr;

    std::shared_ptr<Breakout::Ship> ship = nullptr;

	std::shared_ptr<Breakout::Pot> pot = nullptr;

	std::vector<std::shared_ptr<Breakout::Item>> item_list;
};
