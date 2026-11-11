//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
//
#include "stdafx.hpp"

namespace nwk {
	using namespace std::literals;

	Gui::Gui()
		:
		GuiBase(720, 2, 3, 0),
		r_cdu_({ 0, 0, 0, 0 }),
		cdu_screen_line_(),
		previous_cdu_screen_line_(),
		cdu_screen_color_(),
		previous_cdu_screen_color_(),
		cdu_green_{ RGB(50, 205, 50) } {
		initialised_.store(true, std::memory_order_relaxed);
	}

	Gui::~Gui() {
		main_window_ = nullptr;
		hdc_ = nullptr;
		if (hFontSmall_) {
			DeleteObject(hFontSmall_);
		}
		if (hFontLarge_) {
			DeleteObject(hFontLarge_);
		}
	}

	auto Gui::paintWindow(HDC hdc) -> void {
		const static std::string kHostaddressStr{ "hostaddress:"s };

		hdc_ = hdc;
		if (cduActive_) {
			SetBkColor(hdc_, kColBlack);
			SetBkMode(hdc_, OPAQUE);
			SetTextColor(hdc_, cdu_green_);
			fonttype_ = -1;
			updateScreen();
		} else {
			prePaintWindow(hdc);

			// SIOC related stuff
			myTextOut(x3 + kRowHeight, static_cast<int32_t>(r_config_.top) + 7, "SIOC"s, SIOC_status_ ? kColGreen : kColYellow);
			if (g_siocparameters) {
				myTextOut(x3 + kRowHeight, r_config_.top + kRowHeight + kvo, (*g_siocparameters).configfile.value_, kColWhite);
			} else {
				myTextOut(x3 + kRowHeight, r_config_.top + kRowHeight + kvo, gkQuestionmarkStr, kColWhite);
			}
			myTextOut(x3 + kRowHeight, y1, kHostaddressStr);
			if (g_siocparameters) {
				if ((*g_siocparameters).hostaddress.isSet_) {
					myTextOut(x4, y1, (*g_siocparameters).hostaddress.value_);
				}
			}
			writePort(x3 + kRowHeight, y2);
			if (g_siocparameters) {
				if ((*g_siocparameters).hostaddress.isSet_) {
					if ((*g_siocparameters).port.isSet_) {
						myTextOut(x4, y2, std::format("{}", (*g_siocparameters).port.value_));
					}
				}
			}
			myTextOut(x3 + kRowHeight, y3, "# variables:"s, kColBlack);
			if (numofvars_ > 0) {
				myTextOut(x4, y3, std::format("{}", numofvars_), kColBlack);
			}
			fillMiddleBorderRight(SIOC_status_ ? kGreenBrush_ : kRedBrush_);

			// PSX related stuff
			writePSX();
			myTextOut(x1, y1, kHostaddressStr);
			myTextOut(x2, y1, *g_CfgPSXHostAddress);
			writePort(x1, y2);
			myTextOut(x2, y2, "10747"s);

			fillMiddleBorderLeft(PSX_status_ ? kGreenBrush_ : kRedBrush_);

			paintMessageLine();
		}
	}

