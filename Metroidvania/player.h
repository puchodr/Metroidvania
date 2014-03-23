#ifndef PLAYER_H
#define PLAYER_H

#include "sprite_state.h"

#include "units.h"
#include "timer.h"
#include "sprite.h"
#include "sprite_state.h"
#include "varying_width_sprite.h"
#include "rectangle.h"
#include "number_sprite.h"
#include "damage_text.h"
#include "damageable.h"
#include "polar_star.h"

struct Graphics;
struct Projectile;
struct Map;
struct ParticleTools;

struct Player : public Damageable {
	Player(Graphics& graphics, units::Game x, units::Game y);

	void update(units::MS elapsed_time_ms, const Map& map, ParticleTools& particle_tools);
	void draw(Graphics& graphics);
	void drawHUD(Graphics& graphics);

	void startMovingLeft();
	void startMovingRight();
	void stopMoving();

	void lookUp();
	void lookDown();
	void lookHorizontal();

	void startFire(ParticleTools& particle_tools);
	void stopFire();

	void startJump();
	void stopJump();

	void takeDamage(units::HP damage);

	Rectangle damageRectangle() const;
	units::Game center_x() const { return x_ + units::kHalfTile; }
	units::Game center_y() const { return y_ + units::kHalfTile; }

	boost::shared_ptr<DamageText> get_damage_text() { return damage_text_; }

	std::vector<boost::shared_ptr<Projectile> > getProjectiles()
		{ return polar_star_.getProjectiles(); }

	private:
		enum MotionType {
			FIRST_MOTION_TYPE,
			STANDING = FIRST_MOTION_TYPE,
			INTERACTING,
			WALKING,
			JUMPING,
			FALLING,
			LAST_MOTION_TYPE,
		};
		enum StrideType {
			FIRST_STRIDE_TYPE,
			STRIDE_MIDDLE = FIRST_STRIDE_TYPE,
			STRIDE_LEFT,
			STRIDE_RIGHT,
			LAST_STRIDE_TYPE,
		};

		typedef boost::tuple<MotionType, HorizontalFacing, VerticalFacing, StrideType> SpriteTuple;
		struct SpriteState : public SpriteTuple {
			SpriteState(SpriteTuple& tuple) : SpriteTuple(tuple) {}

			MotionType motion_type() const { return get<0>(); }
			HorizontalFacing horizontal_facing() const { return get<1>(); }
			VerticalFacing vertical_facing() const { return get<2>(); }
			StrideType stride_type() const { return get<3>(); }
		};

		struct WalkingAnimation {
			WalkingAnimation();

			void update();
			void reset();

			StrideType stride() const;
			
			private:
				Timer frame_timer_;
				int current_index_;
				bool forward_;
		};

		struct Health {
			Health(Graphics& graphics);

			void update(units::MS elapsed_time);
			void draw(Graphics& graphics);
			// returns true if health == 0, i.e dead.
			bool takeDamage(units::HP damage);
			
			private:
				units::HP damage_;
				Timer damage_timer_;

				units::HP max_health_;
				units::HP current_health_;
				Sprite health_bar_sprite_;
				VaryingWidthSprite health_fill_sprite_;
				VaryingWidthSprite damage_fill_sprite_;
		};

		void initializeSprites(Graphics& graphics);
		void initializeSprite(Graphics& graphics, const SpriteState& sprite_state);
		SpriteState getSpriteState();

		Rectangle leftCollision(units::Game delta) const;
		Rectangle rightCollision(units::Game delta) const;
		Rectangle topCollision(units::Game delta) const;
		Rectangle bottomCollision(units::Game delta) const;

		void updateX(units::MS elapsed_time_ms, const Map& map);
		void updateY(units::MS elapsed_time_ms, const Map& map, ParticleTools& particle_tools);

		MotionType motionType() const;
		bool onGround() const { return on_ground_; }
		const bool gun_up() const 
			{ return motionType() == WALKING && walking_animation_.stride() != STRIDE_MIDDLE; }
		VerticalFacing vertical_facing() const { 
			return onGround() && intended_vertical_facing == DOWN ? 
			HORIZONTAL : intended_vertical_facing; }

		bool spriteIsVisible() const;

		units::Game x_, y_;
		units::Velocity velocity_x_;
		units::Velocity velocity_y_;
		int acceleration_x_;
		//float acceleration_y_;
		HorizontalFacing horizontal_facing_;
		VerticalFacing intended_vertical_facing;
		bool on_ground_;
		bool jump_active_;
		bool interacting_;
		
		Health health_;
		Timer invincible_timer_;
		boost::shared_ptr<DamageText> damage_text_;

		WalkingAnimation walking_animation_;

		PolarStar polar_star_;

		std::map<SpriteState, boost::shared_ptr<Sprite> > sprites_;
};

#endif // PLAYER_H