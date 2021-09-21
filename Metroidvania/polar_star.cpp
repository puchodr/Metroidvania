#include "polar_star.h"

#include <vector>
#include <algorithm>

#include "sprite.h"
#include "graphics.h"
#include "gun_experience_hud.h"
#include "map.h"
#include "particle_system.h"
#include "projectile_star_particle.h"
#include "projectile_wall_particle.h"

namespace {
	const int kPolarStarIndex = 2; // 0-based indexing
	const units::Game kGunWidth = 3 * units::kHalfTile;
	const units::Game kGunHeight = 2 * units::kHalfTile;
	const std::string kSpritePath("Arms");

	const units::Tile kHorizontalOffset = 0;
	const units::Tile kUpOffset = 2;
	const units::Tile kDownOffset = 4;

	const units::Tile kLeftOffset = 0;
	const units::Tile kRightOffset = 1;

	// Nozzle offsets
	const units::Game kNozzleHorizontalY = 23;
	const units::Game kNozzleHorizontalLeftX = 10;
	const units::Game kNozzleHorizontalRightX = 38;

	const units::Game kNozzleUpY = 4;
	const units::Game kNozzleUpLeftX = 27;
	const units::Game kNozzleUpRightX = 21;

	const units::Game kNozzleDownY = 28;
	const units::Game kNozzleDownLeftX = 29;
	const units::Game kNozzleDownRightX = 19;
	
	// Projectile Sprite
	const units::Tile kProjectileSourceYs[units::kMaxGunLevel] = { 2, 2, 3 };
	const units::Tile kHorizontalProjectileSourceXs[units::kMaxGunLevel] = { 8, 10, 8 };

	// Projectile Velocity
	const units::Velocity kProjectileVelocity = 0.6f;

	const units::Game kProjectileMaxOffsets[units::kMaxGunLevel] = 
		{ 7 * units::kHalfTile, units::tileToGame(5), units::tileToGame(7) };
	const units::Game kProjectileWidths[units::kMaxGunLevel] = { 4.0f, 8.0f, 12.0f };

	const units::HP kDamages[units::kMaxGunLevel] = { 1, 2, 4 };
	const units::GunExperience kExperiences[] = { 0, 10, 30, 40 };
}

PolarStar::PolarStar(Graphics& graphics) : 
	current_experience_(0)
{
	initializeSprites(graphics);
}

void PolarStar::updateProjectiles(units::MS elapsed_time, const Map& map, ParticleTools& particle_tools) {
	if (projectile_a_) {
		if (!projectile_a_->update(elapsed_time, map, particle_tools)) {
			projectile_a_.reset();
		}
	}
	if (projectile_b_) {
		if (!projectile_b_->update(elapsed_time, map, particle_tools)) {
			projectile_b_.reset();
		}
	}
}

void PolarStar::drawHUD(Graphics& graphics, GunExperienceHud& hud) {
	const units::GunLevel level = current_level();
	hud.draw(graphics, 
		level, 
		current_experience_ - kExperiences[level - 1], 
		kExperiences[level] - kExperiences[level - 1]);
}

void PolarStar::draw(
		Graphics& graphics, 
		HorizontalFacing horizontal_facing, VerticalFacing vertical_facing, 
		const bool gun_up, units::Game player_x, units::Game player_y) {
	units::Game x = gun_x(horizontal_facing, player_x);
	units::Game y = gun_y(vertical_facing, gun_up, player_y);

	
	sprite_map_[SpriteState(std::make_tuple(horizontal_facing, vertical_facing))]->draw(graphics, x, y);
	if (projectile_a_) {
		projectile_a_->draw(graphics);
	}
	if (projectile_b_) {
		projectile_b_->draw(graphics);
	}
}

units::Game PolarStar::gun_y(VerticalFacing vertical_facing, bool gun_up, units::Game player_y) const {
	units::Game gun_y = player_y;
	if (vertical_facing == VerticalFacing::UP) {
		gun_y -= units::kHalfTile / 2;
	}
	if (vertical_facing == VerticalFacing::DOWN) {
		gun_y += units::kHalfTile / 2;
	}
	if (gun_up) {
		gun_y -= 2.0f;
	}
	return gun_y;
}

void PolarStar::collectExperience(units::GunExperience experience) {
	current_experience_ += experience;
	current_experience_ = std::min(kExperiences[units::kMaxGunLevel], current_experience_);
}

