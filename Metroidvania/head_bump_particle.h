#ifndef HEAD_BUMP_PARTICLE_H_
#define HEAD_BUMP_PARTICLE_H_

#include "particle.h"
#include "sprite.h"
#include "timer.h"
#include "units.h"
#include "polar_vector.h"

struct Graphics;

struct HeadBumpParticle : public Particle {
	HeadBumpParticle(Graphics& graphics, units::Game center_x, units::Game center_y);
	bool update(units::MS elasped_time);
	void draw(Graphics& graphics);

	private:
		const units::Game center_x_, center_y_;
		Timer timer_;
		Sprite sprite_;
		PolarVector particle_a_;
		const units::Game max_offset_a_;
		PolarVector particle_b_;
		const units::Game max_offset_b_;
};

#endif // HEAD_BUMP_PARTICLE_H_