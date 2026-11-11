//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
#include "stdafx.hpp"

namespace nwk {
	using namespace std::literals;

	GuiBase::GuiBase(const int32_t windowwidth, const int32_t nrConfigRows, const int32_t nrDataRows, const int32_t nrInfoRows)
		:
		kWindowWidth{ windowwidth },
		kWindowMiddle{ windowwidth / 2 - 15 },
		kWindowHeight{ ((nrConfigRows + nrDataRows + nrInfoRows + 2) * kRowHeight) + kTitleBarHeight },
		x3{ kWindowMiddle + x1 },
		x2{ (x1 + x3) / 2 },
		x4{ x3 + (x2 - x1) },
		x5{ x4 + (x2 - x1) / 2 },
		x12{ (x1 + x2) / 2 },
		x23{ (x2 + x3) / 2 },
		x34{ (x3 + x4) / 2 },
		x45{ x4 + (x34 - x23) / 2 },
		r_header_{ 0, 0, kWindowWidth, kRowHeight },
		r_config_{ 0, r_header_.bottom, kWindowWidth, r_header_.bottom + nrConfigRows * kRowHeight },
		r_data_{ 0, r_config_.bottom, kWindowWidth, r_config_.bottom + nrDataRows * kRowHeight },
		r_status_{ 0, r_data_.bottom, kWindowWidth, r_data_.bottom + nrInfoRows * kRowHeight },
		r_msgs_{ 0, r_status_.bottom, kWindowWidth, r_status_.bottom + kRowHeight },
		r_middle_border_left_block_({ kWindowMiddle - kRowHeight, r_config_.top + 1 , kWindowMiddle, r_data_.bottom + 1 }),
		r_middle_border_right_block_({ kWindowMiddle, r_config_.top + 1 , kWindowMiddle + kRowHeight, r_data_.bottom + 1 }),
		kWhiteBrush_{ CreateSolidBrush(kColWhite) },
		kRedBrush_{ CreateSolidBrush(kColRed) },
		kBlueBrush_{ CreateSolidBrush(kColBlue) },
		kBlackBrush_{ CreateSolidBrush(kColBlack) },
		kGrayBrush_{ CreateSolidBrush(kColGray) },
		kGreenBrush_{ CreateSolidBrush(kColGreen) },
		kYellowBrush_{ CreateSolidBrush(kColYellow) },
		kWhitePen_{ CreatePen(PS_SOLID, 1, kColWhite) },
		kBlackPen_{ CreatePen(PS_SOLID, 1, kColBlack) },
		kGrayPen_{ CreatePen(PS_SOLID, 1, kColGray) },
		p_config_({ POINT{0, r_config_.top}, POINT{kWindowWidth, r_config_.top} }),
		p_data_up_({ POINT{0, r_data_.top + 1}, POINT{kWindowWidth, r_data_.top + 1} }),
		p_status_({ POINT{0, r_status_.top}, POINT{kWindowWidth, r_status_.top} }),
		p_msgs_({ POINT{0, r_msgs_.top + 1}, POINT{kWindowWidth, r_msgs_.top + 1} }),
		statusline_{ nrInfoRows > 0 } {
	}

	GuiBase::~GuiBase() {
		DeleteObject(kWhiteBrush_);
		DeleteObject(kRedBrush_);
		DeleteObject(kBlueBrush_);
		DeleteObject(kBlackBrush_);
		DeleteObject(kGrayBrush_);
		DeleteObject(kGreenBrush_);
		DeleteObject(kYellowBrush_);
		DeleteObject(kWhitePen_);
		DeleteObject(kBlackPen_);
		DeleteObject(kGrayPen_);
		main_window_ = nullptr;
		hdc_ = nullptr;
	}

	auto GuiBase::initInstance(HINSTANCE hInstance, WCHAR const* szTitle) -> bool {
		auto b{ false };
		auto cnt{ 1 };
		while (!initialised_.load(std::memory_order_relaxed)) {
			if (++cnt; cnt >= 20) {
				return FALSE;
			}
			std::this_thread::sleep_for(50ms);
		}

		g_CfgkWindowX.readValue();
		g_CfgkWindowY.readValue();
		GuiBase::checkAndCorrectXY(g_CfgkWindowX.init_val_, g_CfgkWindowY.init_val_);
		if (main_window_ = CreateWindow(szTitle, szTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, *g_CfgkWindowX, *g_CfgkWindowY,
			kWindowWidth, kWindowHeight, nullptr, nullptr, hInstance, nullptr); main_window_) {
			ShowWindow(main_window_, SW_SHOW);
			if (UpdateWindow(main_window_)) {
				b = true;
			}
		}
		return b;
	}

	auto GuiBase::restoreWindow() noexcept -> void {
		if (main_window_) {
			ShowWindow(main_window_, SW_RESTORE);
		}
	}

	auto GuiBase::alwaysStayOnTop(const bool yes) noexcept -> void {
		// change the Z order of the window.
		::SetWindowPos(
			main_window_,        // handle to window
			yes ? HWND_TOPMOST : HWND_NOTOPMOST,   // placement-order handle
			0,      // horizontal position
			0,       // vertical position
			0,	   // width
			0,  // height
			SWP_NOMOVE | SWP_NOSIZE // window-positioning options
		);
	}

