#ifndef ENERGY_CRYSTAL_H_
#define ENERGY_CRYSTAL_H_

#include "animated_sprite.h"
#include "kinematics.h"
#include "map_collidable.h"
#include "pickup.h"

struct EnergyCrystal : Pickup,
					   private MapCollidable {
	enum SizeType {
		SMALL = 0,
		MEDIUM = 1,
		LARGE = 2,
	};

	EnergyCrystal(Graphics& graphics, units::Game center_x, units::Game center_y, SizeType size);

	Rectangle collisionRectangle() const;
	bool update(units::MS elapsed_time, const Map& map);
	void draw(Graphics& graphics);
	int value() const;
	PickupType type() const { return EXPERIENCE; }

	private:
		void onCollision(sides::SideType side, bool is_delta_direction);
		void onDelta(sides::SideType side) {}

		Kinematics kinematics_x_, kinematics_y_;
		AnimatedSprite sprite_;
		SizeType size_;
		Timer timer_;
};

#endif // ENERGY_CRYSTAL_H_