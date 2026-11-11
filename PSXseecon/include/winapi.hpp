//
// Nico Kaan
#pragma once

// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
	[[nodiscard]] auto gf_myProgramFilesPath() -> std::filesystem::path;
	[[nodiscard]] auto gf_myProgramFilesX86Path() -> std::filesystem::path;

	auto gf_winMain(HINSTANCE hInstance, WNDPROC message_handler) noexcept -> int32_t;
	auto gf_postWindowsMessage(const UINT m) noexcept -> void;
	auto gf_MSG_EXIT() -> void;
}