	auto GuiBase::prePaintWindow(HDC hdc) -> void {
		hdc_ = hdc;
		FillRect(hdc_, &r_header_, kBlackBrush_);
		FillRect(hdc_, &r_config_, kBlackBrush_);
		FillRect(hdc_, &r_data_, kBlueBrush_);
		if (statusline_) {
			FillRect(hdc_, &r_status_, kBlackBrush_);
		}
		FillRect(hdc_, &r_msgs_, kGreenBrush_);
		SelectObject(hdc_, kWhitePen_);
		Polyline(hdc_, p_config_.data(), 2);
		Polyline(hdc_, p_data_up_.data(), 2);
		if (statusline_) {
			Polyline(hdc_, p_status_.data(), 2);
		}
		// header
		SetBkMode(hdc_, TRANSPARENT);
		const auto nv{ !newversion_.empty() };
		myTextOut(3, kvo, "Version "s + gkCurrentVersion.toString() + (nv ? " (new=" + newversion_ + ")"s : gkEmptyStr), nv ? (minor_update_ ? kColOrange : kColYellow) : kColGreen);
		myTextOut((kWindowWidth / 2L) - 70, kvo, g_ProductName, kColGreen);
		myTextOut(x4, kvo, "(c) 2007-2026 Nico Kaan"s, kColGreen);
	}

	auto GuiBase::paintMessageLine() -> void {
		const std::lock_guard muti(l_msgs_mutex_);
		if (!error_message_.empty()) {
			paintErrorMessageLine();
		} else {
			paintInfoMessageLine();
		}
	}

	auto GuiBase::setNewVersionNumber(std::string const& newversion, const bool minor_update) -> void {
		newversion_ = newversion;
		minor_update_ = minor_update;
		redrawWindow();
	}

	auto GuiBase::checkAndCorrectXY(const int32_t x, const int32_t y) -> void {
		if ((*g_CfgkWindowX == g_CfgkWindowX.min_val_) || (*g_CfgkWindowX == g_CfgkWindowX.max_val_)) {   // if closed from task bar ... #fix 15.5
			g_CfgkWindowX.setValue(x);
		}
		if ((*g_CfgkWindowY == g_CfgkWindowY.min_val_) || (*g_CfgkWindowY == g_CfgkWindowY.max_val_)) {   // if closed from task bar ... #fix 15.5
			g_CfgkWindowY.setValue(y);
		}
	}

	auto GuiBase::saveWindowPos() -> void {
		RECT r;
		GetWindowRect(main_window_, &r);

		const auto xSav{ *g_CfgkWindowX };
		const auto ySav{ *g_CfgkWindowY };
		g_CfgkWindowX.setValue(r.left);
		g_CfgkWindowY.setValue(r.top);

		GuiBase::checkAndCorrectXY(xSav, ySav);
	}

	auto GuiBase::redrawWindow() noexcept -> void {
		InvalidateRect(main_window_, nullptr, FALSE);
		UpdateWindow(main_window_);
	}

	auto GuiBase::fullWidthLine(const int32_t y) noexcept -> Gui::TwoPoints {
		return TwoPoints{ POINT{0, y}, POINT{kWindowWidth, y} };
	}

	auto GuiBase::myTextOut(const int32_t x, const int32_t y, std::string const& s, const bool black) const -> void {
		myTextOut(x, y, s, black ? kColBlack : kColGray);
	}

	auto GuiBase::myTextOut(const int32_t x, const int32_t y, std::string const& s, COLORREF color) const -> void {
		SetTextColor(hdc_, color);
		try {
			const auto ws{ utf8_to_wide(s) };
			TextOut(hdc_, x, y, ws.c_str(), static_cast<int>(ws.length()));
		} catch (const std::exception& e) {
			g_log.line(s + std::string(e.what()));
		}
	}

	auto GuiBase::getMainWindow() const noexcept -> HWND {
		return main_window_;
	}

	auto GuiBase::paintInfoMessageLine() -> void {
		FillRect(hdc_, &r_msgs_, kGreenBrush_);
		if (!info_message_.empty()) {
			myTextOut(3, r_msgs_.top + kvo, info_message_, kColYellow);
		}
	}

	auto GuiBase::paintErrorMessageLine() -> void {
		FillRect(hdc_, &r_msgs_, kRedBrush_);
		myTextOut(3, r_msgs_.top + kvo, error_message_, kColYellow);
	}

	auto GuiBase::error(std::string_view message) -> void {
		const std::lock_guard mut(l_msgs_mutex_);
		info_message_.clear();
		secondOfLastMessage_ = 0;
		error_message_ = message;
		g_log.line("Error "s + error_message_);
		paintErrorMessageLine();
		InvalidateRect(main_window_, &r_msgs_, TRUE);
	}

	auto GuiBase::line(std::string_view message) -> void {
		const std::lock_guard mut(l_msgs_mutex_);
		if (error_message_.empty()) {
			error_message_.clear();
			info_message_ = message;
			secondOfLastMessage_ = message.empty() ? 0 : (seconds_ > 0 ? seconds_ : 1);
			if (!message.empty()) {
				g_log.line(info_message_);
			}
			paintInfoMessageLine();
			InvalidateRect(main_window_, &r_msgs_, TRUE);
		}
	}

	auto GuiBase::secondBeat() -> void {
		++seconds_;
		if (secondOfLastMessage_ > 0) {
			if (seconds_ - secondOfLastMessage_ >= 25) {
				clearMessage();
			}
		}
	}

	auto GuiBase::clearMessage() -> void {
		line(gkEmptyStr);
	}

	auto GuiBase::fillMiddleBorderLeft(HBRUSH color) const noexcept -> void {
		FillRect(hdc_, &r_middle_border_left_block_, color);
	}

	auto GuiBase::fillMiddleBorderRight(HBRUSH color) const noexcept -> void {
		FillRect(hdc_, &r_middle_border_right_block_, color);
	}

	auto GuiBase::userClosedApp() -> void {
		g_log.line(std::format("{} User closed {}", gf_UTCTime(), g_ProductName));
	}
}