	auto Gui::myTextOutCh(const int32_t x, CONST int32_t y, const char c, const char color) -> void {
		const std::string s{ c };
		try {
			const auto ws{ utf8_to_wide(s) };
			const int length{ static_cast<int>(ws.length()) };
			auto bkcol_switched{ false };
			if (*g_CfgLCDCdu) {
				auto RGB{ kColWhite };
				if (color != gkSpaceChar) {
					switch (color) {
					case 'a':
						RGB = lcd_cdu_amber_;
						break;
					case 'b':
						RGB = lcd_cdu_blue_;
						break;
					case 'c':
						RGB = lcd_cdu_cyan_;
						break;
					case 'g':
						RGB = lcd_cdu_green_;
						break;
					case 'm':
						RGB = lcd_cdu_magenta_;
						break;
					case 'r':
						RGB = kColRed;
						break;
						//case 'w':
						//  RGB = lcd_cdu_white_;
						//  break;
						//case 'y':
						//  RGB = lcd_cdu_white_;
						//  break;
					default:
						break;
					}
					SetTextColor(hdc_, RGB);
				}
				if (color == 'y') {
					SetBkColor(hdc_, lcd_cdu_gray_);
					bkcol_switched = true;
				}
			}
			SetBkMode(hdc_, OPAQUE);
			TextOut(hdc_, x, y, ws.c_str(), length);
			if (*g_CfgCduBold) {
				// emulate BOLD by re-printing to the right and up:
				SetBkMode(hdc_, (c == gkSpaceChar) ? OPAQUE : TRANSPARENT);  // spaces always OPAQUE so it will erase previous char completely
				TextOut(hdc_, x + 1, y, ws.c_str(), length);
				TextOut(hdc_, x, y + 1, ws.c_str(), length);
			}
			if (bkcol_switched) {
				SetBkColor(hdc_, kColBlack);
			}
		} catch (const std::exception& e) {
			g_log.line(s);
			g_log.line(__func__ + gkSpaceStr + e.what());
		}
	}

	auto Gui::writePSX() -> void {
		myTextOut(x1, r_config_.top + kvo, "PSX"s, PSX_status_ ? kColGreen : kColYellow);
		myTextOut(x1, r_config_.top + kRowHeight + kvo, PSX_version_, kColWhite);
	}

	auto Gui::setSIOCStatus(const bool connected) noexcept -> void {
		SIOC_status_ = connected;
		redrawWindow();
	}

	auto Gui::setNumberOfPSXseeconVars(const uint32_t num) noexcept -> void {
		numofvars_ = num;
		redrawWindow();
	}

	auto Gui::setPSXVersion(std::string const& version) -> void {
		PSX_version_ = version;
		redrawWindow();
	}

	auto Gui::setSimVersion(std::string const& s) -> void {
		Sim_version_ = s;
		redrawWindow();
	}

	auto Gui::setPSXStatus(const bool connected) noexcept -> void {
		PSX_status_ = connected;
		redrawWindow();
	}

	auto Gui::writePort(const int32_t x, const int32_t y) const -> void {
		myTextOut(x, y, "port:"s);
	}

	auto lf_CoordinateNotFound(std::string const& what, int32_t def) {
		return what + std::format(" not found, default {} is taken.", def);
	}

	auto Gui::setCdu(const bool onoff) -> void {
		if ((onoff && !cduActive_) || (cduActive_ && !onoff)) {
			CfgInt l_cduX{ "MAIN_CDU_WINDOW_X"s, 400 , -30'000, 30'000 };
			CfgInt l_cduY{ "MAIN_CDU_WINDOW_Y"s, 20 , -30'000, 30'000 };
			if (onoff) {
				saveWindowPos();
				// set CDU window at its place
				l_cduX.readValue();
				l_cduY.readValue();
				r_cdu_ = { 0, 0, static_cast<LONG>(*g_CfgPSXseeconAsCduWindowWidth), static_cast<LONG>(*g_CfgPSXseeconAsCduWindowHeight) };
				SetWindowPos(main_window_, nullptr, *l_cduX, *l_cduY, *g_CfgPSXseeconAsCduWindowWidth, *g_CfgPSXseeconAsCduWindowHeight, SWP_SHOWWINDOW);
				FillRect(hdc_, &r_cdu_, kBlackBrush_);
				CDULineDistance_ = *g_CfgCduScreenHeight / static_cast<int32_t>(kCDULines);
				cdu_green_ = RGB(*g_CfgCduColorR, *g_CfgCduColorG, *g_CfgCduColorB);
				// Note FW_BOLD has no effect on Aerowinx fonts...ANTIALIASED_QUALITY has...
				hFontSmall_ = CreateFont(-*g_CfgFontSize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, TEXT("AEROWINX MCDU SMALL"));
				hFontLarge_ = CreateFont(-*g_CfgFontSize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, TEXT("AEROWINX MCDU LARGE"));
				if (!hFontSmall_) {
					g_log.line("? hFontSmall_ == NULL"s);
				}
				if (!hFontLarge_) {
					g_log.line("? hFontLarge_  == NULL"s);
				}
				for (auto& line : cdu_screen_line_) {
					line = k48Empty;
				}
				if (*g_CfgLCDCdu) {
					for (auto& line : cdu_screen_color_) {
						line.clear();
					}
				}
				if (!*g_CfgCduBorder) {
					SetWindowLongPtr(main_window_, GWL_STYLE, GetWindowLongPtr(main_window_, GWL_STYLE) & ~(WS_BORDER | WS_DLGFRAME | WS_THICKFRAME));
				}
				g_log.line(g_ProductName + " window changed into CDU screen."s);
				cduActive_ = true;
			} else {
				RECT r;
				GetWindowRect(main_window_, &r);
				l_cduX.setValue(r.left);
				l_cduY.setValue(r.top);
				cduActive_ = false;
				SetWindowPos(main_window_, nullptr, *g_CfgkWindowX, *g_CfgkWindowY, kWindowWidth, kWindowHeight, SWP_SHOWWINDOW);
				g_log.line(g_ProductName + " window back to normal."s);
			}
			redrawWindow();
		}
	}

