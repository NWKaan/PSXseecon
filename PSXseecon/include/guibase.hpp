#pragma once
//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
	class GuiBase {
	public:
		static constexpr int32_t kRowHeight{ 28 };
		static constexpr int32_t kMaxXY{ 30'000 };
		static constexpr int32_t x1{ 5 };
		static constexpr auto kColGray{ RGB(160, 160, 160) };
		static constexpr auto kColWhite{ RGB(255, 255, 255) };
		static constexpr auto kColBlack{ RGB(0, 0, 0) };
		static constexpr auto kColBlue{ RGB(153, 204, 255) };
		static constexpr auto kColRed{ RGB(255, 0, 0) };
		static constexpr auto kColOrange{ RGB(255, 165, 0) };
		static constexpr auto kColYellow{ RGB(255, 255, 0) };
		static constexpr auto kColGreen{ RGB(50, 205, 50) };
		virtual ~GuiBase();
		const int32_t kWindowWidth, kWindowMiddle, kWindowHeight;
		const int32_t x3, x2, x4, x5, x12, x23, x34, x45;

		[[nodiscard]] auto initInstance(const HINSTANCE hInstance, WCHAR const* szTitle) -> bool;
		auto restoreWindow() noexcept -> void;
		auto alwaysStayOnTop(const bool yes) noexcept -> void;
		auto saveWindowPos() -> void;
		auto redrawWindow() noexcept -> void;
		auto getMainWindow() const noexcept -> HWND;

		auto paintInfoMessageLine() -> void;
		auto paintErrorMessageLine() -> void;

		auto error(std::string_view message) -> void;
		auto line(std::string_view message) -> void;
		auto clearMessage() -> void;
		auto secondBeat() -> void;
		auto setNewVersionNumber(std::string const& newversion, const bool minor_update) -> void;  // if no minor, it is a patch update
		static auto userClosedApp() -> void;
		[[nodiscard]] inline auto greenOrYellow(const bool b) -> COLORREF {
			return b ? kColGreen : kColYellow;
		};
		[[nodiscard]] inline auto greenOrRed(const bool b) -> COLORREF {
			return b ? kColGreen : kColRed;
		};
		[[nodiscard]] inline auto blackOrYellow(const bool b) -> COLORREF {
			return b ? kColBlack : kColYellow;
		};
		[[nodiscard]] inline auto blackOrRed(const bool b) -> COLORREF {
			return b ? kColBlack : kColRed;
		}
		[[nodiscard]] inline auto yellowkOrRed(const bool b) -> COLORREF {
			return b ? kColYellow : kColRed;
		};

	protected:
		static constexpr int32_t kIndent{ 20 };
		static constexpr int32_t x1i = x1 + kIndent;
		static constexpr int32_t kTitleBarHeight{ 40 };
		static constexpr int32_t kQuarterRowheight{ kRowHeight / 4 };
		static constexpr int32_t kvo{ 7 };

		GuiBase(const int32_t windowwidth, const int32_t nrConfigRows, const int32_t nrDataRows, const int32_t nrInfoRows);

		GuiBase(GuiBase const& v) = delete;
		auto operator=(GuiBase const& v) -> GuiBase& = delete;
		GuiBase(GuiBase && v) = delete;
		auto operator=(GuiBase && v) -> GuiBase& = delete;

		auto prePaintWindow(const HDC hdc) -> void;
		auto paintMessageLine() -> void;

		auto fillMiddleBorderLeft(HBRUSH color) const noexcept -> void;
		auto fillMiddleBorderRight(HBRUSH color) const noexcept -> void;
		auto myTextOut(const int32_t x, const int32_t y, std::string const& s, COLORREF color) const -> void;
		auto myTextOut(const int32_t x, const int32_t y, std::string const& s, const bool black = true) const -> void;  // otherwise gray
		using TwoPoints = std::array<POINT, 2>;
		auto fullWidthLine(const int32_t y) noexcept -> TwoPoints;

		const RECT r_header_, r_config_, r_data_, r_status_, r_msgs_, r_middle_border_left_block_, r_middle_border_right_block_;
		const HBRUSH kWhiteBrush_, kRedBrush_, kBlueBrush_, kBlackBrush_, kGrayBrush_, kGreenBrush_, kYellowBrush_;
		const HPEN kWhitePen_, kBlackPen_, kGrayPen_;
		const TwoPoints p_config_, p_data_up_, p_status_, p_msgs_;

		const int32_t x2i = x2 + kIndent;
		const int32_t x3i = x3 + kIndent;
		const int32_t x4i = x4 + kIndent;
		const int32_t x12i = x12 + kIndent;
		const int32_t x23i = x23 + kIndent;
		const int32_t x34i = x34 + kIndent;
		const int32_t x45i = x45 + kIndent;

		const int32_t y1{ r_config_.bottom + kvo };
		const int32_t y2{ y1 + kRowHeight };
		const int32_t y3{ y2 + kRowHeight };
		const int32_t y4{ y3 + kRowHeight };
		const int32_t y5{ y4 + kRowHeight };
		const int32_t y6{ y5 + kRowHeight };
		const int32_t y7{ y6 + kRowHeight };
		const int32_t y8{ y7 + kRowHeight };
		const int32_t y9{ y8 + kRowHeight };
		const int32_t y10{ y9 + kRowHeight };
		const int32_t y11{ y10 + kRowHeight };
		const int32_t y12{ y11 + kRowHeight };
		const int32_t y13{ y12 + kRowHeight };
		const int32_t y14{ y13 + kRowHeight };
		const int32_t y15{ y14 + kRowHeight };
		const int32_t y16{ y15 + kRowHeight };
		const int32_t y17{ y16 + kRowHeight };
		const int32_t y18{ y17 + kRowHeight };
		const int32_t y19{ y18 + kRowHeight };
		const int32_t y20{ y19 + kRowHeight };
		const int32_t y21{ y20 + kRowHeight };
		const int32_t y22{ y21 + kRowHeight };
		const int32_t y23{ y22 + kRowHeight };
		const int32_t y24{ y23 + kRowHeight };
		const int32_t y25{ y24 + kRowHeight };
		const int32_t y26{ y25 + kRowHeight };
		const int32_t y27{ y26 + kRowHeight };
		const int32_t y28{ y27 + kRowHeight };
		const int32_t y29{ y28 + kRowHeight };
		const int32_t y1l{ y1 - kvo };
		const int32_t y2l{ y2 - kvo };
		const int32_t y3l{ y3 - kvo };
		const int32_t y4l{ y4 - kvo };
		const int32_t y5l{ y5 - kvo };
		const int32_t y6l{ y6 - kvo };
		const int32_t y7l{	y7- kvo };
		const int32_t y8l{ y8 - kvo };
		const int32_t y9l{ y9 - kvo };
		const int32_t y10l{ y10 - kvo };
		const int32_t y11l{	y11 - kvo };
		const int32_t y12l{ y12 - kvo };
		const int32_t y13l{ y13 - kvo };
		const int32_t y14l{ y14 - kvo };
		const int32_t y15l{ y15 - kvo };
		const int32_t y16l{ y16 - kvo };
		const int32_t y17l{ y17 - kvo };

		const int32_t kstatus_{ r_status_.top + kvo };

		std::mutex l_msgs_mutex_;
		std::string info_message_{};
		std::string error_message_{};
		std::string newversion_{};
		HDC hdc_ = nullptr;
		HWND main_window_ = nullptr;
		int32_t seconds_ = 0;
		int32_t secondOfLastMessage_ = 0;
		bool statusline_ = false;
		bool minor_update_ = false;
		std::atomic<bool> initialised_ = false;

	private:
		static auto checkAndCorrectXY(const int32_t x, const int32_t y) -> void;
	};
}
