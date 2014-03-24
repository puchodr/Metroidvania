#ifndef ACCELERATORS_H_
#define ACCELERATORS_H_

#include "units.h"

struct Kinematics;

struct Accelerator {
	virtual void updateVelocity(Kinematics& kinematics, units::MS elapsed_time) = 0;
	virtual ~Accelerator() = 0;
};

inline Accelerator::~Accelerator() {}

struct ZeroAccelerator : Accelerator {
	void updateVelocity(Kinematics&, units::MS) {}
	static const ZeroAccelerator kZero;
};

struct ConstantAccelerator : Accelerator {
	ConstantAccelerator(units::Acceleration acceleration, units::Velocity max_velocity) :
		acceleration_(acceleration),
		max_velocity_(max_velocity) {}

	void updateVelocity(Kinematics& kinematics, units::MS elapsed_time);
	static const ConstantAccelerator kGravity;

	private:
		units::Acceleration acceleration_;
		units::Velocity max_velocity_;
};

#endif // ACCELERATORS_H_