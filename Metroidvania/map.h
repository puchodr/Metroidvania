#ifndef MAP_H_
#define MAP_H_

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>

#include "units.h"
#include "backdrop.h"

struct Graphics;
struct Sprite;
struct Rectangle;

struct Map {
	enum TileType {
		AIR_TILE,
		WALL_TILE,
	}; 
	struct CollisionTile {
		CollisionTile(units::Tile row, units::Tile col, TileType tile_type) :
			row(row),
			col(col),
			tile_type(tile_type) {}
		units::Tile row, col;
		TileType tile_type;
	};
	static Map* createTestMap(Graphics& graphics);

	std::vector<CollisionTile> getCollidingTiles(const Rectangle& rectangle) const;

	void drawBackground(Graphics& graphics) const;
	void draw(Graphics& graphics) const;

	private:
		struct Tile {
			Tile(TileType tile_type=AIR_TILE,
			     boost::shared_ptr<Sprite> sprite = boost::shared_ptr<Sprite>()) :
			   tile_type(tile_type),
			   sprite(sprite) {}

			TileType tile_type;
			boost::shared_ptr<Sprite> sprite;
		};

		boost::scoped_ptr<Backdrop> backdrop_;
		std::vector<std::vector<boost::shared_ptr<Sprite> > > background_tiles_;
		std::vector<std::vector<Tile> > tiles_;
};

#endif // MAP_H_