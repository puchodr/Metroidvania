#include "collision_tile.h"

boost::optional<units::Game> CollisionTile::testCollision(
			sides::SideType side,
			units::Game position) const {
	if (tile_type_ == tiles::WALL_TILE) {
		if (side == sides::TOP_SIDE)
			return units::tileToGame(row_);
		if (side == sides::BOTTOM_SIDE)
			return units::tileToGame(row_ + 1);
		if (side == sides::LEFT_SIDE)
			return units::tileToGame(col_);
		// side == RIGHT_SIDE
		return units::tileToGame(col_ + 1);
	}
	return boost::none;
}