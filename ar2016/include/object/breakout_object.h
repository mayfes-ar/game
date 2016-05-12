/*
 * BreakoutGameで使うオブジェクト一覧
 */

#pragma once

#include <Eigen/Core>
#include <memory>
#include <array>
#include <list>
#include <random>
#include <unordered_map>
#include <string>
#include "object/object.h"
#include "object/shape.h"
#include "moving/moving.h"
#include "math/collision_detection.h"
#include "math/math_util.h"
#include "util/util.h"
#include "util/breakout_params.h"
#include "util/timer.h"
#include "util/score.h"
#include "util/color_palette.h"
#include "moving/newton_behavior.h"
#include "moving/spring_behavior.h"
#include "moving/random_behavior.h"
#include "Dxlib.h"


namespace Breakout {

	// 描画の優先順位
	constexpr int PRIORITY_BACKGROUND = 0; // 背景画像
	constexpr int PRIORITY_SECTION = 10; // レイアウト（ゲームフィールド、スコア表示、デバッグ表示等）
	constexpr int PRIORITY_FIREBALL_REFLECT = 15; // ファイアーボールの反射の優先度
	constexpr int PRIORITY_STATIC_OBJECT = 20; // 静的オブジェクト (Block)
	constexpr int PRIORITY_DYNAMIC_OBJECT = 30; // 動的オブジェクト(Ship, Fireball)
	constexpr int PRIORITY_CHARACTER = 40; // キャラクター(マリオ)
	constexpr int PRIORITY_INFO = 50; // インフォメーション

	// 方向をあらわすenum
	enum Direction { Top, Left, Bottom, Right };

	//Fireball の Modeを決定するためのもの
	//EnemyかPlayerか、StrongかWeakかの4種類からなっている。
	//それぞれ描画時に色を変える？
	enum FireballKind {
		EnemyWeak,
		EnemyStrong,
		PlayerWeak,
		PlayerStrong
	};

	class Effect : public Object {
	public:
		Effect(std::vector<int>& effectHandle, int frames_per_scene, const int priority) {
			m_effectHandles = effectHandle;
			m_count_max = m_effectHandles.size();
			m_frames_per_scene = frames_per_scene;
			GetGraphSize(m_effectHandles[0], &m_effect_width, &m_effect_height);
			Object::layer = priority;
		}
		Effect(std::vector<int>& effectHandle, const int priority) {
			m_effectHandles = effectHandle;
			m_count_max = m_effectHandles.size();
			GetGraphSize(m_effectHandles[0], &m_effect_width, &m_effect_height);
			Object::layer = priority;
		}

		void drawWithRealm(Shape::Rectangle realm) {
			DrawExtendGraph(realm.left(), realm.top(),
				realm.right(), realm.bottom(),
				m_effectHandles[m_counter / m_frames_per_scene], TRUE);
		}

		int getCounter() {
			return m_counter;
		}

		int getCountMax() {
			return m_count_max;
		}

		void setCounter(int counter) {
			m_counter = counter;
		}

		int getFramesPerScene() {
			return m_frames_per_scene;
		}

		void incrementCounterWhenDrawWithRealm(Shape::Rectangle realm) {
			DrawExtendGraph(realm.left(), realm.top(),
				realm.right(), realm.bottom(),
				m_effectHandles[m_counter / m_frames_per_scene], TRUE);
			incrementCounter();
		}

		bool returnFalseWhenFinishDraw(Shape::Rectangle realm) {
			if (m_counter < m_count_max) {
				DrawExtendGraph(realm.left(), realm.top(),
					realm.right(), realm.bottom(),
					m_effectHandles[m_counter / m_frames_per_scene], TRUE);
				m_counter++;
				return true;
			}
			return false;
		}
		bool returnFalseWhenFinishDrawWithDirection(Shape::Rectangle realm, Direction dir) {
			float degree = M_PI / 180.0 * (int)dir * 90;
			if (m_counter < m_count_max) {
				DrawRotaGraph(realm.getCenterPoint().x(), realm.getCenterPoint().y(),
					(double)realm.width / (double)m_effect_width, degree,
					m_effectHandles[m_counter / m_frames_per_scene], TRUE, FALSE);
				m_counter++;
				return true;
			}
			return false;
		}

		bool draw() override { return true; };

		void incrementCounter() {
			m_counter++;
			if (m_counter == m_count_max * m_frames_per_scene) m_counter = 0;
		}
	private:
		std::vector<int> m_effectHandles;
		int m_count_max = 0;
		int m_counter = 0;
		int m_effect_width = 0;
		int m_effect_height = 0;
		// 1つの差分を連続で何フレーム映すか
		int m_frames_per_scene = 1;
	};

	class ItemReceiverBase;

	enum ItemKind {
		RestoreShip,
		DamageShip,
		EnhanceShip,
		RestorePot,
		RestoreTime
	};

	//アイテムクラス
	class ItemBase
	{
	public:
		virtual ~ItemBase() {}
		virtual void updateStatus() = 0;

		void attachItemReceiver(std::shared_ptr<ItemReceiverBase> item_receiver) {
			m_item_receiver = item_receiver;
			updateStatus();
		}
		void detachItemReceiver() {
			m_item_receiver = nullptr;
			updateStatus();
		}

		bool isAttached() {
			return m_item_receiver == nullptr ? false : true;
		}

		ItemKind getItemKind() {
			return m_item_kind;
		}

	protected:
		std::shared_ptr<ItemReceiverBase> m_item_receiver = nullptr;
		ItemKind m_item_kind;
	};

	//アイテムレシーバー基礎クラス
	class ItemReceiverBase
	{
	public:
		ItemReceiverBase() {}
		ItemReceiverBase(const Shape::Rectangle& realm) : m_realm(realm) {}
		virtual ~ItemReceiverBase() {}
		//本当は純粋仮想にしたかったけどそうするとこのクラスのインスタンスthisがつくれないからこのようにした
		// applyItems は毎ループ呼ぶためにdraw()の中に入れるようにする。
		virtual void applyItems() {}
		virtual const Shape::Rectangle getRealm() { return m_realm; }

		void detachItem(std::shared_ptr<ItemBase> item) {
			item->detachItemReceiver();
			deleteItem(item);
		}

