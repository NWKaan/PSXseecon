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
	//
	// SiocParameters
	// is a class containg the parameters to connect to the IOCP server
	// and to the .ssi or .lst file that is run by SIOC.
	//
	class SiocParameters final {
	public:
		SiocParameters() = default;
		~SiocParameters() = default;

		SiocParameters(SiocParameters const& v) = default;
		auto operator=(SiocParameters const& v) -> SiocParameters& = default;

		auto toString() const -> std::string;

		StringPar hostaddress = { {}, "IOCP_HOSTADDRESS" };
		Uint16Par port = { 0, "IOCP_port" };
		StringPar configfile = { {gkQuestionmarkStr}, "CONFIG_FILE" };
	};

	using SiocParametersPtr = std::shared_ptr<SiocParameters>;

	//
	// gf_determineSiocParameters 
	// provides very powerful (!) functionality for reading the SIOC 
	// connection parameters, either from a sioc.ini file (using the gf_findPathToSioCFolder function above)
	// or from an <productname>.ini file in the MyDocuments folder or in the current folder. 
	// inicfg: if TRUE .ini else .cfg
	// Note: Parmeters in the <productname>.ini file (if found) have priority 
	// over parameters found in sioc.ini.
	//
	auto gf_determineSiocParameters() -> SiocParametersPtr;
} // namespace nwk

