#ifndef DAMAGE_TEXTS_H_
#define DAMAGE_TEXTS_H_

#include <memory>
#include <map>

#include "units.h"

struct FloatingNumber;
struct Damageable;
struct Graphics;

struct DamageTexts {
	void addDamageable(std::shared_ptr<Damageable> damageable);
	void update(units::MS elapsed_time);
	void draw(Graphics& graphics);

	private:
		typedef std::map<std::shared_ptr<FloatingNumber>, std::weak_ptr<Damageable> > DamageTextMap;

		DamageTextMap damage_text_map_;
};

#endif //DAMAGE_TEXTS_H_