#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2024
//
// www.lekseecon.nl
//

namespace nwk {
	class VersionData final {
	public:
		VersionData(const int32_t major, const int32_t minor, const int32_t patch);

		VersionData(std::string const& s);  // normal version string or nlohmann::json string:
		// "32.5.1"
		// {"major":32,"minor":0,"patch":0}


		auto toJson() const -> nlohmann::json;
		auto toString() const -> std::string;
		auto operator*() const -> std::string;
		friend auto operator==(const VersionData lhs, const VersionData rhs) noexcept -> bool;
		friend auto operator>(const VersionData lhs, const VersionData rhs) noexcept -> bool;
		friend auto operator!=(const VersionData lhs, const VersionData rhs) noexcept -> bool;
		friend auto operator<<(const VersionData lhs, const VersionData rhs) noexcept -> bool;
		friend auto operator>>(const VersionData lhs, const VersionData rhs) noexcept -> bool;

	private:
		int32_t major;
		int32_t minor;
		int32_t patch;
	};

	auto gf_versionCheck() -> void;
}
