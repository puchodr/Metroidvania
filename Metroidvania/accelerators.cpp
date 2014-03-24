#include "accelerators.h"

#include <algorithm>

#include "kinematics.h"


// Fall Motion
const units::Acceleration kGravityAcceleration = 0.00078125f;
const units::Velocity kTerminalSpeed = 0.2998046875f;

//static
const ZeroAccelerator ZeroAccelerator::kZero;
//static
const ConstantAccelerator ConstantAccelerator::kGravity(kGravityAcceleration, kTerminalSpeed);


void ConstantAccelerator::updateVelocity(Kinematics& kinematics, units::MS elapsed_time) {
	if (acceleration_ < 0.0f) {
		kinematics.velocity = std::max(kinematics.velocity + acceleration_ * elapsed_time, max_velocity_);
	}
	else {
		kinematics.velocity = std::min(kinematics.velocity + acceleration_ * elapsed_time, max_velocity_);
	}
}
