

#pragma once

#include <list>
#include <memory>
#include <vector>
#include <unordered_map>
#include "object/breakout_object.h"
#include "const.h"
#include <Eigen/Core>

/*
enum class Mode : uint8_t {
	Easy = 0,
	Hard
};

struct ModeEnumHash {
	size_t operator()(Mode mode) const
	{
		return std::hash<uint8_t>()(static_cast<uint8_t>(mode));
	}
};
*/


class BreakoutComponents
{
public:
	BreakoutComponents() {}

	void setup();

	//std::shared_ptr<Breakout::Select<std::string>> select = nullptr;
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

	std::shared_ptr<Breakout::EnemyHead> enemy_head = nullptr;
};
