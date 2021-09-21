#ifndef SIDE_TYPE_H_
#define SIDE_TYPE_H_

#include "sprite_state.h"

namespace sides {
	enum class SideType {
		TOP_SIDE,
		BOTTOM_SIDE,
		LEFT_SIDE,
		RIGHT_SIDE,
	};

	inline SideType opposite_side(SideType side) {
		if (side == SideType::TOP_SIDE) {
			return SideType::BOTTOM_SIDE;
		}
		if (side == SideType::BOTTOM_SIDE) {
			return SideType::TOP_SIDE;
		}
		if (side == SideType::LEFT_SIDE) {
			return SideType::RIGHT_SIDE;
		}
		return SideType::LEFT_SIDE;
	}

	inline bool vertical(SideType side) {
		return side == SideType::TOP_SIDE || side == SideType::BOTTOM_SIDE;
	}

	inline bool horizontal(SideType side) {
		return !vertical(side);
	}

	inline SideType from_facing(HorizontalFacing h_facing, VerticalFacing v_facing) {
		if (v_facing == VerticalFacing::UP) 
			return SideType::TOP_SIDE;
		if (v_facing == VerticalFacing::DOWN) 
			return SideType::BOTTOM_SIDE;
		if (h_facing == HorizontalFacing::LEFT)
			return SideType::LEFT_SIDE;
		return SideType::RIGHT_SIDE;
	}

	inline bool is_max(SideType side) {
		return side == SideType::RIGHT_SIDE || side == SideType::BOTTOM_SIDE;
	}
	inline bool is_min(SideType side) {
		return !is_max(side);
	}
}

#endif // SIDE_TYPE_H_