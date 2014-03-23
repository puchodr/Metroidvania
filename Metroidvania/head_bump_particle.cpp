#include "head_bump_particle.h"

#include "polar_vector.h"

#include <cmath>
#include <stdlib.h>



namespace {
	const units::Game kSourceX = 116;
	const units::Game kSourceY = 54;
	const units::Game kWidth = 6;
	const units::Game kHeight = 6;

	const units::MS kLifeTime = 600;
	const units::MS kFlashPeriod = 25;

	const units::Velocity kSpeed = 0.12f;
}

HeadBumpParticle::HeadBumpParticle(Graphics& graphics, units::Game center_x, units::Game center_y) :
	center_x_(center_x),
	center_y_(center_y),
	timer_(kLifeTime, true),
	sprite_(graphics, "Caret", units::gameToPixel(kSourceX), units::gameToPixel(kSourceY), units::gameToPixel(kWidth), units::gameToPixel(kHeight)),
	particle_a_(0, static_cast<units::Degrees>(rand() & 360)),
	particle_b_(0, static_cast<units::Degrees>(rand() & 360)),
	max_offset_a_(static_cast<units::Game>(4 + (rand() % 16))),
	max_offset_b_(static_cast<units::Game>(4 + (rand() % 16)))
{

}

bool HeadBumpParticle::update(units::MS elasped_time) {
	particle_a_.magnitude = std::fmin(particle_a_.magnitude + kSpeed * elasped_time, max_offset_a_);
	particle_b_.magnitude = std::fmin(particle_b_.magnitude + kSpeed * elasped_time, max_offset_b_);
	return timer_.active();
}

void HeadBumpParticle::draw(Graphics& graphics) {
	if (timer_.current_time() / kFlashPeriod % 2 == 0) {
		sprite_.draw(graphics, center_x_ + particle_a_.get_x(), center_y_ + particle_a_.get_y());
		sprite_.draw(graphics, center_x_ + particle_b_.get_x(), center_y_ + particle_b_.get_y());
	}
}