void PolarStar::damageExperience(units::GunExperience experience) {
	current_experience_ = std::max(0, current_experience_ - experience);
}

void PolarStar::startFire(units::Game player_x,
		units::Game player_y,
		HorizontalFacing horizontal_facing, 
		VerticalFacing vertical_facing, 
		bool gun_up,
		ParticleTools& particle_tools) {
	if (projectile_a_ && projectile_b_) return;

	units::Game bullet_x = gun_x(horizontal_facing, player_x) - units::kHalfTile;
	units::Game bullet_y = gun_y(vertical_facing, gun_up, player_y) - units::kHalfTile;
	switch (vertical_facing) {
	   case VerticalFacing::HORIZONTAL:
			bullet_y += kNozzleHorizontalY;
			if (horizontal_facing == HorizontalFacing::LEFT) {
				bullet_x += kNozzleHorizontalLeftX;
			}
			else {
				bullet_x += kNozzleHorizontalRightX;
			}
			break;
		case VerticalFacing::UP:
			bullet_y += kNozzleUpY;
			if (horizontal_facing == HorizontalFacing::LEFT) {
				bullet_x += kNozzleUpLeftX;
			}
			else {
				bullet_x += kNozzleUpRightX;
			}
			break;
		case VerticalFacing::DOWN:
			bullet_y += kNozzleDownY;
			if (horizontal_facing == HorizontalFacing::LEFT) {
				bullet_x += kNozzleDownLeftX;
			}
			else {
				bullet_x += kNozzleDownRightX;
			}
			break;
		default:
			break;
	}
	
	// create out projectile.
	if (!projectile_a_) {
		projectile_a_.reset(new Projectile(
			vertical_facing == VerticalFacing::HORIZONTAL ? 
					horizontal_projectiles_[current_level() - 1] : 
					vertical_projectiles_[current_level() - 1],
			horizontal_facing, vertical_facing,
			bullet_x, bullet_y,
			current_level(),
			particle_tools));
	}
	else if (!projectile_b_) {
		projectile_b_.reset(new Projectile(
			vertical_facing == VerticalFacing::HORIZONTAL ? 	
				horizontal_projectiles_[current_level() - 1] : 
				vertical_projectiles_[current_level() - 1],
			horizontal_facing, vertical_facing,
			bullet_x, bullet_y, 
			current_level(),
			particle_tools));
	}
}

std::vector<std::shared_ptr< ::Projectile> > PolarStar::getProjectiles() {
	std::vector<std::shared_ptr< ::Projectile> > projectiles;
	if (projectile_a_) projectiles.push_back(projectile_a_);
	if (projectile_b_) projectiles.push_back(projectile_b_);
	return projectiles;
}


void PolarStar::initializeSprites(Graphics& graphics) {
	for (units::GunLevel gun_level = 0; gun_level < units::kMaxGunLevel; ++gun_level) {
		horizontal_projectiles_[gun_level].reset(new Sprite(
			graphics, "Bullet",
			units::tileToPixel(kHorizontalProjectileSourceXs[gun_level]), 
			units::tileToPixel(kProjectileSourceYs[gun_level]),
			units::tileToPixel(1), units::tileToPixel(1)));
		vertical_projectiles_[gun_level].reset(new Sprite(
			graphics, "Bullet",
			units::tileToPixel(kHorizontalProjectileSourceXs[gun_level] + 1), 
			units::tileToPixel(kProjectileSourceYs[gun_level]),
			units::tileToPixel(1), units::tileToPixel(1)));
	}

	ENUM_FOREACH(hFacing, HORIZONTAL_FACING) {
		ENUM_FOREACH(vFacing, VERTICAL_FACING) {
			initializeSprite(graphics, SpriteState(std::make_tuple((HorizontalFacing)hFacing, (VerticalFacing)vFacing)));
		}
	}
}

units::GunLevel PolarStar::current_level() const {
	units::GunLevel level;

	for (level = units::kMaxGunLevel;
		  current_experience_ < kExperiences[level - 1];
		  --level) 
	{

	}

	return level;
}

