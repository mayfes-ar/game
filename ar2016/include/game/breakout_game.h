#pragma once

#include <thread>
#include "game/game.h"
#include "game/image_process.h"
#include "object/breakout_object.h"
#include "game/breakout_components.h"


class BreakoutGame : public Game
{
public:
    // コンストラクタ
    explicit BreakoutGame() : m_components(new BreakoutComponents){}

    bool onStart() override 
    {
        init();
        mode.setMode([&]() {

            drawList.push_back(std::make_shared<Breakout::Background>(
                        share.handle));

			drawList.push_back(m_components->info);
			drawList.push_back(m_components->field);
			

            for (const auto& block: m_components->block_list) {
                drawList.push_back(block);
            }

            drawList.push_back(m_components->fireball);
            drawList.push_back(m_components->ship);
            }, -1);

		// Result画面
        mode.setMode([this]() {
            drawList.clear();
            drawList.push_back(std::make_shared<Breakout::Background>(
                        share.handle));
			m_components->result->init();
			drawList.push_back(m_components->result);
                }, -1);

        return Game::onStart();
    }

    bool onUpdate() override
    {
		moveShip();
		moveFireBall();
		updateBlockStatus();
		updateShipStatus();
		updateGameState();

        return Game::onUpdate();
    }

    bool onFinish() override
    {
        m_detect_thread.join();
        //m_detect_thread.detach();
        return true;
    }
private:
    std::thread m_detect_thread;

    BreakoutComponents* m_components = new BreakoutComponents();

    void init()
    {
		fps.isShow = true;
        // 認識スレッドを回す
        m_detect_thread = std::thread(capture, std::ref(share));

        m_components->setup();
		m_components->info->init();
    }

    // マーカからの情報から舟を移動
	void moveShip();

	void updateShipStatus();

    // Firaballを移動
	void moveFireBall();

    // FireBallとBlockのあたり判定をし、blockを消すかを決める
	void updateBlockStatus();

	// Game画面の状態更新
	void updateGameState();
};
