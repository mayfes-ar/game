#pragma once

#include <thread>
#include "game/game.h"
#include "game/image_process.h"
#include "object/breakout_object.h"


class BreakoutGame : public Game
{
public:
    // コンストラクタ
    BreakoutGame() {}

    bool onStart() override 
    {
        init();
        // 描画する
        // フィールド、オブジェクト、キャラクターの順に
        drawList.push_back(std::make_shared<Breakout::Background>());

        return Game::onStart();
    }

    bool onUpdate() override
    {
        // 動的オブジェクトの位置更新
        // キャラクターの位置更新
        // 静的オブジェクトの位置更新
        // 描画
        return Game::onUpdate();
    }

    bool onFinish() override
    {
        m_detect_thread.detach();
        return true;
    }
private:
    std::thread m_detect_thread;

    void init()
    {
        // 認識スレッドを回す
        m_detect_thread = std::thread(capture, std::ref(share));
        m_detect_thread.join();
    }

    void draw() {}
};
