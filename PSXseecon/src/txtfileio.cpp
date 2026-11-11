//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

#include "stdafx.hpp"

namespace nwk {
	TxtFileReader::~TxtFileReader() noexcept {
		try {
			closeFile();
		} catch (...) {
		}
	};

  auto TxtFileReader::openFile(std::filesystem::path const& path_name) -> bool {
    if (file_.open(path_name.string().c_str()); !file_.is_open()) {
      file_.clear();
      return false;
    }
    return true;
  }

  auto TxtFileReader::closeFile() -> void {
		if (file_.is_open()) {
			file_.close();
		}
	};

	auto TxtFileReader::toStringVector(std::vector<std::string>& sv) -> void {
		for (std::string l; getline(file_, l); ) {
			sv.push_back(l);
		}
		file_.clear();
	}


	auto TxtFileReader::toStringList(std::list<std::string>& li) -> void {
		for (std::string l; getline(file_, l); ) {
			li.push_back(l);
		}
		file_.clear();
	}

	auto TxtFileReader::toStringList(std::forward_list<std::string>& li) -> void {
		for (std::string l; getline(file_, l); ) {
			li.push_front(l);
		}
		file_.clear();
	}

	auto TxtFileReader::getLine(std::string& s) -> bool {
		return getline(file_, s) ? true : false;  // NOLINT
	}

	auto TxtFileReader::gotoBeginOfFile() -> void {
		try {
			constexpr std::streamoff so{ 0 };
			file_.seekg(so, std::ios::beg);
		} catch (...) {
		}
	}

	auto TxtFileReader::findArg(const std::regex& r) -> std::string {
		gotoBeginOfFile();
		std::string result;
		for (std::string l; getline(file_, l); ) {
			if (std::smatch m; regex_match(l, m, r)) {
				result = m[1];
				break;
			}
		}
		file_.clear();
		return result;
	}

	auto TxtFileReader::findArgIntAsBool(std::string const& argument) -> std::string {
		const auto r{ std::regex{ R"(\s*)" + argument + R"(\s*=\s*(\d)\s*)", std::regex_constants::icase } };   // minus sign added Feb 25, 2015
		return findArg(r);
	}

	auto TxtFileReader::findArgNumber(std::string const& argument) -> std::string {
		const auto r{ std::regex{ R"(\s*)" + argument + R"(\s*=\s*(-?\d+)\s*)", std::regex_constants::icase } };   // minus sign added Feb 25, 2015
		return findArg(r);
	}

	auto TxtFileReader::findArgString(std::string const& argument) -> std::string {
		const auto r{ std::regex{ R"(\s*)" + argument + R"(\s*=\s*(.+))", std::regex_constants::icase } };
		return findArg(r);
	}

	auto TxtFileReader::findArgIPaddress(std::string const& argument) -> std::string {
		const auto r{ std::regex{ R"(\s*)" + argument + R"(\s*=\s*(\d+\.\d+\.\d+\.\d+)\s*)", std::regex_constants::icase } };
		return findArg(r);
	}





	TxtFileWriter::~TxtFileWriter() noexcept {
		try {
			closeFile();
		} catch (...) {
		}
	};

	auto TxtFileWriter::openFile(std::filesystem::path const& path_name) -> bool {
		if (file_.open(path_name.string().c_str()); !file_.is_open()) {
			file_.clear();
			return false;
		}
		return true;
	}

	auto TxtFileWriter::closeFile() -> void {
		if (file_.is_open()) {
			file_.flush();
			file_.close();
		}
	}

	auto TxtFileWriter::writeString(std::string_view s) -> void {
		if (file_.is_open()) {
			file_ << s;
		}
	}

	auto TxtFileWriter::writeStringList(const std::list<std::string>& li) -> void {
		if (file_.is_open()) {
			for (auto const& s : li) {
				(file_ << s) << std::endl;
			};
		}
	}

	auto TxtFileWriter::line(std::string_view s) -> void {
		if (file_.is_open()) {
			const std::lock_guard lk(mutex_);
			(file_ << s) << std::endl;
		}
	}

	auto TxtFileWriter::newLine() -> void {
		if (file_.is_open()) {
			const std::lock_guard lk(mutex_);
			file_ << std::endl;
		}
	}
}

