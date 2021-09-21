#ifndef MAP_H_
#define MAP_H_

#include <memory>
#include <vector>

#include "backdrop.h"
#include "collision_tile.h"
#include "tile_type.h"
#include "units.h"

struct Graphics;
struct Sprite;
struct Rectangle;

struct Map {
	static Map* createSlopeTestMap(Graphics& graphics);
	static Map* createTestMap(Graphics& graphics);

	std::vector<CollisionTile> getCollidingTiles(
			const Rectangle& rectangle,
			sides::SideType direction) const;

	void drawBackground(Graphics& graphics) const;
	void draw(Graphics& graphics) const;

	private:
		struct Tile {
			Tile(tiles::TileType tile_type=tiles::TileType().set(tiles::EMPTY),
			     std::shared_ptr<Sprite> sprite = std::shared_ptr<Sprite>()) :
			   tile_type(tile_type),
			   sprite(sprite) {}

			tiles::TileType tile_type;
			std::shared_ptr<Sprite> sprite;
		};

		std::unique_ptr<Backdrop> backdrop_;
		std::vector<std::vector<std::shared_ptr<Sprite> > > background_tiles_;
		std::vector<std::vector<Tile> > tiles_;
};

#endif // MAP_H_