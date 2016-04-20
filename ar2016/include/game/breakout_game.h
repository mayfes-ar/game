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
    explicit BreakoutGame() {}

    // modeに登録する関数の実行回数の定義
    enum TimerKind
    {
        OnOnce = -1,
    };

    bool onStart() override 
    {
        init();
        // 描画する
        // フィールド、オブジェクト、キャラクターの順に
        //
        // mode 0
        mode.setMode([this]() {
            // フィールド
            drawList.push_back(std::make_shared<Breakout::Background>(
                        share.handle));
            // BLOCK
            }, OnOnce);

        //mode 1
        mode.setMode([this]() {
            //drawList.clear();
                }, OnOnce);

        return Game::onStart();
    }

    bool onUpdate() override
    {
        // 動的オブジェクトの位置更新
        moveShip();
        moveFireBall();
        // キャラクターの位置更新
        // 静的オブジェクトの位置更新
        updateBlockStatus();
        // 描画
        draw();
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

    std::unique_ptr<BreakoutComponents> m_components;

    void init()
    {
		fps.isShow = true;
        // 認識スレッドを回す
        m_detect_thread = std::thread(capture, std::ref(share));
    }

    void draw() {}

    void moveShip() {}

    void moveFireBall() {}

    void updateBlockStatus() {}
};
