#include "projectile_wall_particle.h"

namespace {
	const std::string kSpriteName("Caret");
	units::Tile kSourceX = 11;
	units::Tile kSourceY = 0;
	units::Tile kSourceWidth = 1;
	units::Tile kSourceHeight = 1;
	units::FPS kFps = 20;
	const units::Frame kNumFrames = 4;
}

ProjectileWallParticle::ProjectileWallParticle(Graphics& graphics, units::Game x, units::Game y) :
	ImmobileSingleLoopParticle(graphics, kSpriteName,
		units::tileToPixel(kSourceX), units::tileToPixel(kSourceY),
		units::tileToPixel(kSourceWidth), units::tileToPixel(kSourceHeight),
		kFps, kNumFrames, x, y) {}