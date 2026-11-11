// Nico Kaan
// The Netherlands
// copyright (c) 2024
//
// www.lekseecon.nl
//

#include "stdafx.hpp"

namespace nwk {
	using namespace std::literals;

	VersionData::VersionData(std::string const& s)
		: major{ 0 }, minor{ 0 }, patch{ 0 } {
		try {
			const auto l_r{ std::regex{ R"((\d+).(\d+).(\d+))" } };
			if (std::smatch m; regex_match(s, m, l_r)) {
				major = str2int32(m[1U]);
				minor = str2int32(m[2U]);
				patch = str2int32(m[3U]);
			} else {
				const nlohmann::json j{ nlohmann::json::parse(s) };
				major = j["major"].get<int32_t>();
				minor = j["minor"].get<int32_t>();
				patch = j["patch"].get<int32_t>();
			}
		} catch (const std::exception& er) {
			g_log.line(er.what());
		}
	}

	VersionData::VersionData(const int32_t major, const int32_t minor, const int32_t patch)
		: major{ major }, minor{ minor }, patch{ patch } {
	}

	auto operator==(const VersionData lhs, const VersionData rhs) noexcept -> bool {
		return lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch == rhs.patch;
	};

	auto operator>(const VersionData lhs, const VersionData rhs) noexcept -> bool {
		return lhs.major > rhs.major || 
			(lhs.major == rhs.major && lhs.minor > rhs.minor) ||
			(lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch > rhs.patch);
	};

	auto operator<<(const VersionData lhs, const VersionData rhs) noexcept -> bool {
		return lhs.minor > rhs.minor;
	};

	auto operator>>(const VersionData lhs, const VersionData rhs) noexcept -> bool {
		return lhs.major > rhs.major;
	};

	auto VersionData::toString() const -> std::string {
		return std::format("{}.{}.{}", major, minor, patch);
	}

  auto VersionData::operator*() const -> std::string {
    return toString();
  }

	auto VersionData::toJson() const -> nlohmann::json {
		nlohmann::json j;
		j["major"] = major;
		j["minor"] = minor;
		j["patch"] = patch;
		return j;
	}

	// #todo, als Learner geen version check uitvoert komt er een sys err failed to create the socket. !!!!!!!!!!!!!!!!!!!!

	auto lf_getVersionStringFromInternet() -> std::string{
		const auto me{ "lf_getVersionStringFromInternet"s };
		std::string version{};

		// {"dev":"","major":32,"minor":0,"patch":0}

		try {
			if (const HINTERNET hInet{ InternetOpen(utf8_to_wide(me).c_str(), INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0) }; hInet != nullptr) {
			    const auto file{ "https://www.lekseecon.nl/PSXseecon.json"s };
				if (const HINTERNET hUrl{ InternetOpenUrl(
					  hInet, 
					  utf8_to_wide(file).c_str(), 
					  nullptr, 
					  static_cast<DWORD>(-1L),
					  INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE | WININET_API_FLAG_ASYNC,
					  NULL)  // #todo warning C6553 ?
					}; hUrl != nullptr) {
					char buffer[64U];
					DWORD dwRead{ 0 };
					if (InternetReadFile(hUrl, buffer, sizeof(buffer), &dwRead)) {
						if (dwRead > 0) {
							for (DWORD i{ 0 }; i < dwRead; ++i) {
								if (buffer[i] == gkLfChar) {
									break;
								} 
								version += buffer[i];
							}
						}
					} else {
						g_log.line("file "s + file + " not found?"s);
					}
					InternetCloseHandle(hUrl);
				} else {
					g_log.line("hUrl=NULL"s);
				}
				InternetCloseHandle(hInet);
			} else {
				g_gui.line("no internet access"s);
			}
		} catch (std::exception const& e) {
			g_log.line(e.what());
		}
		return version;
	}
} // namespace nwk
