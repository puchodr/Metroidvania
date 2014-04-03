#include "player.h"

namespace {
	// HUD Constants
	const units::Game kHealthBarX = units::tileToGame(1);
	const units::Game kHealthBarY = units::tileToGame(2);
	const units::Game kHealthBarSourceX = 0;
	const units::Game kHealthBarSourceY = 5 * units::kHalfTile;
	const units::Game kHealthBarSourceWidth = units::tileToGame(4);
	const units::Game kHealthBarSourceHeight = units::kHalfTile;

	const units::Game kHealthFillX = 5 * units::kHalfTile;
	const units::Game kHealthFillY = units::tileToGame(2);

	const units::Game kMaxFillWidth = 5 * units::kHalfTile - 2.0f;

	const units::Game kDamageSourceX = 0;
	const units::Game kDamageSourceY = units::tileToGame(2);
	const units::Game kDamageSourceHeight = units::kHalfTile;

	const units::Game kHealthFillSourceX = 0;
	const units::Game kHealthFillSourceY = 3 * units::kHalfTile;
	const units::Game kHealthFillSourceHeight = units::kHalfTile;

	const units::Game kHealthNumberX = units::tileToGame(3) / 2;
	const units::Game kHealthNumberY = units::tileToGame(2);
	const int kHealthNumDigits = 2;

	const std::string kSpritePath("TextBox");

	const units::MS kDamageDelay = 1250;
}

Player::Health::Health(Graphics& graphics) : 
	damage_(0),
	damage_timer_(kDamageDelay),
	max_health_(6),
	current_health_(6),
	health_bar_sprite_(graphics, kSpritePath,
		units::gameToPixel(kHealthBarSourceX), units::gameToPixel(kHealthBarSourceY),
		units::gameToPixel(kHealthBarSourceWidth), units::gameToPixel(kHealthBarSourceHeight)),
	health_fill_sprite_(graphics, kSpritePath,
		units::gameToPixel(kHealthFillSourceX), units::gameToPixel(kHealthFillSourceY),
		units::gameToPixel(kMaxFillWidth),
		units::gameToPixel(kMaxFillWidth),
		units::gameToPixel(kHealthFillSourceHeight)),
	damage_fill_sprite_(graphics, kSpritePath,
		units::gameToPixel(kDamageSourceX), units::gameToPixel(kDamageSourceY),
		units::gameToPixel(kMaxFillWidth),
		units::gameToPixel(0),
		units::gameToPixel(kDamageSourceHeight))
{

}

void Player::Health::update(units::MS elapsed_time) {
	if (damage_ > 0 && damage_timer_.expired()) {
		current_health_ -= damage_;
		damage_ = 0;
	}
}

void Player::Health::draw(Graphics& graphics) {
	health_bar_sprite_.draw(graphics, kHealthBarX, kHealthBarY);

	if (damage_ > 0) {
		damage_fill_sprite_.draw(graphics, kHealthFillX, kHealthFillY);
	}

	health_fill_sprite_.draw(graphics, kHealthFillX, kHealthFillY);

	NumberSprite::HUDNumber(graphics, current_health_, kHealthNumDigits).draw(
		graphics, kHealthNumberX, kHealthNumberY);
}

bool Player::Health::takeDamage(units::HP damage) {
	damage_ = damage;
	damage_timer_.reset();
	resetFillSprites();
	return false;
}

void Player::Health::addHealth(units::HP health) {
	if (damage_ > health) {
		damage_ -= health;
		health = 0;
	}
	else if (damage_ > 0) {
		health -= damage_;
		damage_ = 0;
	}

	current_health_ = std::min(max_health_, current_health_ + health);
	resetFillSprites();
}

void Player::Health::resetFillSprites() {
	health_fill_sprite_.set_percentage_width((current_health_ - damage_) * 1.0f / max_health_);
	damage_fill_sprite_.set_percentage_width(current_health_ * 1.0f / max_health_);
}