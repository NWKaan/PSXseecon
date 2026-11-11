#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
	//
	// SsiFileReader 
	// provides very powerful (!) functionality for reading the a .ssi or a .lst file for 
	// presence of static SIOC variables in a certain range.
	//


	class SsiFileReader final {
	public:
		SsiFileReader(const Qcode static_var_from, const Qcode static_var_to);
		// detects static vars in the file in this range (inclusive)
		virtual ~SsiFileReader();
		SsiFileReader() = delete;
		SsiFileReader(const SsiFileReader& v) = delete;
		auto operator=(const SsiFileReader& v) -> SsiFileReader& = delete;
		SsiFileReader(const SsiFileReader&& v) = delete;
		auto operator=(const SsiFileReader&& v) -> SsiFileReader& = delete;

		auto openFile(std::filesystem::path const& ssi_file) noexcept -> bool;  // with automatic close file when objects life time ends.
		auto closeFile() noexcept -> void;

		auto collectStaticVarNumbers() -> void;  // returns the static variables in the range given in the constructor of this class.
		QcodeSet static_vars_;

	private:
		std::ifstream file_;
		const Qcode static_var_from_;
		const Qcode static_var_to_;
	};
} // namespace nwk

