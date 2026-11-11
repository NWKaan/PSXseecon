#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
  inline const VersionData gkCurrentVersion{ VER_FILEVERSION_STR };

	auto gf_getCfgData() -> void;

	extern QcodeSet g_blacklist_vars, g_whitelist_vars;

	extern bool g_CfgCduScreen;

	extern CfgBool g_CfgCduBold, g_CfgCduBorder, g_CfgCduLScreen, g_CfgCduCScreen, g_CfgCduRScreen, g_CfgInvertHfSensL, g_CfgLCDCdu,
		
		g_CfgInvertBigFreqKnobL, g_CfgInvertSmallFreqKnobL, g_CfgInvertHfSensC, g_CfgInvertBigFreqKnobC, g_CfgInvertSmallFreqKnobC,
		g_CfgInvertHfSensR, g_CfgInvertBigFreqKnobR, g_CfgInvertSmallFreqKnobR, g_CfgInvertDeltaStbyIasBug1, g_CfgInvertDeltaStbyIasBug2,
		g_CfgInvertDeltaStbyIasBug3, g_CfgInvertAbsStbyIasBug1, g_CfgInvertAbsStbyIasBug2, g_CfgInvertAbsStbyIasBug3, g_CfgInvertEcpMinsCp,
		g_CfgInvertEcpBaroCp, g_CfgInvertEcpMinsFo, g_CfgInvertEcpBaroFo, g_CfgInvertMcpTurnSpd, g_CfgInvertMcpTurnHdg, g_CfgInvertMcpTurnVs,
		g_CfgInvertMcpTurnAlt, g_CfgInvertTurnStbyBaro, g_CfgMONITOR_PSX_IN_Qh, g_CfgMONITOR_PSX_IN_Qi, g_CfgMONITOR_PSX_IN_Qs, g_CfgMONITOR_PSX_IN_NONQ,
		g_CfgMONITOR_PSX_OUT, g_CfgMONITOR_SIOC_IN, g_CfgMONITOR_SIOC_OUT, g_cfgEngStartersDominant, g_cfgPTTSwitchesDominant,
		g_cfgAutobrakesDominant, g_cfgParkBrkLevDominant,	
    g_cfgFireEng1Dominant, g_cfgFireEng2Dominant, g_cfgFireEng3Dominant, g_cfgFireEng4Dominant, g_cfgFireApuDominant,
		g_cfgGearLockOverrideDominant, g_CfgDualRCPLeft, g_CfgDualRCPCenter, g_CfgDualRCPRight, g_CfgSquawkAsInt;

	extern CfgInt g_CfgPSXseeconAsCduWindowWidth, g_CfgPSXseeconAsCduWindowHeight, g_CfgCduScreenWidth, g_CfgCduScreenHeight, g_CfgCduScreenX,
		g_CfgCduScreenY, g_CfgCduColorR, g_CfgCduColorG, g_CfgCduColorB, g_CfgFontSize, g_CfgSmallFontYOffset, g_cduX, g_cduY, g_CfgkWindowX,
		g_CfgkWindowY;

	extern CfgString g_CfgPSXHostAddress;

	auto gf_isQhvarlist(const uint32_t varnum) -> bool;
	auto gf_isQivarlist(const uint32_t varnum) -> bool;
	auto gf_isQsvarlist(const uint32_t varnum) -> bool;

	auto gf_initReadCfg() -> void;
}
