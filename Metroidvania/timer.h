#ifndef TIMER_H_
#define TIMER_H_
 
#include <set>
#include <boost/noncopyable.hpp>

#include "units.h"

struct Timer : private boost::noncopyable {
	Timer(units::MS expiration_time, bool start_active=false) :
		current_time_(start_active ? 0 : expiration_time + 1),
		expiration_time_(expiration_time) { timers_.insert(this); }
	
	~Timer() { timers_.erase(this); }

	void reset() { current_time_ = 0; }
	bool active() const { return current_time_ < expiration_time_; }
	bool expired() const { return !active(); }

	// Assumes the user knows if this is active or not.
	units::MS current_time() const { return current_time_; }

	static void updateAll(units::MS elapsed_time_);

	private:
		void update(units::MS elapsed_time)
			{ if (active()) {current_time_ += elapsed_time; } }
		units::MS current_time_;
		const units::MS expiration_time_;
		
		static std::set<Timer*> timers_;
};

#endif // TIMER_H_