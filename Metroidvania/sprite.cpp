#include "sprite.h"

#include "graphics.h"

Sprite::Sprite(Graphics& graphics,
		const std::string& file_name,
		units::Pixel source_x, units::Pixel source_y,
		units::Pixel width, units::Pixel height) {
	const bool black_is_transparent = true;
	sprite_sheet_ = graphics.loadImage(file_name.c_str(), black_is_transparent);
	source_rect_.x = source_x;
	source_rect_.y = source_y;
	source_rect_.w = width;
	source_rect_.h = height;
}

// Method used to draw a sprite
void Sprite::draw(Graphics& graphics, units::Game x, units::Game y) {
	graphics.renderTexture(sprite_sheet_, 
      units::gameToPixel(x), 
      units::gameToPixel(y), 
      &source_rect_);
}