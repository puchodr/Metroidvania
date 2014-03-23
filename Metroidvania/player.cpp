#include "player.h"

#include <cmath>

#include "game.h"
#include "animated_sprite.h"
#include "graphics.h"
#include "map.h"
#include "rectangle.h"
#include "number_sprite.h"
#include "particle_system.h"
#include "head_bump_particle.h"

namespace {
	// Walk Motion
	const units::Acceleration kWalkingAcceleration = 0.00083007812f;
	const units::Velocity kMaxSpeedX = 0.15859375f;
	const units::Acceleration kFriction = 0.00049804687f;

	// Fall Motion
	const units::Acceleration kGravity = 0.00078125f;
	const units::Velocity kMaxSpeedY = 0.2998046875f;

	// Jump Motion
	const units::Velocity kJumpSpeed = 0.25f;
	const units::Velocity kShortJumpSpeed = kJumpSpeed / 1.5f;
	const units::Acceleration kAirAcceleration = 0.0003125f;
	const units::Acceleration kJumpGravity = 0.0003125f;

	// Sprites
	const std::string kSpriteFilePath("MyChar");

	// Sprite Frames
	const units::Frame kCharacterFrame = 0;

	const units::Frame kWalkFrame = 0;
	const units::Frame kStandFrame = 0;
	const units::Frame kJumpFrame = 1;
	const units::Frame kFallFrame = 2;

	const units::Frame kUpFrameOffset = 3;
	const units::Frame kDownFrame = 6;
	const units::Frame kBackFrame = 7;

	// Collision Rectangle
	const Rectangle kCollisionX(6, 10, 20, 12);
	//const Rectangle kCollisionY(10, 2, 12, 30);

	const units::Game kCollisionYTop = 2;
	const units::Game kCollisionYHeight = 30;
	const units::Game kCollisionTopWidth = 18;
	const units::Game kCollisionBottomWidth = 10;
	const units::Game kCollisionTopLeft = (units::tileToGame(1) - kCollisionTopWidth) / 2;
	const units::Game kCollisionBottomLeft = (units::tileToGame(1) - kCollisionBottomWidth) / 2;

	const units::MS kInvincibleFlashTime = 50;
	const units::MS kInvincibleTime = 1750;

	struct CollisionInfo {
		bool collided;
		units::Tile row, col;
	};
	CollisionInfo getWallCollisionInfo(const Map& map, const Rectangle& rectangle) {
		CollisionInfo info = { false, 0, 0 };
		std::vector<Map::CollisionTile> tiles(map.getCollidingTiles(rectangle));
		for (size_t i = 0; i < tiles.size(); ++i) {
			if (tiles[i].tile_type == Map::WALL_TILE) {
				info = { true, tiles[i].row, tiles[i].col };
				break;
			}
		}
		return info;
	}
}

Player::Player(Graphics& graphics, units::Game x, units::Game y) :
	x_(x),
	y_(y),
	velocity_x_(0.0f),
	velocity_y_(0.0f),
	acceleration_x_(0),
	horizontal_facing_(LEFT),
	intended_vertical_facing(HORIZONTAL),
	on_ground_(false),
	jump_active_(false),
	interacting_(false),
	health_(graphics),
	invincible_timer_(kInvincibleTime),
	damage_text_(new DamageText()),
	gun_experience_hud_(graphics),
	polar_star_(graphics) 
{
		initializeSprites(graphics);
}

void Player::update(units::MS elapsed_time_ms, const Map& map, ParticleTools& particle_tools) {
	health_.update(elapsed_time_ms);

	walking_animation_.update();

	polar_star_.updateProjectiles(elapsed_time_ms, map, particle_tools);

	updateX(elapsed_time_ms, map);
	updateY(elapsed_time_ms, map, particle_tools);
}

void Player::draw(Graphics& graphics) {
	if (spriteIsVisible()) {
		polar_star_.draw(graphics, horizontal_facing_, vertical_facing(), gun_up(), x_, y_);
		sprites_[getSpriteState()]->draw(graphics, x_, y_);
	}
}

void Player::drawHUD(Graphics& graphics) {
	if (spriteIsVisible()) {
		health_.draw(graphics);
		polar_star_.drawHUD(graphics, gun_experience_hud_);
	}
}

void Player::startMovingLeft() {
	if (on_ground_ && acceleration_x_ == 0) {
		walking_animation_.reset();
	}
	acceleration_x_ = -1;
	horizontal_facing_ = LEFT;
	interacting_ = false;
}

void Player::startMovingRight() {
	if (on_ground_ && acceleration_x_ == 0) {
		walking_animation_.reset();
	}
	acceleration_x_ = 1;
	horizontal_facing_ = RIGHT;
	interacting_ = false;
}

void Player::stopMoving() {
	acceleration_x_ = 0;
}

void Player::lookUp() {
	intended_vertical_facing = UP;
	interacting_ = false;
}

void Player::lookDown() {
	if (intended_vertical_facing == DOWN) return;
	intended_vertical_facing = DOWN;
	interacting_ = onGround();
}

