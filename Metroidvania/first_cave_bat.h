#ifndef FIRST_CAVE_BAT_H_
#define FIRST_CAVE_BAT_H_

#include "sprite_state.h"
#include "units.h"
#include "rectangle.h"
#include "floating_number.h"
#include "damageable.h"

struct Graphics;
struct Sprite;

struct FirstCaveBat : public Damageable {
	FirstCaveBat(Graphics& graphics, units::Game x, units::Game y);

	void draw(Graphics& graphics);
	bool update(units::MS elapsed_time, units::Game player_x);

	Rectangle damageRectangle() const
		{ return Rectangle(x_ + units::kHalfTile, y_ + units::kHalfTile, 0, 0); }
	Rectangle collisionRectangle() const
		{ return Rectangle(x_, y_, units::tileToGame(1), units::tileToGame(1)); }

	units::HP contactDamage() const;
	void takeDamage(units::HP damage) { damage_text_->addValue(damage); alive_ = false; }

	units::Game center_x() const { return x_ + units::kHalfTile; }
	units::Game center_y() const { return y_ + units::kHalfTile; }

	boost::shared_ptr<FloatingNumber> get_damage_text() { return damage_text_; }

	private:
		typedef boost::tuple<HorizontalFacing> SpriteTuple;
		struct SpriteState : public SpriteTuple {
			SpriteState(SpriteTuple& tuple) : SpriteTuple(tuple) {}

			HorizontalFacing horizontal_facing() const { return get<0>(); }
		};

		void initializeSprites(Graphics& graphics);
		void initializeSprite(Graphics& graphics, const SpriteState& sprite_state);
		SpriteState getSpriteState() const;

		const units::Game flight_center_y_;
		bool alive_;
		units::Game x_, y_;
		HorizontalFacing facing_;
		units::Degrees flight_angle_;
		std::map<SpriteState, boost::shared_ptr<Sprite> > sprites_;
		Timer damage_timer_;
		boost::shared_ptr<FloatingNumber> damage_text_;
};

#endif // FIRST_CAVE_BAT_H_