#pragma once

#include <integer.hpp>

namespace unicode {

	struct code_point {
		uint32 value_;
		constexpr operator uint32() const { return value_; }
	};

}