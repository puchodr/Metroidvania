#ifndef PICKUP_H_
#define PICKUP_H_

#include "rectangle.h"

struct Map;
struct Graphics;

struct Pickup {
	enum PickupType {
		HEALTH,
		MISSLES,
		EXPERIENCE,
	};

	virtual Rectangle collisionRectangle() const = 0;
	virtual bool update(units::MS elapsed_time_ms, const Map& map) = 0;
	virtual void draw(Graphics& graphics) = 0;
	virtual int value() = 0;
	virtual PickupType type() const = 0;
	virtual ~Pickup() = 0;
};

inline Pickup::~Pickup() {}

#endif // PICKUP_H_