		void detachAllItems() {
			for (auto item : m_items) {
				item->detachItemReceiver();
			}
			m_items.clear();
		}

		void attachItem(std::shared_ptr<ItemBase> item) {
			m_items.push_back(item);
			item->attachItemReceiver(std::make_shared<ItemReceiverBase>(*this));
			//applyItems();
		}

		//使用済みのアイテムを消す
		void deleteItem(std::shared_ptr<ItemBase> item) {
			for (int i = 0; i < m_items.size(); i++) {
				if (m_items[i] != item) continue;
				m_items.erase(m_items.begin() + i);
			}
		}

	protected:
		std::vector<std::shared_ptr<ItemBase>> m_items = {};
		Shape::Rectangle m_realm = Shape::Rectangle();
	};

	class Item : public Object, public ItemBase
	{
	public:
		virtual ~Item() {}
		Item(ItemKind item_kind) {
			m_item_kind = item_kind;
			Object::layer = PRIORITY_DYNAMIC_OBJECT;
		}

		Shape::Rectangle getRealm() const {
			return m_realm;
		}

		void updateStatus() override {
			if (m_item_receiver != nullptr) {
				m_realm.start_point = m_item_receiver->getRealm().start_point;// + Eigen::Vector2i((m_item_receiver->getRealm().width - m_realm.width) / 2, 0);
			}
			else {
				m_realm.start_point += Eigen::Vector2i(0, 10);
			}
		}

		bool draw() override {
			//誰にかに保持されていたら描画しない
			if (m_item_receiver != nullptr) return true;

			switch (m_item_kind) {
			case RestoreShip:
				DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_restore_ship"], TRUE);
				break;
			case DamageShip:
				DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_damage_ship"], TRUE);
				break;
			case EnhanceShip:
				DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_enhance_ship"], TRUE);
				break;
			case RestorePot:
				DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_pot"], TRUE);
				break;
			case RestoreTime:
				DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_restore_time"], TRUE);
				break;
			}

			//DrawBox(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), GetColor(255, 255, 0), TRUE);
			return true;
		}

	private:
		Shape::Rectangle m_realm = Shape::Rectangle(Eigen::Vector2i(0, 0), BLOCK_WIDTH / 2, BLOCK_HEIGHT);
	};



	// 縦方向に選択ボタンを配置するセレクト画面。
	// 一方向のみ対応

	enum class Move : uint8_t {
		Up,
		Down,
		None
	};

	template <typename Mode, typename Hash = std::hash<Mode> >
	class Select : public Object
	{
	public:
		using modeToImageKey = std::unordered_map<Mode, std::string, Hash>;
		Select(const std::list<Mode>& mode_list,
			const modeToImageKey& mode_to_key,
			const Shape::Rectangle& realm)
			:m_mode_list(mode_list), m_mode_to_key(mode_to_key), m_realm(realm)
		{
			Object::layer = PRIORITY_SECTION;
			m_now_mode = m_mode_list.begin();
		}


		bool draw() override {
			int image_num = static_cast<int>(m_mode_list.size());
			int image_width = m_realm.width / 2;
			int image_height = m_realm.height / (4 * image_num);

			int i = 0;
			for (auto it = m_mode_list.begin(); it != m_mode_list.end(); ++it) {
				DrawExtendGraph(m_realm.left(), m_realm.top() + image_height * (2 * i + 1),
					m_realm.left() + image_width, m_realm.top() + image_height * (2 * i + 2),
					Object::imgHandles.at(m_mode_to_key.at(*it)), TRUE);
				// 現在のモードの枠線を白色で囲む
				if (it == m_now_mode) {
					DrawBox(m_realm.left(), m_realm.top() + image_height * (2 * i + 1),
						m_realm.left() + image_width, m_realm.top() + image_height * (2 * i + 2),
						Color::WHITE, FALSE);
				}
				i += 1;

				DrawString(m_realm.left(), m_realm.bottom(), "Press Enter to select", Color::BLUE);
			}
			return true;
		}


		void move(const Move& move)
		{
			m_move = move;
			switch (move) {
			case Move::None:
				break;
			case Move::Up:
				if (m_mode_list.size() <= 1) {
					return;
				}
				if (m_now_mode == --m_mode_list.end()) {
					return;
				}
				m_now_mode++;
				break;

			case Move::Down:
				if (m_mode_list.size() <= 1) {
					return;
				}
				if (m_now_mode == m_mode_list.begin()) {
					return;
				}
				m_now_mode--;
				break;
			}
		}

		void select() {
			m_selected = true;
		}

		bool selected() const {
			return m_selected;
		}

		Mode getMode() const {
			return *m_now_mode;
		}

	private:
		Move m_move = Move::None;
		typename std::list<Mode>::iterator m_now_mode;
		std::list<Mode> m_mode_list;
		modeToImageKey m_mode_to_key;
		Shape::Rectangle m_realm = Shape::Rectangle();
		bool m_selected = false;
	};


// 背景画像
// 溶岩の絵がほしい
class Background : public Object
{
public:
	Background()
	{
		Object::layer = PRIORITY_BACKGROUND;
	}

	void init() {
		m_is_last_phase = false;
		m_forest_saturation = 50;
		m_magma_saturation = 50;
	}

	bool draw() override {
		if (m_is_last_phase) {
			SetDrawBright(m_forest_saturation, m_forest_saturation, m_forest_saturation);
			if (m_forest_saturation < m_saturation_max) {
				m_forest_saturation++;
			}

			DrawExtendGraph(0, 0, WIDTH, HEIGHT,
				imgHandles["b_magma"], true);
			SetDrawBright(255, 255, 255);
			return true;
		}
		else {
			if (m_magma_saturation < m_saturation_max) {
				m_magma_saturation++;
			}
			SetDrawBright(m_magma_saturation, m_magma_saturation, m_magma_saturation);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT,
				imgHandles["b_hill"], true);
			SetDrawBright(255, 255, 255);
			return true;
		}
	}

	void turnLastPhase() {
		m_is_last_phase = true;
	}

private:
	int m_forest_saturation = 50;
	int m_magma_saturation = 50;
	bool m_is_last_phase = false;
	const int m_saturation_max = 100;
};

