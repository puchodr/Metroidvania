#ifndef PARTICLE_SYSTEM_H_
#define PARTICLE_SYSTEM_H_

#include <memory>
#include <set>

#include "units.h"

struct Particle;
struct Graphics;

struct ParticleSystem {
	void addNewParticle(std::shared_ptr<Particle> particle) { particles_.insert(particle); }

	void update(units::MS elapsed_time);
	void draw(Graphics& graphics);

	private:
		typedef std::set<std::shared_ptr<Particle> > ParticleSet;
		ParticleSet particles_;
};

struct ParticleTools {
	ParticleSystem& front_system;
	ParticleSystem& entity_system;
	Graphics& graphics;
};

#endif // PARTICLE_SYSTEM_H_