#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
	extern std::atomic<int32_t> g_nr_threads; // ? in support

	[[nodiscard]] auto gf_removeLeadTrailSpaces(std::string_view s) -> std::string;   // at beginning of end of the std::string, leading and trailing

	auto gf_saveUpto2GenerationsOf(std::filesystem::path file) -> void;

	[[nodiscard]] auto gf_isWin10OrLater() noexcept -> bool;

	auto gf_openLog() -> void;
	auto gf_logUnexpectedAndUnhandledException(std::string const& s) noexcept -> void;
	auto gf_threadStart(std::string_view s) noexcept -> void;
	auto gf_threadFinished(std::string_view s) noexcept -> void;

	[[nodiscard]] auto gf_Plural(const size_t num) -> std::string;

	[[nodiscard]] auto gf_UTCDateTime() -> std::string;
	[[nodiscard]] auto gf_UTCTime() -> std::string;
	[[nodiscard]] auto gf_UTCDate() -> std::string;

	[[nodiscard]] auto gf_UTChhmmss() -> std::string;

	struct UtcData final {
		int32_t hour, min, month, day, year;
	};

	[[nodiscard]] auto gf_decode_utc(time_t seconds_since_1970) -> UtcData;

	[[nodiscard]] auto gf_errornum2String(const int32_t error_num) -> std::string;

	template<typename T>
	auto gt_string2Container(std::string const& s, const char separator) -> T {
		T res;
		std::stringstream linestream{ s };
		std::string e;
		while (getline(linestream, e, separator)) {
			if constexpr (std::is_same<std::forward_list<std::string>, T>::value) {
				res.emplace_front(gf_removeLeadTrailSpaces(e));
			} else {
				res.emplace_back(gf_removeLeadTrailSpaces(e));
			}
		}
		return res;
	}

	[[nodiscard]]
	inline auto str2int32(std::string const& s) -> int32_t {
		int32_t result{};
		auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), result);
		if (ec != std::errc{} || ptr != s.data() + s.size()) {
			throw std::invalid_argument{ "str2int32: invalid integer input" };
		}
		return result;
	}

}