class Explanation : public Object
{
public:
	explicit Explanation(const Shape::Rectangle& realm)
		: m_realm(realm)
	{}

	bool draw() override {
		DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(),
			imgHandles["b_explanation"], TRUE);
		return true;
	}

private:
	Shape::Rectangle m_realm = Shape::Rectangle();
};

// 時間やスコアを表示する
class Info : public Object
{
public:
	explicit Info(const Shape::Rectangle realm, const std::shared_ptr<Timer>& timer)
		: m_realm(realm), m_timer(timer)
	{
		Object::layer = PRIORITY_INFO;
		m_score = std::make_shared<Score>();
	}

	void init() {
		m_timer->start();
		m_score->reset();
	}

	void resetTimer(int max_time) {
		m_timer->resetMaxTime(max_time);
		m_timer->reset();
	}

	void restoreTimer(int minute, int second) {
		m_timer->increaseStartTime(minute, second);
	}

	bool draw() {
		SetDrawBright(100, 100, 100);
		DrawBox(m_realm.left(), m_realm.top(),
			m_realm.right(), m_realm.bottom(),
			GetColor(50, 50, 50), TRUE);
		SetDrawBright(255, 255, 255);
		// Timer の描画
		const auto time = m_timer->getLeftedTime();
		std::array<int, 6> time_num;
		const int minute = std::get<0>(time);
		const int sec = std::get<1>(time);
		const int msec = std::get<2>(time);
		if (minute < 10) {
			time_num.at(0) = 0;
			time_num.at(1) = minute;
		}
		else {
			time_num.at(0) = minute / 10;
			time_num.at(1) = minute - (minute / 10) * 10;
		}

		if (sec < 10) {
			time_num.at(2) = 0;
			time_num.at(3) = sec;
		}
		else {
			time_num.at(2) = sec / 10;
			time_num.at(3) = sec - (sec / 10) * 10;
		}
		if (msec < 100) {
			time_num.at(4) = 0;
			//time_num.at(5) = msec / 10;
		}
		else {
			time_num.at(4) = msec / 100;
			//time_num.at(5) = (msec - (msec / 100) * 100) / 10;
		}

		const int num_width = (m_realm.right() - m_realm.left()) / 8;
		const int num_height = (m_realm.bottom() - m_realm.top()) / 8;
		for (int i = 0; i < 5; ++i) {
			std::ostringstream key;
			if (i < 2) {
				key << "red_" << time_num.at(i);

			}
			else if (i < 4) {
				key << "blue_" << time_num.at(i);
			}
			else {
				key << "green_" << time_num.at(i);
			}

			DrawExtendGraph((i + 1) * num_width, num_height, (i + 2)* num_width, 2 * num_height,
				imgHandles[key.str()], TRUE);
		}

		auto score = m_score->getPoint();
		std::array<int, 4> score_num;
		score_num.at(0) = score / 1000;
		score -= (score / 1000) * 1000;
		score_num.at(1) = score / 100;
		score -= (score / 100) * 100;
		score_num.at(2) = score / 10;
		score -= (score / 10) * 10;
		score_num.at(3) = score;

		const int score_width = m_realm.width / 6;
		const int score_height = m_realm.height / 12;
		for (int i = 0; i < 4; ++i) {
			std::ostringstream key;
			key << "yellow_" << score_num.at(i);
			DrawExtendGraph((i + 1) * score_width, 2 * num_height + 50/*きもち*/, (i + 2)* score_width, 2 * num_height + 50 + score_width,
				imgHandles[key.str()], TRUE);
		}
		// Score の描画
		return true;
	}

	bool isTimeOver() const {
		return m_timer->isTimerEnd();
	}

	bool isLastPhase() {
		float ratio = m_timer->getRatio();
		return ratio < 0.3f;
	}

	// Scoreをいじるときはこの関数から直接scoreたたいてください
	void addScore(int score) {
		m_score->addPoint(score);
	}

private:
	Shape::Rectangle m_realm = Shape::Rectangle();
	std::shared_ptr<Timer> m_timer = nullptr;
	std::shared_ptr<Score> m_score = nullptr;
};

class Result : public Object
{
public:
	explicit Result(const Shape::Rectangle& m_realm)
		: m_realm(m_realm) {
		Object::layer = PRIORITY_INFO;
	}

	void init()
	{
		m_cnt = 40;
	}


	bool draw() override {
		if (m_cnt >= 255) {
			m_cnt = 255;
		}
		SetDrawBright(m_cnt, m_cnt, m_cnt);
		m_cnt += 2;

		if (m_is_game_clear) {
			DrawExtendGraph(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				imgHandles["b_game_clear"], TRUE);
			return true;
		}

		DrawExtendGraph(m_realm.left(), m_realm.top(),
			m_realm.right(), m_realm.bottom(),
			imgHandles["b_game_over"], TRUE);

		return true;
	}

	// ゲームをクリアしたかどうか
	void clearGame() {
		m_is_game_clear = true;
	}

private:
	Shape::Rectangle m_realm = Shape::Rectangle();
	bool m_is_game_clear = false;
	int m_cnt = 0;
};

// フィールド 
class Field : public Object
{
public:
	explicit Field(const Shape::Rectangle& realm)
		: m_realm(realm)
	{
		Object::layer = PRIORITY_SECTION;
	}

	bool draw() override {
		SetDrawBright(100, 100, 100);
		DrawBox(m_realm.left(), m_realm.top(),
			m_realm.right(), m_realm.bottom(),
			Color::WHITE, false);
		SetDrawBright(255, 255, 255);
		return true;
	}

	Shape::Rectangle getRealm() const {
		return m_realm;
	}

	// フィールド外にいるかどうか
	bool isOutOfField(const Shape::Rectangle& rect);
	bool isOutOfField(const Shape::Circle& cir);

private:
	Shape::Rectangle m_realm = Shape::Rectangle();
};

// Block崩しに使われるBlock
// Firebollにぶつかると消える
// 初期位置に固定という意味で静的オブジェクト
class Block : public Object, public ItemReceiverBase
{
public:
	enum class Color : uint8_t {
		Green,
		Blue,
		Red
	};