	auto Gui::selectFont(const bool big) noexcept -> void {  // true = big, false = small
		if (big) {
			if (2L != fonttype_) {
				if (const auto h0{static_cast<HFONT>(SelectObject(hdc_, hFontLarge_))}; h0 && HGDI_ERROR != h0) {
					fonttype_ = 2L;
				}
			}
		} else {
			if (1 != fonttype_) {
				if (const auto h0{static_cast<HFONT>(SelectObject(hdc_, hFontSmall_))}; h0 && HGDI_ERROR != h0) {
					fonttype_ = 1;
				}
			}
		}
	}

	auto Gui::fill(CduLine const& cduline) -> void {
		if (cduline.num < kCDULines) {   // old green cdu line or NG text version
			cdu_screen_line_.at(cduline.num) = cduline.text;
			if (*g_CfgLCDCdu) {
				previous_cdu_screen_line_.at(cduline.num) = cduline.text;
				cdu_screen_color_.at(cduline.num) = previous_cdu_screen_color_.at(cduline.num);
			}
		} else {  // must be NG color version
			const auto i{ cduline.num - kCDULines };
			cdu_screen_color_.at(i) = cduline.color;
			previous_cdu_screen_color_.at(i) = cduline.color;
			cdu_screen_line_.at(i) = previous_cdu_screen_line_.at(i);
		}
	}

	// the next two functions work on updating_ protected by mutex2

	auto Gui::updateScreen() -> void {
		if (updating_.empty()) {  // caused by windows paint message window due to resizing/repositioning, and some more ...?
			if (blanked_) {
#ifdef TRACE_CDU
				g_log.line("Paint blank screen");
#endif
				FillRect(hdc_, &r_cdu_, kBlackBrush_);  // repeat
			} else {
#ifdef TRACE_CDU
				g_log.line("Paint WriteCduScreen");
#endif
				writeCduScreen();
			}
		} else {
			for (auto const& cduline : updating_) {
				if (kBlankScreen == cduline.num) {
					g_log.line("BLANK screen");
					blanked_ = true;
					FillRect(hdc_, &r_cdu_, kBlackBrush_);
				} else if (kWriteScreen == cduline.num) {
#ifdef TRACE_CDU
					g_log.line("WriteCduScreen");
#endif
					blanked_ = false;
					writeCduScreen();
				} else if (blanked_) {
					// fill while blanked
					fill(cduline);
				} else {
					const auto i{ (*g_CfgLCDCdu && cduline.num >= kCDULines) ? cduline.num - kCDULines : cduline.num };
					cdu_screen_line_.at(i) = k48Empty;  // clean the line first
					if (*g_CfgLCDCdu) {
						cdu_screen_color_.at(i).clear();
					}
					writeCduLine(i);
					fill(cduline);
#ifdef TRACE_CDU
					g_log.line("WriteCduLINE " + std::format("{}", cduline.num) + "=" + cduline.text + ", colors:"s + cduline.color);
#endif
					writeCduLine(i);
				}
			}
			updating_.clear();
		}
	}

