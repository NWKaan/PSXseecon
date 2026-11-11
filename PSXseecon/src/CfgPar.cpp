//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
#include "stdafx.hpp"

namespace nwk {
	auto gft_logline(std::string const& s) -> void {
		g_log.line(s);
	}

	auto gf_updateCfgPar(std::string const& arg, std::string const& value) -> void {
		const auto newPar{ arg + gkEqualStr + value };
		g_log.line(newPar);

		if (TxtFileReader t; t.openFile(gkProductNameCfgPath)) {
			std::list<std::string> li;
			t.toStringList(li);
			auto found{ false };
			const std::regex r{ R"(\s*(\w+)\s*=\s*(.*))" };  // just like in findArgString of TxtFileReader AND also negative numbers
			for (auto& line : li) {
				if (std::smatch m; regex_match(line, m, r)) {
					if (arg == m[1]) {
						found = true;
						line = newPar;
						break;
					}
				}
			};
			if (!found) {  // add at the end
				li.push_back(newPar);
			}
			t.closeFile();
			li.sort();
			li.unique();
			remove(gkProductNameCfgPath);
			if (TxtFileWriter w; w.openFile(gkProductNameCfgPath)) {
				w.writeStringList(li);
			}
		} else { // create a new one
			if (TxtFileWriter w; w.openFile(gkProductNameCfgPath)) {
				w.writeString(newPar);
			}
		}
	}
}