	explicit Block(const Shape::Rectangle& realm, Color color)
		: m_color(color), ItemReceiverBase(realm)
	{
		Object::layer = PRIORITY_STATIC_OBJECT;
	}

	virtual bool draw() override {
		if (m_is_disappeared) {
			// 何も描画しない
			return true;
		}

		switch (m_color) {
		case Color::Green:
			DrawExtendGraph(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				imgHandles["b_block_green"], TRUE);
			break;
		case Color::Blue:
			DrawExtendGraph(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				imgHandles["b_block_blue"], TRUE);
			break;
		case Color::Red:
			DrawExtendGraph(m_realm.left(), m_realm.top(),
				m_realm.right(), m_realm.bottom(),
				imgHandles["b_block_red"], TRUE);
			break;
		}

		return true;
	}

	const Shape::Rectangle getRealm() override {
		return m_realm;
	}

	Color getColor() const {
		return m_color;
	}

	void applyItems() override {
		return;
	}

	bool isDisappeared() const {
		return m_is_disappeared;
	}

	// Blockの消滅
	void disappear()
	{
		m_is_disappeared = true;
	}

	void appear()
	{
		m_is_disappeared = false;
	}

	bool damageBlock(int amount) {
		if (m_life.damage(amount)) {
			if (!isAlive()) {
				disappear();
				detachAllItems();
			}
			return true;
		}
		else {
			return false;
		}
	}

	void resetBlock() {
		m_life.resetLife();
		appear();
	}

	bool isAlive() {
		return m_life.isAlive();
	}

	virtual std::string getBlockKind() = 0;
protected:
	Life m_life = Life(1, 1);
private:
	bool m_is_disappeared = false; // 火の玉にあったかどうか
	Color m_color = Color::Green;
};

class NormalBlock : public Block {
public:
	explicit NormalBlock(const Shape::Rectangle& realm, Color color)
		: Block(realm, color)
	{
		Object::layer = PRIORITY_STATIC_OBJECT;
		m_life = Life(NORMAL_BLOCK_LIFE_NUM, NORMAL_BLOCK_LIFE_NUM);
	}
	bool draw() override {
		if (isDisappeared()) return true;
		Block::draw();
		DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_hatena_block"], TRUE);
		return true;
	}
	std::string getBlockKind() override {
		return "NormalBlock";
	}
};

class HardBlock : public Block {
public:
	explicit HardBlock(const Shape::Rectangle& realm, Color color)
		: Block(realm, color)
	{
		Object::layer = PRIORITY_STATIC_OBJECT;
		m_life = Life(HARD_BLOCK_LIFE_NUM, HARD_BLOCK_LIFE_NUM);
	}
	bool draw() override {
		if (isDisappeared()) return true;
		Block::draw();
		DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_hard_block"], TRUE);
		return true;
	}
	std::string getBlockKind() override {
		return "HardBlock";
	}
};

class UnbreakableBlock : public Block {
public:
	explicit UnbreakableBlock(const Shape::Rectangle& realm, Color color)
		: Block(realm, color)
	{
		Object::layer = PRIORITY_STATIC_OBJECT;
		m_life = Life(UNBREAKABLE_BLOCK_LIFE_NUM, UNBREAKABLE_BLOCK_LIFE_NUM);
	}
	bool draw() override {
		if (isDisappeared()) return true;
		Block::draw();
		DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), imgHandles["b_unbreakable_block"], TRUE);
		return true;
	}
	std::string getBlockKind() override {
		return "UnbreakableBlock";
	}
};

class FireballReflect : public Object
{
public:
	FireballReflect() { 
		Object::layer = PRIORITY_FIREBALL_REFLECT;
	}
	FireballReflect(Eigen::Vector2i& hit_point, Direction dir, FireballKind kind) {
		is_collide = true;
		realm.width = width;
		realm.height = height;
		switch (dir) {
		case Top:
			realm.start_point = hit_point + Eigen::Vector2i( -width / 2, -height / 2);
			break;
		case Left:
			realm.start_point = hit_point + Eigen::Vector2i( -width / 2, -height / 2);
			break;
		case Bottom:
			realm.start_point = hit_point;// +Eigen::Vector2i(-width / 2, 0);
		case Right:
			realm.start_point = hit_point + Eigen::Vector2i(-width / 2, - height / 2);
		}
		switch (kind) {
		case PlayerStrong:
		case EnemyStrong:
			setReflectEffect("b_strong_fireball_reflect");
			break;
		case EnemyWeak:
			break;
		case PlayerWeak:
			setReflectEffect("b_green_fireball_reflect");
			break;
		}
		Object::layer = PRIORITY_FIREBALL_REFLECT;
	}

	bool draw() override {
		return reflect_effect.returnFalseWhenFinishDrawWithDirection(realm, direction);
	}

	void setReflectEffect(std::string effect_name) {
		reflect_effect = Effect(effectHandles[effect_name], PRIORITY_FIREBALL_REFLECT);
	}

	bool isCollide() {
		return is_collide;
	}

private:
	bool is_collide = false;
	Direction direction = Top;
	Shape::Rectangle realm = Shape::Rectangle();
	int width = 40;
	int height = 40;
	Effect reflect_effect = Effect(effectHandles["b_fireball_reflect"], PRIORITY_FIREBALL_REFLECT);
};

// Block崩しに使われるBlock
// Firebollにぶつかると消える
// 初期位置に固定という意味で静的オブジェクト
/*
*/
class Fireball : public Object
{
public:
	Fireball() {}

	Fireball(const Shape::Circle& realm, const std::shared_ptr<Moving>& moving)
		: m_realm(realm), m_moving(moving)
	{
		m_bounding_box = Shape::Rectangle(m_realm.center - Eigen::Vector2i(m_realm.radius, m_realm.radius), m_realm.radius * 2, m_realm.radius * 2);
		Object::layer = PRIORITY_DYNAMIC_OBJECT;
	}
	Fireball(const Shape::Circle& realm, const std::shared_ptr<Moving>& moving, FireballKind mode)
		: m_realm(realm), m_moving(moving), m_mode(mode)
	{
		m_bounding_box = Shape::Rectangle(m_realm.center - Eigen::Vector2i(m_realm.radius, m_realm.radius), m_realm.radius * 2, m_realm.radius * 2);
		Object::layer = PRIORITY_DYNAMIC_OBJECT;
	}

