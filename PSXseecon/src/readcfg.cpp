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

#pragma warning(push)
#pragma warning(disable:26426)
	CfgBool g_CfgCduBold, g_CfgCduBorder, g_CfgCduLScreen, g_CfgCduCScreen, g_CfgCduRScreen, g_CfgInvertHfSensL, g_CfgLCDCdu,
		
		g_CfgInvertBigFreqKnobL, g_CfgInvertSmallFreqKnobL, g_CfgInvertHfSensC, g_CfgInvertBigFreqKnobC, g_CfgInvertSmallFreqKnobC,
		g_CfgInvertHfSensR, g_CfgInvertBigFreqKnobR, g_CfgInvertSmallFreqKnobR, g_CfgInvertDeltaStbyIasBug1, g_CfgInvertDeltaStbyIasBug2,
		g_CfgInvertDeltaStbyIasBug3, g_CfgInvertAbsStbyIasBug1, g_CfgInvertAbsStbyIasBug2, g_CfgInvertAbsStbyIasBug3, g_CfgInvertEcpMinsCp,
		g_CfgInvertEcpBaroCp, g_CfgInvertEcpMinsFo, g_CfgInvertEcpBaroFo, g_CfgInvertMcpTurnSpd, g_CfgInvertMcpTurnHdg, g_CfgInvertMcpTurnVs,
		g_CfgInvertMcpTurnAlt, g_CfgInvertTurnStbyBaro, g_CfgMONITOR_PSX_IN_Qh, g_CfgMONITOR_PSX_IN_Qi, g_CfgMONITOR_PSX_IN_Qs, g_CfgMONITOR_PSX_IN_NONQ,
		g_CfgMONITOR_PSX_OUT, g_CfgMONITOR_SIOC_IN, g_CfgMONITOR_SIOC_OUT, g_cfgEngStartersDominant, g_cfgPTTSwitchesDominant, 
		g_cfgGearLockOverrideDominant, g_cfgFireEng1Dominant, g_cfgFireEng2Dominant, g_cfgFireEng3Dominant, g_cfgFireEng4Dominant, g_cfgFireApuDominant,
		g_cfgAutobrakesDominant, g_cfgParkBrkLevDominant,g_CfgDualRCPLeft, g_CfgDualRCPCenter, g_CfgDualRCPRight, g_CfgSquawkAsInt;

	CfgInt g_CfgPSXseeconAsCduWindowWidth, g_CfgPSXseeconAsCduWindowHeight, g_CfgCduScreenWidth, g_CfgCduScreenHeight, g_CfgCduScreenX,
		g_CfgCduScreenY, g_CfgCduColorR, g_CfgCduColorG, g_CfgCduColorB, g_CfgFontSize, g_CfgSmallFontYOffset, g_cduX, g_cduY, g_CfgkWindowX, g_CfgkWindowY;

	CfgString g_CfgPSXHostAddress;

	std::set<uint32_t> l_Qh_VarList;
	std::set<uint32_t> l_Qi_VarList;
	std::set<uint32_t> l_Qs_VarList;

	QcodeSet g_blacklist_vars;
	QcodeSet g_whitelist_vars;
