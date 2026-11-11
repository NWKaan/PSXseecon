//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
#include "stdafx.hpp"

namespace nwk {
	// signed

	auto gf_i2BCD3bn(const int32_t i) -> bcd {
		if (i >= 0) {
			return gf_ui2BCD3b(static_cast<uint32_t>(i));
		}
		const auto ii{ static_cast<uint32_t>(-i) };
		const auto d0{ ii % gkTenU };
		auto d1{ (ii % gkHundredU) / gkTenU };
		auto d2{ (ii % gkThousandU) / gkHundredU };
		if (0 != d2) {
			throw std::runtime_error(std::format("gf_i2BCD3bn: negative var to big: {}", i));
		}
		if (0 == d1) {  // shift minus to middle digit
			d1 = gkElevenU;
			d2 = gkTenU;
		} else {
			d2 = gkElevenU;
		}
		return d0 + (d1 * gk16) + (d2 * gk256);
	}
}
