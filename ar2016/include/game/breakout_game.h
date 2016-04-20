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
                },
        OnOnce);

        //mode 1
        mode.setMode([this]() {
            //drawList.clear();
                },
        OnOnce);

        return Game::onStart();
    }

    bool onUpdate() override
    {
        // 動的オブジェクトの位置更新
        // キャラクターの位置更新
        // 静的オブジェクトの位置更新
        // 描画
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

    void init()
    {
		fps.isShow = true;
        // 認識スレッドを回す
        m_detect_thread = std::thread(capture, std::ref(share));
    }

    void draw() {}
};
