// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
#include "stdafx.hpp"

namespace nwk {
	using namespace std::literals;

	constinit std::atomic<int32_t> g_nr_threads{ 0 };

	auto gf_threadStart(std::string_view s) noexcept -> void {
#ifdef TRACE_THREADS
		g_log.line(std::format("{} +++ THREAD {} started ...", gf_UTCTime(), s));
#else
		UNREFERENCED_PARAMETER(s);
#endif
		++g_nr_threads;
	}

	auto gf_threadFinished(std::string_view s) noexcept -> void {
#ifdef TRACE_THREADS
		g_log.line(std::format("{} --- THREAD {} finished", gf_UTCTime(), s));
#else
		UNREFERENCED_PARAMETER(s);
#endif 
		--g_nr_threads;
	}
}

namespace nwk {

	static auto lf_isWin(const DWORD num) noexcept {
		OSVERSIONINFOEX osvi;
		DWORDLONG dwlConditionMask = 0;
		constexpr BYTE op = VER_EQUAL;

		// Initialize the OSVERSIONINFOEX structure.

		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		osvi.dwMajorVersion = num;

		// Initialize the condition mask.

		VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);

		// Perform the test.

		VerifyVersionInfo(&osvi, VER_MAJORVERSION, dwlConditionMask);
		return osvi.dwMajorVersion == num;
	}

	auto gf_isWin10OrLater() noexcept -> bool {
		return lf_isWin(10) || lf_isWin(11);
	}

	auto gf_Plural(const size_t num) -> std::string {
		return (num == 0) || (num > 1) ? gksStr : gkEmptyStr;
	}

	const auto kWhite_space{ " \t"s };

	auto gf_removeLeadTrailSpaces(std::string_view s) -> std::string {
		const auto words_begin{ s.find_first_not_of(kWhite_space) };
		s.remove_prefix(min(words_begin, s.size()));
		if (const auto words_end{ s.find_last_not_of(kWhite_space) }; words_end != std::string_view::npos) {
			s.remove_suffix(s.size() - words_end - 1);
		}
		return static_cast<std::string>(s);
	}


	// Time....

	static auto lf_dateTimeStamp(const time_t t) -> std::string {
		std::string res;
		struct tm buf;
		gmtime_s(&buf, &t);
		char str[26];
		asctime_s(str, sizeof str, &buf);
		auto res0{ std::string_view(str) };
		res0.remove_suffix(1);
		res = static_cast<std::string>(res0);
		return res;
	}

	auto gf_UTCDateTime() -> std::string {
		return lf_dateTimeStamp(time(nullptr));
	}

	auto gf_UTCTime() -> std::string {
		const auto s{ lf_dateTimeStamp(time(nullptr)) };
		return s.substr(s.size() - 13, 8);
	}

	auto gf_UTCDate() -> std::string {
		const auto s{ lf_dateTimeStamp(time(nullptr)) };
		return s.substr(4, 6) + gkCommaChar + gkSpaceChar + s.substr(20, 4);  // Mon dd, YEAR
	}

	auto gf_UTChhmmss() -> std::string {
		const auto q{ lf_dateTimeStamp(time(nullptr)) };
		return q.substr(11, 2) + q.substr(14, 2) + q.substr(17, 2);
	}

	auto gf_decode_utc(time_t seconds_since_1970) -> UtcData {
		UtcData u{ 0,0,0,0,0 };
		struct tm buf;
		gmtime_s(&buf, &seconds_since_1970);  // Vul de tijdstructuur in (UTC)
		u.year = 1900 + buf.tm_year;
		u.hour = buf.tm_hour;
		u.min = buf.tm_min;
		u.day = buf.tm_mday;
		u.month = buf.tm_mon + 1;
		return u;
	}

	auto gf_errornum2String(const int32_t error_num) -> std::string {
		// Meaning of connect error numbers:
		auto res{ std::format("{} ", error_num) };
		switch (error_num) {
		case 10004: res += "Interrupted system call"s; break;
		case 10009: res += "Bad file number"s; break;
		case 10013: res += "Permission denied"s; break;
		case 10014: res += "Bad address"s; break;
		case 10022: res += "Invalid argument"s; break;
		case 10024: res += "Too many open sockets"s; break;
		case 10035: res += "Operation would block"s; break;
		case 10036: res += "Operation now in progress"s; break;
		case 10037: res += "Operation already in progress"s; break;
		case 10038: res += "Socket operation on non-socket"s; break;
		case 10039: res += "Destination address required"s; break;
		case 10040: res += "Messenger too long"s; break;
		case 10041: res += "Protocol wrong type for socket"s; break;
		case 10042: res += "Bad protocol option"s; break;
		case 10043: res += "Protocol not supported"s; break;
		case 10044: res += "Socket type not supported"s; break;
		case 10045: res += "Operation not supported on socket"s; break;
		case 10046: res += "Protocol family not supported"s; break;
		case 10047: res += "Address family not supported"s; break;
		case 10048: res += "Address already in use"s; break;
		case 10049: res += "Can't assign requested address"s; break;
		case 10050: res += "Network is down"s; break;
		case 10051: res += "Network is unreachable"s; break;
		case 10052: res += "Net connection reset"s; break;
		case 10053: res += "Software caused connection abort"s; break;
		case 10054: res += "Connection reset by peer"s; break;
		case 10055: res += "No buffer space available"s; break;
		case 10056: res += "Socket is already connected"s; break;
		case 10057: res += "Socket is not connected"s; break;
		case 10058: res += "Can't send numLines socket shutdown"s; break;
		case 10059: res += "Too many references, can't splice"s; break;
		case 10060: res += "Connection timed out"s; break;
		case 10061: res += "Connection refused"s; break;
		case 10062: res += "Too many levels of symbolic links"s; break;
		case 10063: res += "File name too long"s; break;
		case 10064: res += "Host is down"s; break;
		case 10065: res += "No route to host"s; break;
		case 10066: res += "Directory not empty"s; break;
		case 10067: res += "Too many processes"s; break;
		case 10068: res += "Too many users"s; break;
		case 10069: res += "Disc quota exceeded"s; break;
		case 10070: res += "Stale NFS file handle"s; break;
		case 10071: res += "Too many levels of remote in path"s; break;
		case 10091: res += "Network system is unavailable"s; break;
		case 10092: res += "Winsock version out of range"s; break;
		case 10093: res += "WSAStartup not yet called"s; break;
		case 10101: res += "Graceful shutdown in progress"s; break;
		case 11001: res += "Host not found"s; break;
		case 11004: res += "No host data of that type was found"s; break;
		default:
			res = std::format("Unknown errror code: {}", error_num);
			break;
		}
		return res;
	}

	auto gf_saveUpto2GenerationsOf(std::filesystem::path file) -> void {
		auto s{ file.string() };
		if (auto found{ s.find_last_of(gkDotChar) }; found != std::string::npos) {
			s.insert(found, gkDigit0Str);
			const std::filesystem::path g0 = s;
			s.insert(found, gkDigit0Str);
			const std::filesystem::path g00 = s;
			if (exists(g0)) {
				copy_file(g0, g00, std::filesystem::copy_options::overwrite_existing);
			}
			if (exists(file)) {
				copy_file(file, g0, std::filesystem::copy_options::overwrite_existing);
			}
		} else {
			assert(false);
		}
	}

	auto gf_openLog() -> void {
		const std::filesystem::path nlog{ "Log.txt" };
		gf_saveUpto2GenerationsOf(nlog);
		if (g_log.openFile(nlog)) {
			g_log.line(lf_dateTimeStamp(time(nullptr)) + " (utc) "s + g_ProductName + " version " + gkCurrentVersion.toString());
#ifndef PRODUCTION
			g_log.line(to_string(gkCurrentVersion.toJson()));
#endif

		} else {
			throw SysErrorException(40, "Failed to write file "s + nlog.string());
		}
	}

	auto gf_logUnexpectedAndUnhandledException(std::string const& s) noexcept -> void {
		try {
			g_log.line("! Exception in "s + s);
		} catch (...) {
		}
	}
}