	bool draw() {
		if (m_is_disappeared) {
			// 何も描画しない
			return true;
		}
		int x = m_realm.center.x();
		int y = m_realm.center.y();
		int r = m_realm.radius;

		switch (m_mode) {
		case EnemyWeak:
			if (m_fireball_effect.getCounter() < 11 * m_fireball_effect.getFramesPerScene()) {
				m_fireball_effect.incrementCounterWhenDrawWithRealm(m_bounding_box);
			} else {
				m_fireball_effect.incrementCounterWhenDrawWithRealm(m_bounding_box);
				if (m_fireball_effect.getCounter() == 25 * m_fireball_effect.getFramesPerScene() - 1) {
					m_fireball_effect.setCounter(m_fireball_effect.getFramesPerScene() * 11);
				}
			}
			
			break;
		case EnemyStrong:
			m_enemy_strong_fireball_effect.incrementCounterWhenDrawWithRealm(m_bounding_box);
			break;
		case PlayerWeak:
			m_green_fireball_effect.incrementCounterWhenDrawWithRealm(m_bounding_box);
			break;
		case PlayerStrong:
			DrawCircle(x, y, r, Color::GREEN, true);
			break;
		}

		return true;
	}

	void updatePosition() {
		switch (m_mode) {
		case EnemyStrong:
			// 十分大きくなるまではぶるぶる動かす
			if (m_realm.radius < 40) {
				std::shared_ptr<MovingBehavior> rnd_behavior = std::make_shared<RandomBehavior>(m_bounding_box.left(), m_bounding_box.right(), m_bounding_box.top(), m_bounding_box.bottom());
				m_moving->setBehavior(rnd_behavior);
				increaseRadius(1);
			}
			// 十分大きくなったら下に向けて発射
			else {
				std::shared_ptr<MovingBehavior> nwt_behavior = std::make_shared<NewtonBehavior>();
				m_moving->setBehavior(nwt_behavior);
				m_moving->setVelocity(Eigen::Vector2f(0, 5.0f));
			}
		}
		m_moving->updatePoistion(m_realm.center);
		//fireball描画に使うため
		m_bounding_box = Shape::Rectangle(m_realm.center - Eigen::Vector2i(m_realm.radius, m_realm.radius), m_realm.radius * 2, m_realm.radius * 2);
	}

	void increaseRadius(int amount) {
		m_realm.radius += amount;
	}

	void setRadius(int radius) {
		m_realm.radius = radius;
	}

	void setPosition(const Eigen::Vector2i& pos) {
		m_realm.center = pos;
	}

	void setVelocity(const Eigen::Vector2f& velocity) {
		m_moving->setVelocity(velocity);
	}

	Eigen::Vector2f getVelocity() {
		return m_moving->getVelocity();
	}

	bool isDisappeared() const {
		return m_is_disappeared;
	}

	void appear(const Shape::Circle& start_realm) {
		m_realm = start_realm;
		m_is_disappeared = false;
	}

	// Fireballの消滅
	void disappear()
	{
		m_is_disappeared = true;
	}

	void appear()
	{
		m_is_disappeared = false;
	}

	Shape::Circle getRealm() const
	{
		return m_realm;
	}
	// if in then -1, out then 1, default is 1
	bool isCollided(const Shape::Rectangle& parent, const int inOrOut = 1, const int parentVelocity = 0);
	std::shared_ptr<FireballReflect> returnFireballReflect(const Shape::Rectangle& parent, const int inOrOut = 1, const int parentVelocity = 0);

	FireballKind getMode() {
		return m_mode;
	}

	void setMode(FireballKind mode) {
		m_mode = mode;
	}

	void changeModeToPlayer() {
		switch (m_mode) {
		case EnemyWeak:
			m_mode = PlayerWeak;
			break;
		case EnemyStrong:
			m_mode = PlayerStrong;
			break;
		}
	}

	bool isEnemy() {
		switch (m_mode) {
		case EnemyWeak:
		case EnemyStrong:
			return true;
		}
		return false;
	}

	bool isStrong() {
		switch (m_mode) {
		case EnemyStrong:
		case PlayerStrong:
			return true;
		}
		return false;
	}

	int giveDamage() {
		switch (m_mode) {
		case PlayerStrong:
		case EnemyStrong:
			return 3;
		case PlayerWeak:
		case EnemyWeak:
			return 1;
		}
	}


private:
	bool m_is_disappeared = false; // 火の玉にあったかどうか
	Shape::Circle m_realm = Shape::Circle(); // Firebollの領域
	Shape::Rectangle m_bounding_box = Shape::Rectangle();
	std::shared_ptr<Moving> m_moving = nullptr;
	FireballKind m_mode = EnemyWeak;
	Effect m_fireball_effect = Effect(effectHandles["b_fireball"], 3, PRIORITY_DYNAMIC_OBJECT);
	Effect m_green_fireball_effect = Effect(effectHandles["b_green_fireball"], PRIORITY_DYNAMIC_OBJECT);
	Effect m_enemy_strong_fireball_effect = Effect(effectHandles["b_enemy_strong_fireball"], PRIORITY_DYNAMIC_OBJECT);
	Effect m_player_strong_fireball_effect = Effect(effectHandles["b_player_strong_fireball"], PRIORITY_DYNAMIC_OBJECT);
};

class FireballManager : public Object {
public:
	FireballManager(int max_num) : m_max_num(max_num) {
		Object::layer = PRIORITY_DYNAMIC_OBJECT;
	}

	bool isMax() {
		return m_fireball_list.size() == m_max_num;
	}

	bool add(std::shared_ptr<Fireball>& fireball) {
		if (m_fireball_list.size() == m_max_num) return false;
		m_fireball_list.push_back(fireball);
		return true;
	}
	// index を渡して消す
	bool destroy(int index) {
		if (m_fireball_list.size() <= index + 1) return false;
		m_fireball_list.erase(m_fireball_list.begin() + index);
		return true;
	}

