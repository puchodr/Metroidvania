#ifndef GAME_H_
#define GAME_H_

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "units.h"
#include "damage_texts.h"
#include "particle_system.h"

#include "pickups.h"

struct Graphics;
struct Player;
struct FirstCaveBat;
struct Map;

struct Game {
	Game();
	~Game();

	static units::Tile kScreenWidth;
	static units::Tile kScreenHeight;

	private:
		void eventLoop();
		void update(units::MS elapsed_time_ms, Graphics& graphics);
		void draw(Graphics& graphics);

		boost::shared_ptr<Player> player_;
		boost::shared_ptr<FirstCaveBat> bat_;
		boost::scoped_ptr<Map> map_;

		DamageTexts damage_texts_;
		ParticleSystem front_particle_system_, entity_particle_system_;
		Pickups pickups_;
};

#endif // GAME_H_