#pragma warning(pop)

	bool g_CfgCduScreen{ false };

	auto gf_initReadCfg() -> void {
		g_CfgCduLScreen = { "CDU_L"s, false };
		g_CfgCduCScreen = { "CDU_C"s, false };
		g_CfgCduRScreen = { "CDU_R"s, false };

		g_CfgLCDCdu = { "LCD_CDU"s, false };

		g_CfgkWindowX = { "MAIN_WINDOW_X"s, 400, -30'000, 30'000 };
		g_CfgkWindowY = { "MAIN_WINDOW_Y"s, 20, -30'000, 30'000 };

		g_CfgPSXHostAddress = { "PSX_HOSTADDRESS"s, gkEmptyStr };

		g_CfgPSXseeconAsCduWindowWidth = { "PSXSEECON_AS_CDU_WIDTH"s, 900, 600, 1200 };
		g_CfgPSXseeconAsCduWindowHeight = { "PSXSEECON_AS_CDU_HEIGHT"s, 650, 400, 900 };

		g_CfgCduScreenWidth = { "CDU_WIDTH"s, 620, 200, 1200 };
		g_CfgCduScreenHeight = { "CDU_HEIGHT"s, 510, 200, 1000 };

		g_CfgCduScreenX = { "CDU_X_OFFSET"s, 100, 0, 400 };
		g_CfgCduScreenY = { "CDU_Y_OFFSET"s, 50, 0, 400 };

		g_CfgCduColorR = { "CDU_COLOR_R"s, 50, 0, 255 };
		g_CfgCduColorG = { "CDU_COLOR_G"s, 205, 0, 255 };
		g_CfgCduColorB = { "CDU_COLOR_B"s, 50, 0, 255 };

		g_CfgFontSize = { "CDU_FONTSIZE"s, 44, 12, 76 };
		g_CfgSmallFontYOffset = { "CDU_SMALL_FONT_Y_OFFSET"s, 7, 0, 18 };

		g_CfgCduBold = { "CDU_BOLD", true };
		g_CfgCduBorder = { "CDU_BORDER"s, true };

		g_CfgInvertHfSensL = { "InvertHfSenseL"s, false };   // 1 = true = invert, 0 false is normal.
		g_CfgInvertBigFreqKnobL = { "InvertBigFreqKnobL"s, false };
		g_CfgInvertSmallFreqKnobL = { "InvertSmallFreqKnobL"s, false };

		g_CfgInvertHfSensC = { "InvertHfSenseC"s, false };
		g_CfgInvertBigFreqKnobC = { "InvertBigFreqKnobC"s, false };
		g_CfgInvertSmallFreqKnobC = { "InvertSmallFreqKnobC"s, false };
		g_CfgInvertHfSensR = { "InvertHfSenseR"s, false };
		g_CfgInvertBigFreqKnobR = { "InvertBigFreqKnobR"s, false };
		g_CfgInvertSmallFreqKnobR = { "InvertSmallFreqKnobR"s, false };

		g_CfgInvertDeltaStbyIasBug1 = { "InvertDeltaStbyIasBug1"s, false };
		g_CfgInvertDeltaStbyIasBug2 = { "InvertDeltaStbyIasBug2"s, false };
		g_CfgInvertDeltaStbyIasBug3 = { "InvertDeltaStbyIasBug3"s, false };

		g_CfgInvertAbsStbyIasBug1 = { "InvertAbsStbyIasBug1"s, false };
		g_CfgInvertAbsStbyIasBug2 = { "InvertAbsStbyIasBug2"s, false };
		g_CfgInvertAbsStbyIasBug3 = { "InvertAbsStbyIasBug3"s, false };

		g_CfgInvertEcpMinsCp = { "InvertEcpMinsCp"s, false };
		g_CfgInvertEcpBaroCp = { "InvertEcpBaroCp"s, false };
		g_CfgInvertEcpMinsFo = { "InvertEcpMinsFo"s, false };
		g_CfgInvertEcpBaroFo = { "InvertEcpBaroFo"s, false };

		g_CfgInvertMcpTurnSpd = { "InvertMcpTurnSpd"s, false };
		g_CfgInvertMcpTurnHdg = { "InvertMcpTurnHdg"s, false };
		g_CfgInvertMcpTurnVs = { "InvertMcpTurnVs"s, false };
		g_CfgInvertMcpTurnAlt = { "InvertMcpTurnAlt"s, false };
		g_CfgInvertTurnStbyBaro = { "InvertTurnStbyBaro"s, false };

		g_CfgMONITOR_PSX_IN_Qh = { "MONITOR_PSX_IN_Qh"s, false };
		g_CfgMONITOR_PSX_IN_Qi = { "MONITOR_PSX_IN_Qi"s, false };
		g_CfgMONITOR_PSX_IN_Qs = { "MONITOR_PSX_IN_Qs"s, false };
		g_CfgMONITOR_PSX_IN_NONQ = { "MONITOR_PSX_IN_NONQ"s, false };

		g_CfgMONITOR_PSX_OUT = { "MONITOR_PSX_OUT"s, false };

		g_CfgMONITOR_SIOC_IN = { "MONITOR_SIOC_IN"s, false };
		g_CfgMONITOR_SIOC_OUT = { "MONITOR_SIOC_OUT"s, false };

		g_cfgEngStartersDominant = {"ENG_STARTERS_DOMINANT"s, true};
    g_cfgPTTSwitchesDominant = {"PTT_SWITCHES_DOMINANT"s, true};
    g_cfgAutobrakesDominant = {"AUTOBRAKES_DOMINANT"s, true};
    g_cfgParkBrkLevDominant = {"PARKBRKLEV_DOMINANT"s, true};
    g_cfgGearLockOverrideDominant = {"GEARLOCK_OVERRIDE_DOMINANT"s, true};
    g_cfgFireEng1Dominant = {"FIRE_ENG1_DOMINANT"s, true};
    g_cfgFireEng2Dominant = {"FIRE_ENG2_DOMINANT"s, true};
    g_cfgFireEng3Dominant = {"FIRE_ENG3_DOMINANT"s, true};
    g_cfgFireEng4Dominant = {"FIRE_ENG4_DOMINANT"s, true};
    g_cfgFireApuDominant = {"FIRE_APU_DOMINANT"s, true};
    g_CfgDualRCPLeft = {"DUAL_ENCODER_RCPL"s, true};
    g_CfgDualRCPCenter = {"DUAL_ENCODER_RCPC"s, true};
    g_CfgDualRCPRight = {"DUAL_ENCODER_RCPR"s, true};
		g_CfgSquawkAsInt = {"SQUAWK_AS_INT"s, false};
	}

	auto lf_isInVarset(std::set<uint32_t> const& set, const uint32_t varnum) -> bool {
		// if there is no list, we want to monitor all vars, that's why true....
		return set.empty() ? true : set.contains(varnum);
	}

	auto gf_isQhvarlist(const uint32_t varnum) -> bool {
		return lf_isInVarset(l_Qh_VarList, varnum);
	}

	auto gf_isQsvarlist(const uint32_t varnum) -> bool {
		return lf_isInVarset(l_Qs_VarList, varnum);
	}

	auto gf_isQivarlist(const uint32_t varnum) -> bool {
		return lf_isInVarset(l_Qi_VarList, varnum);
	}

	auto lf_string2QcodeSet(std::string const& s, const std::regex& separator, QcodeSet& svars) -> void {
		const std::sregex_token_iterator end;
		for (std::sregex_token_iterator i(s.begin(), s.end(), separator); i != end; ++i) {
			svars.emplace(static_cast<Qcode>(static_cast<uint32_t>(str2int32(*i))));
		}
	}

	auto lf_string2QihVarnumSet(const Qcode qcode, std::string const& comment, std::string const& s, const std::regex& separator, std::set<uint32_t>& svars) -> void {
		const std::sregex_token_iterator end;
		for (std::sregex_token_iterator i(s.begin(), s.end(), separator); i != end; ++i) {
			if (const int32_t iv{ str2int32(*i) }; iv >= 0) {
				if (const auto num1{ static_cast<uint32_t>(iv) + static_cast<uint32_t>(qcode) }; (num1 >= static_cast<uint32_t>(Qcode::Qh0)) && (num1 < static_cast<uint32_t>(Qcode::QhSIZE))) {
					svars.emplace(static_cast<uint32_t>(iv));
				} else {
					g_log.line(std::format("{} varlist processing: {} is not a {} var", comment, static_cast<uint32_t>(iv), comment));
				}
			}
		}
	}

	auto lf_string2QhVarnumSet(std::string const& s, const std::regex& separator, std::set<uint32_t>& svars) -> void {
		lf_string2QihVarnumSet(Qcode::Qh0, "Qh"s, s, separator, svars);
	}

	auto lf_string2QsVarnumSet(std::string const& s, const std::regex& separator, std::set<uint32_t>& svars) -> void {
		const std::sregex_token_iterator end;
		for (std::sregex_token_iterator i(s.begin(), s.end(), separator); i != end; ++i) {
			if (const int32_t iv{ str2int32(*i) }; iv >= 0) {
				if (const auto num{ static_cast<uint32_t>(iv) }; (num >= static_cast<uint32_t>(Qcode::Qs0)) && (num < static_cast<uint32_t>(Qcode::QsSIZE))) {
					svars.emplace(num);
				} else {
					g_log.line(std::format("Qs varlist processing: {} is not a Qs var", num));
				}
			}
		}
	}

	auto lf_string2QiVarnumSet(std::string const& s, const std::regex& separator, std::set<uint32_t>& svars) -> void {
		lf_string2QihVarnumSet(Qcode::Qi0, "Qi"s, s, separator, svars);
	}

	auto gf_getCfgData() -> void {
		static const std::string kLocalHostIPAddressStr{ "127.0.0.1"s };
		if (TxtFileReader r1; !r1.openFile(gkProductNameCfgPath)) {
			g_log.line("No "s + gkProductNameCfgPath.string() + " found, default values taken"s);
			g_CfgPSXHostAddress.value_ = kLocalHostIPAddressStr;
		} else {
			try {

				g_CfgMONITOR_PSX_IN_Qh.readValue();
				g_CfgMONITOR_PSX_IN_Qi.readValue();
				g_CfgMONITOR_PSX_IN_Qs.readValue();
				g_CfgMONITOR_PSX_OUT.readValue();
				g_CfgMONITOR_PSX_IN_NONQ.readValue();
				g_CfgMONITOR_SIOC_IN.readValue();
				g_CfgMONITOR_SIOC_OUT.readValue();

				g_CfgInvertHfSensL.readValue();
				g_CfgInvertBigFreqKnobL.readValue();

				g_CfgInvertSmallFreqKnobL.readValue();
				g_CfgInvertHfSensC.readValue();
				g_CfgInvertBigFreqKnobC.readValue();
				g_CfgInvertSmallFreqKnobC.readValue();
				g_CfgInvertHfSensR.readValue();
				g_CfgInvertBigFreqKnobR.readValue();
				g_CfgInvertSmallFreqKnobR.readValue();
				g_CfgInvertDeltaStbyIasBug1.readValue();
				g_CfgInvertDeltaStbyIasBug2.readValue();
				g_CfgInvertDeltaStbyIasBug3.readValue();
				g_CfgInvertAbsStbyIasBug1.readValue();

				g_CfgInvertAbsStbyIasBug2.readValue();
				g_CfgInvertAbsStbyIasBug2.readValue();
				g_CfgInvertEcpMinsCp.readValue();
				g_CfgInvertEcpBaroCp.readValue();
				g_CfgInvertEcpMinsFo.readValue();
				g_CfgInvertEcpBaroFo.readValue();
				g_CfgInvertMcpTurnSpd.readValue();
				g_CfgInvertMcpTurnHdg.readValue();
				g_CfgInvertMcpTurnVs.readValue();
				g_CfgInvertMcpTurnAlt.readValue();
				g_CfgInvertTurnStbyBaro.readValue();
				g_cfgEngStartersDominant.readValue();
        g_cfgPTTSwitchesDominant.readValue();
        g_cfgAutobrakesDominant.readValue();
        g_cfgParkBrkLevDominant.readValue();
        g_cfgGearLockOverrideDominant.readValue();
        g_cfgFireEng1Dominant.readValue();
        g_cfgFireEng2Dominant.readValue();
        g_cfgFireEng3Dominant.readValue();
        g_cfgFireEng4Dominant.readValue();
        g_cfgFireApuDominant.readValue();
        g_CfgDualRCPLeft.readValue();
        g_CfgDualRCPCenter.readValue();
        g_CfgDualRCPRight.readValue();
        g_CfgSquawkAsInt.readValue();

				if (TxtFileReader r; r.openFile(gkProductNameCfgPath)) {
					if (const auto h{ r.findArgIPaddress(g_CfgPSXHostAddress.arg_) }; !h.empty()) {
						g_CfgPSXHostAddress.value_ = h;
						g_log.line(g_CfgPSXHostAddress.arg_ + gkEqualStr + *g_CfgPSXHostAddress);
					}
					r.closeFile();
				}

				if (g_CfgPSXHostAddress.value_.empty()) {
					g_CfgPSXHostAddress.value_ = kLocalHostIPAddressStr;
					g_log.line("No "s + g_CfgPSXHostAddress.arg_ + " parameter found in "s + gkProductNameCfgPath.string() + " default "s + kLocalHostIPAddressStr + " is taken."s);
				}

				const std::regex kCommaSeparator{ R"([^,]+)" };

				CfgString lCfgQhVarList{ "Qh_VarList"s, gkEmptyStr };
				lCfgQhVarList.readValue();
				if (!lCfgQhVarList.value_.empty()) {
					lf_string2QhVarnumSet(*lCfgQhVarList, kCommaSeparator, l_Qh_VarList);
				}
				CfgString lCfgQiVarList{ "Qi_VarList"s, gkEmptyStr };
				lCfgQiVarList.readValue();
				if (!lCfgQiVarList.value_.empty()) {
					lf_string2QiVarnumSet(*lCfgQiVarList, kCommaSeparator, l_Qi_VarList);
				}
				CfgString lCfgQsVarList{ "Qs_VarList"s, gkEmptyStr };
				lCfgQsVarList.readValue();
				if (!lCfgQsVarList.value_.empty()) {
					lf_string2QsVarnumSet(*lCfgQsVarList, kCommaSeparator, l_Qs_VarList);
				}

				CfgString lCfgBvars{ "BLACKLIST"s, gkEmptyStr };
				lCfgBvars.readValue();
				if (!lCfgBvars.value_.empty()) {
					lf_string2QcodeSet(*lCfgBvars, kCommaSeparator, g_blacklist_vars);
				}

				CfgString lCfgWvars{ "WHITELIST"s, gkEmptyStr };
				lCfgWvars.readValue();
				if (!lCfgWvars.value_.empty()) {
					lf_string2QcodeSet(*lCfgWvars, kCommaSeparator, g_whitelist_vars);
				}

				if (TxtFileReader r2; r2.openFile(gkProductNameCfgPath)) {
					if (auto h{ r2.findArgNumber("OCCDU"s) }; (!h.empty()) && (str2int32(h) == 0)) {
						g_log.line("\nOCCDU=0\nSearching for USB-Keys definitions..."s);
						auto error = false;
						CDUArray codes;
						for (auto& i : codes) {
							i = 70U;  // default not used value
						}
						// 0 .. 9           :  0 ..  9
						// A .. CLR         : 10 .. 39
						// EXEC             : 40
						// LSKs L           : 41 .. 46
						// MENU             : 47
						// NAV RAD          : 48
						// PREV PG          : 49
						// NEXT PG          : 50
						// LSKs R           : 51 .. 56
						// INITREF .. VNAV  : 57 .. 61
						// FIX .. PROG      : 62 .. 66
						// .                : 67
						// +/-              : 68
						const std::array<std::string, 69> keys{
							"DIG_0"s, "DIG_1"s, "DIG_2"s, "DIG_3"s, "DIG_4"s, "DIG_5"s, "DIG_6"s, "DIG_7"s, "DIG_8"s, "DIG_9"s,
							"LET_A"s, "LET_B"s, "LET_C"s, "LET_D"s, "LET_E"s, "LET_F"s, "LET_G"s, "LET_H"s, "LET_I"s, "LET_J"s,
							"LET_K"s, "LET_L"s, "LET_M"s, "LET_N"s, "LET_O"s, "LET_P"s, "LET_Q"s, "LET_R"s, "LET_S"s, "LET_T"s,
							"LET_U"s, "LET_V"s, "LET_W"s, "LET_X"s, "LET_Y"s, "LET_Z"s, "KEY_SP"s, "KEY_DEL"s, "KEY_SLASH"s, "KEY_CLR"s,
							"KEY_EXEC"s, "LSK_L1"s, "LSK_L2"s, "LSK_L3"s, "LSK_L4"s, "LSK_L5"s, "LSK_L6"s, "KEY_MENU"s, "KEY_NAVRAD"s,"KEY_PREV"s,
							"KEY_NEXT"s, "LSK_R1"s, "LSK_R2"s, "LSK_R3"s, "LSK_R4"s, "LSK_R5"s, "LSK_R6"s, "KEY_INITREF"s, "KEY_RTE"s, "KEY_DEPARR"s,
							"KEY_ATC"s, "KEY_VNAV"s, "KEY_FIX"s, "KEY_LEGS"s, "KEY_HOLD"s, "KEY_FMCCOMM"s, "KEY_PROG"s, "KEY_DOT"s, "KEY_PLUSMINUS"s };

						for (auto i{ 0U }; i <= 68U; ++i) {
							if (const auto h2{ r2.findArgNumber(keys.at(i)) }; h2.empty()) {
								g_log.line(keys.at(i) + " is missing ..."s);
								error = true;
							} else {
								if (const auto code{ stoul(h2) }; (code >= 0) && (code <= 88U)) {
									auto allreadyin{ false };
									for (const auto ii : codes) {
										if (i == ii) {
											allreadyin = true;
											break;
										}
									}
									if (allreadyin) {
										g_log.line(std::format("Key code: {} already used...", i));
										error = true;
									} else {
										codes.at(code) = i;
									}
								} else {
									g_log.line(std::format("Wrong key code: {}", i));
									error = true;
								}
							}
						}
						if (!error) {
							g_log.line("All CDU USB-Keys codes found and correct!\n"s);
							updateOCCduKeys(codes);
						}
					}
				}
				g_CfgCduLScreen.readValue();
				if (!*g_CfgCduLScreen) {
					g_CfgCduCScreen.readValue();
				}
				if ((!*g_CfgCduLScreen) && (!*g_CfgCduCScreen)) {
					g_CfgCduRScreen.readValue();
				}
				if (*g_CfgCduLScreen || *g_CfgCduCScreen || *g_CfgCduRScreen) {
					g_CfgLCDCdu.readValue();
					g_CfgCduScreen = true;
					g_CfgCduBold.readValue();
					g_CfgCduBorder.readValue();
					g_CfgPSXseeconAsCduWindowWidth.readValue();
					g_CfgPSXseeconAsCduWindowHeight.readValue();
					g_CfgCduScreenWidth.readValue();
					g_CfgCduScreenHeight.readValue();
					g_CfgCduScreenX.readValue();
					g_CfgCduScreenY.readValue();
					g_CfgFontSize.readValue();
					g_CfgSmallFontYOffset.readValue();
					g_CfgCduColorR.readValue();
					g_CfgCduColorG.readValue();
					g_CfgCduColorB.readValue();
				}
			} catch (...) {
				gf_logUnexpectedAndUnhandledException(__func__);
			}
		}
	}
}