void Player::lookHorizontal() {
	intended_vertical_facing = HORIZONTAL;
}

void Player::startFire(ParticleTools& particle_tools) {
	polar_star_.startFire(x_, y_, horizontal_facing_, vertical_facing(), gun_up(), particle_tools);
}

void Player::stopFire() {
	polar_star_.stopFire();
}

void Player::startJump() {
	jump_active_ = true;
	interacting_ = false;

	if (onGround()) {
		velocity_y_ = -kJumpSpeed;
	}
}

void Player::stopJump() {
	jump_active_ = false;
}

void Player::takeDamage(units::HP damage) {
	if (invincible_timer_.active()) return;
	
	health_.takeDamage(damage);
	damage_text_->setDamage(damage);

	velocity_y_ = std::min(velocity_y_, -kShortJumpSpeed);
	invincible_timer_.reset();
}

Rectangle Player::damageRectangle() const {
	return Rectangle(x_ + kCollisionX.left(),
		y_ + kCollisionYTop,
		kCollisionX.width(),
		kCollisionYHeight);
}

void Player::initializeSprites(Graphics& graphics) {
	//health_number_sprite_.reset(new NumberSprite(graphics, 56, kHealthNumberNumDigits));

	ENUM_FOREACH(motion, MOTION_TYPE) {
		ENUM_FOREACH(hFacing, HORIZONTAL_FACING) {
			ENUM_FOREACH(vFacing, VERTICAL_FACING) {
				ENUM_FOREACH(sType, STRIDE_TYPE) {
					initializeSprite(graphics, boost::make_tuple((MotionType)motion,
						(HorizontalFacing)hFacing,
						(VerticalFacing)vFacing,
						(StrideType)sType));
				}
			}
		}
	}
}

void Player::initializeSprite(Graphics& graphics, const SpriteState& sprite_state) {	
	units::Tile tile_y = sprite_state.horizontal_facing() == LEFT ?
		kCharacterFrame :
		1 + kCharacterFrame;

	units::Tile tile_x;
	switch (sprite_state.motion_type()) {
		case WALKING:
			tile_x = kWalkFrame;
			break;
		case STANDING:
			tile_x = kStandFrame;
			break;
		case INTERACTING:
			tile_x = kBackFrame;
			break;
		case JUMPING:
			tile_x = kJumpFrame;
			break;
		case FALLING:
			tile_x = kFallFrame;
			break;
		case LAST_MOTION_TYPE:
			break;
	}
	switch (sprite_state.vertical_facing()) {
		case HORIZONTAL:
			break;
		case UP:
			tile_x += kUpFrameOffset;
			break;
		case DOWN:
			tile_x = kDownFrame;
			break;
		default:
			break;
	}

	if (sprite_state.motion_type() == WALKING) {
		switch (sprite_state.stride_type()) {
			case STRIDE_MIDDLE:
				break;
			case STRIDE_LEFT:
				tile_x += 1;
				break;
			case STRIDE_RIGHT:
				tile_x += 2;
				break;
			default:
				break;
		}
		sprites_[sprite_state] = boost::shared_ptr<Sprite>(new Sprite(
			graphics,
			kSpriteFilePath,
			units::tileToPixel(tile_x), units::tileToPixel(tile_y), 
			units::tileToPixel(1), units::tileToPixel(1)));
	}
	else {
		sprites_[sprite_state] = boost::shared_ptr<Sprite>(new Sprite(
			graphics,
			kSpriteFilePath,
			units::tileToPixel(tile_x), units::tileToPixel(tile_y),
			units::tileToPixel(1), units::tileToPixel(1)));
	}
}

Player::MotionType Player::motionType() const {
	MotionType motion;

	if (interacting_) {
		motion = INTERACTING;
	}
	else if (on_ground_) {
		motion = acceleration_x_ == 0 ? STANDING : WALKING;
	}
	else {
		motion = velocity_y_ < 0.0f ? JUMPING : FALLING;
	}
	return motion;
}

Player::SpriteState Player::getSpriteState() {
	return boost::make_tuple(
		motionType(),
		horizontal_facing_,
		vertical_facing(),
		walking_animation_.stride());
}

Rectangle Player::leftCollision(units::Game delta) const {
	assert(delta <= 0);
	return Rectangle(
		x_ + kCollisionX.left() + delta,
		y_ + kCollisionX.top(),
		(kCollisionX.width() / 2) - delta,
		kCollisionX.height());
}

Rectangle Player::rightCollision(units::Game delta) const {
	assert(delta >= 0);
	return Rectangle(
		x_ + kCollisionX.left() + (kCollisionX.width() / 2),
		y_ + kCollisionX.top(),
		(kCollisionX.width() / 2) + delta,
		kCollisionX.height());
}