	bool destroy(std::shared_ptr<Fireball>& fireball) {
		for (int i = 0; i < m_fireball_list.size(); i++) {
			if (m_fireball_list[i] != fireball) continue;
			m_fireball_list.erase(m_fireball_list.begin() + i);
			return true;
		}
		return false;
	}

	std::vector<std::shared_ptr<Fireball>> getFireballList() {
		return m_fireball_list;
	}

	bool draw() override {
		bool isSuccess = true;
		for (auto& fireball : m_fireball_list) {
			isSuccess &= fireball->draw();
		}
		return isSuccess;
	}

private:
	std::vector<std::shared_ptr<Fireball>> m_fireball_list = {};
	int m_max_num = 0;
};


class Pot : public Object
{
public:
	explicit Pot(const Shape::Rectangle& realm)
		: m_realm(realm)
	{
		Object::layer = PRIORITY_DYNAMIC_OBJECT;
	}

	bool translate(const Eigen::Vector2i& translation) {
		//範囲外なら移動させない
		//realmの更新
		if (m_realm.right() + translation.x() > FIELD_START_POS.x() + FIELD_WIDTH ||
			m_realm.left() + translation.x() < FIELD_START_POS.x() ||
			m_realm.top() + translation.y() < FIELD_START_POS.y() ||
			m_realm.bottom() + translation.y() > FIELD_START_POS.y() + FIELD_HEIGHT) return false;

		m_realm.start_point += translation;
		updateFireballPosition();
		return true;
	}

	bool rotate(float rotation) {
		m_rotation += rotation;
		return true;
	}

	// マーカーを認識したらis_disappearedはonにする。吸い込んでから3病後に発射。このときポットにカウントダウンを表示する。0になったとき発射不可能な場所にいた場合は発射失敗。

	// fireballを吸い込む（特定のfireballのほうがいいかも？）
	// StrongかWeakかを見たあとに、Player~にする
	void inhareFireball(std::shared_ptr<Fireball>& fireball) {
		m_fireball = fireball;
		m_fireball->disappear();
		m_fireball->setPosition(m_realm.getLeftTopPoint());
		m_initial_fireball_speed = m_fireball->getVelocity().norm();
		m_fireball->setVelocity(Eigen::Vector2f::Zero());
		m_fireball->changeModeToPlayer();
	}

	// fireballを吐き出す。吸い込んでから3秒後。
	void exhareFireball() {
		m_fireball->setVelocity(Eigen::Vector2f(-m_initial_fireball_speed * (float)cos(m_rotation + M_PI / 2.0), -m_initial_fireball_speed * (float)sin(m_rotation + M_PI / 2.0)));
		m_fireball->appear();
		m_fireball = nullptr;
		m_count = -1;
	}

	Shape::Rectangle getRealm() const {
		return m_realm;
	}

	float getRotation() const {
		return m_rotation;
	}

	bool hasFireball() {
		return m_fireball == nullptr ? false : true;
	}

	void appear() {
		m_is_disappered = false;
	}

	void disappear() {
		m_is_disappered = true;
	}

	bool isDisappeared() {
		return m_is_disappered;
	}

	bool isAvailable() {
		return !isExhared();
	}

	// 吐き出したかどうか（countをみる)
	bool isExhared() {
		return m_count >= 0 ? false : true;
	}

	bool isInhared() {
		return m_fireball == nullptr ? false : true;
	}

	void countDown() {
		m_count--;
	}

	std::shared_ptr<Fireball> getInharedFireball() {
		return m_fireball;
	}

	void resetStatus() {
		m_is_disappered = true;
		m_rotation = 0;
		m_fireball = nullptr;
		m_initial_fireball_speed = 0;
		m_count = 90;
	}

	bool draw() override {
		if (m_is_disappered || !isAvailable()) {
			// 何も描画しない
			return true;
		}

		auto rotatedTopLine = getRealm().getTopLine().getRotatedLine(m_realm.getCenterPoint(), getRotation());
		auto rotatedLeftLine = getRealm().getLeftLine().getRotatedLine(getRealm().getCenterPoint(), getRotation());
		auto rotatedRightLine = getRealm().getRightLine().getRotatedLine(getRealm().getCenterPoint(), getRotation());
		auto rotatedBottomLine = getRealm().getBottomLine().getRotatedLine(getRealm().getCenterPoint(), getRotation());


		DrawLine(rotatedTopLine.point.x(), rotatedTopLine.point.y(), (rotatedTopLine.point + rotatedTopLine.dir).x(), (rotatedTopLine.point + rotatedTopLine.dir).y(), GetColor(0, 255, 0), 3);
		DrawLine(rotatedLeftLine.point.x(), rotatedLeftLine.point.y(), (rotatedLeftLine.point + rotatedLeftLine.dir).x(), (rotatedLeftLine.point + rotatedLeftLine.dir).y(), GetColor(200, 255, 0), 3);
		DrawLine(rotatedRightLine.point.x(), rotatedRightLine.point.y(), (rotatedRightLine.point + rotatedRightLine.dir).x(), (rotatedRightLine.point + rotatedRightLine.dir).y(), GetColor(0, 255, 200), 3);
		DrawLine(rotatedBottomLine.point.x(), rotatedBottomLine.point.y(), (rotatedBottomLine.point + rotatedBottomLine.dir).x(), (rotatedBottomLine.point + rotatedBottomLine.dir).y(), GetColor(200, 0, 200), 3);

		DrawRotaFormatString(m_realm.left(), m_realm.top(),
			1.0, 1.0,
			m_realm.width / 2, m_realm.height / 2,
			m_rotation, GetColor(255, 0, 0), 0, FALSE, "%d", m_count);

		DrawRotaGraph(m_realm.getCenterPoint().x(), m_realm.getCenterPoint().y(),
			(double)m_realm.width / 150.0, m_rotation,
			imgHandles["b_pot"], TRUE, FALSE);


		DrawBox(m_realm.left(), m_realm.top(),
			m_realm.right(), m_realm.bottom(),
			GetColor(255, 0, 0), false);

		return true;
	}

private:
	bool m_is_disappered = true;
	float m_rotation = 0;
	std::shared_ptr<Fireball> m_fireball = nullptr;
	Shape::Rectangle m_realm = Shape::Rectangle();
	float m_initial_fireball_speed = 0;
	// 吸い込んでからどのくらいで打ち出すか(基本30frames/secだから3秒くらい)
	int m_count = 90;

