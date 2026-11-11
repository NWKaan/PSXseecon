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

	auto gf_setPSXTfcExternal(const bool on) -> void;
	auto gf_sendCDUClearDelayed(const Qcode var) -> void;
	auto gf_isInternalQhvar(const Qcode var) -> bool;

	inline TxtFileWriter g_log;
	inline Gui g_gui;

	inline constinit SiocParametersPtr g_siocparameters{ nullptr };

	inline constexpr auto g_ProductName{ "PSXseecon" };

}