Rectangle Player::topCollision(units::Game delta) const {
	assert(delta <= 0);
	return Rectangle(
		x_ + kCollisionTopLeft,
		y_ + kCollisionYTop + delta,
		kCollisionTopWidth,
		(kCollisionYHeight / 2) - delta);
}
Rectangle Player::bottomCollision(units::Game delta) const {
	assert(delta >= 0);
	return Rectangle(
		x_ + kCollisionBottomLeft,
		y_ + kCollisionYTop + (kCollisionYHeight / 2),
		kCollisionBottomWidth,
		(kCollisionYHeight / 2) + delta);
}

void Player::updateX(units::MS elapsed_time_ms, const Map& map) {
	// Update Velocity
	units::Acceleration acceleration_x = 0.0f;
	if (acceleration_x_ < 0) 
		acceleration_x = on_ground_ ? -kWalkingAcceleration : -kAirAcceleration;
	else if (acceleration_x_ > 0)
		acceleration_x = on_ground_ ? kWalkingAcceleration : kAirAcceleration;

	velocity_x_ += acceleration_x * elapsed_time_ms;

	if (acceleration_x_ < 0) {
		velocity_x_ = std::max(velocity_x_, -kMaxSpeedX);
	}
	else if (acceleration_x_ > 0) {
		velocity_x_ = std::min(velocity_x_, kMaxSpeedX);
	}
	else if (on_ground_) {
		velocity_x_ = velocity_x_ > 0.0f ?
			std::max(0.0f, velocity_x_ - kFriction * elapsed_time_ms) :
			std::min(0.0f, velocity_x_ + kFriction * elapsed_time_ms);
	}
	
	// Handling right collision.
	const units::Game delta = velocity_x_ * elapsed_time_ms;
	if (delta > 0.0f) {
		// Check collision in the direction of delta.
		CollisionInfo info = getWallCollisionInfo(map, rightCollision(delta));

		// Collision Reaction
		if (info.collided) {
			x_ = units::tileToGame(info.col) - kCollisionX.right();
			velocity_x_ = 0.0f;
		}
		else {
			x_ += delta;
		}

		// Check collision in other direction.
		info = getWallCollisionInfo(map, leftCollision(0));
		if (info.collided) {
			// TODO: Add a Game::kTileSize
			x_ = units::tileToGame(info.col) + kCollisionX.right();
		}
	}
	// Handling left collision
	else {
		// Check collision in the direction of delta.
		CollisionInfo info = getWallCollisionInfo(map, leftCollision(delta));

		// Collision Reaction
		if (info.collided) {
			// TODO: Add a Game::kTileSize
			x_ = units::tileToGame(info.col) + kCollisionX.right();
			velocity_x_ = 0.0f;
		}
		else {
			x_ += delta;
		}

		// Check collision in other direction.
		info = getWallCollisionInfo(map, rightCollision(0));
		if (info.collided) {
			x_ = units::tileToGame(info.col) - kCollisionX.right();
		}
	}
}
void Player::updateY(units::MS elapsed_time_ms, const Map& map, ParticleTools& particle_tools) {
	// Update Velocity
	const units::Acceleration gravity = jump_active_ && velocity_y_ < 0.0f ? kJumpGravity : kGravity;
	velocity_y_ = std::min(velocity_y_ + gravity * elapsed_time_ms, kMaxSpeedY);

	// Handle Bottom Collision
	const units::Game delta = velocity_y_ * elapsed_time_ms;
	if (delta > 0) {
		// Check collision in the direction of delta.
		CollisionInfo info = getWallCollisionInfo(map, bottomCollision(delta));
		
		// Collision Reaction
		if (info.collided) {
			y_ = units::tileToGame(info.row) - (kCollisionYTop + kCollisionYHeight); 
			velocity_y_ = 0.0f;
			on_ground_ = true;
		}
		else {
			y_ += delta;
			on_ground_ = false;
		}

		// Check collision in other direction.
		info = getWallCollisionInfo(map, topCollision(0));
		if (info.collided) {
			y_ = units::tileToGame(info.row) + kCollisionYHeight;
			particle_tools.front_system.addNewParticle(boost::shared_ptr<Particle>(
				new HeadBumpParticle(particle_tools.graphics, center_x(), y_ + kCollisionYTop)));
		}
	}
	// Handling top Collision.
	else {
		CollisionInfo info = getWallCollisionInfo(map, topCollision(delta));
		
		// Collision Reaction
		if (info.collided) {
			y_ = units::tileToGame(info.row) + kCollisionYHeight;
			velocity_y_ = 0.0f;
			particle_tools.front_system.addNewParticle(boost::shared_ptr<Particle>(
				new HeadBumpParticle(particle_tools.graphics, center_x(), y_ + kCollisionYTop)));
		}
		else {
			y_ += delta;
			on_ground_ = false;
		}

		// Check collision in other direction.
		info = getWallCollisionInfo(map, bottomCollision(0));
		if (info.collided) {
			y_ = units::tileToGame(info.row) - (kCollisionYTop + kCollisionYHeight);
			on_ground_ = true;
		}
	}
}

bool Player::spriteIsVisible() const {
	return !(invincible_timer_.active() && 
		     invincible_timer_.current_time() / kInvincibleFlashTime % 2 == 0);
}