	void updateFireballPosition() {
		if (hasFireball()) {
			m_fireball->setPosition(m_realm.getLeftTopPoint());
		}
	}
};

// キャラクタがのる船
// ライフをもつ
class Ship : public Object, public ItemReceiverBase
{
public:
	explicit Ship(const Eigen::Vector2i& start_point, const Life& life, std::shared_ptr<Pot>& pot, std::shared_ptr<Info>& info)
		: m_start_point(start_point), m_life(life), m_pot(pot), m_info(info)
	{
		Object::layer = PRIORITY_DYNAMIC_OBJECT;
		for (int i = 0; i < m_life.getLifeNum() * 2; i++) {
			m_blocks.push_back(Shape::Rectangle(start_point + Eigen::Vector2i{ SHIP_BLOCK_WIDTH, 0 } *i, SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT));
		}
	}

	//! @brief 舟を動かす
	bool translate(int translation) {
		//範囲外なら移動させない
		//start_pointの更新
		m_start_point = m_blocks[0].getLeftTopPoint();
		if (m_blocks[m_blocks.size() - 1].right() + translation > FIELD_START_POS.x() + FIELD_WIDTH ||
			m_blocks[0].left() + translation < FIELD_START_POS.x()) return false;

		for (auto &block : m_blocks) {
			block.start_point.x() += translation;
		}
		// m_velocityのセット
		m_velocity = translation;
		return true;
	}

	bool restoreShip(int amount) {
		if (!m_life.restore(amount)) return false;
		if (m_blocks.empty()) {
			m_blocks.push_back(Shape::Rectangle(m_start_point, SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT));
			m_blocks.push_back(Shape::Rectangle(m_start_point + Eigen::Vector2i(SHIP_BLOCK_WIDTH, 0), SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT));
			return true;
		}
		for (int i = 0; i < amount; i++) {
			//もし右にぶつかっていたら
			if (m_blocks[m_blocks.size() - 1].right() + SHIP_BLOCK_WIDTH > FIELD_START_POS.x() + FIELD_WIDTH) {
				m_blocks.insert(m_blocks.begin(), Shape::Rectangle(m_blocks[0].getLeftTopPoint() - Eigen::Vector2i(SHIP_BLOCK_WIDTH, 0), SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT));
				m_blocks.insert(m_blocks.begin(), Shape::Rectangle(m_blocks[0].getLeftTopPoint() - Eigen::Vector2i(2 * SHIP_BLOCK_WIDTH, 0), SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT));
			}
			//もし左にぶつかっていたら
			else if (m_blocks[0].left() - SHIP_BLOCK_WIDTH < FIELD_START_POS.x()) {
				m_blocks.push_back(Shape::Rectangle(m_blocks[m_blocks.size() - 1].getRightTopPoint(), SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT));
				m_blocks.push_back(Shape::Rectangle(m_blocks[m_blocks.size() - 1].getRightTopPoint() + Eigen::Vector2i(SHIP_BLOCK_WIDTH, 0), SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT));
			}
			//どちらでもなかったら
			else {
				m_blocks.push_back(Shape::Rectangle(m_blocks[m_blocks.size() - 1].getRightTopPoint(), SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT));
				m_blocks.insert(m_blocks.begin(), Shape::Rectangle(m_blocks[0].getLeftTopPoint() - Eigen::Vector2i(SHIP_BLOCK_WIDTH, 0), SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT));
			}
		}
		m_start_point = m_blocks[0].getLeftTopPoint();
		return true;
	}

	bool damageShip(int amount) {
		if (!m_life.damage(amount)) return false;
		for (int i = 0; i < amount; i++) {
			//両側から一個ずつへらす
			m_blocks.pop_back();
			m_blocks.erase(m_blocks.begin());
		}
		return true;
	}

	void resetShip() {
		m_blocks.clear();
		m_life.resetLife();
		for (int i = 0; i < m_life.getLifeNum(); i++) {
			m_blocks.push_back(Shape::Rectangle(m_start_point + Eigen::Vector2i{ SHIP_BLOCK_WIDTH, 0 } *i, SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT));
		}
	}

	int getLifeNum() {
		return m_life.getLifeNum();
	}

	bool isAlive() const {
		return m_life.isAlive();
	}

	int left() {
		return m_start_point[0];
	}
	int right() {
		return m_start_point[0] + m_blocks.size() * BLOCK_WIDTH;
	}
	int top() {
		return m_start_point[1];
	}
	int bottom() {
		return m_start_point[1] + BLOCK_HEIGHT;
	}
	int getVelocity() {
		return m_velocity;
	}

	// Blockの消滅
	void disapper()
	{
		m_is_disappered = true;
	}

	const Shape::Rectangle getRealm() override {
		return Shape::Rectangle(m_start_point, m_blocks.size() * SHIP_BLOCK_WIDTH, SHIP_BLOCK_HEIGHT);
	}

	bool isEnhanced() {
		return m_is_enhanced;
	}

	void applyItems() override {
		for (int i = 0; i < m_items.size(); i++) {

			switch (m_items[i]->getItemKind()) {
				//継続的な効果を持たないものは適用したら消す
			case RestoreShip:
				restoreShip(1);
				deleteItem(m_items[i]);
				//消したのでその分indexを減らす
				i--;
				break;
			case DamageShip:
				damageShip(1);
				deleteItem(m_items[i]);
				i--;
				break;
			case EnhanceShip:
				if (!m_is_enhanced) m_is_enhanced = true;
				else {
					m_enhanced_count--;
				}

				// 強化時間を越えたら
				if (m_enhanced_count < 0) {
					m_is_enhanced = false;
					m_enhanced_count = 150;
					deleteItem(m_items[i]);
					i--;
				}
				break;
			case RestorePot:
				// available じゃなかったら適用せず保持しておく。
				if (!m_pot->isAvailable()) {
					m_pot->resetStatus();
					deleteItem(m_items[i]);
					i--;
				}
				break;
			case RestoreTime:
				m_info->restoreTimer(0, 10);
				deleteItem(m_items[i]);
				i--;
			default:
				break;
			}
		}
	}

