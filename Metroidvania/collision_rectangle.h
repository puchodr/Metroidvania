#ifndef COLLISION_RECTANGLE_H_
#define COLLISION_RECTANGLE_H_

#include <assert.h>

#include "rectangle.h"

struct CollisionRectangle {
	virtual Rectangle boundingBox() const = 0;

	Rectangle collision(sides::SideType side, units::Game x, units::Game y, units::Game delta) const {
		if (side == sides::SideType::LEFT_SIDE)
			return leftCollision(x, y, delta);
		if (side == sides::SideType::RIGHT_SIDE)
			return rightCollision(x, y, delta);
		if (side == sides::SideType::TOP_SIDE)
			return topCollision(x, y, delta);
		return bottomCollision(x, y, delta);
	}

	virtual Rectangle leftCollision(units::Game x, units::Game y, units::Game delta) const = 0;
	virtual Rectangle rightCollision(units::Game x, units::Game y, units::Game delta) const = 0;

	virtual Rectangle topCollision(units::Game x, units::Game y, units::Game delta) const = 0;
	virtual Rectangle bottomCollision(units::Game x, units::Game y, units::Game delta) const = 0;
	
	virtual ~CollisionRectangle() = 0;
};

inline CollisionRectangle::~CollisionRectangle() {}

#endif // COLLISION_RECTANGLE_H_