#ifndef PLAYER_H
#define PLAYER_H

#include "units.h"
#include "kinematics.h"
#include "map_collidable.h"
#include "timer.h"
#include "sprite.h"
#include "sprite_state.h"
#include "varying_width_sprite.h"
#include "rectangle.h"
#include "number_sprite.h"
#include "floating_number.h"
#include "damageable.h"
#include "polar_star.h"
#include "gun_experience_hud.h"

struct Graphics;
struct Projectile;
struct Map;
struct ParticleTools;
struct Pickup;

struct Player : public Damageable,
				public MapCollidable {
	Player::Player(Graphics& graphics, ParticleTools& particle_tools, units::Game x, units::Game y);

	void update(units::MS elapsed_time_ms, const Map& map);
	void draw(Graphics& graphics);
	void drawHUD(Graphics& graphics);

	void startMovingLeft();
	void startMovingRight();
	void stopMoving();

	void lookUp();
	void lookDown();
	void lookHorizontal();

	void startFire();
	void stopFire();

	void startJump();
	void stopJump();

	void takeDamage(units::HP damage);
	void collectPickup(const Pickup& pickup);

	Rectangle damageRectangle() const;
	units::Game center_x() const { return kinematics_x_.position + units::kHalfTile; }
	units::Game center_y() const { return kinematics_y_.position + units::kHalfTile; }

	std::shared_ptr<FloatingNumber> get_damage_text() { return damage_text_; }

	std::vector<std::shared_ptr<Projectile> > getProjectiles()
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

		typedef std::tuple<MotionType, HorizontalFacing, VerticalFacing, StrideType> SpriteTuple;
		struct SpriteState {
			SpriteState(SpriteTuple& tuple) : tuple_(tuple) {}
			SpriteState(const SpriteState& ss) : tuple_(ss.tuple_) {}
			bool operator<(const SpriteState& ss) const { return tuple_ < ss.tuple_; }

			MotionType motion_type() const { return std::get<0>(tuple_); }
			HorizontalFacing horizontal_facing() const { return std::get<1>(tuple_); }
			VerticalFacing vertical_facing() const { return std::get<2>(tuple_); }
			StrideType stride_type() const { return std::get<3>(tuple_); }

			SpriteTuple tuple_;
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
			void addHealth(units::HP health);
			
			private:
				void resetFillSprites();
				
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

		void updateX(units::MS elapsed_time_ms, const Map& map);
		void updateY(units::MS elapsed_time_ms, const Map& map);

		void onCollision(sides::SideType side, bool is_delta_direction);
		void onDelta(sides::SideType side);

		bool spriteIsVisible() const;

		MotionType motionType() const;
		bool on_ground() const { return on_ground_; }
		const bool gun_up() const 
			{ return motionType() == MotionType::WALKING && walking_animation_.stride() != StrideType::STRIDE_MIDDLE; }
		VerticalFacing vertical_facing() const { 
			return on_ground() && intended_vertical_facing == VerticalFacing::DOWN ? 
			VerticalFacing::HORIZONTAL : intended_vertical_facing; }

		ParticleTools& particle_tools_;
		Kinematics kinematics_x_, kinematics_y_;
		int acceleration_x_;
		HorizontalFacing horizontal_facing_;
		VerticalFacing intended_vertical_facing;
		bool on_ground_;
		bool jump_active_;
		bool interacting_;
		
		Health health_;
		Timer invincible_timer_;
		std::shared_ptr<FloatingNumber> damage_text_;
		FloatingNumber experience_text;

		WalkingAnimation walking_animation_;

		GunExperienceHud gun_experience_hud_;
		PolarStar polar_star_;

		std::map<SpriteState, std::shared_ptr<Sprite> > sprites_;
};

#endif // PLAYER_H