	bool draw() override {
		// drawは毎ループ呼ばれるからここに書く
		applyItems();

		if (m_is_disappered) {
			// 何も描画しない
			return true;
		}

		if (isEnhanced()) {
			for (auto block : m_blocks) {
				DrawExtendGraph(block.left(), block.top(),
					block.right(), block.bottom(),
					imgHandles["b_ship_block"], TRUE);
				m_muteki_effect.drawWithRealm(block);
			}
		}
		else {
			for (auto block : m_blocks) {
				DrawExtendGraph(block.left(), block.top(),
					block.right(), block.bottom(),
					imgHandles["b_ship_block"], TRUE);
			}
		}

		if (isEnhanced()) {
			m_muteki_effect.incrementCounter();
		}
		return true;
	}

private:
	bool m_is_disappered = false;
	std::vector<Shape::Rectangle> m_blocks = {};
	Eigen::Vector2i m_start_point = Eigen::Vector2i::Zero();
	int m_velocity = 0; // マーカーを認識しているときにtranslateで渡されるもの。実際の速度的な概念とは違う。
	Life m_life = Life(); // defaultは４つ。火の玉に当たるごとに一つ減る
	bool m_is_enhanced = false;
	int m_enhanced_count = 150;
	Effect m_muteki_effect = Effect(effectHandles["b_muteki"], PRIORITY_DYNAMIC_OBJECT);
	// item を pot に適用させるため
	std::shared_ptr<Pot> m_pot = nullptr;
	// item を　info に適用させるため
	std::shared_ptr<Info> m_info = nullptr;
};


class Town : public Object
{
public:
	Town() {}
	virtual ~Town() {}
	bool isFixed() {
		if (m_moving->getVelocity() == Eigen::Vector2f::Zero(), m_moving->getAccel() == Eigen::Vector2f::Zero()) return true;
		else return false;
	}
	virtual void changeBehavior(std::shared_ptr<MovingBehavior>& behavior) {
		m_moving->setBehavior(behavior);
	}
	virtual void updatePosition() {
		m_moving->updatePoistion(m_realm.start_point);
	}
	virtual void damageTown(int amount) {
		m_life.damage(amount);
	}
	virtual int giveDamage() {
		return 1;
	}
	virtual void damageByFireball(std::shared_ptr<Fireball>& fireball, std::shared_ptr<FireballManager>& manager) {
		// もし敵のfireballだったら。（ここらへんどうしようか）
		if (fireball->isEnemy()) {
			m_life.damage(fireball->giveDamage());

			//触れたら消える
			manager->destroy(fireball);
		}
	}
	bool isAlive() {
		return m_life.isAlive();
	}
	Shape::Rectangle getRealm() {
		return m_realm;
	}

	// 子クラスで呼んでもらう
	virtual bool draw() override {
		//死んでいて
		if (!isAlive()) {
			//燃える処理が終わってないなら
			if (m_dead_count >= 0) {
				m_dead_effect.incrementCounterWhenDrawWithRealm(m_realm);
				m_dead_count--;
			}
		}
		return true;
	}

	bool isEffectContinuing() {
		return m_dead_count >= 0;
	}

	void setDeadEffect(std::string effect_name, int frames_per_scene, int dead_count = 0) {
		m_dead_effect = Effect(effectHandles[effect_name], frames_per_scene, PRIORITY_DYNAMIC_OBJECT);
		if (dead_count == 0) dead_count = m_dead_effect.getCountMax() * m_dead_effect.getFramesPerScene();
		m_dead_count = dead_count;
	}
	
protected:
	Shape::Rectangle m_realm = Shape::Rectangle();
	Life m_life;
	std::shared_ptr<Moving> m_moving = std::make_shared<Moving>(1.0f/30.0f, std::make_shared<NewtonBehavior>());

private:
	Effect m_dead_effect = Effect(effectHandles["b_burning"], 5, PRIORITY_DYNAMIC_OBJECT);
	int m_dead_count = 60;
};

class House : public Town
{
public:
	House(Shape::Rectangle realm, Life life) {
		m_realm = realm;
		m_life = life;
		Object::layer = PRIORITY_STATIC_OBJECT;
	}
	House(Shape::Rectangle realm, Life life, int img_handle) {
		m_realm = realm;
		m_life = life;
		m_img_handle = img_handle;
		Object::layer = PRIORITY_STATIC_OBJECT;
	}
	bool draw() override {
		if (m_life.isAlive() || isEffectContinuing()) {
			DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), m_img_handle, TRUE);
		}
		Town::draw();
		return true;
	}
private:
	int m_img_handle = imgHandles["b_house01"];
};

class Resident : public Town
{
public:
	Resident(Shape::Rectangle realm, Life life, std::shared_ptr<MovingBehavior>& behavior) {
		m_realm = realm;
		m_life = life;
		m_moving->setBehavior(behavior);
		Object::layer = PRIORITY_DYNAMIC_OBJECT;
	}
	Resident(Shape::Rectangle realm, Life life, std::shared_ptr<MovingBehavior>& behavior, int img_handle) {
		m_realm = realm;
		m_life = life;
		m_moving->setBehavior(behavior);
		m_img_handle = img_handle;
		Object::layer = PRIORITY_DYNAMIC_OBJECT;
	}
	Resident(Shape::Rectangle realm, Life life, std::shared_ptr<MovingBehavior>& behavior, int img_handle, int img_handle_damaged) {
		m_realm = realm;
		m_life = life;
		m_moving->setBehavior(behavior);
		m_img_handle = img_handle;
		m_img_handle_damaged = img_handle_damaged;
		Object::layer = PRIORITY_DYNAMIC_OBJECT;
	}

	bool draw() override {
		if (m_life.isAlive()) {
			DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), m_img_handle, TRUE);
		}
		else {
			if (isEffectContinuing()) {
				DrawExtendGraph(m_realm.left(), m_realm.top(), m_realm.right(), m_realm.bottom(), m_img_handle_damaged, TRUE);
			}
		}
		Town::draw();
		return true;
	}
private:
	int m_img_handle = imgHandles["b_hime"];
	int m_img_handle_damaged = imgHandles["b_hime_damaged"];
};

} // namespace Breakout
