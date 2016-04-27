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
    explicit BreakoutGame() : m_components(){}

    // modeに登録する関数の実行回数の定義
    enum TimerKind
    {
        OnOnce = -1,
    };

    bool onStart() override 
    {
        init();
        m_components = new BreakoutComponents();
        m_components->setup();
        mode.setMode([&]() {

            drawList.push_back(std::make_shared<Breakout::Background>(
                        share.handle));

			drawList.push_back(m_components->info);
			drawList.push_back(m_components->debug);
			drawList.push_back(m_components->field);
			

            for (const auto& block: m_components->block_list) {
                drawList.push_back(block);
            }

            drawList.push_back(m_components->fireball);
            drawList.push_back(m_components->ship);
			drawList.push_back(m_components->pot);
            }, OnOnce);

        mode.setMode([this]() {
            //drawList.clear();
                }, OnOnce);

        return Game::onStart();
    }

    bool onUpdate() override
    {
		moveShip();
		moveFireBall();
		updateBlockStatus();
		updateShipStatus();
		updatePotStatus();
		
		if (key[KEY_INPUT_ESCAPE]) {
			share.willFinish = true;
		}
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
    }

    // マーカからの情報から舟を移動
	void moveShip();

	void updateShipStatus();

    // Firaballを移動
	void moveFireBall();

    // FireBallとBlockのあたり判定をし、blockを消すかを決める
	void updateBlockStatus();

	// PotとFireBallの当たり判定をし、fireballを吸い込むかを決める
	void updatePotStatus();
};
