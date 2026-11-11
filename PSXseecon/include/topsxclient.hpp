#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

#include "stdafx.hpp"

namespace nwk {
	class ToPSXClient final : public ToTCPIPClient {
	public:
		ToPSXClient();
		~ToPSXClient() = default;
		auto demandForQi(uint32_t num) const -> void;
		auto demandForQs(uint32_t num) const -> void;
		auto exit() -> void override;

	protected:
		auto flushElement(QcodeValueData const& e) -> void override;
		auto endOfFlush() -> void override;

	private:
		const QcodeSet switchesWithInfoElements_;
		const QcodeSet fireswitches_;
		auto sendQh(const uint32_t var, const int32_t value) -> void;
		auto sendQi(const uint32_t var, const int32_t value) -> void;
		auto sendQs(const uint32_t var) -> void;
		std::string flushbuf_;

	};
}