	auto Gui::upgrade2() -> void {
		const std::lock_guard mut(mutex2_);
		updating_ = updates_;
		updates_.clear();
	}

	//
	// The next 4 functions work on the same variable updates_, so  it is being protected by a mutex
	//

	auto Gui::changeCdu() -> void {
		const std::lock_guard mut(mutex_);
		if (cduActive_) {
			auto blank{ false };
			if (*g_CfgCduLScreen) {
				blank = CDUL_blank_;
			} else if (*g_CfgCduCScreen) {
				blank = CDUC_blank_;
			} else {
				blank = CDUR_blank_;
			}
			updates_.clear();
			updates_.emplace_back(CduLine{ .num = kBlankScreen, .text{gkEmptyStr}, .color{gkEmptyStr} });
			for (auto i{ 0U }; i < kCDULines; ++i) {
				updates_.emplace_back(CduLine{ .num = i, .text{composeCduScreenLine(g_str[
				  static_cast<size_t>(static_cast<uint32_t>(*g_CfgCduLScreen ? Qcode::Qs62 : (*g_CfgCduCScreen ? Qcode::Qs76 : Qcode::Qs90))) + static_cast<size_t>(i)], i)},
				  .color{*g_CfgLCDCdu ? gkEmptyStr :
				  Gui::composeCduColorLine(g_str[
				  static_cast<size_t>(static_cast<uint32_t>(*g_CfgCduLScreen ? Qcode::Qs500 : (*g_CfgCduCScreen ? Qcode::Qs514 : Qcode::Qs528))) + static_cast<size_t>(i)])} });
			}
			updates_.emplace_back(CduLine{ .num = blank ? kBlankScreen : kWriteScreen, .text{gkEmptyStr}, .color{gkEmptyStr} });
		}
	}

	auto Gui::setCduBlankWrite(const bool blank) -> void {
		const std::lock_guard mut(mutex_);
		updates_.emplace_back(CduLine{ .num = blank ? kBlankScreen : kWriteScreen, .text{gkEmptyStr},.color{ gkEmptyStr} });
	}

	auto Gui::setCduLBlankWrite(const bool blank) -> void {
		CDUL_blank_ = blank;
		setCduBlankWrite(blank);
	};

	auto Gui::setCduCBlankWrite(const bool blank) -> void {
		CDUC_blank_ = blank;
		setCduBlankWrite(blank);
	};

	auto Gui::setCduRBlankWrite(const bool blank) -> void {
		CDUR_blank_ = blank;
		setCduBlankWrite(blank);
	};

	auto Gui::setCduTextLine(const uint32_t linenumber, std::string const& s) -> void {
		const auto text{ composeCduScreenLine(s, linenumber) };
		const std::lock_guard mut(mutex_);
		const CduLine cdl{ .num = linenumber, .text{text}, .color{gkEmptyStr} };
		updates_.push_back(cdl);
	}

	auto Gui::setCduColorLine(const uint32_t linenumber, std::string const& s) -> void {
		const auto colors{ Gui::composeCduColorLine(s) };
		const std::lock_guard mut(mutex_);
		const CduLine cdl{ .num = linenumber + kCDULines, .text{gkEmptyStr}, .color{colors } };
		updates_.push_back(cdl);
	}

	auto Gui::upgrade() -> bool {
		bool res{ false };
		const std::lock_guard mut(mutex_);
		if (!updates_.empty()) {
			upgrade2();
			res = true;
		}
		return res;
	}

