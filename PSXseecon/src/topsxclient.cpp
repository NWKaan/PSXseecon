//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
#include "stdafx.hpp"

namespace nwk {
	using namespace std::literals;

	constexpr auto kMaxBytesToPSX = 4'000U;

	ToPSXClient::ToPSXClient()
		:
		ToTCPIPClient("ToPSX"s),
		switchesWithInfoElements_{ QcodeSet{
		Qcode::Qh63, Qcode::Qh64, Qcode::Qh65,
		Qcode::Qh66, Qcode::Qh67, Qcode::Qh68, Qcode::Qh69,
		Qcode::Qh70, Qcode::Qh71, Qcode::Qh72, Qcode::Qh73,
		Qcode::Qh74,  Qcode::Qh75, Qcode::Qh114, Qcode::Qh115,
		Qcode::Qh165,
		Qcode::Qh197, Qcode::Qh198, Qcode::Qh199, Qcode::Qh200,
		Qcode::Qh209, Qcode::Qh210, Qcode::Qh211, Qcode::Qh212,
		Qcode::Qh283, Qcode::Qh310, Qcode::Qh315, Qcode::Qh417,
		// big alt action switches
		Qcode::Qh162, Qcode::Qh163, Qcode::Qh164,
		Qcode::Qh166, Qcode::Qh167, Qcode::Qh168, Qcode::Qh188,
		Qcode::Qh189, Qcode::Qh190, Qcode::Qh191, Qcode::Qh194,
		Qcode::Qh195, Qcode::Qh196, Qcode::Qh201, Qcode::Qh202,
		Qcode::Qh203, Qcode::Qh204, Qcode::Qh205, Qcode::Qh206,
		Qcode::Qh207, Qcode::Qh208, Qcode::Qh216, Qcode::Qh217,
		Qcode::Qh218, Qcode::Qh219, Qcode::Qh225, Qcode::Qh226,
		Qcode::Qh227, Qcode::Qh228, Qcode::Qh233, Qcode::Qh239,
		Qcode::Qh240, Qcode::Qh241, Qcode::Qh242, Qcode::Qh243,
		Qcode::Qh244, Qcode::Qh245, Qcode::Qh246, Qcode::Qh247,
		Qcode::Qh248, Qcode::Qh249, Qcode::Qh250, Qcode::Qh251,
		Qcode::Qh252, Qcode::Qh253, Qcode::Qh254, Qcode::Qh255,
		Qcode::Qh256, Qcode::Qh257, Qcode::Qh258, Qcode::Qh259,
		Qcode::Qh260, Qcode::Qh261, Qcode::Qh262, Qcode::Qh264,
		Qcode::Qh266, Qcode::Qh271, Qcode::Qh272, Qcode::Qh280,
		Qcode::Qh281, Qcode::Qh282, Qcode::Qh291, Qcode::Qh294,
		Qcode::Qh295, Qcode::Qh298, Qcode::Qh299, Qcode::Qh309,
		Qcode::Qh311, Qcode::Qh312, Qcode::Qh313, Qcode::Qh314,
		Qcode::Qh316, Qcode::Qh317, Qcode::Qh318, Qcode::Qh323,
		Qcode::Qh324, Qcode::Qh325, Qcode::Qh326, Qcode::Qh327,
		Qcode::Qh328, Qcode::Qh329,
		// green squib buttons too
		Qcode::Qh365, Qcode::Qh366, Qcode::Qh367, Qcode::Qh368,
		Qcode::Qh369, Qcode::Qh370, Qcode::Qh371, Qcode::Qh372,
		Qcode::Qh373, Qcode::Qh374, Qcode::Qh375, Qcode::Qh376,
		Qcode::Qh377, Qcode::Qh378, Qcode::Qh379, Qcode::Qh380,
		Qcode::Qh381, Qcode::Qh382,
		// fuel cutoff too:
		Qcode::Qh392, Qcode::Qh393, Qcode::Qh394, Qcode::Qh395,
		// Eng Start switches
		Qcode::Qh267, Qcode::Qh268, Qcode::Qh269, Qcode::Qh270,
		// RudderCtr
		Qcode::Qh417
	} },
		fireswitches_{ QcodeSet{Qcode::Qh275,  Qcode::Qh276, Qcode::Qh277, Qcode::Qh278, Qcode::Qh279} } {
	}

	auto ToPSXClient::sendQh(const uint32_t var, const int32_t value) -> void {
		flushbuf_ += std::format("Qh{}={}\r\n", var, value);
		if (flushbuf_.size() > kMaxBytesToPSX) {
			endOfFlush();
		}
	}

