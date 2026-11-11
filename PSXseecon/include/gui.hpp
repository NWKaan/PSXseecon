#pragma once

// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {

	class Gui final : public GuiBase {
	public:
		Gui();
		~Gui();
		Gui(const Gui& v) = delete;
		auto operator=(const Gui& v) -> Gui& = delete;
		Gui(const Gui&& v) = delete;
		auto operator=(const Gui&& v) -> Gui& = delete;
		auto paintWindow(HDC hdc) -> void;
		auto changeCdu() -> void;

		auto setSIOCStatus(const bool connected) noexcept -> void;
		auto setNumberOfPSXseeconVars(const uint32_t num) noexcept -> void;

		auto setCdu(const bool onoff) -> void;
		auto setCduBlankWrite(const bool blank) -> void;
		auto setCduLBlankWrite(const bool blank) -> void;
		auto setCduCBlankWrite(const bool blank) -> void;
		auto setCduRBlankWrite(const bool blank) -> void;
		auto setCduTextLine(const uint32_t linenumber, std::string const& s) -> void;
		auto setCduColorLine(const uint32_t linenumber, std::string const& s) -> void;
		auto areThereCduChanges() -> void;

		auto setPSXStatus(const bool connected) noexcept -> void;
		auto setPSXVersion(std::string const& version) -> void;
		auto setSimVersion(std::string const& s) -> void;

	private:
		static constexpr uint32_t kCDULines{ 14U };
		static constexpr uint32_t kCDUNumScreenChars{ 24U };
		static constexpr uint32_t kBlankScreen{ 98U };
		static constexpr uint32_t kWriteScreen{ 99U };
		const std::string k24Plus = "++++++++++++++++++++++++";
		const std::string k24Min = "------------------------";
		const std::string k24Empty = "                        ";
		const std::string k48Empty = k24Empty + k24Plus;

		auto writePSX() -> void;
		auto selectFont(const bool big) noexcept -> void;  // true = big, false = small
		auto myTextOutCh(const int32_t x, CONST int32_t y, const char c, const char color) -> void;
		auto writeCduLine(const uint32_t i) -> void;
		auto writeCduScreen() -> void;
		auto composeCduScreenLine(std::string l, const int32_t i) -> std::string;
		static auto composeCduColorLine(std::string l) -> std::string;
		auto upgrade() -> bool;
		auto upgrade2() -> void;
		auto updateScreen() -> void;
		auto writePort(const int32_t x, const int32_t y) const -> void;
		RECT r_cdu_;
		uint32_t numofvars_{ 0 };
		int32_t CDULineDistance_{ 0 };
		bool cduActive_{ false };
		HFONT hFontSmall_ = nullptr;
		HFONT hFontLarge_ = nullptr;
		HFONT hFontDef_ = nullptr;
		bool SIOC_status_ = false;
		int32_t fonttype_{ -1 };  // -1 = default, 0 = small, 1 = big
		std::mutex mutex_;
		std::mutex mutex2_;
		struct CduLine {
			uint32_t num;
			std::string text;
			std::string color;  // in NG FMC: if empty white, if not NG FMC: default green
		};
		auto fill(CduLine const& cduline) -> void;
		std::vector<CduLine> updates_;
		std::vector<CduLine> updating_;
		std::array<std::string, kCDULines> cdu_screen_line_;
		std::array<std::string, kCDULines> previous_cdu_screen_line_;
		std::array<std::string, kCDULines> cdu_screen_color_;
		std::array<std::string, kCDULines> previous_cdu_screen_color_;
		COLORREF cdu_green_;
		COLORREF lcd_cdu_amber_{ RGB(255, 0, 255) };
		COLORREF lcd_cdu_blue_{ RGB(0, 0, 255) };
		COLORREF lcd_cdu_cyan_{ RGB(0, 255, 255) };
		COLORREF lcd_cdu_green_{ RGB(0, 255, 0) };
		COLORREF lcd_cdu_magenta_{ RGB(255, 0, 255) };
		COLORREF lcd_cdu_gray_{ RGB(128, 128, 128) };

		bool PSX_status_ = false;
		std::string Sim_version_;
		std::string PSX_version_;
		bool blanked_ = true;
		bool CDUL_blank_ = false;
		bool CDUC_blank_ = false;
		bool CDUR_blank_ = false;
	};

} // namespace nwk