	//
	// End of mutex use
	//

	auto Gui::areThereCduChanges() -> void {
		if (cduActive_) {
			if (upgrade()) {
				redrawWindow();
			}
		}
	}

	auto Gui::writeCduLine(const uint32_t i) -> void {
		static const auto kCharWidth{ *g_CfgCduScreenWidth / static_cast<int32_t>(kCDUNumScreenChars) };
		const auto l{ cdu_screen_line_.at(i) };
		const auto lc{ cdu_screen_color_.at(i) };
		const auto verticalpos{ (static_cast<int32_t>(i) * CDULineDistance_) + *g_CfgCduScreenY };
		auto x{ *g_CfgCduScreenX };
		for (auto ii{ 0U }; ii < kCDUNumScreenChars; ++ii) {
			const char c{ l.at(ii) };
			const char cc{ lc.empty() ? 'w' : lc.at(ii) };
			const auto font{ (gkPlusChar == l.at(static_cast<size_t>(ii) + static_cast<size_t>(kCDUNumScreenChars))) };
			selectFont(font);
			myTextOutCh(x, verticalpos + (!font ? *g_CfgSmallFontYOffset : 0), c, cc);
			x += kCharWidth;
		}
	}

	auto Gui::writeCduScreen() -> void {
		for (auto i{ 0U }; i < kCDULines; i++) {
#ifdef TRACE_CDU
			g_log.line("WriteCduScreen line " + std::format("{}", i) + cdu_screen_line_.at(i) + ", colors: "s + cdu_screen_color_.at(i));
#endif
			writeCduLine(i);
		}
	}

	auto Gui::composeCduScreenLine(std::string l, const int32_t i) -> std::string {
		const bool big_all{ 0 == (i % 2U) || 13U == i};
		if (l.empty()) {
			l = k48Empty;
		} else if (l.length() < kCDUNumScreenChars) {
			// scratchpad, add spaces
			const uint32_t n{ kCDUNumScreenChars - static_cast<uint32_t>(l.length()) };
			for (auto ii{ 0U }; ii < n; ++ii) {
				l.push_back(gkUnderscoreChar);
			}
			l += k24Plus;
		} else if (l.length() == kCDUNumScreenChars) {
			l += big_all ? k24Plus : k24Min;
		} else if (l.length() < (2 * kCDUNumScreenChars)) {
			// repeat last char up to the end
			const char c{ l.back() };
			const uint32_t n{ (2 * kCDUNumScreenChars) - static_cast<uint32_t>(l.length()) };
			for (auto iii{ 0U }; iii < n; ++iii) {
				l.push_back(c);
			}
		}
		for (auto ii{ 0U }; ii < kCDUNumScreenChars; ++ii) {
			const char c{l.at(ii)};
			if (gkUnderscoreChar == c) {
				l.at(ii) = gkSpaceChar;      // space
			} else if (gkbChar == c) {  // box
				l.at(ii) = gkcChar;
			} else if (gktChar == c) {  // triangle
				l.at(ii) = gknChar;
			} else if (gkuChar == c) {  // up arrow
				l.at(ii) = gkUpArrowChar;
			} else if (gklChar == c) {  // left arrow
				l.at(ii) = gkUnderscoreChar;
			} else if (gkrChar == c) {  // right arrow
				l.at(ii) = gkeChar;
			} else if (gkoChar == c) {  // degree
				l.at(ii) = gkbChar;
			}                           // d = down arrow = d 
		}
		return l;
	}

	auto Gui::composeCduColorLine(std::string l) -> std::string {
		if (!l.empty()) {
			if (l.length() < kCDUNumScreenChars) {
				const auto lastchar{ l.back() };
				const uint32_t n{ kCDUNumScreenChars - static_cast<uint32_t>(l.length()) };
				for (auto ii{ 0U }; ii < n; ++ii) {
					l.push_back(lastchar);
				}
			}
		}
		return l;
	}
}
