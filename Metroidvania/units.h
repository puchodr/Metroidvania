#ifndef UNITS_H_
#define UNITS_H_

#include <cmath>

#include "config.h"

namespace units {

	typedef int HP;
	typedef int GunExperience;
	
	typedef unsigned int GunLevel;
	const GunLevel kMaxGunLevel = 3;

	typedef float Game; // Intrinsic units of position.
	typedef int Pixel;
	typedef unsigned int Tile;
	typedef unsigned int Frame;
	typedef float Degrees;

	typedef unsigned int MS; // Milliseconds
	typedef unsigned int FPS; // Frames per second (HZ or 1 / second)

	typedef float Velocity; // Game / MS
	typedef float AngularVelocity; // Degrees / MS
	typedef float Acceleration; // Game / MS / MS

	namespace {
		const Game kTileSize = 32.0f;
		const double kPi = atan(1) * 4;

		int round(double val) {
			return (int)(val > 0.0 ? val + 0.5 : val - 0.5);
		}
	}

	inline double degreesToRaidians(units::Degrees degrees) {
		return degrees * kPi / 180.0f;
	}

	inline Game sin(Degrees degrees) {
		return static_cast<Game>(std::sin(degreesToRaidians(degrees)));
	}

	inline Game cos(Degrees degrees) {
		return static_cast<Game>(std::cos(degreesToRaidians(degrees)));
	}

	inline Pixel gameToPixel(Game game) {
		return config::getGraphicsQuality() == config::GraphicsQuality::HIGH_QUALITY ?
			Pixel(round(game)) :
			Pixel(round(game) / 2);
	}
	
	inline Tile gameToTile(Game game) {
		return Tile(game / kTileSize);
	}
	
	inline Game tileToGame(Tile tile) {
		return tile * kTileSize;
	}
	
	inline Pixel tileToPixel(Tile tile) {
		return gameToPixel(tileToGame(tile));
	}

	const Game kHalfTile = tileToGame(1) / 2.0f;

} // units

#endif // UNITS_H_