	auto ToPSXClient::sendQi(const uint32_t var, const int32_t value) -> void {
		flushbuf_ += std::format("Qi{}={}\r\n", var, value);
		if (flushbuf_.size() > kMaxBytesToPSX) {
			endOfFlush();
		}
	}

	auto ToPSXClient::sendQs(const uint32_t var) -> void {
		flushbuf_ += std::format("Qs{}={}\r\n", var, g_str.at(var));
		if (flushbuf_.size() > kMaxBytesToPSX) {
			ToPSXClient::endOfFlush();
		}
	}

	auto ToPSXClient::exit() -> void {
		sendData(kexitStr + gkCrLfStr);
	}

	auto ToPSXClient::endOfFlush() -> void {
		if (!flushbuf_.empty()) {
			sendData(flushbuf_);
			if (*g_CfgMONITOR_PSX_OUT) {
				g_log.writeString(kSend + flushbuf_);
			}
			flushbuf_.clear();
		}
	};

	auto ToPSXClient::flushElement(QcodeValueData const& e) -> void {
		//
		// See General Design in FromPSXClient::decodemessage  (further down)
		//
		//   - Conversion of Qs is easy because kQsoffset is 0.
		//   - Conversion of Qh is done by subtracting kQhoffset.
		//
		constexpr int32_t kMask{-2};  // clear bit 0
		constexpr int32_t kFireSwActionMask{2 + 4 + 8 + 16 + 64};  // clear all bits except info bits
		constexpr int32_t kELTMask{8 + 8192};  // clear all bits except info bits
		constexpr int32_t kEvacMask{2 + 4};  // clear all bits except info bits
		const auto q_as_uint{ gf_qcodeIndex(e.qcode) };
		if (gf_isQsQcode(e.qcode)) {
			sendQs(static_cast<uint32_t>(q_as_uint));
		} else if (e.qcode < Qcode::Qi0) {  // Qh
			const auto var{ gf_qcodeNumber(e.qcode, Qcode::Qh0) };
			if (switchesWithInfoElements_.contains(e.qcode)) {
				const auto newval{ (g_dbp.at(q_as_uint) & kMask) + e.value };  // sync switch position !  (needed for bit 20 and 21 of big alternate switches , etc...)
				g_dbp.at(q_as_uint) = newval;
				sendQh(var, newval);
			} else if (fireswitches_.find(e.qcode) != fireswitches_.end()) {
				const auto newval{ (g_dbp.at(q_as_uint) & kFireSwActionMask) + e.value };
				g_dbp.at(q_as_uint) = newval;
				sendQh(var, newval);
			} else if (e.qcode == Qcode::Qh224) {  // ELT
				const auto newval{ (g_dbp.at(q_as_uint) & kELTMask) + e.value };
				g_dbp.at(q_as_uint) = newval;
				sendQh(var, newval);
			} else if (e.qcode == Qcode::Qh424) {  // EvacLight
				const auto newval{ (g_dbp.at(q_as_uint) & kEvacMask) + e.value };
				g_dbp.at(q_as_uint) = newval;
				sendQh(var, newval);
			} else if (e.qcode == Qcode::Qh998) {  // Cdu Screen Selector
				g_CfgCduLScreen.value_ = (0 == e.value);
				g_CfgCduCScreen.value_ = (1 == e.value);
				g_CfgCduRScreen.value_ = (2 == e.value);
				g_gui.changeCdu();
			} else {
				// g_dbp[q_as_uint] = e.value;  //   TODO: yes or no?
				sendQh(var, e.value);
			}
		} else if (e.qcode < Qcode::Qexpanded) {  // Qi
			sendQi(static_cast<uint32_t>(q_as_uint) - static_cast<uint32_t>(Qcode::Qi0), e.value);
		} else {
			throw std::runtime_error(std::format("unexpected flushing of {} to PSX?", q_as_uint));
		}
	}

	[[nodiscard]] auto lf_CDUStr(const uint32_t cdu) {
		auto res{ "R"s };
		if (0 == cdu) {
			res = "L"s;
		} else {
			if (1 == cdu) {
				res = "C"s;
			}
		}
		return res;
	}

	auto ToPSXClient::demandForQi(uint32_t num) const -> void {
		sendData(std::format("demand=Qi{}\r\n", num));
	}

	auto ToPSXClient::demandForQs(uint32_t num) const -> void {
		sendData(std::format("demand=Qs{}\r\n", num));
	}
}
