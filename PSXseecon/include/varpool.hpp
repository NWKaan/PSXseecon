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
	class VarPoolImpl;

	class VarPool final {
	public:
		VarPool();
		~VarPool() = default;
		VarPool(const VarPool& v) = delete;
		auto operator=(const VarPool& v) -> VarPool& = delete;

		auto configurePool(QcodeSet const& vars_found_in_configfile) const -> void;
		auto preInitAllVars() const -> void;
		auto getInicio() const noexcept -> QcodeSet const&;
		auto getInicioPsx() const noexcept -> QcodeSet const&;
    auto getInternalvarsAdded() const noexcept ->QcodeSet const&;
		auto getVarPtr(const Qcode q) const -> const SharedVarPtr;
		auto getNumberOfVars() const noexcept -> uint32_t;
		auto getVarNumbersForLog() const noexcept -> const std::list<std::string>&;

#ifdef LIST_SIOC_VARS
    auto generateSIOCVarsList() -> void;
#endif

	private:
		const std::shared_ptr<VarPoolImpl> pImpl_;
	};
}
