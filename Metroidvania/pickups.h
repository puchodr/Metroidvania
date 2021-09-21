#ifndef PICKUPS_H_
#define PICKUPS_H_

#include <memory>
#include <set>

#include "units.h"

struct Graphics;
struct Map;
struct Pickup;
struct Player;

struct Pickups {

	void add(std::shared_ptr<Pickup> pickup) { pickups_.insert(pickup); }

	void handleCollision(Player& player);

	void update(units::MS elapsed_time_ms, const Map& map);
	void draw(Graphics& graphics);

	private:
		typedef std::set<std::shared_ptr<Pickup> > PickupSet;
		PickupSet pickups_;
};

#endif // PICKUPS_H_