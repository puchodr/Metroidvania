#include "game.h"

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

#include "graphics.h"
#include "player.h"
#include "map.h"
#include "input.h"
#include "first_cave_bat.h"
#include "timer.h"
#include "death_cloud_particle.h"

#include "energy_crystal.h"
#include "flashing_pickup.h"

namespace {
	const units::FPS kFps = 60;
	const units::MS kMaxFrameTime = 5 * 1000 / 60;
}

// static
units::Tile Game::kScreenWidth = 20;
// static
units::Tile Game::kScreenHeight = 15;

Game::Game() {
	srand(static_cast<unsigned int>(time(NULL)));
	SDL_Init(SDL_INIT_EVERYTHING);
	eventLoop();
}

Game::~Game() {
	SDL_Quit();
}

void Game::eventLoop() {
	Graphics graphics;
	Input input;
	SDL_Event event;

	ParticleTools particle_tools = { front_particle_system_, entity_particle_system_, graphics };
	player_.reset(new Player(graphics, particle_tools, units::tileToGame(kScreenWidth / 2), units::tileToGame(kScreenHeight / 2)));
	damage_texts_.addDamageable(player_);
	bat_.reset(new FirstCaveBat(graphics, units::tileToGame(7), units::tileToGame(kScreenHeight / 2)));
	damage_texts_.addDamageable(bat_);
	map_.reset(Map::createSlopeTestMap(graphics));

	bool running = true;
	units::MS last_update_time = SDL_GetTicks();
	while (running) {
		const units::MS start_time_ms = SDL_GetTicks();
		input.beginNewFrame();
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					input.keyDownEvent(event);
					break;
				case SDL_KEYUP:
					input.keyUpEvent(event);
					break;
				case SDL_QUIT:
					running = false;
					break;
				default:
					break;
			}
		}

		// Checks if escape was pressed.
		running = !input.wasKeyPressed(SDL_SCANCODE_ESCAPE);

		// Player Horizontal Movement
		// if both left and right are being pressed, stop moving
		if (input.isKeyHeld(SDL_SCANCODE_LEFT) && input.isKeyHeld(SDL_SCANCODE_RIGHT)) {
			player_->stopMoving();
		}
		else if (input.isKeyHeld(SDL_SCANCODE_LEFT)) {
			player_->startMovingLeft();
		}
		else if (input.isKeyHeld(SDL_SCANCODE_RIGHT)) {
			player_->startMovingRight();
		} 
		else {
			player_->stopMoving();
		}

		// Player Look Direction.
		if (input.isKeyHeld(SDL_SCANCODE_UP) && input.isKeyHeld(SDL_SCANCODE_DOWN)) {
			player_->lookHorizontal();
		}
		else if (input.isKeyHeld(SDL_SCANCODE_UP)) {
			player_->lookUp();
		}
		else if (input.isKeyHeld(SDL_SCANCODE_DOWN)) {
			player_->lookDown();
		}
		else {
			player_->lookHorizontal();
		}

		// Player Fire
		if (input.wasKeyPressed(SDL_SCANCODE_X)) {
			player_->startFire();
		}
		else if (input.wasKeyReleased(SDL_SCANCODE_X)) {
			player_->stopFire();
		}

		// Player Jump
		if (input.wasKeyPressed(SDL_SCANCODE_Z)) {
			player_->startJump();
		}
		else if (input.wasKeyReleased(SDL_SCANCODE_Z)) {
			player_->stopJump();
		}

		/* TODO TESTING REMOVE LATER */
		if (input.wasKeyPressed(SDL_SCANCODE_1)) {
			bat_.reset(new FirstCaveBat(graphics, units::tileToGame(7), units::tileToGame(kScreenHeight / 2)));
		}
		if (input.wasKeyPressed(SDL_SCANCODE_2)) {
			player_.reset(new Player(graphics, particle_tools, units::tileToGame(kScreenWidth / 2), units::tileToGame(kScreenHeight / 2)));
		}
		/* TODO TESTING REMOVE LATER */

		const units::MS current_time_ms = SDL_GetTicks();
		const units::MS elapsed_time = current_time_ms - last_update_time;
		update(std::min(elapsed_time, kMaxFrameTime), graphics);
		last_update_time = current_time_ms;

		draw(graphics);
		const units::MS ms_per_frame = 1000 / kFps;
		const units::MS elapsed_time_ms = SDL_GetTicks() - start_time_ms;
		if (elapsed_time_ms < ms_per_frame) {
			SDL_Delay(ms_per_frame - elapsed_time_ms);
		}
	}
}

void Game::update(units::MS elapsed_time_ms, Graphics& graphics) {
	Timer::updateAll(elapsed_time_ms);
	damage_texts_.update(elapsed_time_ms);
	pickups_.update(elapsed_time_ms, *map_);
	front_particle_system_.update(elapsed_time_ms);
	entity_particle_system_.update(elapsed_time_ms);

	ParticleTools particle_tools = { front_particle_system_, entity_particle_system_, graphics };
	player_->update(elapsed_time_ms, *map_);
	if (bat_) {
		if (!bat_->update(elapsed_time_ms, player_->center_x())) {
			for (int i = 0; i < 3; ++i) {
				pickups_.add(std::shared_ptr<Pickup>(
					new EnergyCrystal(graphics, bat_->center_x(), bat_->center_y(), EnergyCrystal::MEDIUM)));
			}

			pickups_.add(FlashingPickup::heartPickup(graphics, bat_->center_x(), bat_->center_y()));
			DeathCloudParticle::createRandomDeathClouds(particle_tools,
				bat_->center_x(), bat_->center_y(), 3);
			bat_.reset();
		}
	}

	if (bat_) {
		std::vector<std::shared_ptr<Projectile> > projectiles(player_->getProjectiles());
		for (size_t i = 0; i < projectiles.size(); ++i) {
			if (bat_->collisionRectangle().collidesWith(projectiles[i]->collisionRectangle())) {
				bat_->takeDamage(projectiles[i]->contactDamage());
				projectiles[i]->collideWithEnemy();
			}
		}

		if (bat_->damageRectangle().collidesWith(player_->damageRectangle())) {
			player_->takeDamage(bat_->contactDamage());
		}
	}

	pickups_.handleCollision(*player_);
}

void Game::draw(Graphics& graphics) {
	graphics.clear();
	map_->drawBackground(graphics);
	if (bat_) {
		bat_->draw(graphics);
	}

	entity_particle_system_.draw(graphics);
	pickups_.draw(graphics);
	player_->draw(graphics);
	map_->draw(graphics);
	front_particle_system_.draw(graphics);

	damage_texts_.draw(graphics);
	player_->drawHUD(graphics);
	graphics.flip();
}