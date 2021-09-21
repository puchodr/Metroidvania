#include "map_collidable.h"

#include "accelerators.h"
#include "kinematics.h"
#include "collision_rectangle.h"
#include "map.h"

using std::optional;

namespace {
	optional<units::Game> testMapCollision(
			const Map& map, 
			const Rectangle& rectangle,
			sides::SideType direction) {
		std::vector<CollisionTile> tiles(map.getCollidingTiles(rectangle, direction));
		for (size_t i = 0; i < tiles.size(); ++i) {
			const sides::SideType side = sides::opposite_side(direction);
			const units::Game perpendicular_position = sides::vertical(side) ?
				rectangle.center_x() :
				rectangle.center_y();
			const units::Game leading_position = rectangle.side(direction);
			const bool should_test_slopes = sides::vertical(side);
			const optional<units::Game> maybe_position(
					tiles[i].testCollision(side, perpendicular_position, leading_position, should_test_slopes));
			if (maybe_position) {
				return maybe_position;
			}
		}
		return {};
	}
}

void MapCollidable::updateX(const CollisionRectangle& collision_rectangle,
		const Accelerator& accelerator,
		Kinematics& kinematics_x, const Kinematics& kinematics_y,
		units::MS elapsed_time_ms, const Map& map) {
	update(collision_rectangle,
			accelerator,
			kinematics_x, kinematics_y,
			elapsed_time_ms, map,
			kinematics_x, X_AXIS);
}

void MapCollidable::updateY(const CollisionRectangle& collision_rectangle,
		const Accelerator& accelerator,
		const Kinematics& kinematics_x, Kinematics& kinematics_y,
		units::MS elapsed_time_ms, const Map& map) {
	update(collision_rectangle,
			accelerator,
			kinematics_x, kinematics_y,
			elapsed_time_ms, map,
			kinematics_y, Y_AXIS);
}

void MapCollidable::onCollision(sides::SideType side, bool is_delta_direction) {

}

void MapCollidable::onDelta(sides::SideType side) {

}

void MapCollidable::update(const CollisionRectangle& collision_rectangle,
		const Accelerator& accelerator,
		const Kinematics& kinematics_x, const Kinematics& kinematics_y,
		units::MS elapsed_time_ms, const Map& map,
		Kinematics& kinematics, AxisType axis) {
	accelerator.updateVelocity(kinematics, elapsed_time_ms);
	// Handle Bottom Collision
	const units::Game delta = kinematics.velocity * elapsed_time_ms;
	// Check collision in the direction of delta.
	const sides::SideType direction = axis == X_AXIS ?
		(delta > 0 ? sides::SideType::RIGHT_SIDE : sides::SideType::LEFT_SIDE) :
		(delta > 0 ? sides::SideType::BOTTOM_SIDE : sides::SideType::TOP_SIDE);

	{ 
		optional<units::Game> maybe_position = testMapCollision(
				map,
				collision_rectangle.collision(direction, kinematics_x.position, kinematics_y.position, delta),
				direction);
		
		// Collision Reaction
		if (maybe_position) {
			kinematics.position = *maybe_position - collision_rectangle.boundingBox().side(direction);
			onCollision(direction, true);
		}
		else {
			kinematics.position += delta;
			onDelta(direction);
		}
	}

	// Check collision in other direction.
	const sides::SideType opposite_direction = sides::opposite_side(direction);
	optional<units::Game> maybe_position = testMapCollision(
			map, 
			collision_rectangle.collision(opposite_direction, kinematics_x.position, kinematics_y.position, 0),
			opposite_direction);
	if (maybe_position) {
		kinematics.position = *maybe_position - collision_rectangle.boundingBox().side(opposite_direction);
		onCollision(opposite_direction, false);
	}
}