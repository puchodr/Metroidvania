#include "graphics.h"

#include <stdexcept>
#include <SDL2/SDL_image.h>

#include "game.h"

namespace {
	const int kBitsPerPixel = 32;
}

Graphics::Graphics() {
	sdl_window_ = SDL_CreateWindow(
		"Metroidvania",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		units::tileToPixel(Game::kScreenWidth),
		units::tileToPixel(Game::kScreenHeight),
		SDL_WINDOW_OPENGL);

	sdl_renderer_ = SDL_CreateRenderer(
		sdl_window_,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(sdl_renderer_,
		units::tileToPixel(Game::kScreenWidth),
		units::tileToPixel(Game::kScreenHeight));

	if (sdl_window_ == nullptr)
		throw std::runtime_error("SDL_CreateWindow");
	if (sdl_renderer_ == nullptr)
		throw std::runtime_error("SDL_CreateRenderer");

	//SDL_ShowCursor(SDL_DISABLE);

	//camera_ = std::shared_ptr<Camera>(new Camera());
	fullscreen_ = false;
}

Graphics::~Graphics() {
	for (SpriteMap::iterator iter = sprite_sheets_.begin();
		iter != sprite_sheets_.end();
		++iter) {
		SDL_DestroyTexture(iter->second);
	}
	SDL_DestroyRenderer(sdl_renderer_);
	SDL_DestroyWindow(sdl_window_);
}

SDL_Texture* Graphics::loadImage(const std::string& file_name, bool black_is_transparent) {
	const std::string file_path = "../content/" + file_name + ".bmp";

	// Sprite cache, so we're not loading the same images over and over again.
	if (sprite_sheets_.count(file_path) == 0) {
		SDL_Texture* texture;
		if (black_is_transparent) {
			SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());
			if (surface == nullptr) {
				std::string error = "Cannot load texture '" + file_path + "'!";
				throw std::runtime_error(error);
			}
			const Uint32 black_color = SDL_MapRGB(surface->format, 0, 0, 0);
			SDL_SetColorKey(surface, SDL_TRUE, black_color);
			texture = SDL_CreateTextureFromSurface(sdl_renderer_, surface);
			SDL_FreeSurface(surface);
		}
		else {
			texture = IMG_LoadTexture(sdl_renderer_, file_path.c_str());
		}
		if (texture == nullptr) {
			throw std::runtime_error("Cannot load texture!");
		}
		sprite_sheets_[file_path] = texture;
	}
	return sprite_sheets_[file_path];
}

void Graphics::renderTexture(SDL_Texture* texture,
	const SDL_Rect destination,
	const SDL_Rect* clip) const {
	SDL_RenderCopy(sdl_renderer_, texture, clip, &destination);
}

void Graphics::renderTexture(SDL_Texture* texture,
		int x, int y,
		const SDL_Rect* clip) const {
	SDL_Rect destination;
	destination.x = x; // -camera_->camera.x;
	destination.y = y; // - camera_->camera.y;
	if (clip != nullptr) {
		destination.w = clip->w;
		destination.h = clip->h;
	}
	else {
		SDL_QueryTexture(texture, nullptr, nullptr, &destination.w, &destination.h);
	}
	renderTexture(texture, destination, clip);
}

void Graphics::clear() {
	SDL_RenderClear(sdl_renderer_);
}

void Graphics::flip() const {
	SDL_RenderPresent(sdl_renderer_);
}

void Graphics::toggleFullscreen() {
	Uint32 flags = (SDL_GetWindowFlags(sdl_window_) ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
	int error = SDL_SetWindowFullscreen(sdl_window_, flags);
	if (error < 0) {
		throw std::runtime_error("Cannot make window fullscreen!");
	}
}
