#include "damage_texts.h"

#include "floating_number.h"
#include "damageable.h"


void DamageTexts::addDamageable(std::shared_ptr<Damageable> damageable) {
	damage_text_map_[damageable->get_damage_text()] = std::weak_ptr<Damageable>(damageable);
}

void DamageTexts::update(units::MS elapsed_time) {
	for (DamageTextMap::iterator iter = damage_text_map_.begin();
			iter != damage_text_map_.end(); ) {
		if (!iter->second.expired()) {
			std::shared_ptr<Damageable> damageable(iter->second);
			iter->first->setPosition(damageable->center_x(), damageable->center_y());
		}
		if (iter->first->update(elapsed_time) || !iter->second.expired()) {
			++iter;
		}
		else {
			damage_text_map_.erase(iter++);
			printf("erased\n");
		}
	}
}

void DamageTexts::draw(Graphics& graphics) {
	for (DamageTextMap::iterator iter = damage_text_map_.begin();
			iter != damage_text_map_.end();
			++iter) {
		iter->first->draw(graphics);
	}
}