void PolarStar::initializeSprite(Graphics& graphics, const SpriteState& sprite_state) {
	units::Tile tile_y = sprite_state.horizontal_facing() == HorizontalFacing::LEFT ? kLeftOffset : kRightOffset;
	switch (sprite_state.vertical_facing()) {
	   case VerticalFacing::HORIZONTAL:
			tile_y += kHorizontalOffset;
			break;
		case VerticalFacing::UP:
			tile_y += kUpOffset;
			break;
		case VerticalFacing::DOWN:
			tile_y += kDownOffset;
			break;
		case VerticalFacing::LAST_VERTICAL_FACING:
			break;
	}
	sprite_map_[sprite_state] = std::shared_ptr<Sprite>(new Sprite(graphics, kSpritePath,
		units::gameToPixel(kPolarStarIndex * kGunWidth), units::tileToPixel(tile_y),
		units::gameToPixel(kGunWidth), units::gameToPixel(kGunHeight)));
}

PolarStar::Projectile::Projectile(std::shared_ptr<Sprite> sprite,
								  HorizontalFacing horizontal_direction,
								  VerticalFacing vertical_direction,
								  units::Game x, units::Game y,
								  units::GunLevel gun_level,
								  ParticleTools& particle_tools) :
		sprite_(sprite),
		horizontal_direction_(horizontal_direction),
		vertical_direction_(vertical_direction),
		x_(x),
		y_(y),
		gun_level_(gun_level),
		offset_(0),
		alive_(true) 
{
	particle_tools.front_system.addNewParticle(std::shared_ptr<Particle>(
		new ProjectileStarParticle(particle_tools.graphics, x_, y_)));
}

bool PolarStar::Projectile::update(units::MS elapsed_time, const Map& map, 
								   ParticleTools& particle_tools) {
	offset_ += kProjectileVelocity * elapsed_time;

	const sides::SideType direction = sides::from_facing(
			horizontal_direction_, vertical_direction_);
	const Rectangle rectangle(collisionRectangle());
	std::vector<CollisionTile> colliding_tiles(
		map.getCollidingTiles(rectangle, direction));

	for (size_t i = 0; i < colliding_tiles.size(); ++i) {
		const sides::SideType side = sides::opposite_side(direction);
		const units::Game perpendicular_position = sides::vertical(side) ?
			rectangle.center_x() :
			rectangle.center_y();
		const units::Game leading_position = rectangle.side(direction);
		const bool should_test_slopes = true;
		const std::optional<units::Game> maybe_position(
			colliding_tiles[i].testCollision(side, perpendicular_position, leading_position, should_test_slopes));
			
		if (maybe_position) {
			const units::Game collision_x = sides::vertical(side) ?
				perpendicular_position :
				*maybe_position;
			const units::Game collision_y = sides::vertical(side) ?
				*maybe_position :
				perpendicular_position;
			particle_tools.front_system.addNewParticle(std::shared_ptr<Particle>(
				new ProjectileWallParticle(particle_tools.graphics, 
					collision_x - units::kHalfTile, 
					collision_y - units::kHalfTile)));
			return false;
		}
	}

	if (!alive_) {
		return false;
	}
	else if (offset_ >= kProjectileMaxOffsets[gun_level_ -1]) {
		// create a particle
		particle_tools.front_system.addNewParticle(std::shared_ptr<Particle>(
			new ProjectileStarParticle(particle_tools.graphics, getX(), getY())));
		return false;
	}
	return true;
}

void PolarStar::Projectile::draw(Graphics& graphics) {
	units::Game x = x_;
	units::Game y = y_;
	
	sprite_->draw(graphics, getX(), getY());
}

Rectangle PolarStar::Projectile::collisionRectangle() const {
	const units::Game width = vertical_direction_ == VerticalFacing::HORIZONTAL ? 
		units::tileToGame(1) : kProjectileWidths [gun_level_ - 1];
	const units::Game height = vertical_direction_ != VerticalFacing::HORIZONTAL ? 
		units::tileToGame(1) : kProjectileWidths[gun_level_ - 1];

	return Rectangle(getX() + units::kHalfTile - width / 2,
					 getY() + units::kHalfTile - height / 2, 
					 width, height);
}

units::HP PolarStar::Projectile::contactDamage() const {
	return kDamages[gun_level_ - 1];
}

units::Game PolarStar::Projectile::getX() const {
	if (vertical_direction_ == VerticalFacing::HORIZONTAL)
		return x_ + (horizontal_direction_ == HorizontalFacing::LEFT ? -offset_ : offset_);
	return x_;
}
				
units::Game PolarStar::Projectile::getY() const {
	units::Game y = y_;
	switch (vertical_direction_) {
	   case VerticalFacing::UP:
			y -= offset_;
			break;
		case VerticalFacing::DOWN:
			y += offset_;
			break;
		default:
			break;
	}
	return y;
}
