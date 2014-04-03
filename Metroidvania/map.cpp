#include "map.h"

#include "sprite.h"
#include "graphics.h"
#include "game.h"
#include "rectangle.h"

using boost::shared_ptr;
using std::vector;

Map* Map::createTestMap(Graphics& graphics) {
	Map* map = new Map();

	map->backdrop_.reset(new FixedBackdrop("bkBlue", graphics));
	const units::Tile num_rows = 15;
	const units::Tile num_cols = 20;
	// Ensure tiles_ is and background_tiles_ num_rows x num_cols in size.
	map->tiles_ = vector<vector<Tile> >(
		num_rows, vector<Tile>(
		num_cols, Tile()));
	map->background_tiles_ = vector<vector<boost::shared_ptr<Sprite> > >(
		num_rows, vector<boost::shared_ptr<Sprite> >(
		num_cols, boost::shared_ptr<Sprite>()));

	shared_ptr<Sprite> sprite(new Sprite(
		graphics,
		"PrtCave",
		units::tileToPixel(1), 0,
		units::tileToPixel(1), units::tileToPixel(1)));
	Tile tile(tiles::WALL_TILE, sprite);
	const units::Tile row = 11;
	for (units::Tile col = 0; col < num_cols; ++col) {
		map->tiles_[row][col] = tile;
	}
	map->tiles_[10][5] = tile;
	map->tiles_[9][4] = tile;
	map->tiles_[8][3] = tile;
	map->tiles_[7][2] = tile;
	map->tiles_[10][3] = tile;

	shared_ptr<Sprite> chainTop(new Sprite(
		graphics,
		"PrtCave",
		units::tileToPixel(11), units::tileToPixel(2),
		units::tileToPixel(1), units::tileToPixel(1)));
	shared_ptr<Sprite> chainMiddle(new Sprite(
		graphics,
		"PrtCave",
		units::tileToPixel(12), units::tileToPixel(2),
		units::tileToPixel(1), units::tileToPixel(1)));
	shared_ptr<Sprite> chainBottom(new Sprite(
		graphics,
		"PrtCave",
		units::tileToPixel(13), units::tileToPixel(2),
		units::tileToPixel(1), units::tileToPixel(1)));

	map->background_tiles_[8][2] = chainTop;
	map->background_tiles_[9][2] = chainMiddle;
	map->background_tiles_[10][2] = chainBottom;

	return map;
}

vector<CollisionTile> Map::getCollidingTiles(const Rectangle& rectangle) const {
	const units::Tile first_row = units::gameToTile(rectangle.top());
	const units::Tile last_row = units::gameToTile(rectangle.bottom());
	const units::Tile first_col = units::gameToTile(rectangle.left());
	const units::Tile last_col = units::gameToTile(rectangle.right());
	vector<CollisionTile> collision_tiles;
	for (units::Tile row = first_row; row <= last_row; ++row) {
		for (units::Tile col = first_col; col <= last_col; ++col) {
			collision_tiles.push_back(CollisionTile(row, col, tiles_[row][col].tile_type));
		}
	}
	return collision_tiles;
}

void Map::drawBackground(Graphics& graphics) const {
	backdrop_->draw(graphics);

	for (size_t row = 0; row < background_tiles_.size(); ++row) {
		for (size_t col = 0; col < background_tiles_[row].size(); ++col) {
			if (background_tiles_[row][col]) {
				background_tiles_[row][col]->draw(
					graphics, units::tileToGame(col), units::tileToGame(row));
			}
		}
	}
}

void Map::draw(Graphics& graphics) const {
	for (size_t row = 0; row < tiles_.size(); ++row) {
		for (size_t col = 0; col < tiles_[row].size(); ++col) {
			if (tiles_[row][col].sprite) {
				tiles_[row][col].sprite->draw(
					graphics, units::tileToGame(col), units::tileToGame(row));
			}
		}
	}
}