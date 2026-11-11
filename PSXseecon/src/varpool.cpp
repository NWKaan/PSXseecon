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

	using SharedVarPtrPair = std::pair<Qcode, SharedVarPtr>;
	using SharedVarPtrMap = std::map<Qcode, SharedVarPtr>;

	class VarPoolImpl final {
	public:
		VarPoolImpl() = default;

		~VarPoolImpl() {
			internalvar_defs_.clear();
			varnumbers_.clear();
			internalvars_.clear();
			pool_.clear();
			inicio_.clear();
			inicio_psx_.clear();
		}

		VarPoolImpl(VarPoolImpl const& v) = delete;
		auto operator=(VarPoolImpl const & v) -> VarPoolImpl& = delete;
		VarPoolImpl(VarPoolImpl&& v) = delete;
		auto operator=(VarPoolImpl&& v) -> VarPoolImpl& = delete;

		struct InternalVarStruct {
			InternalVarStruct(const Qcode i, QcodeVector v)
				:
				internalvar_number{ i },
				terminals{ std::move(v) } {
			}
			~InternalVarStruct() = default;
			InternalVarStruct(const InternalVarStruct& v) = default;
			auto operator=(const InternalVarStruct& v) -> InternalVarStruct& = default;

			Qcode internalvar_number;
			QcodeVector terminals;
		};

    auto createInternalVarDefs() -> void;
    auto createPsxseeconVars() -> void;
		auto createInternalVars() -> void;
		auto preInitAllVars() const -> void;
		auto logVarnumbers() -> void;
#ifdef LIST_SIOC_VARS
		auto getSIOCVarsList(std::list<std::string>& li) const -> void;
#endif
		auto createInicioVars() -> void;
		auto createInicioPsxVars() -> void;
		auto findAndEraseInactives(QcodeSet const& activ) -> void;
		[[nodiscard]] auto getVarPtr(Qcode q) const -> SharedVarPtr;
		uint32_t total_vars_ = 0;
		std::vector<InternalVarStruct> internalvar_defs_;
		std::list<std::string> varnumbers_;
		QcodeSet internalvars_;
		SharedVarPtrMap pool_;
		QcodeSet inicio_;
		QcodeSet inicio_psx_;

	private:
		auto storeVarPtr(SharedVarPtr const ptr) -> void {
			if (!ptr) {
				throw std::runtime_error("VarPoolImpl::storeVarPtr is null"s);
			}
			const auto q{ (*ptr).getQcode() };
			if (const auto ret{ pool_.insert(SharedVarPtrPair(q, ptr)) }; !ret.second) {
				throw std::runtime_error("VarPoolImpl::storeVarPtr var already exists, var "s + gf_Qcode2Str(q) + " already exists"s);
			}
		}

		auto ch_AlternateActionSwitch(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<AlternateActionSwitch>(q, name));
		}

		auto ch_MomentaryActionSwitch(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<MomentaryActionSwitch>(q, name));
		}

		auto ch_AutoStart1234Var(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<AutoStart1234Var>(q, name));
		}

		auto ch_GuaMonToggleType2(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<GuaMonToggleType2>(q, name));
		}

		auto ch_GuaOnOffOnVar(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<GuaOnOffOnVar>(q, name));
		}

		auto ch_ValueControlVar(const Qcode q, std::string const& name, const MinMax minmax) -> void {
			storeVarPtr(std::make_shared<ValueControlVar>(q, name, minmax));
		}

		auto ch_ContinuousValueControlVar(const Qcode q, std::string const& name, const MinMax minmax) -> void {
			storeVarPtr(std::make_shared<ContinuousValueControlVar>(q, name, minmax));
		}

		auto ch_Potentiometer270(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<Potentiometer270ControlVar>(q, name));
		}

		auto ch_OCKeyboard(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<OCKeyboard>(q, name));
		}

		auto ch_InfiniteRotarySelector(const Qcode q, std::string const& name, const int32_t range, const bool direction) -> void {
			storeVarPtr(std::make_shared<InfiniteRotarySelector>(q, name, range, direction));
		};

		auto ch_TransponderKey(const Qcode q, std::string const& name, uint32_t key_number, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<TransponderKey>(q, name, key_number, exp_qcode));
		}

		auto ch_RcpPushButton(const Qcode q, std::string const& name, uint32_t key_number, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<RcpPushbutton>(q, name, key_number, exp_qcode));
		}

		auto ch_AcpPushButton(const Qcode q, std::string const& name, uint32_t key_number, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<AcpPushbutton>(q, name, key_number, exp_qcode));
		}

		auto ch_CduKeyboardVar(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<CduKeyboardVar>(q, name, exp_qcode));
		}

		auto ch_QhMomActionCollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements) -> void {
			storeVarPtr(std::make_shared<QhMomActionCollectionVar>(q, name, num_elements));
		}

		auto ch_QhEVACPshButtonsCollectionVar(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<QhEVACPshButtonsCollectionVar>(q, name));
		}

		auto ch_QhEVACSignalCmndsCollectionVar(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<QhEVACSignalCmndsCollectionVar>(q, name));
		}

		auto ch_QhRotSwitchCollectionVar(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<QhRotSwitchCollectionVar>(q, name));
		}

		auto ch_QhOnOffOnSwitchCollectionVar(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<QhOnOffOnSwitchCollectionVar>(q, name));
		}

		auto ch_QhFireSwitchVar(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<QhFireSwitchVar>(q, name));
		}

		auto ch_CollRotSwitchTerminal(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number, const uint32_t terminal_number) -> void {
			storeVarPtr(std::make_shared<CollRotSwitchTerminal>(q, name, controlling_var, element_number, terminal_number));
		}

		auto ch_CollRotSwitchTerminal1(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t terminal_number) -> void {
			storeVarPtr(std::make_shared<CollRotSwitchTerminal1>(q, name, controlling_var, terminal_number));
		}

		auto ch_CollValueControlVar(const Qcode q, std::string const& name, const MinMax minmax, const Qcode controlling_var, const uint32_t element_number) -> void {
			storeVarPtr(std::make_shared<CollValueControlVar>(q, name, minmax, controlling_var, element_number));
		}

		auto ch_CollContinuousValueControlVar(const Qcode q, std::string const& name, const MinMax minmax, const Qcode controlling_var, const uint32_t element_number) -> void {
			storeVarPtr(std::make_shared<CollContinuousValueControlVar>(q, name, minmax, controlling_var, element_number));
		}

		auto ch_CollPotentiometer270(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number) -> void {
			storeVarPtr(std::make_shared<CollPotentiometer270ControlVar>(q, name, controlling_var, element_number));
		}

		auto ch_CollOnOffSwitch(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number) -> void {
			storeVarPtr(std::make_shared<CollOnOffSwitch>(q, name, controlling_var, element_number));
		}

		auto ch_OnOffOnSwitch(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number) -> void {
			storeVarPtr(std::make_shared<OnOffOnSwitch>(q, name, controlling_var, element_number));
		}

		auto ch_CollMomActionSwitch(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number) -> void {
			storeVarPtr(std::make_shared<CollMomActionSwitch>(q, name, controlling_var, element_number));
		}

		auto ch_CollInfiniteRotarySelector(const Qcode q, std::string const& name, const int32_t range, const Qcode controlling_var, const uint32_t element_number, const bool direction) -> void {
			storeVarPtr(std::make_shared<CollInfiniteRotarySelector>(q, name, range, controlling_var, element_number, direction));
		}

		auto ci_QiControlVar(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<QiControlVar>(q, name, exp_qcode));
		}

		auto ci_QiValueControlVar(const Qcode q, std::string const& name, const Qcode exp_qcode, const MinMax minmax) -> void {
			storeVarPtr(std::make_shared<QiValueControlVar>(q, name, exp_qcode, minmax));
		}

		auto ci_QiMomentaryActionSwitch(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<QiMomentaryActionSwitch>(q, name, exp_qcode));
		}

		auto ci_QiAlternateActionSwitch(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<QiAlternateActionSwitch>(q, name, exp_qcode));
		}

		auto ci_QiDeltaControlVar(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<QiDeltaControlVar>(q, name, exp_qcode));
		}

		auto cs_QsECONCollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements) -> void {
			storeVarPtr(std::make_shared<QsECONCollectionVar>(q, name, num_elements));
		}

		auto cs_Qs120Var(const Qcode q, std::string const& name, const uint32_t num_elements) -> void {
			storeVarPtr(std::make_shared<Qs120Var>(q, name, num_elements));
		}

		auto cs_Qs357Var(const Qcode q, std::string const& name, const uint32_t num_elements) -> void {
			storeVarPtr(std::make_shared<Qs357Var>(q, name, num_elements));
		}

		auto cs_QsCBLineVar(const Qcode q, std::string const& name, const uint32_t num_elements) -> void {
			storeVarPtr(std::make_shared<QsCBLineVar>(q, name, num_elements));
		}

		auto cs_QsDeltaCollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements) -> void {
			storeVarPtr(std::make_shared<QsDeltaCollectionVar>(q, name, num_elements));
		}

    auto cs_QsDeltaCollectionVar2(const Qcode q, std::string const& name) -> void {
      storeVarPtr(std::make_shared<QsDeltaCollectionVar2>(q, name));
    }

		auto cs_WxRdrPanelPushVar(const Qcode q, std::string const& name, const uint32_t num_elements) -> void {
			storeVarPtr(std::make_shared<WxRdrPanelPushVar>(q, name, num_elements));
		}

		auto cs_WxRdrPanelTurnVar(const Qcode q, std::string const& name, const uint32_t num_elements) -> void {
			storeVarPtr(std::make_shared<WxRdrPanelTurnVar>(q, name, num_elements));
		}

		auto cs_MalFunctionControlVar(const Qcode q, std::string const& name, Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<MalFunctionControlVar>(q, name, exp_qcode));
		}

		auto ih_IndicatorLightMcpIntegrated(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<IndicatorLightMcpIntegrated>(q, name, exp_qcode));
		}

		auto ih_IndicatorLightBigActionSwitch(const Qcode q, std::string const& name, const Qcode volt_up, const Qcode volt_down, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<IndicatorLightBigActionSwitch>(q, name, volt_up, volt_down, exp_qcode));
		}

		auto ih_IndicatorLightFuelCutoffSwitch(const Qcode q, std::string const& name, const Qcode volt, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<IndicatorLightFuelCutoffSwitch>(q, name, volt, exp_qcode));
		}

		auto ih_IndicatorLightFireSwitch(const Qcode q, std::string const& name, const Qcode volt, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<IndicatorLightFireSwitch>(q, name, volt, exp_qcode));
		}

		auto ii_WhiteLightEngStartSwitch(const Qcode q, std::string const& name, const Qcode volt, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<WhiteLightEngStartSwitch>(q, name, volt, exp_qcode));
		}

		auto ih_IndicatorLightSquibButton(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<IndicatorLightSquibButton>(q, name, exp_qcode));
		}

		auto ih_IndicatorLightELT(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<IndicatorLightELT>(q, name, exp_qcode));
		}

		auto ih_EvacLight(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<EvacLight>(q, name, exp_qcode));
		}

		auto ih_QhcodeInfo(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<QhcodeInfo>(q, name, exp_qcode));
		}

		auto ii_RCPIndicatorLights(const Qcode q, std::string const& name, const Qcode volt) -> void {
			storeVarPtr(std::make_shared<RCPIndicatorLights>(q, name, volt));
		}

		auto ii_QcodeInfo(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<QcodeInfo>(q, name));
		}

		auto ii_StbyCompass(const Qcode q, std::string const& name) -> void {
			storeVarPtr(std::make_shared<StbyCompass>(q, name));
		}

		auto ii_ACPgreenLEDs(const Qcode q, std::string const& name, const LCR lcr) -> void {
			storeVarPtr(std::make_shared<ACPgreenLEDs>(q, name, lcr));
		}

		auto ii_AcpMICLights(const Qcode q, std::string const& name, const LCR lcr) -> void {
			storeVarPtr(std::make_shared<AcpMICLights>(q, name, lcr));
		}

		auto ii_CallFrLight(const Qcode q, std::string const& name, const LCR lcr, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<CallFrLight>(q, name, lcr, exp_qcode));
		}

		auto ii_CduIndicatorLights(const Qcode q, std::string const& name, const Qcode volt) -> void {
			storeVarPtr(std::make_shared<CduIndicatorLights>(q, name, volt));
		}

		auto ii_IndicatorLightNotIntegrated(const Qcode q, std::string const& name, const Qcode volt) -> void {
			storeVarPtr(std::make_shared<IndicatorLightNotIntegrated>(q, name, volt));
		}

		auto is_ExpQcodeInfo(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<ExpQscodeInfo>(q, name, exp_qcode));
		}

		auto is_RcpDispl(const Qcode q, std::string const& name, const Qcode exp_qcode, const Qcode indicatorlightvar, const Qcode volt) -> void {
			storeVarPtr(std::make_shared<RcpDispl>(q, name, exp_qcode, indicatorlightvar, volt));
		}

    auto is_SquawkDispl(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
      storeVarPtr(std::make_shared<SquawkDispl>(q, name, exp_qcode));
    }

    auto is_XpdrFail(const Qcode q, std::string const& name, const Qcode exp_qcode) -> void {
      storeVarPtr(std::make_shared<XpdrFail>(q, name, exp_qcode));
    }

		auto is_ClockVar(const Qcode q, std::string const& name, const Qcode  volt, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<ClockVar>(q, name, volt, exp_qcode));
		}

		auto is_ClockVar2(const Qcode q, std::string const& name, const Qcode  volt, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<ClockVar2>(q, name, volt, exp_qcode));
		}

		auto is_ClockVar3(const Qcode q, std::string const& name, const Qcode  volt, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<ClockVar3>(q, name, volt, exp_qcode));
		}

		auto is_TimerVar(const Qcode q, std::string const& name, const Qcode  volt, const Qcode exp_qcode) -> void {
			storeVarPtr(std::make_shared<EtChrVar>(q, name, volt, exp_qcode));
		}

		auto createInternalVarElements() -> void;

	};

	auto VarPoolImpl::createPsxseeconVars() -> void { // NOLINT
		createInternalVarElements();

		// Qh
		ch_InfiniteRotarySelector(Qcode::Qh0, "DeltaStbyIasBug1"s, 6'000L, *g_CfgInvertDeltaStbyIasBug1);
		ch_InfiniteRotarySelector(Qcode::Qh1, "DeltaStbyIasBug2"s, 6'000L, *g_CfgInvertDeltaStbyIasBug2);
		ch_InfiniteRotarySelector(Qcode::Qh2, "DeltaStbyIasBug3"s, 6'000L, *g_CfgInvertDeltaStbyIasBug3);
		ch_InfiniteRotarySelector(Qcode::Qh3, "AbsStbyIasBug1"s, 6'000L, *g_CfgInvertAbsStbyIasBug1);
		ch_InfiniteRotarySelector(Qcode::Qh4, "AbsStbyIasBug2"s, 6'000L, *g_CfgInvertAbsStbyIasBug2);
		ch_InfiniteRotarySelector(Qcode::Qh5, "AbsStbyIasBug3"s, 6'000L, *g_CfgInvertAbsStbyIasBug3);
		ch_AlternateActionSwitch(Qcode::Qh6, "LtStorm"s);
		ch_Potentiometer270(Qcode::Qh7, "LtOvhd"s);
		ch_Potentiometer270(Qcode::Qh8, "LtDome"s);
		ch_Potentiometer270(Qcode::Qh9, "LtGlrshPanel"s);
		ch_Potentiometer270(Qcode::Qh10, "LtGlrshFlood"s);
		ch_Potentiometer270(Qcode::Qh11, "LtAislePanel"s);
		ch_Potentiometer270(Qcode::Qh12, "LtAisleFlood"s);
		// 13 is OnOfOn switch
		// 14 is OnOfOn switch
		ch_MomentaryActionSwitch(Qcode::Qh15, "EcpWxrCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh16, "EcpStaCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh17, "EcpWptCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh18, "EcpArptCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh19, "EcpDataCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh20, "EcpPosCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh21, "EcpTerrCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh22, "EcpFpvCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh23, "EcpMtrsCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh24, "EcpRstCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh25, "EcpStdCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh26, "EcpCtrCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh27, "EcpTfcCp"s);
		ch_InfiniteRotarySelector(Qcode::Qh28, "EcpMinsCp"s, 6'000, *g_CfgInvertEcpMinsCp);
		ch_AlternateActionSwitch(Qcode::Qh29, "EcpMinsModeCp"s);
		ch_InfiniteRotarySelector(Qcode::Qh30, "EcpBaroCp"s, 6'000, *g_CfgInvertEcpBaroCp);
		ch_AlternateActionSwitch(Qcode::Qh31, "EcpBaroModeCp"s);
		// 32 and 33 are rotswitches
		// 34 is OnOfOn switch
		// 35 is OnOfOn switch
		ch_MomentaryActionSwitch(Qcode::Qh36, "EcpWxrFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh37, "EcpStaFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh38, "EcpWptFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh39, "EcpArptFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh40, "EcpDataFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh41, "EcpPosFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh42, "EcpTerrFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh43, "EcpFpvFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh44, "EcpMtrsFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh45, "EcpRstFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh46, "EcpStdFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh47, "EcpCtrFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh48, "EcpTfcFo"s);
		ch_InfiniteRotarySelector(Qcode::Qh49, "EcpMinsFo"s, 6'000, *g_CfgInvertEcpMinsFo);
		ch_AlternateActionSwitch(Qcode::Qh50, "EcpMinsModeFo"s);
		ch_InfiniteRotarySelector(Qcode::Qh51, "EcpBaroFo"s, 6'000, *g_CfgInvertEcpBaroFo);
		ch_AlternateActionSwitch(Qcode::Qh52, "EcpBaroModeFo"s);
		// 53 and 54 are rotswitches
		ch_AlternateActionSwitch(Qcode::Qh55, "McpApDis"s);
		ch_AlternateActionSwitch(Qcode::Qh56, "McpFdCp"s);
		ch_AlternateActionSwitch(Qcode::Qh57, "McpFdFo"s);
		ch_AlternateActionSwitch(Qcode::Qh58, "McpAtArm"s);
		ch_MomentaryActionSwitch(Qcode::Qh59, "McpPshMachSel"s);
		ch_MomentaryActionSwitch(Qcode::Qh60, "McpPshSpdSel"s);
		ch_MomentaryActionSwitch(Qcode::Qh61, "McpPshHdgSel"s);
		ch_MomentaryActionSwitch(Qcode::Qh62, "McpPshAltSel"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshThr, "McpPshThr"s, Qcode::Qh63);
		ch_MomentaryActionSwitch(Qcode::Qh63, "McpPshThr"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshSpd, "McpPshSpd"s, Qcode::Qh64);
		ch_MomentaryActionSwitch(Qcode::Qh64, "McpPshSpd"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshLnv, "McpPshLnv"s, Qcode::Qh65);
		ch_MomentaryActionSwitch(Qcode::Qh65, "McpPshLnv"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshVnv, "McpPshVnv"s, Qcode::Qh66);
		ch_MomentaryActionSwitch(Qcode::Qh66, "McpPshVnv"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshFlc, "McpPshFlc"s, Qcode::Qh67);
		ch_MomentaryActionSwitch(Qcode::Qh67, "McpPshFlc"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshHdgHld, "McpPshHdgHld"s, Qcode::Qh68);
		ch_MomentaryActionSwitch(Qcode::Qh68, "McpPshHdgHld"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshVs, "McpPshVs"s, Qcode::Qh69);
		ch_MomentaryActionSwitch(Qcode::Qh69, "McpPshVs"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshAltHld, "McpPshAltHld"s, Qcode::Qh70);
		ch_MomentaryActionSwitch(Qcode::Qh70, "McpPshAltHld"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshLoc, "McpPshLoc"s, Qcode::Qh71);
		ch_MomentaryActionSwitch(Qcode::Qh71, "McpPshLoc"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshApp, "McpPshApp"s, Qcode::Qh72);
		ch_MomentaryActionSwitch(Qcode::Qh72, "McpPshApp"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshCmdL, "McpPshCmdL"s, Qcode::Qh73);
		ch_MomentaryActionSwitch(Qcode::Qh73, "McpPshCmdL"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshCmdC, "McpPshCmdC"s, Qcode::Qh74);
		ch_MomentaryActionSwitch(Qcode::Qh74, "McpPshCmdC"s);
		ih_IndicatorLightMcpIntegrated(Qcode::McpPshCmdR, "McpPshCmdR"s, Qcode::Qh75);
		ch_MomentaryActionSwitch(Qcode::Qh75, "McpPshCmdR"s);
		// 76 is rotswitch
		ch_InfiniteRotarySelector(Qcode::Qh77, "McpTurnSpd"s, 16, *g_CfgInvertMcpTurnSpd);
		ch_InfiniteRotarySelector(Qcode::Qh78, "McpTurnHdg"s, 16, *g_CfgInvertMcpTurnHdg);
		ch_InfiniteRotarySelector(Qcode::Qh79, "McpTurnVs"s, 16, *g_CfgInvertMcpTurnVs);
		ch_InfiniteRotarySelector(Qcode::Qh80, "McpTurnAlt"s, 16, *g_CfgInvertMcpTurnAlt);
		ch_MomentaryActionSwitch(Qcode::Qh81, "LcpClockCp"s);
		ch_AlternateActionSwitch(Qcode::Qh82, "LcpPttCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh83, "LcpPvdOnCp"s);
		ch_Potentiometer270(Qcode::Qh84, "LcpPvdCp"s);
		ch_Potentiometer270(Qcode::Qh85, "LcpPanelCp"s);
		ch_Potentiometer270(Qcode::Qh86, "LcpFloodCp"s);
		ch_Potentiometer270(Qcode::Qh87, "LcpOutbdCp"s);
		ch_Potentiometer270(Qcode::Qh88, "LcpWxrCp"s);
		ch_Potentiometer270(Qcode::Qh89, "LcpNdCp"s);
		ch_Potentiometer270(Qcode::Qh90, "LcpMapCp"s);
		ch_AlternateActionSwitch(Qcode::Qh91, "LcpMapOnCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh92, "LcpClockFo"s);
		ch_AlternateActionSwitch(Qcode::Qh93, "LcpPttFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh94, "LcpPvdOnFo"s);
		ch_Potentiometer270(Qcode::Qh95, "LcpPvdFo"s);
		ch_Potentiometer270(Qcode::Qh96, "LcpPanelFo"s);
		ch_Potentiometer270(Qcode::Qh97, "LcpFloodFo"s);
		ch_Potentiometer270(Qcode::Qh98, "LcpOutbdFo"s);
		ch_Potentiometer270(Qcode::Qh99, "LcpWxrFo"s);
		ch_Potentiometer270(Qcode::Qh100, "LcpNdFo"s);
		ch_Potentiometer270(Qcode::Qh101, "LcpMapFo"s);
		ch_AlternateActionSwitch(Qcode::Qh102, "LcpMapOnFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh103, "EicasEng"s);
		ch_MomentaryActionSwitch(Qcode::Qh104, "EicasStat"s);
		ch_MomentaryActionSwitch(Qcode::Qh105, "EicasElec"s);
		ch_MomentaryActionSwitch(Qcode::Qh106, "EicasFuel"s);
		ch_MomentaryActionSwitch(Qcode::Qh107, "EicasEcs"s);
		ch_MomentaryActionSwitch(Qcode::Qh108, "EicasHyd"s);
		ch_MomentaryActionSwitch(Qcode::Qh109, "EicasDrs"s);
		ch_MomentaryActionSwitch(Qcode::Qh110, "EicasGear"s);
		ch_MomentaryActionSwitch(Qcode::Qh111, "EicasCanc"s);
		ch_MomentaryActionSwitch(Qcode::Qh112, "EicasRcl"s);
		ch_MomentaryActionSwitch(Qcode::Qh113, "EicasCancRcl"s);
		ih_IndicatorLightBigActionSwitch(Qcode::MastWarnCp, "MastWarnCp"s, Qcode::nonDimC358, Qcode::nonDimC358, Qcode::Qh114);
		ch_MomentaryActionSwitch(Qcode::Qh114, "MastWarnCp"s);
		ih_IndicatorLightBigActionSwitch(Qcode::MastWarnFo, "MastWarnFo"s, Qcode::nonDimC614, Qcode::nonDimC614, Qcode::Qh115);
		ch_MomentaryActionSwitch(Qcode::Qh115, "MastWarnFo"s);
		// 116 .. 127 are rotswitches
		ch_AlternateActionSwitch(Qcode::Qh128, "AltnEfis"s);
		// 129, 130 and 131 are rotswitches
		ch_AlternateActionSwitch(Qcode::Qh132, "FmcEicas"s);
		// 133 is rotswitch
		ch_ValueControlVar(Qcode::Qh134, "Autobr"s, MinMax{ 7 });
		ch_MomentaryActionSwitch(Qcode::Qh135, "IsfdPushStd"s);
		ch_InfiniteRotarySelector(Qcode::Qh136, "TurnStbyBaro"s, 16, *g_CfgInvertTurnStbyBaro);
		// 137 is rotswitch
		ch_MomentaryActionSwitch(Qcode::Qh138, "Caging"s);
		ch_Potentiometer270(Qcode::Qh139, "EicasBrtUpr"s);
		ch_Potentiometer270(Qcode::Qh140, "EicasBrtLwrInner"s);
		ch_Potentiometer270(Qcode::Qh141, "EicasBrtLwrOuter"s);
		ch_MomentaryActionSwitch(Qcode::Qh142, "EventRcd"s);
		ch_AlternateActionSwitch(Qcode::Qh143, "HdgTru"s);
		ch_AlternateActionSwitch(Qcode::Qh144, "StbyMls"s);
		ch_MomentaryActionSwitch(Qcode::Qh145, "IsfdPushFred"s);
		ch_MomentaryActionSwitch(Qcode::Qh146, "IsfdPushApp"s);
		ch_AlternateActionSwitch(Qcode::Qh147, "IsfdPushPlu"s);
		ch_AlternateActionSwitch(Qcode::Qh148, "IsfdPushMin"s);
		ch_AlternateActionSwitch(Qcode::Qh149, "IsfdPushRst"s);
		ch_MomentaryActionSwitch(Qcode::Qh150, "ChronoPushCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh151, "DatePushCp"s);
		ch_MomentaryActionSwitch(Qcode::Qh152, "ChronoPushFo"s);
		ch_MomentaryActionSwitch(Qcode::Qh153, "DatePushFo"s);
		ch_ValueControlVar(Qcode::Qh154, "ClkEtCp"s, MinMax{ 2 });
		ch_ValueControlVar(Qcode::Qh155, "ClkSetCp"s, MinMax{ 3 });
		ch_ValueControlVar(Qcode::Qh156, "ClkEtFo"s, MinMax{ 2 });
		ch_ValueControlVar(Qcode::Qh157, "ClkSetFo"s, MinMax{ 3 });
		ch_AlternateActionSwitch(Qcode::Qh158, "RmiRedCp"s);
		ch_AlternateActionSwitch(Qcode::Qh159, "RmiGreenCp"s);
		ch_AlternateActionSwitch(Qcode::Qh160, "RmiRedFo"s);
		ch_AlternateActionSwitch(Qcode::Qh161, "RmiGreenFo"s);
		// July 16, 2021:  6 times gR7736 replaced by gT102
		ih_IndicatorLightBigActionSwitch(Qcode::AltnFlap, "AltnFlap"s, Qcode::gT102, Qcode::gT102, Qcode::Qh162);
		ch_AlternateActionSwitch(Qcode::Qh162, "AltnFlap"s);
		ih_IndicatorLightBigActionSwitch(Qcode::AltnGearNsBd, "AltnGearNsBd"s, Qcode::gT102, Qcode::gT102, Qcode::Qh163);
		ch_AlternateActionSwitch(Qcode::Qh163, "AltnGearNsBd"s);
		ih_IndicatorLightBigActionSwitch(Qcode::AltnGearWing, "AltnGearWing"s, Qcode::gT102, Qcode::gT102, Qcode::Qh164);
		ch_AlternateActionSwitch(Qcode::Qh164, "AltnGearWing"s);
		ih_IndicatorLightBigActionSwitch(Qcode::GsInhb, "GsInhb"s, Qcode::dimRelayK8b, Qcode::dimRelayK8b, Qcode::Qh165);
		ch_MomentaryActionSwitch(Qcode::Qh165, "GsInhb"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FlapOvrd, "FlapOvrd"s, Qcode::gT102, Qcode::gT102, Qcode::Qh166);
		ch_AlternateActionSwitch(Qcode::Qh166, "FlapOvrd"s);
		ih_IndicatorLightBigActionSwitch(Qcode::GearOvrd, "GearOvrd"s, Qcode::gT102, Qcode::gT102, Qcode::Qh167);
		ch_AlternateActionSwitch(Qcode::Qh167, "GearOvrd"s);
		ih_IndicatorLightBigActionSwitch(Qcode::TerrOvrd, "TerrOvrd"s, Qcode::gT102, Qcode::gT102, Qcode::Qh168);
		// end 
		ch_AlternateActionSwitch(Qcode::Qh168, "TerrOvrd"s);
		ch_AlternateActionSwitch(Qcode::Qh169, "GearLockOvrd"s);
		// 170 is rotswitch
		ch_AlternateActionSwitch(Qcode::Qh171, "LtLandOubL"s);
		ch_AlternateActionSwitch(Qcode::Qh172, "LtLandOubR"s);
		ch_AlternateActionSwitch(Qcode::Qh173, "LtLandInbL"s);
		ch_AlternateActionSwitch(Qcode::Qh174, "LtLandInbR"s);
		ch_AlternateActionSwitch(Qcode::Qh175, "LtRwyTurnL"s);
		ch_AlternateActionSwitch(Qcode::Qh176, "LtRwyTurnR"s);
		ch_AlternateActionSwitch(Qcode::Qh177, "LtTaxi"s);
		// 178 is OnOfOn switch
		ch_AlternateActionSwitch(Qcode::Qh179, "LtNav"s);
		ch_AlternateActionSwitch(Qcode::Qh180, "LtStrobe"s);
		ch_AlternateActionSwitch(Qcode::Qh181, "LtWing"s);
		ch_AlternateActionSwitch(Qcode::Qh182, "LtLogo"s);
		// 183 is OnOfOn switch
		// 184 .. 187 are Rotary switches
		ih_IndicatorLightBigActionSwitch(Qcode::HydEng1, "HydEng1"s, Qcode::gT38, Qcode::dimRelayK6b, Qcode::Qh188);
		ch_AlternateActionSwitch(Qcode::Qh188, "HydEng1"s);
		ih_IndicatorLightBigActionSwitch(Qcode::HydEng2, "HydEng2"s, Qcode::gT38, Qcode::dimRelayK6b, Qcode::Qh189);
		ch_AlternateActionSwitch(Qcode::Qh189, "HydEng2"s);
		ih_IndicatorLightBigActionSwitch(Qcode::HydEng3, "HydEng3"s, Qcode::gT38, Qcode::dimRelayK6b, Qcode::Qh190);
		ch_AlternateActionSwitch(Qcode::Qh190, "HydEng3"s);
		ih_IndicatorLightBigActionSwitch(Qcode::HydEng4, "HydEng4"s, Qcode::gT38, Qcode::dimRelayK6b, Qcode::Qh191);
		ch_AlternateActionSwitch(Qcode::Qh191, "HydEng4"s);
		// 192 and 193 are Rotary switches
		ih_IndicatorLightBigActionSwitch(Qcode::UtilL, "UtilL"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh194);
		ch_AlternateActionSwitch(Qcode::Qh194, "UtilL"s);
		ih_IndicatorLightBigActionSwitch(Qcode::UtilR, "UtilR"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh195);
		ch_AlternateActionSwitch(Qcode::Qh195, "UtilR"s);
		ih_IndicatorLightBigActionSwitch(Qcode::Battery, "Battery"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh196);
		ch_AlternateActionSwitch(Qcode::Qh196, "Battery"s);
		ih_IndicatorLightBigActionSwitch(Qcode::ApuGen1, "ApuGen1"s, Qcode::dimRelayK2a, Qcode::dimRelayK2a, Qcode::Qh197);
		ch_MomentaryActionSwitch(Qcode::Qh197, "ApuGen1"s);
		ih_IndicatorLightBigActionSwitch(Qcode::ApuGen2, "ApuGen2"s, Qcode::dimRelayK2a, Qcode::dimRelayK2a, Qcode::Qh198);
		ch_MomentaryActionSwitch(Qcode::Qh198, "ApuGen2"s);
		ih_IndicatorLightBigActionSwitch(Qcode::ExtPow1, "ExtPow1"s, Qcode::extAvailTest1, Qcode::extAvailTest1, Qcode::Qh199);
		ch_MomentaryActionSwitch(Qcode::Qh199, "ExtPow1"s);
		ih_IndicatorLightBigActionSwitch(Qcode::ExtPow2, "ExtPow2"s, Qcode::extAvailTest2, Qcode::extAvailTest1, Qcode::Qh200);
		ch_MomentaryActionSwitch(Qcode::Qh200, "ExtPow2"s);
		ih_IndicatorLightBigActionSwitch(Qcode::BusTie1, "BusTie1"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh201);
		ch_AlternateActionSwitch(Qcode::Qh201, "BusTie1"s);
		ih_IndicatorLightBigActionSwitch(Qcode::BusTie2, "BusTie2"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh202);
		ch_AlternateActionSwitch(Qcode::Qh202, "BusTie2"s);
		ih_IndicatorLightBigActionSwitch(Qcode::BusTie3, "BusTie3"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh203);
		ch_AlternateActionSwitch(Qcode::Qh203, "BusTie3"s);
		ih_IndicatorLightBigActionSwitch(Qcode::BusTie4, "BusTie4"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh204);
		ch_AlternateActionSwitch(Qcode::Qh204, "BusTie4"s);
		ih_IndicatorLightBigActionSwitch(Qcode::GenCont1, "GenCont1"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh205);
		ch_AlternateActionSwitch(Qcode::Qh205, "GenCont1"s);
		ih_IndicatorLightBigActionSwitch(Qcode::GenCont2, "GenCont2"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh206);
		ch_AlternateActionSwitch(Qcode::Qh206, "GenCont2"s);
		ih_IndicatorLightBigActionSwitch(Qcode::GenCont3, "GenCont3"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh207);
		ch_AlternateActionSwitch(Qcode::Qh207, "GenCont3"s);
		ih_IndicatorLightBigActionSwitch(Qcode::GenCont4, "GenCont4"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh208);
		ch_AlternateActionSwitch(Qcode::Qh208, "GenCont4"s);
		ih_IndicatorLightBigActionSwitch(Qcode::DriveDisc1, "DriveDisc1"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh209);
		ch_MomentaryActionSwitch(Qcode::Qh209, "DriveDisc1"s);
		ih_IndicatorLightBigActionSwitch(Qcode::DriveDisc2, "DriveDisc2"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh210);
		ch_MomentaryActionSwitch(Qcode::Qh210, "DriveDisc2"s);
		ih_IndicatorLightBigActionSwitch(Qcode::DriveDisc3, "DriveDisc3"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh211);
		ch_MomentaryActionSwitch(Qcode::Qh211, "DriveDisc3"s);
		ih_IndicatorLightBigActionSwitch(Qcode::DriveDisc4, "DriveDisc4"s, Qcode::gT40, Qcode::dimRelayK2a, Qcode::Qh212);
		ch_MomentaryActionSwitch(Qcode::Qh212, "DriveDisc4"s);
		// 213, 214 and 215 are rotary switches
		ih_IndicatorLightBigActionSwitch(Qcode::Eec1, "Eec1"s, Qcode::gT38, Qcode::dimRelayK2b, Qcode::Qh216);
		ch_AlternateActionSwitch(Qcode::Qh216, "Eec1"s);
		ih_IndicatorLightBigActionSwitch(Qcode::Eec2, "Eec2"s, Qcode::gT38, Qcode::dimRelayK2b, Qcode::Qh217);
		ch_AlternateActionSwitch(Qcode::Qh217, "Eec2"s);
		ih_IndicatorLightBigActionSwitch(Qcode::Eec3, "Eec3"s, Qcode::gT38, Qcode::dimRelayK2b, Qcode::Qh218);
		ch_AlternateActionSwitch(Qcode::Qh218, "Eec3"s);
		ih_IndicatorLightBigActionSwitch(Qcode::Eec4, "Eec4"s, Qcode::gT38, Qcode::dimRelayK2b, Qcode::Qh219);
		ch_AlternateActionSwitch(Qcode::Qh219, "Eec4"s);
		ch_AutoStart1234Var(Qcode::Qh220, "AutoStart1"s);
		ch_AutoStart1234Var(Qcode::Qh221, "AutoStart2"s);
		ch_AutoStart1234Var(Qcode::Qh222, "AutoStart3"s);
		ch_AutoStart1234Var(Qcode::Qh223, "AutoStart4"s);
		// 224 is On/Off/On
		ih_IndicatorLightBigActionSwitch(Qcode::NaiPush1, "NaiPush1"s, Qcode::gR8098c, Qcode::dimRelayK6b, Qcode::Qh225);
		ch_AlternateActionSwitch(Qcode::Qh225, "NaiPush1"s);
		ih_IndicatorLightBigActionSwitch(Qcode::NaiPush2, "NaiPush2"s, Qcode::gR8098c, Qcode::dimRelayK4a, Qcode::Qh226);
		ch_AlternateActionSwitch(Qcode::Qh226, "NaiPush2"s);
		ih_IndicatorLightBigActionSwitch(Qcode::NaiPush3, "NaiPush3"s, Qcode::gR8098c, Qcode::dimRelayK4a, Qcode::Qh227);
		ch_AlternateActionSwitch(Qcode::Qh227, "NaiPush3"s);
		ih_IndicatorLightBigActionSwitch(Qcode::NaiPush4, "NaiPush4"s, Qcode::gR8098c, Qcode::dimRelayK8b, Qcode::Qh228);
		ch_AlternateActionSwitch(Qcode::Qh228, "NaiPush4"s);
		// 229 - 232 are OnOffOn terminal vars
		ih_IndicatorLightBigActionSwitch(Qcode::WaiPush, "WaiPush"s, Qcode::gR8098c, Qcode::dimRelayK4a, Qcode::Qh233);
		ch_AlternateActionSwitch(Qcode::Qh233, "WaiPush"s);
		// 234 is OnOffOn terminal var
		// 235 and 236 are rotswitches
		ch_AlternateActionSwitch(Qcode::Qh237, "WasherL"s);
		ch_AlternateActionSwitch(Qcode::Qh238, "WasherR"s);
		ih_IndicatorLightBigActionSwitch(Qcode::WdoHeatL, "WdoHeatL"s, Qcode::gT38, Qcode::dimRelayK6b, Qcode::Qh239);
		ch_AlternateActionSwitch(Qcode::Qh239, "WdoHeatL"s);
		ih_IndicatorLightBigActionSwitch(Qcode::WdoHeatR, "WdoHeatR"s, Qcode::gT38, Qcode::dimRelayK6b, Qcode::Qh240);
		ch_AlternateActionSwitch(Qcode::Qh240, "WdoHeatR"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelXfeed1, "FuelXfeed1"s, Qcode::dimRelayK8b, Qcode::gT40, Qcode::Qh241);
		ch_AlternateActionSwitch(Qcode::Qh241, "FuelXfeed1"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelXfeed2, "FuelXfeed2"s, Qcode::dimRelayK6a, Qcode::gT40, Qcode::Qh242);
		ch_AlternateActionSwitch(Qcode::Qh242, "FuelXfeed2"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelXfeed3, "FuelXfeed3"s, Qcode::dimRelayK8b, Qcode::gT40, Qcode::Qh243);
		ch_AlternateActionSwitch(Qcode::Qh243, "FuelXfeed3"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelXfeed4, "FuelXfeed4"s, Qcode::dimRelayK8b, Qcode::gT40, Qcode::Qh244);
		ch_AlternateActionSwitch(Qcode::Qh244, "FuelXfeed4"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelMain1Fwd, "FuelMain1Fwd"s, Qcode::gT40, Qcode::dimRelayK6a, Qcode::Qh245);
		ch_AlternateActionSwitch(Qcode::Qh245, "FuelMain1Fwd"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelMain1Aft, "FuelMain1Aft"s, Qcode::gT40, Qcode::dimRelayK8b, Qcode::Qh246);
		ch_AlternateActionSwitch(Qcode::Qh246, "FuelMain1Aft"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelOvrd2Fwd, "FuelOvrd2Fwd"s, Qcode::gT40, Qcode::dimRelayK6a, Qcode::Qh247);
		ch_AlternateActionSwitch(Qcode::Qh247, "FuelOvrd2Fwd"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelOvrd2Aft, "FuelOvrd2Aft"s, Qcode::gT40, Qcode::dimRelayK8b, Qcode::Qh248);
		ch_AlternateActionSwitch(Qcode::Qh248, "FuelOvrd2Aft"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelMain2Fwd, "FuelMain2Fwd"s, Qcode::gT40, Qcode::dimRelayK6a, Qcode::Qh249);
		ch_AlternateActionSwitch(Qcode::Qh249, "FuelMain2Fwd"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelMain2Aft, "FuelMain2Aft"s, Qcode::gT40, Qcode::dimRelayK8b, Qcode::Qh250);
		ch_AlternateActionSwitch(Qcode::Qh250, "FuelMain2Aft"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelMain3Fwd, "FuelMain3Fwd"s, Qcode::gT40, Qcode::dimRelayK6b, Qcode::Qh251);
		ch_AlternateActionSwitch(Qcode::Qh251, "FuelMain3Fwd"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelMain3Aft, "FuelMain3Aft"s, Qcode::gT40, Qcode::dimRelayK8b, Qcode::Qh252);
		ch_AlternateActionSwitch(Qcode::Qh252, "FuelMain3Aft"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelOvrd3Fwd, "FuelOvrd3Fwd"s, Qcode::gT40, Qcode::dimRelayK6b, Qcode::Qh253);
		ch_AlternateActionSwitch(Qcode::Qh253, "FuelOvrd3Fwd"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelOvrd3Aft, "FuelOvrd3Aft"s, Qcode::gT40, Qcode::dimRelayK8b, Qcode::Qh254);
		ch_AlternateActionSwitch(Qcode::Qh254, "FuelOvrd3Aft"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelMain4Fwd, "FuelMain4Fwd"s, Qcode::gT40, Qcode::dimRelayK6b, Qcode::Qh255);
		ch_AlternateActionSwitch(Qcode::Qh255, "FuelMain4Fwd"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelMain4Aft, "FuelMain4Aft"s, Qcode::gT40, Qcode::dimRelayK8b, Qcode::Qh256);
		ch_AlternateActionSwitch(Qcode::Qh256, "FuelMain4Aft"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelAuxL, "FuelAuxL"s, Qcode::gT40, Qcode::dimRelayK8b, Qcode::Qh257);
		ch_AlternateActionSwitch(Qcode::Qh257, "FuelAuxL"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelAuxR, "FuelAuxR"s, Qcode::gT40, Qcode::dimRelayK6a, Qcode::Qh258);
		ch_AlternateActionSwitch(Qcode::Qh258, "FuelAuxR"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelCtrL, "FuelCtrL"s, Qcode::gT40, Qcode::dimRelayK6a, Qcode::Qh259);
		ch_AlternateActionSwitch(Qcode::Qh259, "FuelCtrL"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelCtrR, "FuelCtrR"s, Qcode::gT40, Qcode::dimRelayK8b, Qcode::Qh260);
		ch_AlternateActionSwitch(Qcode::Qh260, "FuelCtrR"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelStabL, "FuelStabL"s, Qcode::gT40, Qcode::dimRelayK8b, Qcode::Qh261);
		ch_AlternateActionSwitch(Qcode::Qh261, "FuelStabL"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelStabR, "FuelStabR"s, Qcode::gT40, Qcode::dimRelayK6a, Qcode::Qh262);
		ch_AlternateActionSwitch(Qcode::Qh262, "FuelStabR"s);
		// 263 is a rotswitch
		ih_IndicatorLightBigActionSwitch(Qcode::IgnitCon, "IgnitSwCon"s, Qcode::gR8098c, Qcode::gR8098c, Qcode::Qh264);
		ch_AlternateActionSwitch(Qcode::Qh264, "IgnitSwCon"s);
		// 265 is a rotswitch
		ih_IndicatorLightBigActionSwitch(Qcode::AutoStart, "AutoStart"s, Qcode::gT38, Qcode::gT38, Qcode::Qh266);
		ch_AlternateActionSwitch(Qcode::Qh266, "AutoStart"s);
		ii_WhiteLightEngStartSwitch(Qcode::EngStartSw1, "EngStartSw1"s, Qcode::dimRelayK6a, Qcode::Qh267);
		ch_AlternateActionSwitch(Qcode::Qh267, "EngStartSw1"s);
		ii_WhiteLightEngStartSwitch(Qcode::EngStartSw2, "EngStartSw2"s, Qcode::dimRelayK8b, Qcode::Qh268);
		ch_AlternateActionSwitch(Qcode::Qh268, "EngStartSw2"s);
		ii_WhiteLightEngStartSwitch(Qcode::EngStartSw3, "EngStartSw3"s, Qcode::dimRelayK6a, Qcode::Qh269);
		ch_AlternateActionSwitch(Qcode::Qh269, "EngStartSw3"s);
		ii_WhiteLightEngStartSwitch(Qcode::EngStartSw4, "EngStartSw4"s, Qcode::dimRelayK8b, Qcode::Qh270);
		ch_AlternateActionSwitch(Qcode::Qh270, "EngStartSw4"s);
		ih_IndicatorLightBigActionSwitch(Qcode::JettNozzL, "JettNozzL"s, Qcode::gT38, Qcode::dimRelayK6a, Qcode::Qh271);
		ch_AlternateActionSwitch(Qcode::Qh271, "JettNozzL"s);
		ih_IndicatorLightBigActionSwitch(Qcode::JettNozzR, "JettNozzR"s, Qcode::gT38, Qcode::dimRelayK6a, Qcode::Qh272);
		ch_AlternateActionSwitch(Qcode::Qh272, "JettNozzR"s);
		ch_ContinuousValueControlVar(Qcode::Qh273, "JettRemain"s, MinMax{ 62'830 });
		ch_ValueControlVar(Qcode::Qh274, "JettSelSystem"s, MinMax{ 4 });
		ih_IndicatorLightFireSwitch(Qcode::FireEng1, "FireEng1"s, Qcode::nonDimC358, Qcode::Qh275);
		ih_IndicatorLightFireSwitch(Qcode::FireEng2, "FireEng2"s, Qcode::nonDimC614, Qcode::Qh276);
		ih_IndicatorLightFireSwitch(Qcode::FireEng3, "FireEng3"s, Qcode::nonDimC358, Qcode::Qh277);
		ih_IndicatorLightFireSwitch(Qcode::FireEng4, "FireEng4"s, Qcode::nonDimC614, Qcode::Qh278);
		ih_IndicatorLightFireSwitch(Qcode::FireApu, "FireApu"s, Qcode::nonDimC614, Qcode::Qh279);
		ih_IndicatorLightBigActionSwitch(Qcode::FireArmFwd, "FireArmFwd"s, Qcode::gT38, Qcode::dimRelayK8b, Qcode::Qh280);
		ch_AlternateActionSwitch(Qcode::Qh280, "FireArmFwd"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FireArmAft, "FireArmAft"s, Qcode::gT38, Qcode::dimRelayK8a, Qcode::Qh281);
		ch_AlternateActionSwitch(Qcode::Qh281, "FireArmAft"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FireArmMain, "FireArmMain"s, Qcode::gT38, Qcode::dimRelayK8a, Qcode::Qh282);
		ch_AlternateActionSwitch(Qcode::Qh282, "FireArmMain"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FireCargoDisch, "FireCargoDisch"s, Qcode::gT38, Qcode::dimRelayK8b, Qcode::Qh283);
		ch_MomentaryActionSwitch(Qcode::Qh283, "FireCargoDisch"s);
		ch_AlternateActionSwitch(Qcode::Qh284, "FireTestRb"s);
		ch_AlternateActionSwitch(Qcode::Qh285, "FireTestAll"s);
		// 286 is On/off/on
		ch_AlternateActionSwitch(Qcode::Qh287, "VhfLDir"s);
		// 288 is OnOffOn terminal var
		ch_AlternateActionSwitch(Qcode::Qh289, "IntPhoServ"s);
		ch_AlternateActionSwitch(Qcode::Qh290, "IntPhoCargo"s);
		ih_IndicatorLightBigActionSwitch(Qcode::FuelXfer, "FuelXfer"s, Qcode::gT38, Qcode::gT38, Qcode::Qh291);
		ch_AlternateActionSwitch(Qcode::Qh291, "FuelXfer"s);
		ch_ValueControlVar(Qcode::Qh292, "AftCargoAirFlowSel"s, MinMax{ 2 });
		// 293 pass oxygen is on/off/on
		ih_IndicatorLightBigActionSwitch(Qcode::YawDamperUpr, "YawDamperUpr"s, Qcode::gT38, Qcode::dimRelayK8b, Qcode::Qh294);
		ch_AlternateActionSwitch(Qcode::Qh294, "YawDamperUpr"s);
		ih_IndicatorLightBigActionSwitch(Qcode::YawDamperLwr, "YawDamperLwr"s, Qcode::gT38, Qcode::dimRelayK4b, Qcode::Qh295);
		ch_AlternateActionSwitch(Qcode::Qh295, "YawDamperLwr"s);
		ch_MomentaryActionSwitch(Qcode::Qh296, "LdgAltPush"s);
		ch_ContinuousValueControlVar(Qcode::Qh297, "LdgAltTurn"s, MinMax{ 62'830 });
		ih_IndicatorLightBigActionSwitch(Qcode::OutflowManL, "OutflowManL"s, Qcode::gT44, Qcode::gT44, Qcode::Qh298);
		ch_AlternateActionSwitch(Qcode::Qh298, "OutflowManL"s);
		ih_IndicatorLightBigActionSwitch(Qcode::OutflowManR, "OutflowManR"s, Qcode::gT44, Qcode::gT44, Qcode::Qh299);
		ch_AlternateActionSwitch(Qcode::Qh299, "OutflowManR"s);
		// 300 is OnOfOn switch
		// 301 is rotary switch
		ch_ContinuousValueControlVar(Qcode::Qh302, "TempPassenger"s, MinMax{ -4'000, 4'000 });
		ch_ContinuousValueControlVar(Qcode::Qh303, "TempFlightDeck"s, MinMax{ -4'000, 4'000 });
		ch_ContinuousValueControlVar(Qcode::Qh304, "TempMainDkFwd"s, MinMax{ -4'000, 4'000 });
		ch_ContinuousValueControlVar(Qcode::Qh305, "TempMainDkAft"s, MinMax{ -4'000, 4'000 });
		ch_ContinuousValueControlVar(Qcode::Qh306, "TempLwrLobeFwd"s, MinMax{ -4'000, 4'000 });
		ch_ContinuousValueControlVar(Qcode::Qh307, "TempLwrLobeAft"s, MinMax{ -4'000, 4'000 });
		ch_ContinuousValueControlVar(Qcode::Qh308, "TempCargo"s, MinMax{ -4'000, 4'000 });
		ih_IndicatorLightBigActionSwitch(Qcode::FltDeckFan, "FltDeckFan"s, Qcode::gT38, Qcode::gT38, Qcode::Qh309);
		ch_AlternateActionSwitch(Qcode::Qh309, "FltDeckFan"s);
		ih_IndicatorLightBigActionSwitch(Qcode::ZoneReset, "ZoneReset"s, Qcode::gT38, Qcode::dimRelayK8a, Qcode::Qh310);
		ch_MomentaryActionSwitch(Qcode::Qh310, "ZoneReset"s);
		ih_IndicatorLightBigActionSwitch(Qcode::TrimAir, "TrimAir"s, Qcode::gT38, Qcode::gT38, Qcode::Qh311);
		ch_AlternateActionSwitch(Qcode::Qh311, "TrimAir"s);
		ih_IndicatorLightBigActionSwitch(Qcode::RecircUpr, "RecircUpr"s, Qcode::gT38, Qcode::gT38, Qcode::Qh312);
		ch_AlternateActionSwitch(Qcode::Qh312, "RecircUpr"s);
		ih_IndicatorLightBigActionSwitch(Qcode::RecircLwr, "RecircLwr"s, Qcode::gT38, Qcode::gT38, Qcode::Qh313);
		ch_AlternateActionSwitch(Qcode::Qh313, "RecircLwr"s);
		ih_IndicatorLightBigActionSwitch(Qcode::HighFlow, "HighFlow"s, Qcode::gT38, Qcode::gT38, Qcode::Qh314);
		ch_AlternateActionSwitch(Qcode::Qh314, "HighFlow"s);
		ih_IndicatorLightBigActionSwitch(Qcode::PackReset, "PackReset"s, Qcode::gT38, Qcode::dimRelayK8a, Qcode::Qh315);
		ch_MomentaryActionSwitch(Qcode::Qh315, "PackReset"s);
		ih_IndicatorLightBigActionSwitch(Qcode::Gasper, "Gasper"s, Qcode::gT38, Qcode::gT38, Qcode::Qh316);
		ch_AlternateActionSwitch(Qcode::Qh316, "Gasper"s);
		ih_IndicatorLightBigActionSwitch(Qcode::Humid, "Humid"s, Qcode::gT38, Qcode::gT38, Qcode::Qh317);
		ch_AlternateActionSwitch(Qcode::Qh317, "Humid"s);
		ih_IndicatorLightBigActionSwitch(Qcode::AftCargoHeat, "AftCargoHeat"s, Qcode::gT38, Qcode::dimRelayK8a, Qcode::Qh318);
		ch_AlternateActionSwitch(Qcode::Qh318, "AftCargoHeat"s);
		// 319, 320, 321 and 322 are rotswitches
		ih_IndicatorLightBigActionSwitch(Qcode::IslnValveL, "IslnValveL"s, Qcode::dimRelayK6b, Qcode::gR8098ab, Qcode::Qh323);
		ch_AlternateActionSwitch(Qcode::Qh323, "IslnValveL"s);
		ih_IndicatorLightBigActionSwitch(Qcode::IslnValveR, "IslnValveR"s, Qcode::dimRelayK4a, Qcode::gR8098ab, Qcode::Qh324);
		ch_AlternateActionSwitch(Qcode::Qh324, "IslnValveR"s);
		ih_IndicatorLightBigActionSwitch(Qcode::ApuValve, "ApuValve"s, Qcode::gR8098ab, Qcode::dimRelayK4b, Qcode::Qh325);
		ch_AlternateActionSwitch(Qcode::Qh325, "ApuValve"s);
		ih_IndicatorLightBigActionSwitch(Qcode::EngBleed1, "EngBleed1"s, Qcode::gR8098ab, Qcode::dimRelayK6b, Qcode::Qh326);
		ch_AlternateActionSwitch(Qcode::Qh326, "EngBleed1"s);
		ih_IndicatorLightBigActionSwitch(Qcode::EngBleed2, "EngBleed2"s, Qcode::gR8098ab, Qcode::dimRelayK4b, Qcode::Qh327);
		ch_AlternateActionSwitch(Qcode::Qh327, "EngBleed2"s);
		ih_IndicatorLightBigActionSwitch(Qcode::EngBleed3, "EngBleed3"s, Qcode::gR8098ab, Qcode::dimRelayK4a, Qcode::Qh328);
		ch_AlternateActionSwitch(Qcode::Qh328, "EngBleed3"s);
		ih_IndicatorLightBigActionSwitch(Qcode::EngBleed4, "EngBleed4"s, Qcode::gR8098ab, Qcode::dimRelayK8b, Qcode::Qh329);
		ch_AlternateActionSwitch(Qcode::Qh329, "EngBleed4"s);
		ch_AlternateActionSwitch(Qcode::Qh330, "PaxInfo"s);
		ch_ValueControlVar(Qcode::Qh331, "GndTests"s, MinMax{ 7 });
		ch_AlternateActionSwitch(Qcode::Qh332, "FltDkAccessLt"s);
		ch_ValueControlVar(Qcode::Qh333, "HydValveTail1"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh334, "HydValveTail2"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh335, "HydValveTail3"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh336, "HydValveTail4"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh337, "HydValveWing1"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh338, "HydValveWing2"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh339, "HydValveWing3"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh340, "HydValveWing4"s, MinMax{ 7 });
		ch_AlternateActionSwitch(Qcode::Qh341, "ApuStartSource"s);
		ch_ValueControlVar(Qcode::Qh342, "FieldGen1"s, MinMax{ 3 });
		ch_ValueControlVar(Qcode::Qh343, "FieldGen2"s, MinMax{ 3 });
		ch_ValueControlVar(Qcode::Qh344, "FieldGen3"s, MinMax{ 3 });
		ch_ValueControlVar(Qcode::Qh345, "FieldGen4"s, MinMax{ 3 });
		ch_ValueControlVar(Qcode::Qh346, "FieldApu1"s, MinMax{ 3 });
		ch_ValueControlVar(Qcode::Qh347, "FieldApu2"s, MinMax{ 3 });
		ch_ValueControlVar(Qcode::Qh348, "SplitSysBr"s, MinMax{ 3 });
		ch_AlternateActionSwitch(Qcode::Qh349, "TowPower"s);
		ch_ValueControlVar(Qcode::Qh350, "EecMaint1"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh351, "EecMaint2"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh352, "EecMaint3"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh353, "EecMaint4"s, MinMax{ 7 });
		ch_AlternateActionSwitch(Qcode::Qh354, "EecChan1"s);
		ch_AlternateActionSwitch(Qcode::Qh355, "EecChan2"s);
		ch_AlternateActionSwitch(Qcode::Qh356, "EecChan3"s);
		ch_AlternateActionSwitch(Qcode::Qh357, "EecChan4"s);
		// 358 is a rotswitch
		ch_AlternateActionSwitch(Qcode::Qh359, "CvrTest"s);
		ch_AlternateActionSwitch(Qcode::Qh360, "CvrErase"s);
		ch_AlternateActionSwitch(Qcode::Qh361, "SquibTest1"s);
		ch_AlternateActionSwitch(Qcode::Qh362, "SquibTest2"s);
		ch_AlternateActionSwitch(Qcode::Qh363, "SquibTest3"s);
		ch_AlternateActionSwitch(Qcode::Qh364, "SquibTest4"s);
		ih_IndicatorLightSquibButton(Qcode::SquibEng1, "SquibEng1"s, Qcode::Qh365);
		ch_AlternateActionSwitch(Qcode::Qh365, "SquibEng1"s);
		ih_IndicatorLightSquibButton(Qcode::SquibEng2, "SquibEng2"s, Qcode::Qh366);
		ch_AlternateActionSwitch(Qcode::Qh366, "SquibEng2"s);
		ih_IndicatorLightSquibButton(Qcode::SquibEng3, "SquibEng3"s, Qcode::Qh367);
		ch_AlternateActionSwitch(Qcode::Qh367, "SquibEng3"s);
		ih_IndicatorLightSquibButton(Qcode::SquibEng4, "SquibEng4"s, Qcode::Qh368);
		ch_AlternateActionSwitch(Qcode::Qh368, "SquibEng4"s);
		ih_IndicatorLightSquibButton(Qcode::SquibApu, "SquibApu"s, Qcode::Qh369);
		ch_AlternateActionSwitch(Qcode::Qh369, "SquibApu"s);
		ih_IndicatorLightSquibButton(Qcode::SquibA, "SquibA"s, Qcode::Qh370);
		ch_AlternateActionSwitch(Qcode::Qh370, "SquibA"s);
		ih_IndicatorLightSquibButton(Qcode::SquibB, "SquibB"s, Qcode::Qh371);
		ch_AlternateActionSwitch(Qcode::Qh371, "SquibB"s);
		ih_IndicatorLightSquibButton(Qcode::SquibC, "SquibC"s, Qcode::Qh372);
		ch_AlternateActionSwitch(Qcode::Qh372, "SquibC"s);
		ih_IndicatorLightSquibButton(Qcode::SquibD, "SquibD"s, Qcode::Qh373);
		ch_AlternateActionSwitch(Qcode::Qh373, "SquibD"s);
		ih_IndicatorLightSquibButton(Qcode::SquibE, "SquibE"s, Qcode::Qh374);
		ch_AlternateActionSwitch(Qcode::Qh374, "SquibE"s);
		ih_IndicatorLightSquibButton(Qcode::SquibF, "SquibF"s, Qcode::Qh375);
		ch_AlternateActionSwitch(Qcode::Qh375, "SquibF"s);
		ih_IndicatorLightSquibButton(Qcode::SquibAH, "SquibAH", Qcode::Qh376);
		ch_AlternateActionSwitch(Qcode::Qh376, "SquibAH"s);
		ih_IndicatorLightSquibButton(Qcode::SquibBJ, "SquibBJ"s, Qcode::Qh377);
		ch_AlternateActionSwitch(Qcode::Qh377, "SquibBJ"s);
		ih_IndicatorLightSquibButton(Qcode::SquibCK, "SquibCK"s, Qcode::Qh378);
		ch_AlternateActionSwitch(Qcode::Qh378, "SquibCK"s);
		ih_IndicatorLightSquibButton(Qcode::SquibDL, "SquibDL"s, Qcode::Qh379);
		ch_AlternateActionSwitch(Qcode::Qh379, "SquibDL"s);
		ih_IndicatorLightSquibButton(Qcode::SquibEM, "SquibEM"s, Qcode::Qh380);
		ch_AlternateActionSwitch(Qcode::Qh380, "SquibEM"s);
		ih_IndicatorLightSquibButton(Qcode::SquibFN, "SquibFN"s, Qcode::Qh381);
		ch_AlternateActionSwitch(Qcode::Qh381, "SquibFN"s);
		ih_IndicatorLightSquibButton(Qcode::SquibGP, "SquibGP"s, Qcode::Qh382);
		ch_AlternateActionSwitch(Qcode::Qh382, "SquibGP"s);
		ch_ValueControlVar(Qcode::Qh383, "DefuelRsv"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh384, "DefuelCwt"s, MinMax{ 7 });
		ch_ValueControlVar(Qcode::Qh385, "DefuelAux"s, MinMax{ 7 });
		// 386 and 387 are internal (collection) objects
		ch_ContinuousValueControlVar(Qcode::Qh388, "SpdBrkLever"s, MinMax{ 800 });
		ch_ValueControlVar(Qcode::Qh389, "FlapLever"s, MinMax{ 6 });
		// 390 en 391 zijn On/off/on objevts (StabTrimCut)
		ih_IndicatorLightFuelCutoffSwitch(Qcode::FuelCutoff1, "FuelCutoff1"s, Qcode::nonDimC358, Qcode::Qh392);
		ch_AlternateActionSwitch(Qcode::Qh392, "FuelCutoff1"s);
		ih_IndicatorLightFuelCutoffSwitch(Qcode::FuelCutoff2, "FuelCutoff2"s, Qcode::nonDimC614, Qcode::Qh393);
		ch_AlternateActionSwitch(Qcode::Qh393, "FuelCutoff2"s);
		ih_IndicatorLightFuelCutoffSwitch(Qcode::FuelCutoff3, "FuelCutoff3"s, Qcode::nonDimC358, Qcode::Qh394);
		ch_AlternateActionSwitch(Qcode::Qh394, "FuelCutoff3"s);
		ih_IndicatorLightFuelCutoffSwitch(Qcode::FuelCutoff4, "FuelCutoff4"s, Qcode::nonDimC614, Qcode::Qh395);
		ch_AlternateActionSwitch(Qcode::Qh395, "FuelCutoff4"s);
		ch_ValueControlVar(Qcode::Qh396, "AltnStabTrim"s, MinMax{ 15 });
		ch_AlternateActionSwitch(Qcode::Qh397, "ParkBrkLev"s);
		// 398 is OnOffOn switch
		// 399 is OnOffOn switch
		ch_MomentaryActionSwitch(Qcode::Qh400, "ApDisc"s);
		ch_OCKeyboard(Qcode::Qh401, "KeybCduL"s);
		ch_CduKeyboardVar(Qcode::KeybCduL, "KeybCduL"s, Qcode::Qh401);
		ch_OCKeyboard(Qcode::Qh402, "KeybCduC"s);
		ch_CduKeyboardVar(Qcode::KeybCduC, "KeybCduC"s, Qcode::Qh402);
		ch_OCKeyboard(Qcode::Qh403, "KeybCduR"s);
		ch_CduKeyboardVar(Qcode::KeybCduR, "KeybCduR"s, Qcode::Qh403);
		ch_Potentiometer270(Qcode::Qh404, "BrtCduL"s);
		ch_Potentiometer270(Qcode::Qh405, "BrtCduC"s);
		ch_Potentiometer270(Qcode::Qh406, "BrtCduR"s);
		ch_RcpPushButton(Qcode::RcpPshButton_0L, "RcpPshButtonL0"s, 0, Qcode::Qh407);
		ch_RcpPushButton(Qcode::RcpPshButton_1L, "RcpPshButtonL1"s, 1, Qcode::Qh407);
		ch_RcpPushButton(Qcode::RcpPshButton_2L, "RcpPshButtonL2"s, 2, Qcode::Qh407);
		ch_RcpPushButton(Qcode::RcpPshButton_3L, "RcpPshButtonL3"s, 3, Qcode::Qh407);
		ch_RcpPushButton(Qcode::RcpPshButton_4L, "RcpPshButtonL4"s, 4, Qcode::Qh407);
		ch_RcpPushButton(Qcode::RcpPshButton_5L, "RcpPshButtonL5"s, 5, Qcode::Qh407);
		ch_RcpPushButton(Qcode::RcpPshButton_6L, "RcpPshButtonL6"s, 6, Qcode::Qh407);
		ch_RcpPushButton(Qcode::RcpPshButton_7L, "RcpPshButtonL7"s, 7, Qcode::Qh407);
		ch_RcpPushButton(Qcode::RcpPshButton_0C, "RcpPshButtonC0"s, 0, Qcode::Qh408);
		ch_RcpPushButton(Qcode::RcpPshButton_1C, "RcpPshButtonC1"s, 1, Qcode::Qh408);
		ch_RcpPushButton(Qcode::RcpPshButton_2C, "RcpPshButtonC2"s, 2, Qcode::Qh408);
		ch_RcpPushButton(Qcode::RcpPshButton_3C, "RcpPshButtonC3"s, 3, Qcode::Qh408);
		ch_RcpPushButton(Qcode::RcpPshButton_4C, "RcpPshButtonC4"s, 4, Qcode::Qh408);
		ch_RcpPushButton(Qcode::RcpPshButton_5C, "RcpPshButtonC5"s, 5, Qcode::Qh408);
		ch_RcpPushButton(Qcode::RcpPshButton_6C, "RcpPshButtonC6"s, 6, Qcode::Qh408);
		ch_RcpPushButton(Qcode::RcpPshButton_7C, "RcpPshButtonC7"s, 7, Qcode::Qh408);
		ch_RcpPushButton(Qcode::RcpPshButton_0R, "RcpPshButtonR0"s, 0, Qcode::Qh409);
		ch_RcpPushButton(Qcode::RcpPshButton_1R, "RcpPshButtonR1"s, 1, Qcode::Qh409);
		ch_RcpPushButton(Qcode::RcpPshButton_2R, "RcpPshButtonR2"s, 2, Qcode::Qh409);
		ch_RcpPushButton(Qcode::RcpPshButton_3R, "RcpPshButtonR3"s, 3, Qcode::Qh409);
		ch_RcpPushButton(Qcode::RcpPshButton_4R, "RcpPshButtonR4"s, 4, Qcode::Qh409);
		ch_RcpPushButton(Qcode::RcpPshButton_5R, "RcpPshButtonR5"s, 5, Qcode::Qh409);
		ch_RcpPushButton(Qcode::RcpPshButton_6R, "RcpPshButtonR6"s, 6, Qcode::Qh409);
		ch_RcpPushButton(Qcode::RcpPshButton_7R, "RcpPshButtonR7"s, 7, Qcode::Qh409);
		ch_AcpPushButton(Qcode::AcpPshButton_0L, "AcpPshButtonL0"s, 0, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_1L, "AcpPshButtonL1"s, 1, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_2L, "AcpPshButtonL2"s, 2, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_3L, "AcpPshButtonL3"s, 3, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_4L, "AcpPshButtonL4"s, 4, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_5L, "AcpPshButtonL5"s, 5, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_6L, "AcpPshButtonL6"s, 6, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_7L, "AcpPshButtonL7"s, 7, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_8L, "AcpPshButtonL8"s, 8, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_9L, "AcpPshButtonL9"s, 9, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_10L, "AcpPshButtonL10"s, 10, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_11L, "AcpPshButtonL11"s, 11, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_12L, "AcpPshButtonL12"s, 12, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_13L, "AcpPshButtonL13"s, 13, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_14L, "AcpPshButtonL14"s, 14, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_15L, "AcpPshButtonL15"s, 15, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_16L, "AcpPshButtonL16"s, 16, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_17L, "AcpPshButtonL17"s, 17, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_18L, "AcpPshButtonL18"s, 18, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_19L, "AcpPshButtonL19"s, 19, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_20L, "AcpPshButtonL20"s, 20, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_21L, "AcpPshButtonL21"s, 21, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_22L, "AcpPshButtonL22"s, 22, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_26L, "AcpPshButtonL26"s, 26, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_27L, "AcpPshButtonL27"s, 27, Qcode::Qh410);
		ch_AcpPushButton(Qcode::AcpPshButton_0C, "AcpPshButtonC0"s, 0, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_1C, "AcpPshButtonC1"s, 1, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_2C, "AcpPshButtonC2"s, 2, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_3C, "AcpPshButtonC3"s, 3, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_4C, "AcpPshButtonC4"s, 4, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_5C, "AcpPshButtonC5"s, 5, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_6C, "AcpPshButtonC6"s, 6, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_7C, "AcpPshButtonC7"s, 7, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_8C, "AcpPshButtonC8"s, 8, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_9C, "AcpPshButtonC9"s, 9, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_10C, "AcpPshButtonC10"s, 10, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_11C, "AcpPshButtonC11"s, 11, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_12C, "AcpPshButtonC12"s, 12, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_13C, "AcpPshButtonC13"s, 13, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_14C, "AcpPshButtonC14"s, 14, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_15C, "AcpPshButtonC15"s, 15, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_16C, "AcpPshButtonC16"s, 16, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_17C, "AcpPshButtonC17"s, 17, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_18C, "AcpPshButtonC18"s, 18, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_19C, "AcpPshButtonC19"s, 19, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_20C, "AcpPshButtonC20"s, 20, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_21C, "AcpPshButtonC21"s, 21, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_22C, "AcpPshButtonC22"s, 22, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_26C, "AcpPshButtonC26"s, 26, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_27C, "AcpPshButtonC27"s, 27, Qcode::Qh411);
		ch_AcpPushButton(Qcode::AcpPshButton_0R, "AcpPshButtonR0"s, 0, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_1R, "AcpPshButtonR1"s, 1, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_2R, "AcpPshButtonR2"s, 2, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_3R, "AcpPshButtonR3"s, 3, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_4R, "AcpPshButtonR4"s, 4, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_5R, "AcpPshButtonR5"s, 5, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_6R, "AcpPshButtonR6"s, 6, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_7R, "AcpPshButtonR7"s, 7, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_8R, "AcpPshButtonR8"s, 8, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_9R, "AcpPshButtonR9"s, 9, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_10R, "AcpPshButtonR10"s, 10, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_11R, "AcpPshButtonR11"s, 11, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_12R, "AcpPshButtonR12"s, 12, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_13R, "AcpPshButtonR13"s, 13, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_14R, "AcpPshButtonR14"s, 14, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_15R, "AcpPshButtonR15"s, 15, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_16R, "AcpPshButtonR16"s, 16, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_17R, "AcpPshButtonR17"s, 17, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_18R, "AcpPshButtonR18"s, 18, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_19R, "AcpPshButtonR19"s, 19, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_20R, "AcpPshButtonR20"s, 20, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_21R, "AcpPshButtonR21"s, 21, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_22R, "AcpPshButtonR22"s, 22, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_26R, "AcpPshButtonR26"s, 26, Qcode::Qh412);
		ch_AcpPushButton(Qcode::AcpPshButton_27R, "AcpPshButtonR27"s, 27, Qcode::Qh412);
		ch_ValueControlVar(Qcode::Qh413, "CallPanel"s, MinMax{ -1, 8 });
		ch_TransponderKey(Qcode::XpndrPshButton_0, "XpndrPshButton0"s, 0, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_1, "XpndrPshButton1"s, 1, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_2, "XpndrPshButton2"s, 2, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_3, "XpndrPshButton3"s, 3, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_4, "XpndrPshButton4"s, 4, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_5, "XpndrPshButton5"s, 5, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_6, "XpndrPshButton6"s, 6, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_7, "XpndrPshButton7"s, 7, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_CLR, "XpndrPshButtonCLR"s, 8, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_IDNT, "XpndrPshButtonIDNT"s, 9, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_TEST, "XpndrPshButtonTEST"s, 10, Qcode::Qh414);
		ch_TransponderKey(Qcode::XpndrPshButton_FL, "XpndrPshButtonFL"s, 11, Qcode::Qh414);
		ch_ValueControlVar(Qcode::Qh415, "PrinterPush"s, MinMax{ -1, 2 });
		ch_ValueControlVar(Qcode::Qh416, "RudderTrim"s, MinMax{ -2, 2 });
		ih_IndicatorLightBigActionSwitch(Qcode::RudderCtr, "RudderCtr"s, Qcode::gDivAislePnlT144, Qcode::dimRelayK4a, Qcode::Qh417);
		ch_MomentaryActionSwitch(Qcode::Qh417, "RudderCtr"s);
		ch_ValueControlVar(Qcode::Qh418, "AileronTrim"s, MinMax{ 15 });
		ch_ValueControlVar(Qcode::Qh419, "FltDkDoorTurn"s, MinMax{ 2 });
		// 420 .. 424 are collectionvars
		ih_EvacLight(Qcode::EvacLight, "EvacLight", Qcode::Qh424);
		ch_ValueControlVar(Qcode::Qh425, "ToeBrakeTogg"s, MinMax{ 3 });
		ch_ContinuousValueControlVar(Qcode::Qh426, "Tiller"s, MinMax{ -999, 999 });
		ch_ContinuousValueControlVar(Qcode::Qh427, "SmokeEvac"s, MinMax{ 1'000 });
		// 428 .. 430 not (yet?) implemented
		ch_ValueControlVar(Qcode::Qh998, "CDUScreenSelector"s, MinMax{ 2 });  // 0 = Left, 1 = Center, 2 = Right


		// Qi
		ii_QcodeInfo(Qcode::Qi0, "CfgVariant"s);
		ci_QiValueControlVar(Qcode::CfgVariantCtrl, "CfgVariantCtrl"s, Qcode::Qi0, MinMax{ 4 });
		ii_QcodeInfo(Qcode::Qi1, "CfgEngineModel"s);
		ci_QiValueControlVar(Qcode::CfgEngineModelCtrl, "CfgEngineModelCtrl"s, Qcode::Qi1, MinMax{ 2 });
		ii_QcodeInfo(Qcode::Qi2, "CfgAdfUp"s);
		ci_QiAlternateActionSwitch(Qcode::CfgAdfUpCtrl, "CfgAdfUpCtrl"s, Qcode::Qi2);
		ii_QcodeInfo(Qcode::Qi3, "CfgAltnEfisSw"s);
		ci_QiValueControlVar(Qcode::CfgAltnEfisSwCtrl, "CfgAltnEfisSwCtrl"s, Qcode::Qi3, MinMax{ 2 });
		ii_QcodeInfo(Qcode::Qi4, "CfgMlsSw"s);
		ci_QiAlternateActionSwitch(Qcode::CfgMlsSwCtrl, "CfgMlsSwCtrl"s, Qcode::Qi4);
		ii_QcodeInfo(Qcode::Qi5, "CfgLcdIds"s);
		ci_QiAlternateActionSwitch(Qcode::CfgLcdIdsCtrl, "CfgLcdIdsCtrl"s, Qcode::Qi5);
		ii_QcodeInfo(Qcode::Qi6, "CfgStbyInstr"s);
		ci_QiValueControlVar(Qcode::CfgStbyInstrCtrl, "CfgStbyInstrCtrl"s, Qcode::Qi6, MinMax{ 2 });
		ii_QcodeInfo(Qcode::Qi7, "CfgBelowClkCp"s);
		ci_QiValueControlVar(Qcode::CfgBelowClkCpCtrl, "CfgBelowClkCpCtrl"s, Qcode::Qi7, MinMax{ 3 });
		ii_QcodeInfo(Qcode::Qi8, "CfgBelowClkFo"s);
		ci_QiValueControlVar(Qcode::CfgBelowClkFoCtrl, "CfgBelowClkFoCtrl"s, Qcode::Qi8, MinMax{ 3 });
		ii_QcodeInfo(Qcode::Qi9, "CfgIrsAuto"s);
		ci_QiAlternateActionSwitch(Qcode::CfgIrsAutoCtrl, "CfgIrsAutoCtrl"s, Qcode::Qi9);
		ii_QcodeInfo(Qcode::Qi10, "CfgAdc"s);
		ci_QiValueControlVar(Qcode::CfgAdcCtrl, "CfgAdcCtrl"s, Qcode::Qi10, MinMax{ 2 });
		ii_QcodeInfo(Qcode::Qi11, "CfgAutobrFront"s);
		ci_QiAlternateActionSwitch(Qcode::CfgAutobrFrontCtrl, "CfgAutobrFrontCtrl"s, Qcode::Qi11);
		ii_QcodeInfo(Qcode::Qi12, "CfgLcdMcp"s);
		ci_QiAlternateActionSwitch(Qcode::CfgLcdMcpCtrl, "CfgLcdMcpCtrl"s, Qcode::Qi12);
		ii_QcodeInfo(Qcode::Qi13, "CfgMachShort"s);
		ci_QiAlternateActionSwitch(Qcode::CfgMachShortCtrl, "CfgMachShortCtrl"s, Qcode::Qi13);
		ii_QcodeInfo(Qcode::Qi14, "CfgStbyCompLtCp"s);
		ci_QiAlternateActionSwitch(Qcode::CfgStbyCompLtCpCtrl, "CfgStbyCompLtCpCtrl"s, Qcode::Qi14);
		ii_QcodeInfo(Qcode::Qi15, "CfgLightsOnUp"s);
		ci_QiAlternateActionSwitch(Qcode::CfgLightsOnUpCtrl, "CfgLightsOnUpCtrl"s, Qcode::Qi15);
		ii_QcodeInfo(Qcode::Qi16, "CfgTaxiLight"s);
		ci_QiAlternateActionSwitch(Qcode::CfgTaxiLightCtrl, "CfgTaxiLightCtrl"s, Qcode::Qi16);
		ii_QcodeInfo(Qcode::Qi17, "CfgHydAuxPump1"s);
		ci_QiAlternateActionSwitch(Qcode::CfgHydAuxPump1Ctrl, "CfgHydAuxPump1Ctrl"s, Qcode::Qi17);
		ii_QcodeInfo(Qcode::Qi18, "CfgHyd23AC"s);
		ci_QiAlternateActionSwitch(Qcode::CfgHyd23ACCtrl, "CfgHyd23ACCtrl"s, Qcode::Qi18);
		ii_QcodeInfo(Qcode::Qi19, "CfgHydDemFlaps"s);
		ci_QiAlternateActionSwitch(Qcode::CfgHydDemFlapsCtrl, "CfgHydDemFlapsCtrl"s, Qcode::Qi19);
		ii_QcodeInfo(Qcode::Qi20, "CfgAutoStartType"s);
		ci_QiValueControlVar(Qcode::CfgAutoStartTypeCtrl, "CfgAutoStartTypeCtrl"s, Qcode::Qi20, MinMax{ 2 });
		ii_QcodeInfo(Qcode::Qi21, "CfgNaiAuto"s);
		ci_QiAlternateActionSwitch(Qcode::CfgNaiAutoCtrl, "CfgNaiAutoCtrl"s, Qcode::Qi21);
		ii_QcodeInfo(Qcode::Qi22, "CfgWaiAuto"s);
		ci_QiAlternateActionSwitch(Qcode::CfgWaiAutoCtrl, "CfgWaiAutoCtrl"s, Qcode::Qi22);
		ii_QcodeInfo(Qcode::Qi23, "CfgFuelAux"s);
		ci_QiAlternateActionSwitch(Qcode::CfgFuelAuxCtrl, "CfgFuelAuxCtrl"s, Qcode::Qi23);
		ii_QcodeInfo(Qcode::Qi24, "CfgFuelHst"s);
		ci_QiAlternateActionSwitch(Qcode::CfgFuelHstCtrl, "CfgFuelHstCtrl"s, Qcode::Qi24);
		ii_QcodeInfo(Qcode::Qi25, "CfgJettisonMlw"s);
		ci_QiAlternateActionSwitch(Qcode::CfgJettisonMlwCtrl, "CfgJettisonMlwCtrl"s, Qcode::Qi25);
		ii_QcodeInfo(Qcode::Qi26, "CfgFuelHydMecPmp"s);
		ci_QiAlternateActionSwitch(Qcode::CfgFuelHydMecPmpCtrl, "CfgFuelHydMecPmpCtrl"s, Qcode::Qi26);
		ii_QcodeInfo(Qcode::Qi27, "CfgWbs"s);
		ci_QiValueControlVar(Qcode::CfgWbsCtrl, "CfgWbsCtrl"s, Qcode::Qi27, MinMax{ 2 });
		ii_QcodeInfo(Qcode::Qi28, "CfgConIgnHiIdle"s);
		ci_QiAlternateActionSwitch(Qcode::CfgConIgnHiIdleCtrl, "CfgConIgnHiIdleCtrl"s, Qcode::Qi28);
		ii_QcodeInfo(Qcode::Qi29, "CfgNdRwyLim"s);
		ci_QiValueControlVar(Qcode::CfgNdRwyLimCtrl, "CfgNdRwyLimCtrl"s, Qcode::Qi29, MinMax{ 5'000 });
		ii_QcodeInfo(Qcode::Qi30, "CfgPinProg"s);
		ci_QiControlVar(Qcode::CfgPinProgCtrl, "CfgPinProgCtrl"s, Qcode::Qi30);
		ii_QcodeInfo(Qcode::Qi31, "CfgEgpws"s);
		ci_QiControlVar(Qcode::CfgEgpwsCtrl, "CfgEgpwsCtrl"s, Qcode::Qi31);
		storeVarPtr(std::make_shared<McpSpdWin>(Qcode::Qi32, "McpWdoSpd"));
		storeVarPtr(std::make_shared<McpHdgWin>(Qcode::Qi33, "McpWdoHdg"));
		storeVarPtr(std::make_shared<McpVsWin>(Qcode::Qi34, "McpWdoVs"));
		storeVarPtr(std::make_shared<McpAltWin>(Qcode::Qi35, "McpWdoAlt"));
		ii_QcodeInfo(Qcode::Qi36, "McpLcd"s);
		ii_QcodeInfo(Qcode::Qi37, "StbyIasError"s);
		ii_QcodeInfo(Qcode::Qi38, "StbyBaro"s);
		ci_QiValueControlVar(Qcode::CfgStdbyBaro, "CfgStdbyBaro"s, Qcode::Qi38, MinMax{ 74500, 110025 });
		ii_QcodeInfo(Qcode::Qi39, "IsfdInitTime"s);
		ii_QcodeInfo(Qcode::Qi40, "IsfdFreddie"s);
		ii_QcodeInfo(Qcode::Qi41, "IsfdBaroStd"s);
		ii_QcodeInfo(Qcode::Qi42, "IsfdBrtness"s);
		ii_IndicatorLightNotIntegrated(Qcode::Qi43, "BrakeSourceLt"s, Qcode::dimRelayK2a);
		ii_IndicatorLightNotIntegrated(Qcode::Qi44, "HydSysFaultLt1"s, Qcode::dimRelayK6a);
		ii_IndicatorLightNotIntegrated(Qcode::Qi45, "HydSysFaultLt2"s, Qcode::dimRelayK6a);
		ii_IndicatorLightNotIntegrated(Qcode::Qi46, "HydSysFaultLt3"s, Qcode::dimRelayK4a);
		ii_IndicatorLightNotIntegrated(Qcode::Qi47, "HydSysFaultLt4"s, Qcode::dimRelayK2a);
		ii_IndicatorLightNotIntegrated(Qcode::Qi48, "HydDemPressLt1"s, Qcode::dimRelayK6b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi49, "HydDemPressLt2"s, Qcode::dimRelayK6b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi50, "HydDemPressLt3"s, Qcode::dimRelayK6b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi51, "HydDemPressLt4"s, Qcode::dimRelayK6b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi52, "EngBtl1aLt"s, Qcode::dimRelayK2a); // RR
		ii_IndicatorLightNotIntegrated(Qcode::Qi53, "EngBtl1bLt"s, Qcode::dimRelayK2a); // RR
		ii_IndicatorLightNotIntegrated(Qcode::Qi54, "EngBtl2aLt"s, Qcode::dimRelayK2a); // RR
		ii_IndicatorLightNotIntegrated(Qcode::Qi55, "EngBtl2bLt"s, Qcode::dimRelayK2a); // RR
		ii_IndicatorLightNotIntegrated(Qcode::Qi56, "EngBtl3aLt"s, Qcode::dimRelayK2a); // RR
		ii_IndicatorLightNotIntegrated(Qcode::Qi57, "EngBtl3bLt"s, Qcode::dimRelayK2a); // RR
		ii_IndicatorLightNotIntegrated(Qcode::Qi58, "EngBtl4aLt"s, Qcode::dimRelayK2a); // RR
		ii_IndicatorLightNotIntegrated(Qcode::Qi59, "EngBtl4bLt"s, Qcode::dimRelayK2a); // RR
		ii_IndicatorLightNotIntegrated(Qcode::Qi60, "WingBtlLaLt"s, Qcode::dimRelayK2a); // GE or PW
		ii_IndicatorLightNotIntegrated(Qcode::Qi61, "WingBtlLbLt"s, Qcode::dimRelayK2a); // GE or PW
		ii_IndicatorLightNotIntegrated(Qcode::Qi62, "WingBtlRaLt"s, Qcode::dimRelayK2a); // GE or PW
		ii_IndicatorLightNotIntegrated(Qcode::Qi63, "WingBtlRbLt"s, Qcode::dimRelayK2a); // GE or PW
		ii_IndicatorLightNotIntegrated(Qcode::Qi64, "ApuBtlLt"s, Qcode::dimRelayK4b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi65, "EngBleedSysFaultLt1"s, Qcode::dimRelayK6b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi66, "EngBleedSysFaultLt2"s, Qcode::dimRelayK4b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi67, "EngBleedSysFaultLt3"s, Qcode::dimRelayK4a);
		ii_IndicatorLightNotIntegrated(Qcode::Qi68, "EngBleedSysFaultLt4"s, Qcode::dimRelayK8b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi69, "HydValveLtTail1"s, Qcode::dimRelayK8a);
		ii_IndicatorLightNotIntegrated(Qcode::Qi70, "HydValveLtTail2"s, Qcode::dimRelayK8a);
		ii_IndicatorLightNotIntegrated(Qcode::Qi71, "HydValveLtTail3"s, Qcode::dimRelayK8a);
		ii_IndicatorLightNotIntegrated(Qcode::Qi72, "HydValveLtTail4"s, Qcode::dimRelayK8a);
		ii_IndicatorLightNotIntegrated(Qcode::Qi73, "HydValveLtWing1"s, Qcode::dimRelayK4b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi74, "HydValveLtWing2"s, Qcode::dimRelayK4b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi75, "HydValveLtWing3"s, Qcode::dimRelayK4b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi76, "HydValveLtWing4"s, Qcode::dimRelayK4b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi77, "FieldLtGen1"s, Qcode::dimRelayK2b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi78, "FieldLtGen2"s, Qcode::dimRelayK2b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi79, "FieldLtGen3"s, Qcode::dimRelayK2b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi80, "FieldLtGen4"s, Qcode::dimRelayK2b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi81, "FieldLtApu1"s, Qcode::dimRelayK2b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi82, "FieldLtApu2"s, Qcode::dimRelayK2b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi83, "SsbOpenLt"s, Qcode::dimRelayK2b);
		ii_IndicatorLightNotIntegrated(Qcode::Qi84, "TowOnBatLt"s, Qcode::directTowBatLt);
		ii_IndicatorLightNotIntegrated(Qcode::Qi85, "IrsOnBatLt"s, Qcode::directIrsBatLt);
		ii_CduIndicatorLights(Qcode::Qi86, "LightsCduL"s, Qcode::dimRelayK2b);
		ii_CduIndicatorLights(Qcode::Qi87, "LightsCduC"s, Qcode::dimRelayK4a);
		ii_CduIndicatorLights(Qcode::Qi88, "LightsCduR"s, Qcode::dimRelayK6a);
		ii_QcodeInfo(Qcode::Qi89, "BlankTimeCduL"s);
		ii_QcodeInfo(Qcode::Qi90, "BlankTimeCduC"s);
		ii_QcodeInfo(Qcode::Qi91, "BlankTimeCduR"s);
		ii_RCPIndicatorLights(Qcode::Qi92, "IndicatorsRcpL"s, Qcode::dimRelayK2a);
		ii_RCPIndicatorLights(Qcode::Qi93, "IndicatorsRcpC"s, Qcode::dimRelayK4a);
		ii_RCPIndicatorLights(Qcode::Qi94, "IndicatorsRcpR"s, Qcode::dimRelayK6a);
		ii_QcodeInfo(Qcode::Qi95, "LockoutsRcpL"s);
		ii_QcodeInfo(Qcode::Qi96, "LockoutsRcpC"s);
		ii_QcodeInfo(Qcode::Qi97, "LockoutsRcpR"s);
		ii_QcodeInfo(Qcode::Qi98, "CallFrVhfL"s);
		ii_CallFrLight(Qcode::CallFrVhfL_L, "CallFrVhfL_L"s, LCR::left, Qcode::Qi98);
		ii_CallFrLight(Qcode::CallFrVhfL_C, "CallFrVhfL_C"s, LCR::center, Qcode::Qi98);
		ii_CallFrLight(Qcode::CallFrVhfL_R, "CallFrVhfL_R"s, LCR::right, Qcode::Qi98);
		ii_QcodeInfo(Qcode::Qi99, "CallFrVhfC"s);
		ii_CallFrLight(Qcode::CallFrVhfC_L, "CallFrVhfC_L"s, LCR::left, Qcode::Qi99);
		ii_CallFrLight(Qcode::CallFrVhfC_C, "CallFrVhfC_C"s, LCR::center, Qcode::Qi99);
		ii_CallFrLight(Qcode::CallFrVhfC_R, "CallFrVhfC_R"s, LCR::right, Qcode::Qi99);
		ii_QcodeInfo(Qcode::Qi100, "CallFrVhfR"s);
		ii_CallFrLight(Qcode::CallFrVhfR_L, "CallFrVhfR_L"s, LCR::left, Qcode::Qi100);
		ii_CallFrLight(Qcode::CallFrVhfR_C, "CallFrVhfR_C"s, LCR::center, Qcode::Qi100);
		ii_CallFrLight(Qcode::CallFrVhfR_R, "CallFrVhfR_R"s, LCR::right, Qcode::Qi100);
		ii_QcodeInfo(Qcode::Qi101, "CallFrIntph"s);
		ii_CallFrLight(Qcode::CallFrIntph_L, "CallFrIntph_L"s, LCR::left, Qcode::Qi101);
		ii_CallFrLight(Qcode::CallFrIntph_C, "CallFrIntph_C"s, LCR::center, Qcode::Qi101);
		ii_CallFrLight(Qcode::CallFrIntph_R, "CallFrIntph_R"s, LCR::right, Qcode::Qi101);
		ii_QcodeInfo(Qcode::Qi102, "CallFrCabin"s);
		ii_CallFrLight(Qcode::CallFrCabin_L, "CallFrCabin_L"s, LCR::left, Qcode::Qi102);
		ii_CallFrLight(Qcode::CallFrCabin_C, "CallFrCabin_C"s, LCR::center, Qcode::Qi102);
		ii_CallFrLight(Qcode::CallFrCabin_R, "CallFrCabin_R"s, LCR::right, Qcode::Qi102);
		ii_QcodeInfo(Qcode::Qi103, "CallFrHfL"s);
		ii_CallFrLight(Qcode::CallFrHfL_L, "CallFrHfL_L"s, LCR::left, Qcode::Qi103);
		ii_CallFrLight(Qcode::CallFrHfL_C, "CallFrHfL_C"s, LCR::center, Qcode::Qi103);
		ii_CallFrLight(Qcode::CallFrHfL_R, "CallFrHfL_R"s, LCR::right, Qcode::Qi103);
		ii_QcodeInfo(Qcode::Qi104, "CallFrHfR"s);
		ii_CallFrLight(Qcode::CallFrHfR_L, "CallFrHfR_L"s, LCR::left, Qcode::Qi104);
		ii_CallFrLight(Qcode::CallFrHfR_C, "CallFrHfR_C"s, LCR::center, Qcode::Qi104);
		ii_CallFrLight(Qcode::CallFrHfR_R, "CallFrHfR_R"s, LCR::right, Qcode::Qi104);
		ci_QiValueControlVar(Qcode::CallFrCabinCtrl, "CallFrCabinCtrl"s, Qcode::Qi102, MinMax{ 99 });
		ii_QcodeInfo(Qcode::Qi105, "EvacOnByCabin"s);
		ii_ACPgreenLEDs(Qcode::Qi106, "ReceiverSelL"s, LCR::left);
		ii_ACPgreenLEDs(Qcode::Qi107, "ReceiverSelC"s, LCR::center);
		ii_ACPgreenLEDs(Qcode::Qi108, "ReceiverSelR"s, LCR::right);
		ii_AcpMICLights(Qcode::Qi109, "MicSelectL"s, LCR::left);
		ii_AcpMICLights(Qcode::Qi110, "MicSelectC"s, LCR::center);
		ii_AcpMICLights(Qcode::Qi111, "MicSelectR"s, LCR::right);
		ii_QcodeInfo(Qcode::Qi112, "CallPanelStat"s);
		ii_QcodeInfo(Qcode::Qi113, "PaVidInUse"s);
		ii_QcodeInfo(Qcode::Qi114, "CallCargoStat"s);
		ii_QcodeInfo(Qcode::Qi115, "PrinterStat"s);
		ii_QcodeInfo(Qcode::Qi116, "PaperQty"s);
		ii_QcodeInfo(Qcode::Qi117, "FltDkDoorStat"s);
		ii_QcodeInfo(Qcode::Qi118, "SpeedTrim"s);
		ii_QcodeInfo(Qcode::Qi119, "StabTrim"s);
		ii_QcodeInfo(Qcode::Qi120, "AilTrim"s);
		ii_QcodeInfo(Qcode::Qi121, "RudTrim"s);
		ii_QcodeInfo(Qcode::Qi122, "TrueCg"s);
		ii_QcodeInfo(Qcode::Qi123, "TrueZfw"s);
		ii_QcodeInfo(Qcode::Qi124, "ClkChronoOnL"s);
		ii_QcodeInfo(Qcode::Qi125, "ClkChronoOnR"s);
		ii_QcodeInfo(Qcode::Qi126, "ClkDateOnL"s);
		ii_QcodeInfo(Qcode::Qi127, "ClkDateOnR"s);
		ii_QcodeInfo(Qcode::Qi128, "TimeAccel"s);
		ii_QcodeInfo(Qcode::Qi129, "SimPaused"s);
		ci_QiValueControlVar(Qcode::SimPausedCtrl, "SimPausedCtrl"s, Qcode::Qi129, MinMax{ 3 });
		ii_QcodeInfo(Qcode::Qi130, "MfEvent"s);
		ii_QcodeInfo(Qcode::Qi131, "Ias"s);
		ii_QcodeInfo(Qcode::Qi132, "ElecSysBitsMain"s);
		ii_QcodeInfo(Qcode::Qi133, "ElecSysBitsSub"s);
		ii_QcodeInfo(Qcode::Qi134, "BatVoltMain"s);
		ii_QcodeInfo(Qcode::Qi135, "BatVoltApu"s);
		ii_QcodeInfo(Qcode::Qi136, "BatVoltIsfd"s);
		ii_QcodeInfo(Qcode::Qi137, "EicasLwrPaging"s);
		ii_QcodeInfo(Qcode::Qi138, "EicasMsgUpd"s);
		ii_QcodeInfo(Qcode::Qi139, "WxAutoSet"s);
		ii_QcodeInfo(Qcode::Qi140, "FromMetar"s);
		ii_QcodeInfo(Qcode::Qi141, "WxBurst"s);
		ii_QcodeInfo(Qcode::Qi142, "VeryRed"s);
		ii_QcodeInfo(Qcode::Qi143, "TimeJump"s);
		ii_QcodeInfo(Qcode::Qi144, "ParkBrkPawl"s);
		ii_QcodeInfo(Qcode::Qi145, "GpsInitL"s);
		ii_QcodeInfo(Qcode::Qi146, "GpsInitR"s);
		ii_QcodeInfo(Qcode::Qi147, "Anp"s);
		ii_QcodeInfo(Qcode::Qi148, "IrsHdgErr"s);
		ii_QcodeInfo(Qcode::Qi149, "AcarsMsg"s);
		ii_QcodeInfo(Qcode::Qi150, "EgtRedTime1"s);
		ii_QcodeInfo(Qcode::Qi151, "EgtRedTime2"s);
		ii_QcodeInfo(Qcode::Qi152, "EgtRedTime3"s);
		ii_QcodeInfo(Qcode::Qi153, "EgtRedTime4"s);
		ii_QcodeInfo(Qcode::Qi154, "TkofTime"s);
		ii_QcodeInfo(Qcode::Qi155, "EngExceedCue"s);
		ii_QcodeInfo(Qcode::Qi156, "TrSleeve1"s);
		ii_QcodeInfo(Qcode::Qi157, "TrSleeve2"s);
		ii_QcodeInfo(Qcode::Qi158, "TrSleeve3"s);
		ii_QcodeInfo(Qcode::Qi159, "TrSleeve4"s);
		ii_QcodeInfo(Qcode::Qi160, "ApuPhase"s);
		ii_QcodeInfo(Qcode::Qi161, "HydSysBits"s);
		ii_QcodeInfo(Qcode::Qi162, "AilLockout"s);
		ii_QcodeInfo(Qcode::Qi163, "FltCtrlSovs"s);
		ii_QcodeInfo(Qcode::Qi164, "IruOnDcTime"s);
		ii_QcodeInfo(Qcode::Qi165, "BrakeAccu"s);
		ii_QcodeInfo(Qcode::Qi166, "RtoAct"s);
		ii_QcodeInfo(Qcode::Qi167, "SpdBrkAuto"s);
		ii_QcodeInfo(Qcode::Qi168, "SpdBrkFltDet"s);
		ii_QcodeInfo(Qcode::Qi169, "PedStrg"s);
		ii_QcodeInfo(Qcode::Qi170, "LeFlapFail"s);
		ii_QcodeInfo(Qcode::Qi171, "LastFlapCom"s);
		ii_QcodeInfo(Qcode::Qi172, "FlapRelief"s);
		ii_QcodeInfo(Qcode::Qi173, "FmcMem1"s);
		ii_QcodeInfo(Qcode::Qi174, "BleedAirBits"s);
		ii_QcodeInfo(Qcode::Qi175, "AcpPwrBits"s);
		ii_QcodeInfo(Qcode::Qi176, "PitotIceBits"s);
		ii_QcodeInfo(Qcode::Qi177, "AdcRelays"s);
		ii_QcodeInfo(Qcode::Qi178, "AntiIceFluid"s);
		ci_QiValueControlVar(Qcode::AntiIceFluidCtrl, "AntiIceFluidCtrl"s, Qcode::Qi178, MinMax{ 45 });
		ii_QcodeInfo(Qcode::Qi179, "DoorComBits"s);
		ci_QiValueControlVar(Qcode::DoorComBitsCtrl, "DoorComBitsCtrl"s, Qcode::Qi179, MinMax{ 63 });    // leading
		ii_QcodeInfo(Qcode::Qi180, "DoorOpenBits"s);
		ci_QiControlVar(Qcode::DoorOpenBitsCtrl, "DoorOpenBitsCtrl"s, Qcode::Qi180);    // leading
		ii_QcodeInfo(Qcode::Qi181, "DoorManBits"s);
		ci_QiControlVar(Qcode::DoorManBitsCtrl, "DoorManBitsCtrl"s, Qcode::Qi181);    // leading
		ii_QcodeInfo(Qcode::Qi182, "NoseCargo"s);
		ii_QcodeInfo(Qcode::Qi183, "NextArptElev"s);
		ii_QcodeInfo(Qcode::Qi184, "AeroCkBxs"s);
		ii_QcodeInfo(Qcode::Qi185, "ManLdgAltOn"s);
		ii_QcodeInfo(Qcode::Qi186, "MfSelfInduced2"s);
		ii_QcodeInfo(Qcode::Qi187, "PvdBits"s);
		ii_QcodeInfo(Qcode::Qi188, "AltAlert"s);
		ii_QcodeInfo(Qcode::Qi189, "Egpws"s);
		ii_QcodeInfo(Qcode::Qi190, "MaweaBits"s);
		ii_QcodeInfo(Qcode::Qi191, "Towing"s);
		ii_QcodeInfo(Qcode::Qi192, "FuelType"s);
		ci_QiValueControlVar(Qcode::FuelTypeCtrl, "FuelTypeCtrl"s, Qcode::Qi192, MinMax{ 3 });
		ii_QcodeInfo(Qcode::Qi193, "FuelFlow"s);
		ii_QcodeInfo(Qcode::Qi194, "FuelBits"s);
		ii_QcodeInfo(Qcode::Qi195, "FuelFlapDelay"s);
		ii_QcodeInfo(Qcode::Qi196, "FuelEdit"s);
		ii_QcodeInfo(Qcode::Qi197, "Fatal"s);
		ii_QcodeInfo(Qcode::Qi198, "Elev"s);
		ii_QcodeInfo(Qcode::Qi199, "Pws"s);
		ii_QcodeInfo(Qcode::Qi200, "DmeId"s);
		ii_QcodeInfo(Qcode::Qi201, "TfcMode"s);
		ii_QcodeInfo(Qcode::Qi202, "TfcBits"s);
		ii_QcodeInfo(Qcode::Qi203, "Tcas"s);
		storeVarPtr(std::make_shared<SquawkIdent>(Qcode::Qi204, "SquawkIdent"));
		ii_QcodeInfo(Qcode::Qi205, "EltBat"s);
		ii_QcodeInfo(Qcode::Qi206, "EiuMem"s);
		ii_QcodeInfo(Qcode::Qi207, "VertDev"s);
		ii_QcodeInfo(Qcode::Qi208, "FltTime"s);
		ii_QcodeInfo(Qcode::Qi209, "PaxSigns"s);
		ii_QcodeInfo(Qcode::Qi210, "Offpath"s);
		ii_QcodeInfo(Qcode::Qi211, "ElevFeelPsi"s);
		ii_QcodeInfo(Qcode::Qi212, "ActLnavRadius"s);
		ii_QcodeInfo(Qcode::Qi213, "ProcTurn"s);
		ii_StbyCompass(Qcode::Qi214, "StbyCompass"s);
		ii_QcodeInfo(Qcode::Qi215, "EngStall"s);
		ii_QcodeInfo(Qcode::Qi216, "PnfChkl"s);
		ii_QcodeInfo(Qcode::Qi217, "PnfMode"s);
		ii_QcodeInfo(Qcode::Qi218, "PnfExtra"s);
		ii_QcodeInfo(Qcode::Qi219, "AcftHeight"s);
		ii_QcodeInfo(Qcode::Qi220, "Refueling"s);
		ci_QiAlternateActionSwitch(Qcode::RefuelingCtrl, "RefuelingCtrl"s, Qcode::Qi220);
		ii_QcodeInfo(Qcode::Qi221, "FansAtcMsgId"s);
		ii_QcodeInfo(Qcode::Qi222, "FansCont"s);
		ii_QcodeInfo(Qcode::Qi223, "EicasGrWt"s);
		ii_QcodeInfo(Qcode::Qi224, "AtcPhase"s);
		ii_QcodeInfo(Qcode::Qi225, "CrashInhib"s);
		ii_QcodeInfo(Qcode::Qi226, "StbTrmBits"s);
		ii_QcodeInfo(Qcode::Qi227, "CvrMonitor"s);
		ii_QcodeInfo(Qcode::Qi228, "PvdL"s);
		ii_QcodeInfo(Qcode::Qi229, "PvdR"s);
		ci_QiDeltaControlVar(Qcode::SelCduMenuCtrl, "SelCduMenuCtrl"s, Qcode::Qi230);
		ii_QcodeInfo(Qcode::Qi231, "PfdV1"s);
		ii_QcodeInfo(Qcode::Qi232, "PfdVr"s);
		ii_QcodeInfo(Qcode::Qi233, "PfdV2"s);
		ii_QcodeInfo(Qcode::Qi234, "SpdbrkSound"s);
		ii_QcodeInfo(Qcode::Qi235, "FircStop"s);
		ii_QcodeInfo(Qcode::Qi236, "TogaTime"s);
    ii_QcodeInfo(Qcode::Qi237, "StickShake"s);

		//
		// Qi238 is the index of the first cruise waypoint in the FMS. (The active waypoint has index 2.)
		// This Qi is required to sync PSX instances for a certain VNAV guidance function. (Hardy)
		// Not relevant for cockpit builders, skipped.
		//

		ii_QcodeInfo(Qcode::Qi239, "HydBrakeAccuInd"s);
		ii_QcodeInfo(Qcode::Qi240, "FocussedWxZone"s);
		ii_QcodeInfo(Qcode::Qi241, "AirconSources"s);
		ii_QcodeInfo(Qcode::Qi242, "UplinkBits"s);
		ci_QiDeltaControlVar(Qcode::WxSlowTransitCtrl, "WxSlowTransitCtrl"s, Qcode::Qi243);
		ii_QcodeInfo(Qcode::Qi245, "GearBits"s);
		ii_QcodeInfo(Qcode::Qi246, "PowerBits"s);
		ii_QcodeInfo(Qcode::Qi247, "ActDestEta"s);
		ii_QcodeInfo(Qcode::Qi248, "CfgPin2Prog"s);
		ii_QcodeInfo(Qcode::Qi249, "IdleDesTargIdx"s);
		ii_QcodeInfo(Qcode::Qi250, "AeroInhib"s);
		ii_QcodeInfo(Qcode::Qi251, "CfgRudTrmCtrSw"s);
		ii_QcodeInfo(Qcode::Qi252, "DemoTkof"s);
		ii_QcodeInfo(Qcode::Qi253, "RollBump"s);
		ii_QcodeInfo(Qcode::Qi254, "StartAttempts"s);
		ii_QcodeInfo(Qcode::Qi255, "AttemptsDone"s);
		ii_QcodeInfo(Qcode::Qi256, "MagWarn"s);
		ii_QcodeInfo(Qcode::Qi257, "OnGround"s);
		ii_QcodeInfo(Qcode::Qi258, "ScanIls"s);
		ii_QcodeInfo(Qcode::Qi259, "ApuBits"s);
		ii_QcodeInfo(Qcode::Qi260, "ApuDoor"s);
		ii_QcodeInfo(Qcode::Qi261, "MainTanksEqual"s);
		ii_QcodeInfo(Qcode::Qi262, "SigmetOn"s);
		ii_QcodeInfo(Qcode::Qi263, "ElecBits2"s);
		ii_QcodeInfo(Qcode::Qi264, "WxBits"s);
		ii_QcodeInfo(Qcode::Qi265, "FmcPosBiasMode"s);
		ii_QcodeInfo(Qcode::Qi266, "StepSizeNg"s);
		ii_QcodeInfo(Qcode::Qi267, "VertRnp"s);
    ii_QcodeInfo(Qcode::Qi268, "MinFuelTemp"s);
    ii_QcodeInfo(Qcode::Qi269, "CfgMlw"s);
    ii_QcodeInfo(Qcode::Qi270, "ExtBleedPsi"s);
    ii_QcodeInfo(Qcode::Qi271, "GroundSpeed"s);
		ii_QcodeInfo(Qcode::Qi273, "StandbyIas"s);
		ii_QcodeInfo(Qcode::Qi274, "StandbyAlt"s);

		// extra information variables defined by me, in 4700 range:
		ih_QhcodeInfo(Qcode::AutobrVirtual, "AutobrVirtual"s, Qcode::Qh134);
		ih_QhcodeInfo(Qcode::GearLockOvrdVirtual, "GearLockOvrdVirtual"s, Qcode::Qh169);
		ih_QhcodeInfo(Qcode::EngStartSw1Virtual, "EngStartSw1Virtual"s, Qcode::Qh267);
		ih_QhcodeInfo(Qcode::EngStartSw2Virtual, "EngStartSw2Virtual"s, Qcode::Qh268);
		ih_QhcodeInfo(Qcode::EngStartSw3Virtual, "EngStartSw3Virtual"s, Qcode::Qh269);
		ih_QhcodeInfo(Qcode::EngStartSw4Virtual, "EngStartSw4Virtual"s, Qcode::Qh270);
		ih_QhcodeInfo(Qcode::FireEng1Virtual, "FireEng1Virtual"s, Qcode::Qh275);
		ih_QhcodeInfo(Qcode::FireEng2Virtual, "FireEng2Virtual"s, Qcode::Qh276);
		ih_QhcodeInfo(Qcode::FireEng3Virtual, "FireEng3Virtual"s, Qcode::Qh277);
		ih_QhcodeInfo(Qcode::FireEng4Virtual, "FireEng4Virtual"s, Qcode::Qh278);
		ih_QhcodeInfo(Qcode::FireApuVirtual, "FireApuVirtual"s, Qcode::Qh279);
		ih_QhcodeInfo(Qcode::CallPanelVirtual, "CallPanelVirtual"s, Qcode::Qh413);
		ih_QhcodeInfo(Qcode::LtIndTestVirtual, "LtIndTestVirtual"s, Qcode::Qh183);
    ih_QhcodeInfo(Qcode::ParkBrkLevVirtual, "ParkBrkLevVirtual"s, Qcode::Qh397);

		// Qs
    is_ExpQcodeInfo(Qcode::L_ActVHFL, "L_ActVHFL"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_StbVHFL, "L_StbVHFL"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_ActVHFC, "L_ActVHFC"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_StbVHFC, "L_StbVHFC"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_ActVHLR, "L_ActVHLR"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_StbVHFR, "L_StbVHFR"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_ActHFL, "L_ActHFL"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_StbHFL, "L_StbHFL"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_ActHFR, "L_ActHFR"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_StbHFR, "L_StbHFR"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_SenHFL, "L_SenHFL"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_SenHFR, "L_SenHFR"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_AMActHFL, "L_AMActHFL"s, Qcode::Qs106);
    is_ExpQcodeInfo(Qcode::L_AMActHFR, "L_AMActHFR"s, Qcode::Qs106);

    is_ExpQcodeInfo(Qcode::C_ActVHFL, "C_ActVHFL"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_StbVHFL, "C_StbVHFL"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_ActVHFC, "C_ActVHFC"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_StbVHFC, "C_StbVHFC"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_ActVHLR, "C_ActVHLR"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_StbVHFR, "C_StbVHFR"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_ActHFL, "C_ActHFL"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_StbHFL, "C_StbHFL"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_ActHFR, "C_ActHFR"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_StbHFR, "C_StbHFR"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_SenHFL, "C_SenHFL"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_SenHFR, "C_SenHFR"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_AMActHFL, "C_AMActHFL"s, Qcode::Qs107);
    is_ExpQcodeInfo(Qcode::C_AMActHFR, "C_AMActHFR"s, Qcode::Qs107);

    is_ExpQcodeInfo(Qcode::R_ActVHFL, "R_ActVHFL"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_StbVHFL, "R_StbVHFL"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_ActVHFC, "R_ActVHFC"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_StbVHFC, "R_StbVHFC"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_ActVHLR, "R_ActVHLR"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_StbVHFR, "R_StbVHFR"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_ActHFL, "R_ActHFL"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_StbHFL, "R_StbHFL"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_ActHFR, "R_ActHFR"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_StbHFR, "R_StbHFR"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_SenHFL, "R_SenHFL"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_SenHFR, "R_SenHFR"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_AMActHFL, "R_AMActHFL"s, Qcode::Qs108);
    is_ExpQcodeInfo(Qcode::R_AMActHFR, "R_AMActHFR"s, Qcode::Qs108);


		is_ExpQcodeInfo(Qcode::FreqsAntennaVHF_L, "FreqsAntennaVHF_L"s, Qcode::Qs112);
		is_ExpQcodeInfo(Qcode::FreqsAntennaVHF_C, "FreqsAntennaVHF_C"s, Qcode::Qs112);
		is_ExpQcodeInfo(Qcode::FreqsAntennaVHF_R, "FreqsAntennaVHF_R"s, Qcode::Qs112);
		is_ExpQcodeInfo(Qcode::FreqsAntennaHF_L, "FreqsAntennaHF_L"s, Qcode::Qs112);
    is_ExpQcodeInfo(Qcode::FreqsAntennaHF_R, "FreqsAntennaHF_R"s, Qcode::Qs112);

    is_SquawkDispl(Qcode::SquawkDispl, "SquawkDispl"s, Qcode::Qs118);
		is_XpdrFail(Qcode::XpdrFail, "XpdrFail"s, Qcode::Qs118);

		is_ExpQcodeInfo(Qcode::PitchRad, "PitchRad"s, Qcode::Qs121);
		is_ExpQcodeInfo(Qcode::BankRad, "BankRad"s, Qcode::Qs121);
		is_ExpQcodeInfo(Qcode::TrueHeading, "TrueHeading"s, Qcode::Qs121);
		is_ExpQcodeInfo(Qcode::Altitude, "Altitude"s, Qcode::Qs121);
		is_ExpQcodeInfo(Qcode::TAS, "TAS"s, Qcode::Qs121);
		is_ExpQcodeInfo(Qcode::Latitude, "Latitude"s, Qcode::Qs121);
		is_ExpQcodeInfo(Qcode::Longitude, "Longitude"s, Qcode::Qs121);

		is_ClockVar(Qcode::TimeClockL, "TimeClockL"s, Qcode::gT77, Qcode::Qs124);
		is_ClockVar2(Qcode::TimeClockL2, "TimeClockL"s, Qcode::gT77, Qcode::Qs124);
		is_ClockVar3(Qcode::TimeClockL3, "TimeClockL"s, Qcode::gT77, Qcode::Qs124);
		is_ClockVar(Qcode::TimeClockR, "TimeClockR"s, Qcode::gT102, Qcode::Qs125);
		is_ClockVar2(Qcode::TimeClockR2, "TimeClockR"s, Qcode::gT102, Qcode::Qs125);
		is_ClockVar3(Qcode::TimeClockR3, "TimeClockR"s, Qcode::gT102, Qcode::Qs125);
		is_TimerVar(Qcode::ElapMinsL, "ElapMinsL"s, Qcode::gT77, Qcode::Qs126);
		is_TimerVar(Qcode::ElapMinsR, "ElapMinsR"s, Qcode::gT102, Qcode::Qs127);
		is_ExpQcodeInfo(Qcode::ChroSecsL, "ChroSecsL"s, Qcode::Qs128);
		is_ExpQcodeInfo(Qcode::ChroSecsR, "ChroSecsR"s, Qcode::Qs129);

		cs_MalFunctionControlVar(Qcode::MfAilLockout, "MfAilLockout"s, Qcode::Qs137);
		cs_MalFunctionControlVar(Qcode::MfFlapCtrl, "MfFlapCtrl"s, Qcode::Qs138);
		cs_MalFunctionControlVar(Qcode::MfFlapLe, "MfFlapLe"s, Qcode::Qs139);
		cs_MalFunctionControlVar(Qcode::MfFlapTeOutL, "MfFlapTeOutL"s, Qcode::Qs140);
		cs_MalFunctionControlVar(Qcode::MfFlapTeInL, "MfFlapTeInL"s, Qcode::Qs141);
		cs_MalFunctionControlVar(Qcode::MfFlapTeInR, "MfFlapTeInR"s, Qcode::Qs142);
		cs_MalFunctionControlVar(Qcode::MfFlapTeOutR, "MfFlapTeOutR"s, Qcode::Qs143);
		cs_MalFunctionControlVar(Qcode::MfGearNose, "MfGearNose"s, Qcode::Qs144);
		cs_MalFunctionControlVar(Qcode::MfGearWingL, "MfGearWingL"s, Qcode::Qs145);
		cs_MalFunctionControlVar(Qcode::MfGearWingR, "MfGearWingR"s, Qcode::Qs146);
		cs_MalFunctionControlVar(Qcode::MfGearBodyL, "MfGearBodyL"s, Qcode::Qs147);
		cs_MalFunctionControlVar(Qcode::MfGearBodyR, "MfGearBodyR"s, Qcode::Qs148);
		cs_MalFunctionControlVar(Qcode::MfGearTire, "MfGearTire"s, Qcode::Qs149);
		cs_MalFunctionControlVar(Qcode::MfIceDetect, "MfIceDetect"s, Qcode::Qs150);
		cs_MalFunctionControlVar(Qcode::MfPitotLoL, "MfPitotLoL"s, Qcode::Qs151);
		cs_MalFunctionControlVar(Qcode::MfPitotUpL, "MfPitotUpL"s, Qcode::Qs152);
		cs_MalFunctionControlVar(Qcode::MfPitotLoR, "MfPitotLoR"s, Qcode::Qs153);
		cs_MalFunctionControlVar(Qcode::MfPitotUpR, "MfPitotUpR"s, Qcode::Qs154);
		cs_MalFunctionControlVar(Qcode::MfStabTrimMotion, "MfStabTrimMotion"s, Qcode::Qs155);
		cs_MalFunctionControlVar(Qcode::MfStabTrimRunaway, "MfStabTrimRunaway"s, Qcode::Qs156);
		cs_MalFunctionControlVar(Qcode::MfDuctLeakL, "MfDuctLeakL"s, Qcode::Qs157);
		cs_MalFunctionControlVar(Qcode::MfDuctLeakC, "MfDuctLeakC"s, Qcode::Qs158);
		cs_MalFunctionControlVar(Qcode::MfDuctLeakR, "MfDuctLeakR"s, Qcode::Qs159);
		cs_MalFunctionControlVar(Qcode::MfBleedOvht1, "MfBleedOvht1"s, Qcode::Qs160);
		cs_MalFunctionControlVar(Qcode::MfBleedOvht2, "MfBleedOvht2"s, Qcode::Qs161);
		cs_MalFunctionControlVar(Qcode::MfBleedOvht3, "MfBleedOvht3"s, Qcode::Qs162);
		cs_MalFunctionControlVar(Qcode::MfBleedOvht4, "MfBleedOvht4"s, Qcode::Qs163);
		cs_MalFunctionControlVar(Qcode::MfCabDecomp, "MfCabDecomp"s, Qcode::Qs164);
		cs_MalFunctionControlVar(Qcode::MfCabExplo, "MfCabExplo"s, Qcode::Qs165);
		cs_MalFunctionControlVar(Qcode::MfEquipOvht, "MfEquipOvht"s, Qcode::Qs166);
		cs_MalFunctionControlVar(Qcode::MfNai1, "MfNai1"s, Qcode::Qs167);
		cs_MalFunctionControlVar(Qcode::MfNai2, "MfNai2"s, Qcode::Qs168);
		cs_MalFunctionControlVar(Qcode::MfNai3, "MfNai3"s, Qcode::Qs169);
		cs_MalFunctionControlVar(Qcode::MfNai4, "MfNai4"s, Qcode::Qs170);
		cs_MalFunctionControlVar(Qcode::MfPack1, "MfPack1"s, Qcode::Qs171);
		cs_MalFunctionControlVar(Qcode::MfPack2, "MfPack2"s, Qcode::Qs172);
		cs_MalFunctionControlVar(Qcode::MfPack3, "MfPack3"s, Qcode::Qs173);
		cs_MalFunctionControlVar(Qcode::MfAltAlert, "MfAltAlert"s, Qcode::Qs174);
		cs_MalFunctionControlVar(Qcode::MfAltCall, "MfAltCall"s, Qcode::Qs175);
		cs_MalFunctionControlVar(Qcode::MfConfigAlert, "MfConfigAlert"s, Qcode::Qs176);
		cs_MalFunctionControlVar(Qcode::MfEicasDsp, "MfEicasDsp"s, Qcode::Qs177);
		cs_MalFunctionControlVar(Qcode::MfIduCtrUpr, "MfIduCtrUpr"s, Qcode::Qs178);
		cs_MalFunctionControlVar(Qcode::MfIduCtrLwr, "MfIduCtrLwr"s, Qcode::Qs179);
		cs_MalFunctionControlVar(Qcode::MfIndN1Rev1, "MfIndN1Rev1"s, Qcode::Qs180);
		cs_MalFunctionControlVar(Qcode::MfIndN1Rev2, "MfIndN1Rev2"s, Qcode::Qs181);
		cs_MalFunctionControlVar(Qcode::MfIndN1Rev3, "MfIndN1Rev3"s, Qcode::Qs182);
		cs_MalFunctionControlVar(Qcode::MfIndN1Rev4, "MfIndN1Rev4"s, Qcode::Qs183);
		cs_MalFunctionControlVar(Qcode::MfIndEgtFf1, "MfIndEgtFf1"s, Qcode::Qs184);
		cs_MalFunctionControlVar(Qcode::MfIndEgtFf2, "MfIndEgtFf2"s, Qcode::Qs185);
		cs_MalFunctionControlVar(Qcode::MfIndEgtFf3, "MfIndEgtFf3"s, Qcode::Qs186);
		cs_MalFunctionControlVar(Qcode::MfIndEgtFf4, "MfIndEgtFf4"s, Qcode::Qs187);
		cs_MalFunctionControlVar(Qcode::MfMawea, "MfMawea"s, Qcode::Qs188);
		cs_MalFunctionControlVar(Qcode::MfTcasNd, "MfTcasNd"s, Qcode::Qs189);
		cs_MalFunctionControlVar(Qcode::MfTcasRa, "MfTcasRa"s, Qcode::Qs190);
		cs_MalFunctionControlVar(Qcode::MfWindshear, "MfWindshear"s, Qcode::Qs191);
		cs_MalFunctionControlVar(Qcode::MfIdgOil1, "MfIdgOil1"s, Qcode::Qs192);
		cs_MalFunctionControlVar(Qcode::MfIdgOil2, "MfIdgOil2"s, Qcode::Qs193);
		cs_MalFunctionControlVar(Qcode::MfIdgOil3, "MfIdgOil3"s, Qcode::Qs194);
		cs_MalFunctionControlVar(Qcode::MfIdgOil4, "MfIdgOil4"s, Qcode::Qs195);
		cs_MalFunctionControlVar(Qcode::MfAcBus1, "MfAcBus1"s, Qcode::Qs196);
		cs_MalFunctionControlVar(Qcode::MfAcBus2, "MfAcBus2"s, Qcode::Qs197);
		cs_MalFunctionControlVar(Qcode::MfAcBus3, "MfAcBus3"s, Qcode::Qs198);
		cs_MalFunctionControlVar(Qcode::MfAcBus4, "MfAcBus4"s, Qcode::Qs199);
		cs_MalFunctionControlVar(Qcode::MfBatBusMn, "MfBatBusMn"s, Qcode::Qs200);
		cs_MalFunctionControlVar(Qcode::MfBatBusAp, "MfBatBusAp"s, Qcode::Qs201);
		cs_MalFunctionControlVar(Qcode::MfMnBatOvht, "MfMnBatOvht"s, Qcode::Qs202);
		cs_MalFunctionControlVar(Qcode::MfApBatOvht, "MfApBatOvht"s, Qcode::Qs203);
		cs_MalFunctionControlVar(Qcode::MfDcBus1, "MfDcBus1"s, Qcode::Qs204);
		cs_MalFunctionControlVar(Qcode::MfDcBus2, "MfDcBus2"s, Qcode::Qs205);
		cs_MalFunctionControlVar(Qcode::MfDcBus3, "MfDcBus3"s, Qcode::Qs206);
		cs_MalFunctionControlVar(Qcode::MfDcBus4, "MfDcBus4"s, Qcode::Qs207);
		cs_MalFunctionControlVar(Qcode::MfStrike, "MfStrike"s, Qcode::Qs208);
		cs_MalFunctionControlVar(Qcode::MfStall1, "MfStall1"s, Qcode::Qs209);
		cs_MalFunctionControlVar(Qcode::MfStall2, "MfStall2"s, Qcode::Qs210);
		cs_MalFunctionControlVar(Qcode::MfStall3, "MfStall3"s, Qcode::Qs211);
		cs_MalFunctionControlVar(Qcode::MfStall4, "MfStall4"s, Qcode::Qs212);
		cs_MalFunctionControlVar(Qcode::MfFlameOut1, "MfFlameOut1"s, Qcode::Qs213);
		cs_MalFunctionControlVar(Qcode::MfFlameOut2, "MfFlameOut2"s, Qcode::Qs214);
		cs_MalFunctionControlVar(Qcode::MfFlameOut3, "MfFlameOut3"s, Qcode::Qs215);
		cs_MalFunctionControlVar(Qcode::MfFlameOut4, "MfFlameOut4"s, Qcode::Qs216);
		cs_MalFunctionControlVar(Qcode::MfFod1, "MfFod1"s, Qcode::Qs217);
		cs_MalFunctionControlVar(Qcode::MfFod2, "MfFod2"s, Qcode::Qs218);
		cs_MalFunctionControlVar(Qcode::MfFod3, "MfFod3"s, Qcode::Qs219);
		cs_MalFunctionControlVar(Qcode::MfFod4, "MfFod4"s, Qcode::Qs220);
		cs_MalFunctionControlVar(Qcode::MfIgnit1, "MfIgnit1"s, Qcode::Qs221);
		cs_MalFunctionControlVar(Qcode::MfIgnit2, "MfIgnit2"s, Qcode::Qs222);
		cs_MalFunctionControlVar(Qcode::MfIgnit3, "MfIgnit3"s, Qcode::Qs223);
		cs_MalFunctionControlVar(Qcode::MfIgnit4, "MfIgnit4"s, Qcode::Qs224);
		cs_MalFunctionControlVar(Qcode::MfLowOilPrSw1, "MfLowOilPrSw1"s, Qcode::Qs225);
		cs_MalFunctionControlVar(Qcode::MfLowOilPrSw2, "MfLowOilPrSw2"s, Qcode::Qs226);
		cs_MalFunctionControlVar(Qcode::MfLowOilPrSw3, "MfLowOilPrSw3"s, Qcode::Qs227);
		cs_MalFunctionControlVar(Qcode::MfLowOilPrSw4, "MfLowOilPrSw4"s, Qcode::Qs228);
		cs_MalFunctionControlVar(Qcode::MfOilPress1, "MfOilPress1"s, Qcode::Qs229);
		cs_MalFunctionControlVar(Qcode::MfOilPress2, "MfOilPress2"s, Qcode::Qs230);
		cs_MalFunctionControlVar(Qcode::MfOilPress3, "MfOilPress3"s, Qcode::Qs231);
		cs_MalFunctionControlVar(Qcode::MfOilPress4, "MfOilPress4"s, Qcode::Qs232);
		cs_MalFunctionControlVar(Qcode::MfOilTemp1, "MfOilTemp1"s, Qcode::Qs233);
		cs_MalFunctionControlVar(Qcode::MfOilTemp2, "MfOilTemp2"s, Qcode::Qs234);
		cs_MalFunctionControlVar(Qcode::MfOilTemp3, "MfOilTemp3"s, Qcode::Qs235);
		cs_MalFunctionControlVar(Qcode::MfOilTemp4, "MfOilTemp4"s, Qcode::Qs236);
		cs_MalFunctionControlVar(Qcode::MfSquib1, "MfSquib1"s, Qcode::Qs237);
		cs_MalFunctionControlVar(Qcode::MfSquib2, "MfSquib2"s, Qcode::Qs238);
		cs_MalFunctionControlVar(Qcode::MfSquib3, "MfSquib3"s, Qcode::Qs239);
		cs_MalFunctionControlVar(Qcode::MfSquib4, "MfSquib4"s, Qcode::Qs240);
		cs_MalFunctionControlVar(Qcode::MfAfolts1, "MfAfolts1"s, Qcode::Qs241);
		cs_MalFunctionControlVar(Qcode::MfAfolts2, "MfAfolts2"s, Qcode::Qs242);
		cs_MalFunctionControlVar(Qcode::MfAfolts3, "MfAfolts3"s, Qcode::Qs243);
		cs_MalFunctionControlVar(Qcode::MfAfolts4, "MfAfolts4"s, Qcode::Qs244);
		cs_MalFunctionControlVar(Qcode::MfDetLp1, "MfDetLp1"s, Qcode::Qs245);
		cs_MalFunctionControlVar(Qcode::MfDetLp2, "MfDetLp2"s, Qcode::Qs246);
		cs_MalFunctionControlVar(Qcode::MfDetLp3, "MfDetLp3"s, Qcode::Qs247);
		cs_MalFunctionControlVar(Qcode::MfDetLp4, "MfDetLp4"s, Qcode::Qs248);
		cs_MalFunctionControlVar(Qcode::MfFireApu, "MfFireApu"s, Qcode::Qs249);
		cs_MalFunctionControlVar(Qcode::MfFireMdDk, "MfFireMdDk"s, Qcode::Qs250);
		cs_MalFunctionControlVar(Qcode::MfFireLoAft, "MfFireLoAft"s, Qcode::Qs251);
		cs_MalFunctionControlVar(Qcode::MfFireLoFwd, "MfFireLoFwd"s, Qcode::Qs252);
		cs_MalFunctionControlVar(Qcode::MfFireWheel, "MfFireWheel"s, Qcode::Qs253);
		cs_MalFunctionControlVar(Qcode::MfFireEn1bt, "MfFireEn1bt"s, Qcode::Qs254);
		cs_MalFunctionControlVar(Qcode::MfFireEn2bt, "MfFireEn2bt"s, Qcode::Qs255);
		cs_MalFunctionControlVar(Qcode::MfFireEn3bt, "MfFireEn3bt"s, Qcode::Qs256);
		cs_MalFunctionControlVar(Qcode::MfFireEn4bt, "MfFireEn4bt"s, Qcode::Qs257);
		cs_MalFunctionControlVar(Qcode::MfFireEn1nb, "MfFireEn1nb"s, Qcode::Qs258);
		cs_MalFunctionControlVar(Qcode::MfFireEn2nb, "MfFireEn2nb"s, Qcode::Qs259);
		cs_MalFunctionControlVar(Qcode::MfFireEn3nb, "MfFireEn3nb"s, Qcode::Qs260);
		cs_MalFunctionControlVar(Qcode::MfFireEn4nb, "MfFireEn4nb"s, Qcode::Qs261);
		cs_MalFunctionControlVar(Qcode::MfOvhtNac1, "MfOvhtNac1"s, Qcode::Qs262);
		cs_MalFunctionControlVar(Qcode::MfOvhtNac2, "MfOvhtNac2"s, Qcode::Qs263);
		cs_MalFunctionControlVar(Qcode::MfOvhtNac3, "MfOvhtNac3"s, Qcode::Qs264);
		cs_MalFunctionControlVar(Qcode::MfOvhtNac4, "MfOvhtNac4"s, Qcode::Qs265);
		cs_MalFunctionControlVar(Qcode::MfFuelLkE1, "MfFuelLkE1"s, Qcode::Qs266);
		cs_MalFunctionControlVar(Qcode::MfFuelLkE2, "MfFuelLkE2"s, Qcode::Qs267);
		cs_MalFunctionControlVar(Qcode::MfFuelLkE3, "MfFuelLkE3"s, Qcode::Qs268);
		cs_MalFunctionControlVar(Qcode::MfFuelLkE4, "MfFuelLkE4"s, Qcode::Qs269);
		cs_MalFunctionControlVar(Qcode::MfFuelTkCt, "MfFuelTkCt"s, Qcode::Qs270);
		cs_MalFunctionControlVar(Qcode::MfFuelTk1, "MfFuelTk1"s, Qcode::Qs271);
		cs_MalFunctionControlVar(Qcode::MfFuelTk2, "MfFuelTk2"s, Qcode::Qs272);
		cs_MalFunctionControlVar(Qcode::MfFuelTk3, "MfFuelTk3"s, Qcode::Qs273);
		cs_MalFunctionControlVar(Qcode::MfFuelTk4, "MfFuelTk4"s, Qcode::Qs274);
		cs_MalFunctionControlVar(Qcode::MfFuelPumpC, "MfFuelPumpC"s, Qcode::Qs275);
		cs_MalFunctionControlVar(Qcode::MfFuelPump1, "MfFuelPump1"s, Qcode::Qs276);
		cs_MalFunctionControlVar(Qcode::MfFuelPump2, "MfFuelPump2"s, Qcode::Qs277);
		cs_MalFunctionControlVar(Qcode::MfFuelPump3, "MfFuelPump3"s, Qcode::Qs278);
		cs_MalFunctionControlVar(Qcode::MfFuelPump4, "MfFuelPump4"s, Qcode::Qs279);
		cs_MalFunctionControlVar(Qcode::MfFuelPumpJ2, "MfFuelPumpJ2"s, Qcode::Qs280);
		cs_MalFunctionControlVar(Qcode::MfFuelPumpJ3, "MfFuelPumpJ3"s, Qcode::Qs281);
		cs_MalFunctionControlVar(Qcode::MfFuelPumpS, "MfFuelPumpS"s, Qcode::Qs282);
		cs_MalFunctionControlVar(Qcode::MfFuelTemp, "MfFuelTemp"s, Qcode::Qs283);
		cs_MalFunctionControlVar(Qcode::MfHydim1, "MfHydim1"s, Qcode::Qs284);
		cs_MalFunctionControlVar(Qcode::MfHydim2, "MfHydim2"s, Qcode::Qs285);
		cs_MalFunctionControlVar(Qcode::MfHydim3, "MfHydim3"s, Qcode::Qs286);
		cs_MalFunctionControlVar(Qcode::MfHydim4, "MfHydim4"s, Qcode::Qs287);
		cs_MalFunctionControlVar(Qcode::MfHydLeak1, "MfHydLeak1"s, Qcode::Qs288);
		cs_MalFunctionControlVar(Qcode::MfHydLeak2, "MfHydLeak2"s, Qcode::Qs289);
		cs_MalFunctionControlVar(Qcode::MfHydLeak3, "MfHydLeak3"s, Qcode::Qs290);
		cs_MalFunctionControlVar(Qcode::MfHydLeak4, "MfHydLeak4"s, Qcode::Qs291);
		cs_MalFunctionControlVar(Qcode::MfHydOvht1, "MfHydOvht1"s, Qcode::Qs292);
		cs_MalFunctionControlVar(Qcode::MfHydOvht2, "MfHydOvht2"s, Qcode::Qs293);
		cs_MalFunctionControlVar(Qcode::MfHydOvht3, "MfHydOvht3"s, Qcode::Qs294);
		cs_MalFunctionControlVar(Qcode::MfHydOvht4, "MfHydOvht4"s, Qcode::Qs295);
		cs_MalFunctionControlVar(Qcode::MfHydPump1, "MfHydPump1"s, Qcode::Qs296);
		cs_MalFunctionControlVar(Qcode::MfHydPump2, "MfHydPump2"s, Qcode::Qs297);
		cs_MalFunctionControlVar(Qcode::MfHydPump3, "MfHydPump3"s, Qcode::Qs298);
		cs_MalFunctionControlVar(Qcode::MfHydPump4, "MfHydPump4"s, Qcode::Qs299);
		cs_MalFunctionControlVar(Qcode::MfRsvr12, "MfRsvr12"s, Qcode::Qs300);
		cs_MalFunctionControlVar(Qcode::MfRsvr34, "MfRsvr34"s, Qcode::Qs301);
		cs_MalFunctionControlVar(Qcode::MfFccL, "MfFccL"s, Qcode::Qs302);
		cs_MalFunctionControlVar(Qcode::MfFccC, "MfFccC"s, Qcode::Qs303);
		cs_MalFunctionControlVar(Qcode::MfFccR, "MfFccR"s, Qcode::Qs304);
		cs_MalFunctionControlVar(Qcode::MfFmcL, "MfFmcL"s, Qcode::Qs305);
		cs_MalFunctionControlVar(Qcode::MfFmcR, "MfFmcR"s, Qcode::Qs306);
		cs_MalFunctionControlVar(Qcode::MfGpsSignal, "MfGpsSignal"s, Qcode::Qs307);
		cs_MalFunctionControlVar(Qcode::MfGndStaLoc, "MfGndStaLoc"s, Qcode::Qs308);
		cs_MalFunctionControlVar(Qcode::MfGndStaGs, "MfGndStaGs"s, Qcode::Qs309);
		cs_MalFunctionControlVar(Qcode::MfIduInbdCp, "MfIduInbdCp"s, Qcode::Qs310);
		cs_MalFunctionControlVar(Qcode::MfIduOutbdCp, "MfIduOutbdCp"s, Qcode::Qs311);
		cs_MalFunctionControlVar(Qcode::MfIduInbdFo, "MfIduInbdFo"s, Qcode::Qs312);
		cs_MalFunctionControlVar(Qcode::MfIduOutbdFo, "MfIduOutbdFo"s, Qcode::Qs313);
		cs_MalFunctionControlVar(Qcode::MfIrsAtt, "MfIrsAtt"s, Qcode::Qs314);
		cs_MalFunctionControlVar(Qcode::MfIrsDrift, "MfIrsDrift"s, Qcode::Qs315);
		cs_MalFunctionControlVar(Qcode::MfIrsFail, "MfIrsFail"s, Qcode::Qs316);
		cs_MalFunctionControlVar(Qcode::MfRcpL, "MfRcpL"s, Qcode::Qs317);
		cs_MalFunctionControlVar(Qcode::MfRcpC, "MfRcpC"s, Qcode::Qs318);
		cs_MalFunctionControlVar(Qcode::MfRcpR, "MfRcpR"s, Qcode::Qs319);
		cs_MalFunctionControlVar(Qcode::MfStby, "MfStby"s, Qcode::Qs320);

    is_ExpQcodeInfo(Qcode::Dummy1, "Dummy1"s, Qcode::Qs321);
    is_ExpQcodeInfo(Qcode::pitchErrAdd, "pitchErrAdd"s, Qcode::Qs321);
    is_ExpQcodeInfo(Qcode::bankErrAdd, "bankErrAdd"s, Qcode::Qs321);
    is_ExpQcodeInfo(Qcode::gyroRpm, "gyroRpm"s, Qcode::Qs321);

    is_ExpQcodeInfo(Qcode::nonDimC358, "nonDimC358"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::nonDimC614, "nonDimC614"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::nonDimC122, "nonDimC122"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::nonDimC17, "nonDimC17"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::directIrsBatLt, "directIrsBatLt"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::directTowBatLt, "directTowBatLt"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::dimRelayK2a, "dimRelayK2a"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::dimRelayK2b, "dimRelayK2b"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::dimRelayK4a, "dimRelayK4a"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::dimRelayK4b, "dimRelayK4b"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::dimRelayK6a, "dimRelayK6a"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::dimRelayK6b, "dimRelayK6b"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::dimRelayK8a, "dimRelayK8a"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::dimRelayK8b, "dimRelayK8b"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::extAvailTest1, "extAvailTest1"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::extAvailTest2, "extAvailTest2"s, Qcode::Qs323);
		is_ExpQcodeInfo(Qcode::domeLight, "domeLight"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::mapLightL, "mapLightL"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::mapLightR, "mapLightR"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::aisleFlood, "aisleFlood"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::mainFloodL, "mainFloodL"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::mainFloodR, "mainFloodR"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gT38, "gT38"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gT40, "gT40"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gT44, "gT44"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gR8098ab, "gR8098ab"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gR8098c, "gR8098c"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gR8098d, "gR8098d"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gR7737ab, "gR7737ab"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gR7737d, "gR7737d"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gDivAislePnlT144, "gDivAislePnlT144"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gDivAislePnlT951, "gDivAislePnlT951"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gT137, "gT137"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gT961, "gT961"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gR7736, "gR7736"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gT77, "gT77"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gHydInd, "gHydInd"s, Qcode::Qs324);
		is_ExpQcodeInfo(Qcode::gT102, "gT102"s, Qcode::Qs324);

		is_ExpQcodeInfo(Qcode::WxB_hiCloudCov, "WxB_hiCloudCov"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_hiCloudTop, "WxB_hiCloudTop"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_hiCloudBase, "WxB_hiCloudBase"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_loCloudCov, "WxB_loCloudCov"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_loCloudTop, "WxB_loCloudTop"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_loCloudBase, "WxB_loCloudBase"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_turbIntensity, "WxB_turbIntensity"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_turbTop, "WxB_turbTop"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_turbBase, "WxB_turbBase"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_cbCloudCov, "WxB_cbCloudCov"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_cbCloudTop, "WxB_cbCloudTop"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_cbCloudBase, "WxB_cbCloudBase"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_microburstMode, "WxB_microburstMode"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_microburstRandom, "WxB_microburstRandom"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_microburstOutflow, "WxB_microburstOutflow"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_inversionOn, "WxB_inversionOn"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_inversionTop, "WxB_inversionTop"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_inversionTmp, "WxB_inversionTmp"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_arptWindVarEtc, "WxB_arptWindVarEtc"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_arptWindGust, "WxB_arptWindGust"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_visibMtrs, "WxB_visibMtrs"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_precipLevel, "WxB_precipLevel"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_surfaceTmp, "WxB_surfaceTmp"s, Qcode::Qs328);
		is_ExpQcodeInfo(Qcode::WxB_QNH, "WxB_QNH"s, Qcode::Qs328);

		is_ExpQcodeInfo(Qcode::Wx1_hiCloudCov, "Wx1_hiCloudCov"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_hiCloudTop, "Wx1_hiCloudTop"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_hiCloudBase, "Wx1_hiCloudBase"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_loCloudCov, "Wx1_loCloudCov"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_loCloudTop, "Wx1_loCloudTop"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_loCloudBase, "Wx1_loCloudBase"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_turbIntensity, "Wx1_turbIntensity"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_turbTop, "Wx1_turbTop"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_turbBase, "Wx1_turbBase"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_cbCloudCov, "Wx1_cbCloudCov"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_cbCloudTop, "Wx1_cbCloudTop"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_cbCloudBase, "Wx1_cbCloudBase"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_microburstMode, "Wx1_microburstMode"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_microburstRandom, "Wx1_microburstRandom"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_microburstOutflow, "Wx1_microburstOutflow"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_inversionOn, "Wx1_inversionOn"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_inversionTop, "Wx1_inversionTop"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_inversionTmp, "Wx1_inversionTmp"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_arptWindVarEtc, "Wx1_arptWindVarEtc"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_arptWindGust, "Wx1_arptWindGust"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_visibMtrs, "Wx1_visibMtrs"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_precipLevel, "Wx1_precipLevel"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_surfaceTmp, "Wx1_surfaceTmp"s, Qcode::Qs329);
		is_ExpQcodeInfo(Qcode::Wx1_QNH, "Wx1_QNH"s, Qcode::Qs329);

		is_ExpQcodeInfo(Qcode::Wx2_hiCloudCov, "Wx2_hiCloudCov"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_hiCloudTop, "Wx2_hiCloudTop"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_hiCloudBase, "Wx2_hiCloudBase"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_loCloudCov, "Wx2_loCloudCov"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_loCloudTop, "Wx2_loCloudTop"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_loCloudBase, "Wx2_loCloudBase"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_turbIntensity, "Wx2_turbIntensity"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_turbTop, "Wx2_turbTop"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_turbBase, "Wx2_turbBase"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_cbCloudCov, "Wx2_cbCloudCov"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_cbCloudTop, "Wx2_cbCloudTop"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_cbCloudBase, "Wx2_cbCloudBase"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_microburstMode, "Wx2_microburstMode"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_microburstRandom, "Wx2_microburstRandom"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_microburstOutflow, "Wx2_microburstOutflow"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_inversionOn, "Wx2_inversionOn"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_inversionTop, "Wx2_inversionTop"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_inversionTmp, "Wx2_inversionTmp"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_arptWindVarEtc, "Wx2_arptWindVarEtc"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_arptWindGust, "Wx2_arptWindGust"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_visibMtrs, "Wx2_visibMtrs"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_precipLevel, "Wx2_precipLevel"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_surfaceTmp, "Wx2_surfaceTmp"s, Qcode::Qs330);
		is_ExpQcodeInfo(Qcode::Wx2_QNH, "Wx2_QNH"s, Qcode::Qs330);

		is_ExpQcodeInfo(Qcode::Wx3_hiCloudCov, "Wx3_hiCloudCov"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_hiCloudTop, "Wx3_hiCloudTop"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_hiCloudBase, "Wx3_hiCloudBase"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_loCloudCov, "Wx3_loCloudCov"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_loCloudTop, "Wx3_loCloudTop"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_loCloudBase, "Wx3_loCloudBase"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_turbIntensity, "Wx3_turbIntensity"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_turbTop, "Wx3_turbTop"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_turbBase, "Wx3_turbBase"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_cbCloudCov, "Wx3_cbCloudCov"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_cbCloudTop, "Wx3_cbCloudTop"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_cbCloudBase, "Wx3_cbCloudBase"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_microburstMode, "Wx3_microburstMode"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_microburstRandom, "Wx3_microburstRandom"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_microburstOutflow, "Wx3_microburstOutflow"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_inversionOn, "Wx3_inversionOn"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_inversionTop, "Wx3_inversionTop"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_inversionTmp, "Wx3_inversionTmp"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_arptWindVarEtc, "Wx3_arptWindVarEtc"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_arptWindGust, "Wx3_arptWindGust"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_visibMtrs, "Wx3_visibMtrs"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_precipLevel, "Wx3_precipLevel"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_surfaceTmp, "Wx3_surfaceTmp"s, Qcode::Qs331);
		is_ExpQcodeInfo(Qcode::Wx3_QNH, "Wx3_QNH"s, Qcode::Qs331);

		is_ExpQcodeInfo(Qcode::Wx4_hiCloudCov, "Wx4_hiCloudCov"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_hiCloudTop, "Wx4_hiCloudTop"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_hiCloudBase, "Wx4_hiCloudBase"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_loCloudCov, "Wx4_loCloudCov"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_loCloudTop, "Wx4_loCloudTop"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_loCloudBase, "Wx4_loCloudBase"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_turbIntensity, "Wx4_turbIntensity"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_turbTop, "Wx4_turbTop"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_turbBase, "Wx4_turbBase"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_cbCloudCov, "Wx4_cbCloudCov"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_cbCloudTop, "Wx4_cbCloudTop"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_cbCloudBase, "Wx4_cbCloudBase"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_microburstMode, "Wx4_microburstMode"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_microburstRandom, "Wx4_microburstRandom"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_microburstOutflow, "Wx4_microburstOutflow"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_inversionOn, "Wx4_inversionOn"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_inversionTop, "Wx4_inversionTop"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_inversionTmp, "Wx4_inversionTmp"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_arptWindVarEtc, "Wx4_arptWindVarEtc"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_arptWindGust, "Wx4_arptWindGust"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_visibMtrs, "Wx4_visibMtrs"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_precipLevel, "Wx4_precipLevel"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_surfaceTmp, "Wx4_surfaceTmp"s, Qcode::Qs332);
		is_ExpQcodeInfo(Qcode::Wx4_QNH, "Wx4_QNH"s, Qcode::Qs332);

		is_ExpQcodeInfo(Qcode::Wx5_hiCloudCov, "Wx5_hiCloudCov"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_hiCloudTop, "Wx5_hiCloudTop"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_hiCloudBase, "Wx5_hiCloudBase"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_loCloudCov, "Wx5_loCloudCov"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_loCloudTop, "Wx5_loCloudTop"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_loCloudBase, "Wx5_loCloudBase"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_turbIntensity, "Wx5_turbIntensity"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_turbTop, "Wx5_turbTop"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_turbBase, "Wx5_turbBase"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_cbCloudCov, "Wx5_cbCloudCov"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_cbCloudTop, "Wx5_cbCloudTop"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_cbCloudBase, "Wx5_cbCloudBase"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_microburstMode, "Wx5_microburstMode"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_microburstRandom, "Wx5_microburstRandom"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_microburstOutflow, "Wx5_microburstOutflow"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_inversionOn, "Wx5_inversionOn"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_inversionTop, "Wx5_inversionTop"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_inversionTmp, "Wx5_inversionTmp"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_arptWindVarEtc, "Wx5_arptWindVarEtc"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_arptWindGust, "Wx5_arptWindGust"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_visibMtrs, "Wx5_visibMtrs"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_precipLevel, "Wx5_precipLevel"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_surfaceTmp, "Wx5_surfaceTmp"s, Qcode::Qs333);
		is_ExpQcodeInfo(Qcode::Wx5_QNH, "Wx5_QNH"s, Qcode::Qs333);

		is_ExpQcodeInfo(Qcode::Wx6_hiCloudCov, "Wx6_hiCloudCov"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_hiCloudTop, "Wx6_hiCloudTop"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_hiCloudBase, "Wx6_hiCloudBase"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_loCloudCov, "Wx6_loCloudCov"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_loCloudTop, "Wx6_loCloudTop"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_loCloudBase, "Wx6_loCloudBase"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_turbIntensity, "Wx6_turbIntensity"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_turbTop, "Wx6_turbTop"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_turbBase, "Wx6_turbBase"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_cbCloudCov, "Wx6_cbCloudCov"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_cbCloudTop, "Wx6_cbCloudTop"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_cbCloudBase, "Wx6_cbCloudBase"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_microburstMode, "Wx6_microburstMode"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_microburstRandom, "Wx6_microburstRandom"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_microburstOutflow, "Wx6_microburstOutflow"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_inversionOn, "Wx6_inversionOn"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_inversionTop, "Wx6_inversionTop"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_inversionTmp, "Wx6_inversionTmp"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_arptWindVarEtc, "Wx6_arptWindVarEtc"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_arptWindGust, "Wx6_arptWindGust"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_visibMtrs, "Wx6_visibMtrs"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_precipLevel, "Wx6_precipLevel"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_surfaceTmp, "Wx6_surfaceTmp"s, Qcode::Qs334);
		is_ExpQcodeInfo(Qcode::Wx6_QNH, "Wx6_QNH"s, Qcode::Qs334);

		is_ExpQcodeInfo(Qcode::Wx7_hiCloudCov, "Wx7_hiCloudCov"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_hiCloudTop, "Wx7_hiCloudTop"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_hiCloudBase, "Wx7_hiCloudBase"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_loCloudCov, "Wx7_loCloudCov"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_loCloudTop, "Wx7_loCloudTop"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_loCloudBase, "Wx7_loCloudBase"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_turbIntensity, "Wx7_turbIntensity"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_turbTop, "Wx7_turbTop"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_turbBase, "Wx7_turbBase"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_cbCloudCov, "Wx7_cbCloudCov"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_cbCloudTop, "Wx7_cbCloudTop"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_cbCloudBase, "Wx7_cbCloudBase"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_microburstMode, "Wx7_microburstMode"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_microburstRandom, "Wx7_microburstRandom"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_microburstOutflow, "Wx7_microburstOutflow"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_inversionOn, "Wx7_inversionOn"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_inversionTop, "Wx7_inversionTop"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_inversionTmp, "Wx7_inversionTmp"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_arptWindVarEtc, "Wx7_arptWindVarEtc"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_arptWindGust, "Wx7_arptWindGust"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_visibMtrs, "Wx7_visibMtrs"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_precipLevel, "Wx7_precipLevel"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_surfaceTmp, "Wx7_surfaceTmp"s, Qcode::Qs335);
		is_ExpQcodeInfo(Qcode::Wx7_QNH, "Wx7_QNH"s, Qcode::Qs335);

		is_ExpQcodeInfo(Qcode::OutFloValveL, "OutFloValveL"s, Qcode::Qs431);
		is_ExpQcodeInfo(Qcode::OutFloValveR, "OutFloValveR"s, Qcode::Qs431);
		is_ExpQcodeInfo(Qcode::CabinAltitude, "CabinAltitude"s, Qcode::Qs431);

		is_ExpQcodeInfo(Qcode::BldAirDuctL, "BldAirDuctL"s, Qcode::Qs428);
		is_ExpQcodeInfo(Qcode::BldAirDuctC, "BldAirDuctC"s, Qcode::Qs428);
		is_ExpQcodeInfo(Qcode::BldAirDuctR, "BldAirDuctR"s, Qcode::Qs428);

		is_ExpQcodeInfo(Qcode::IceWingL, "IceWingL"s, Qcode::Qs429);
		is_ExpQcodeInfo(Qcode::IceWingR, "IceWingR"s, Qcode::Qs429);
		is_ExpQcodeInfo(Qcode::IceNac1, "IceNac1"s, Qcode::Qs429);
		is_ExpQcodeInfo(Qcode::IceNac2, "IceNac2"s, Qcode::Qs429);
		is_ExpQcodeInfo(Qcode::IceNac3, "IceNac3"s, Qcode::Qs429);
		is_ExpQcodeInfo(Qcode::IceNac4, "IceNac4"s, Qcode::Qs429);
		is_ExpQcodeInfo(Qcode::Iced, "Iced"s, Qcode::Qs429);

		is_ExpQcodeInfo(Qcode::CabTempTarFD, "CabTempTarFD"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActFD, "CabTempActFD"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarUD, "CabTempTarUD"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActUD, "CabTempActUD"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarCR, "CabTempTarCR"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActCR, "CabTempActCR"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarMdCarFWD, "CabTempTarMdCarFWD"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActMdCarFWD, "CabTempActMdCarFWD"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarMdCarAFT, "CabTempTarMdCarAFT"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActMdCarAFT, "CabTempActMdCarAFT"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarLdCarFWD, "CabTempTarLdCarFWD"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActLdCarFWD, "CabTempActLdCarFWD"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarLdCarAFT, "CabTempTarLdCarAFT"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActLdCarAFT, "CabTempActLdCarAFT"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarA, "CabTempTarA"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActA, "CabTempActA"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarB, "CabTempTarB"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActB, "CabTempActB"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarC, "CabTempTarC"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActC, "CabTempActC"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarD, "CabTempTarD"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActD, "CabTempActD"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarE, "CabTempTarE"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActE, "CabTempActE"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempTarLdCarB, "CabTempTarLdCarB"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempActLdCarB, "CabTempActLdCarB"s, Qcode::Qs432);
		is_ExpQcodeInfo(Qcode::CabTempMasterSet, "CabTempMasterSet"s, Qcode::Qs432);

		is_ExpQcodeInfo(Qcode::ATActive, "ATActive"s, Qcode::Qs434);

		is_ExpQcodeInfo(Qcode::Tla1Virtual, "Tla1Virtual"s, Qcode::Qs436);
		is_ExpQcodeInfo(Qcode::Tla2Virtual, "Tla2Virtual"s, Qcode::Qs436);
		is_ExpQcodeInfo(Qcode::Tla3Virtual, "Tla3Virtual"s, Qcode::Qs436);
		is_ExpQcodeInfo(Qcode::Tla4Virtual, "Tla4Virtual"s, Qcode::Qs436);
		is_ExpQcodeInfo(Qcode::TlaMotor, "TlaMotor"s, Qcode::Qs436);

		is_ExpQcodeInfo(Qcode::TankMain1, "TankMain1"s, Qcode::Qs438);
		is_ExpQcodeInfo(Qcode::TankMain2, "TankMain2"s, Qcode::Qs438);
		is_ExpQcodeInfo(Qcode::TankMain3, "TankMain3"s, Qcode::Qs438);
		is_ExpQcodeInfo(Qcode::TankMain4, "TankMain4"s, Qcode::Qs438);
		is_ExpQcodeInfo(Qcode::TankReserve2, "TankReserve2"s, Qcode::Qs438);
		is_ExpQcodeInfo(Qcode::TankReserve3, "TankReserve3"s, Qcode::Qs438);
		is_ExpQcodeInfo(Qcode::TankCenter, "TankCenter"s, Qcode::Qs438);
		is_ExpQcodeInfo(Qcode::TankStab, "TankStab"s, Qcode::Qs438);
		is_ExpQcodeInfo(Qcode::TankAux, "TankAux"s, Qcode::Qs438);
		is_ExpQcodeInfo(Qcode::RefgPres, "RefgPres"s, Qcode::Qs438);
		is_ExpQcodeInfo(Qcode::FuelTemp, "FuelTemp"s, Qcode::Qs438);

		is_ExpQcodeInfo(Qcode::EfisActLhPa, "EfisActLhPa"s, Qcode::Qs448);
		is_ExpQcodeInfo(Qcode::EfisActLBaro, "EfisActLBaro"s, Qcode::Qs448);
		is_ExpQcodeInfo(Qcode::EfisActRhPa, "EfisActRhPa"s, Qcode::Qs449);
		is_ExpQcodeInfo(Qcode::EfisActRBaro, "EfisActRBaro"s, Qcode::Qs449);

		is_ExpQcodeInfo(Qcode::AileronsVirtual, "AileronsVirtual"s, Qcode::Qs120);
		is_ExpQcodeInfo(Qcode::ElevatorVirtual, "ElevatorVirtual"s, Qcode::Qs120);
		is_ExpQcodeInfo(Qcode::RudderVirtual, "RudderVirtual"s, Qcode::Qs120);
		is_ExpQcodeInfo(Qcode::ToeBrakeLeftVirtual, "ToeBrakeLeftVirtual"s, Qcode::Qs357);
		is_ExpQcodeInfo(Qcode::ToeBrakeRightVirtual, "ToeBrakeRightVirtual"s, Qcode::Qs357);

    is_ExpQcodeInfo(Qcode::CallPanelDisp, "CallPanelDisp"s, Qcode::Qs457);

		is_ExpQcodeInfo(Qcode::AilOutbLeft, "AilOutbLeft"s, Qcode::Qs480);
		is_ExpQcodeInfo(Qcode::AilInbLeft, "AilInbLeft"s, Qcode::Qs480);
		is_ExpQcodeInfo(Qcode::AilInbRight, "AilInbRight"s, Qcode::Qs480);
		is_ExpQcodeInfo(Qcode::AilOutbRight, "AilOutbRight"s, Qcode::Qs480);
		is_ExpQcodeInfo(Qcode::ElevOutbLeft, "ElevOutbLeft"s, Qcode::Qs480);
		is_ExpQcodeInfo(Qcode::ElevInbLeft, "ElevInbLeft"s, Qcode::Qs480);
		is_ExpQcodeInfo(Qcode::ElevInbRight, "ElevInbRight"s, Qcode::Qs480);
		is_ExpQcodeInfo(Qcode::ElevOutbRight, "ElevOutbRight"s, Qcode::Qs480);
		is_ExpQcodeInfo(Qcode::RudderUp, "RudderUp"s, Qcode::Qs480);
		is_ExpQcodeInfo(Qcode::RudderLow, "RudderLow"s, Qcode::Qs480);
    
    is_ExpQcodeInfo(Qcode::PhysIAS, "PhysIAS"s, Qcode::Qs483);
    is_ExpQcodeInfo(Qcode::PhysOAT, "PhysOAT"s, Qcode::Qs483);
    is_ExpQcodeInfo(Qcode::PhysTAT, "PhysTAT"s, Qcode::Qs483);
    is_ExpQcodeInfo(Qcode::CompWndDir, "CompWndDir"s, Qcode::Qs483);
    is_ExpQcodeInfo(Qcode::CompWndSpd, "CompWndSpd"s, Qcode::Qs483);
    is_ExpQcodeInfo(Qcode::MagVar, "MagVar"s, Qcode::Qs483);

		is_RcpDispl(Qcode::RcpLa, "RcpLa"s, Qcode::Qs458, Qcode::Qi92, Qcode::gR7737d);
		is_RcpDispl(Qcode::RcpLs, "RcpLs"s, Qcode::Qs458, Qcode::Qi92, Qcode::gR7737d);
		is_RcpDispl(Qcode::RcpCa, "RcpCa"s, Qcode::Qs459, Qcode::Qi93, Qcode::gDivAislePnlT144);
		is_RcpDispl(Qcode::RcpCs, "RcpCs"s, Qcode::Qs459, Qcode::Qi93, Qcode::gDivAislePnlT144);
		is_RcpDispl(Qcode::RcpRa, "RcpRa"s, Qcode::Qs460, Qcode::Qi94, Qcode::gDivAislePnlT144);
		is_RcpDispl(Qcode::RcpRs, "RcpRs"s, Qcode::Qs460, Qcode::Qi94, Qcode::gDivAislePnlT144);


	}

	auto VarPoolImpl::createInternalVars() -> void {
		// Qh
		// NOTE: do not forget to add the Qhxyz to set in Main
		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh13, "EcpAdfLCp"s);
		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh14, "EcpAdfRCp"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh32, "EcpNdModeCp"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh33, "EcpNdRangeCp"s);
		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh34, "EcpAdfLFo"s);
		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh35, "EcpAdfRFo"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh53, "EcpNdModeFo"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh54, "EcpNdRangeFo"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh76, "McpBankLim"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh116, "SrceFltDirCp"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh117, "SrceNavCp"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh118, "SrceEiuCp"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh119, "SrceIrsCp"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh120, "SrceAdcCp"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh121, "SrceFltDirFo"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh122, "SrceNavFo"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh123, "SrceEiuFo"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh124, "SrceIrsFo"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh125, "SrceAdcFo"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh126, "IndbCrtCp"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh127, "LwrCrtCp"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh129, "InbdCrtFo"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh130, "LwrCrtFo"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh131, "EiuEicas"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh133, "AltnFlaps"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh137, "StbyIlsSw"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh170, "GearLever"s);

		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh178, "LtBeacon"s);

		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh183, "LtIndTest"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh184, "HydDem1"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh185, "HydDem2"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh186, "HydDem3"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh187, "HydDem4"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh192, "StbyPower"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh193, "ApuStart"s);

		ch_QhMomActionCollectionVar(Qcode::Qh386, "ThrustTOGALeft"s, 2);
		ch_QhMomActionCollectionVar(Qcode::Qh387, "ThrustTOGARight"s, 2);

		ch_QhRotSwitchCollectionVar(Qcode::Qh213, "IrsL"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh214, "IrsC"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh215, "IrsR"s);

		ch_GuaOnOffOnVar(Qcode::Qh224, "Elt"s);
		ih_IndicatorLightELT(Qcode::Elt, "Elt"s, Qcode::Qh224);

		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh229, "NaiTogg1"s);
		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh230, "NaiTogg2"s);
		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh231, "NaiTogg3"s);
		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh232, "NaiTogg4"s);

		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh234, "WaiTogg"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh235, "WiperL"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh236, "WiperL"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh263, "IgnitSelStby"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh265, "IgnitSelAuto"s);

		ch_QhFireSwitchVar(Qcode::Qh275, "FireEng1"s);
		ch_QhFireSwitchVar(Qcode::Qh276, "FireEng2"s);
		ch_QhFireSwitchVar(Qcode::Qh277, "FireEng3"s);
		ch_QhFireSwitchVar(Qcode::Qh278, "FireEng4"s);
		ch_QhFireSwitchVar(Qcode::Qh279, "FireApu"s);

		ch_GuaOnOffOnVar(Qcode::Qh286, "EmerLt"s);

		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh288, "ObsAudio"s);

		ch_GuaOnOffOnVar(Qcode::Qh293, "PassOxygen"s);

		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh300, "OutflowTogg"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh319, "EquipCooling"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh301, "CabAltContSel"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh320, "PackSel1"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh321, "PackSel2"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh322, "PackSel3"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh358, "CargoCondAirSel"s);

		ch_GuaOnOffOnVar(Qcode::Qh390, "StabTrimCut2"s);
		ch_GuaOnOffOnVar(Qcode::Qh391, "StabTrimCut3"s);

		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh398, "StabTrimCp"s);
		ch_QhOnOffOnSwitchCollectionVar(Qcode::Qh399, "StabTrimFo"s);

		ch_QhRotSwitchCollectionVar(Qcode::Qh420, "NoSmoking"s);
		ch_QhRotSwitchCollectionVar(Qcode::Qh421, "Seatbelts"s);
		ch_GuaOnOffOnVar(Qcode::Qh422, "EvacArm"s);
		ch_QhEVACPshButtonsCollectionVar(Qcode::Qh423, "EvacPush"s);
		ch_QhEVACSignalCmndsCollectionVar(Qcode::Qh424, "EvacLight"s);

		// Qs
		cs_QsCBLineVar(Qcode::Qs4, "P71A"s, 13);
		cs_QsCBLineVar(Qcode::Qs5, "P71B"s, 13);
		cs_QsCBLineVar(Qcode::Qs6, "P71C"s, 13);
		cs_QsCBLineVar(Qcode::Qs7, "P71D"s, 13);
		cs_QsCBLineVar(Qcode::Qs8, "P71E"s, 13);
		cs_QsCBLineVar(Qcode::Qs9, "P71F"s, 13);
		cs_QsCBLineVar(Qcode::Qs10, "P71G"s, 13);
		cs_QsCBLineVar(Qcode::Qs11, "P71H"s, 13);
		cs_QsCBLineVar(Qcode::Qs12, "P71J"s, 13);
		cs_QsCBLineVar(Qcode::Qs13, "P72A"s, 13);
		cs_QsCBLineVar(Qcode::Qs14, "P72B"s, 13);
		cs_QsCBLineVar(Qcode::Qs15, "P72C"s, 13);
		cs_QsCBLineVar(Qcode::Qs16, "P72D"s, 13);
		cs_QsCBLineVar(Qcode::Qs17, "P72E"s, 13);
		cs_QsCBLineVar(Qcode::Qs18, "P72F"s, 13);
		cs_QsCBLineVar(Qcode::Qs19, "P72G"s, 13);
		cs_QsCBLineVar(Qcode::Qs20, "P72H"s, 13);
		cs_QsCBLineVar(Qcode::Qs21, "P72J"s, 13);
		cs_QsCBLineVar(Qcode::Qs22, "P61C"s, 9);
		cs_QsCBLineVar(Qcode::Qs23, "P61D"s, 9);
		cs_QsCBLineVar(Qcode::Qs24, "P61E"s, 9);
		cs_QsCBLineVar(Qcode::Qs25, "P61F"s, 9);
		cs_QsCBLineVar(Qcode::Qs26, "P61G"s, 9);
		cs_QsCBLineVar(Qcode::Qs27, "P61H"s, 9);
		cs_QsCBLineVar(Qcode::Qs28, "P61J"s, 9);
		cs_QsCBLineVar(Qcode::Qs29, "P61K"s, 9);
		cs_QsCBLineVar(Qcode::Qs30, "P61L"s, 9);
		cs_QsCBLineVar(Qcode::Qs31, "P62C"s, 9);
		cs_QsCBLineVar(Qcode::Qs32, "P62D"s, 9);
		cs_QsCBLineVar(Qcode::Qs33, "P62E"s, 9);
		cs_QsCBLineVar(Qcode::Qs34, "P62F"s, 9);
		cs_QsCBLineVar(Qcode::Qs35, "P62G"s, 9);
		cs_QsCBLineVar(Qcode::Qs36, "P62H"s, 9);
		cs_QsCBLineVar(Qcode::Qs37, "P62J"s, 9);
		cs_QsCBLineVar(Qcode::Qs38, "P62K"s, 9);
		cs_QsCBLineVar(Qcode::Qs39, "P62L"s, 9);
		cs_QsCBLineVar(Qcode::Qs40, "P63C"s, 9);
		cs_QsCBLineVar(Qcode::Qs41, "P63D"s, 9);
		cs_QsCBLineVar(Qcode::Qs42, "P63E"s, 9);
		cs_QsCBLineVar(Qcode::Qs43, "P63F"s, 9);
		cs_QsCBLineVar(Qcode::Qs44, "P63G"s, 9);
		cs_QsCBLineVar(Qcode::Qs45, "P63H"s, 9);
		cs_QsCBLineVar(Qcode::Qs46, "P63J"s, 9);
		cs_QsCBLineVar(Qcode::Qs47, "P63K"s, 9);
		cs_QsCBLineVar(Qcode::Qs48, "P63L"s, 9);
		cs_QsCBLineVar(Qcode::Qs49, "P64C"s, 9);
		cs_QsCBLineVar(Qcode::Qs50, "P64D"s, 9);
		cs_QsCBLineVar(Qcode::Qs51, "P64E"s, 9);
		cs_QsCBLineVar(Qcode::Qs52, "P64F"s, 9);
		cs_QsCBLineVar(Qcode::Qs53, "P64G"s, 9);
		cs_QsCBLineVar(Qcode::Qs54, "P64H"s, 9);
		cs_QsCBLineVar(Qcode::Qs55, "P64J"s, 9);
		cs_QsCBLineVar(Qcode::Qs56, "P64K"s, 9);
		cs_QsCBLineVar(Qcode::Qs57, "P64L"s, 9);
		cs_QsCBLineVar(Qcode::Qs58, "P65A"s, 18);
		cs_QsCBLineVar(Qcode::Qs59, "P65B"s, 18);
		cs_QsCBLineVar(Qcode::Qs60, "P66A"s, 18);
		cs_QsCBLineVar(Qcode::Qs61, "P66B"s, 18);
		cs_WxRdrPanelPushVar(Qcode::Qs104, "WxRdrPanelPush"s, 7);
		cs_WxRdrPanelTurnVar(Qcode::Qs105, "WxRdrPanelTurn"s, 4);

    if (*g_CfgDualRCPLeft) {
      cs_QsDeltaCollectionVar(Qcode::Qs109, "RotSelRcpL"s, 3);
    } else {
      cs_QsDeltaCollectionVar2(Qcode::Qs109, "RotSelRcpL"s);
    }
    if (*g_CfgDualRCPCenter) {
      cs_QsDeltaCollectionVar(Qcode::Qs110, "RotSelRcpC"s, 3);
    } else {
      cs_QsDeltaCollectionVar2(Qcode::Qs110, "RotSelRcpC"s);
    }
    if (*g_CfgDualRCPRight) {
      cs_QsDeltaCollectionVar(Qcode::Qs111, "RotSelRcpR"s, 3);
    } else {
      cs_QsDeltaCollectionVar2(Qcode::Qs111, "RotSelRcpR"s);
		}

		cs_QsECONCollectionVar(Qcode::Qs113, "RotSelAudioL"s, 16);
		cs_QsECONCollectionVar(Qcode::Qs114, "RotSelAudioC"s, 16);
		cs_QsECONCollectionVar(Qcode::Qs115, "RotSelAudioR"s, 16);

		cs_QsECONCollectionVar(Qcode::Qs117, "TcasPanSel"s, 4);

		cs_Qs120Var(Qcode::Qs120, "FltControls"s, 3);

		cs_Qs357Var(Qcode::Qs357, "Brakes"s, 2);

		cs_QsECONCollectionVar(Qcode::Qs436, "Tla"s, 4);
	}

	auto VarPoolImpl::createInternalVarDefs() -> void {
		// Qh
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh13,{Qcode::EcpAdfLCpOnL, Qcode::EcpAdfLCpOnR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh14,{Qcode::EcpAdfRCpOnL, Qcode::EcpAdfRCpOnR} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh32,{Qcode::EcpNdModeCpAp, Qcode::EcpNdModeCpVo, Qcode::EcpNdModeCpMa, Qcode::EcpNdModeCpPL} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh33,{Qcode::EcpNdRangeCp5, Qcode::EcpNdRangeCp10, Qcode::EcpNdRangeCp20, Qcode::EcpNdRangeCp40,
									Qcode::EcpNdRangeCp80, Qcode::EcpNdRangeCp160, Qcode::EcpNdRangeCp320, Qcode::EcpNdRangeCp640} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh34,{Qcode::EcpAdfLFoOnL, Qcode::EcpAdfLFoOnR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh35,{Qcode::EcpAdfRFoOnL, Qcode::EcpAdfRFoOnR} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh53,{Qcode::EcpNdModeFoAp, Qcode::EcpNdModeFoVo, Qcode::EcpNdModeFoMa, Qcode::EcpNdModeFoPL} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh54,{Qcode::EcpNdRangeFo5, Qcode::EcpNdRangeFo10, Qcode::EcpNdRangeFo20, Qcode::EcpNdRangeFo40,
									Qcode::EcpNdRangeFo80, Qcode::EcpNdRangeFo160, Qcode::EcpNdRangeFo320, Qcode::EcpNdRangeFo640} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh76,{Qcode::McpBankLimAuto, Qcode::McpBankLim5, Qcode::McpBankLim10, Qcode::McpBankLim15,
									Qcode::McpBankLim20, Qcode::McpBankLim25} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh116,{Qcode::SrceFltDirCpL, Qcode::SrceFltDirCpC, Qcode::SrceFltDirCpR } });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh117,{Qcode::SrceNavCpFL, Qcode::SrceNavCpFR, Qcode::SrceNavCpCL, Qcode::SrceNavCpCC} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh118,{Qcode::SrceEiuCpR, Qcode::SrceEiuCpA, Qcode::SrceEiuCpC, Qcode::SrceEiuCpL} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh119,{Qcode::SrceIrsCp0, Qcode::SrceIrsCp1, Qcode::SrceIrsCp2, Qcode::SrceIrsCp3 } });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh120,{Qcode::SrceAdcCp0, Qcode::SrceAdcCp1, Qcode::SrceAdcCp2, Qcode::SrceAdcCp3} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh121,{Qcode::SrceFltDirFoL, Qcode::SrceFltDirFoC, Qcode::SrceFltDirFoR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh122,{Qcode::SrceNavFoCC, Qcode::SrceNavFoCR, Qcode::SrceNavFoFL, Qcode::SrceNavFoFR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh123,{Qcode::SrceEiuFoL, Qcode::SrceEiuFoC, Qcode::SrceEiuFoA, Qcode::SrceEiuFoR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh124,{Qcode::SrceIrsFo0, Qcode::SrceIrsFo1, Qcode::SrceIrsFo2, Qcode::SrceIrsFo3} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh125,{Qcode::SrceAdcFo0, Qcode::SrceAdcFo1, Qcode::SrceAdcFo2, Qcode::SrceAdcFo3} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh126,{Qcode::IndbCrtCpEicas, Qcode::IndbCrtCpNorm, Qcode::IndbCrtCpPfd} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh127,{Qcode::LwrCrtCpIcasPri, Qcode::LwrCrtCpNorm, Qcode::LwrCrtCpNd} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh129,{Qcode::IndbCrtFoPfd, Qcode::IndbCrtFoNorm, Qcode::IndbCrtFoEicas } });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh130,{Qcode::LwrCrtFoNd, Qcode::LwrCrtFoNorm, Qcode::LwrCrtFoIcasPri } });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh131,{Qcode::EiuEicasL, Qcode::EiuEicasA, Qcode::EiuEicasC, Qcode::EiuEicasR} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh133,{Qcode::AltnFlapsRet, Qcode::AltnFlapsOff, Qcode::AltnFlapsExt} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh137,{Qcode::StbyIlsSw0, Qcode::StbyIlsSw1, Qcode::StbyIlsSw2} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh170,{Qcode::GearLeverUp, Qcode::GearLeverOff, Qcode::GearLeverDn} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh178,{Qcode::LtBeaconOnL, Qcode::LtBeaconOnR} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh183,{Qcode::LtIndTestOnL, Qcode::LtIndTestOnR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh184,{Qcode::HydDem10, Qcode::HydDem11, Qcode::HydDem12, Qcode::HydDem13} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh185,{Qcode::HydDem2Off, Qcode::HydDem2Auto, Qcode::HydDem2On} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh186,{Qcode::HydDem3Off, Qcode::HydDem3Auto, Qcode::HydDem3On} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh187,{Qcode::HydDem4Aux, Qcode::HydDem4_1, Qcode::HydDem4Auto, Qcode::HydDem4On} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh192,{Qcode::StbyPowerOff, Qcode::StbyPowerAuto, Qcode::StbyPowerBat} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh193,{Qcode::ApuStartOff, Qcode::ApuStartOn, Qcode::ApuStartStart} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh213,{Qcode::IrsLOff, Qcode::IrsLAlign, Qcode::IrsLNav, Qcode::IrsLAtt} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh214,{Qcode::IrsCOff, Qcode::IrsCAlign, Qcode::IrsCNav, Qcode::IrsCAtt} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh215,{Qcode::IrsROff, Qcode::IrsRAlign, Qcode::IrsRNav, Qcode::IrsRAtt} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh224,{Qcode::EltL, Qcode::EltR} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh229,{Qcode::NaiTogg1OnL, Qcode::NaiTogg1OnR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh230,{Qcode::NaiTogg2OnL, Qcode::NaiTogg2OnR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh231,{Qcode::NaiTogg3OnL, Qcode::NaiTogg3OnR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh232,{Qcode::NaiTogg4OnL, Qcode::NaiTogg4OnR} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh234,{Qcode::WaiToggOnL, Qcode::WaiToggOnR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh235,{Qcode::WiperL0, Qcode::WiperL1, Qcode::WiperL2, Qcode::WiperL3} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh236,{Qcode::WiperR0, Qcode::WiperR1, Qcode::WiperR2, Qcode::WiperR3} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh263,{Qcode::IgnitSelStby1, Qcode::IgnitSelStbyN, Qcode::IgnitSelStby2} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh265,{Qcode::IgnitSelAuto0, Qcode::IgnitSelAut1, Qcode::IgnitSelAuto2, Qcode::IgnitSelAut3} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh275,{Qcode::FireEng1Pulled, Qcode::FireEng1Override, Qcode::FireEng1M22, Qcode::FireEng1M15, Qcode::FireEng1P15, Qcode::FireEng1P22} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh276,{Qcode::FireEng2Pulled, Qcode::FireEng2Override, Qcode::FireEng2M22, Qcode::FireEng2M15, Qcode::FireEng2P15, Qcode::FireEng2P22} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh277,{Qcode::FireEng3Pulled, Qcode::FireEng3Override, Qcode::FireEng3M22, Qcode::FireEng3M15, Qcode::FireEng3P15, Qcode::FireEng3P22} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh278,{Qcode::FireEng4Pulled, Qcode::FireEng4Override, Qcode::FireEng4M22, Qcode::FireEng4M15, Qcode::FireEng4P15, Qcode::FireEng4P22} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh279,{Qcode::FireApuPulled, Qcode::FireApuOverride, Qcode::FireApuM22, Qcode::FireApuM15, Qcode::FireApuP15, Qcode::FireApuP22} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh286,{Qcode::EmerLtL, Qcode::EmerLtR} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh288,{Qcode::ObsAudioL, Qcode::ObsAudioR} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh293,{Qcode::PassOxygenL, Qcode::PassOxygenR} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh300,{Qcode::OutflowToggOnL, Qcode::OutflowToggOnR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh301,{Qcode::CabAltContSelN, Qcode::CabAltContSelA, Qcode::CabAltContSelB} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh319,{Qcode::EquipCoolingS, Qcode::EquipCoolingN, Qcode::EquipCoolingO} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh320,{Qcode::PackSel1Off, Qcode::PackSel1Norm, Qcode::PackSel1A, Qcode::PackSel1B} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh321,{Qcode::PackSel2Off, Qcode::PackSel2Norm, Qcode::PackSel2A, Qcode::PackSel2B} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh322,{Qcode::PackSel3Off, Qcode::PackSel3Norm, Qcode::PackSel3A, Qcode::PackSel3B} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh358,{Qcode::CargoCondAirSelBOTH, Qcode::CargoCondAirSelLA, Qcode::CargoCondAirSelLF,
									Qcode::CargoCondAirSelOFF, Qcode::CargoCondAirSelHF, Qcode::CargoCondAirSelHA} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh386,{Qcode::ThrustDiscLeft, Qcode::ThrustDiscRight} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh387,{Qcode::ThrustTOGALeft, Qcode::ThrustTOGARight} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh390,{Qcode::StabTrimCut2On, Qcode::StabTrimCut2Off} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh391,{Qcode::StabTrimCut3On, Qcode::StabTrimCut3Off} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh398,{Qcode::StabTrimCpOnL, Qcode::StabTrimCpOnR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh399,{Qcode::StabTrimCFoOnL, Qcode::StabTrimFoOnR} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh420,{Qcode::NoSmokingOff, Qcode::NoSmokingAuto, Qcode::NoSmokingOn} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh421,{Qcode::SeatbeltsOff, Qcode::SeatbeltsAuto, Qcode::SeatbeltsOn} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh422,{Qcode::EvacArmL, Qcode::EvacArmR} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh423,{Qcode::EvacHornPush, Qcode::EvacTestPress} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qh424,{Qcode::EvacSigPilot, Qcode::EvacSigFA} });

		// Qs
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs4,{Qcode::P71A_01, Qcode::P71A_02, Qcode::P71A_03, Qcode::P71A_04 ,Qcode::P71A_05, Qcode::P71A_06, Qcode::P71A_07, Qcode::P71A_08, Qcode::P71A_09, Qcode::P71A_10, Qcode::P71A_11, Qcode::P71A_12, Qcode::P71A_13} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs5,{Qcode::P71B_01, Qcode::P71B_02, Qcode::P71B_03, Qcode::P71B_04, Qcode::P71B_05, Qcode::P71B_06, Qcode::P71B_07, Qcode::P71B_08, Qcode::P71B_09, Qcode::P71B_10, Qcode::P71B_11, Qcode::P71B_12, Qcode::P71B_13} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs6,{Qcode::P71C_01, Qcode::P71C_02, Qcode::P71C_03, Qcode::P71C_04, Qcode::P71C_05, Qcode::P71C_06, Qcode::P71C_07, Qcode::P71C_08, Qcode::P71C_09, Qcode::P71C_10, Qcode::P71C_11, Qcode::P71C_12, Qcode::P71C_13} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs7,{Qcode::P71D_01, Qcode::P71D_02, Qcode::P71D_03, Qcode::P71D_04, Qcode::P71D_05, Qcode::P71D_06, Qcode::P71D_07, Qcode::P71D_08, Qcode::P71D_09, Qcode::P71D_10, Qcode::P71D_11, Qcode::P71D_12, Qcode::P71D_13} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs8,{Qcode::P71E_01, Qcode::P71E_02, Qcode::P71E_03, Qcode::P71E_04, Qcode::P71E_05, Qcode::P71E_06, Qcode::P71E_07, Qcode::P71E_08, Qcode::P71E_09, Qcode::P71E_10, Qcode::P71E_11, Qcode::P71E_12, Qcode::P71E_13} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs9,{Qcode::P71F_01, Qcode::P71F_02, Qcode::P71F_03, Qcode::P71F_04, Qcode::P71F_05, Qcode::P71F_06, Qcode::P71F_07, Qcode::P71F_08, Qcode::P71F_09, Qcode::P71F_10, Qcode::P71F_11, Qcode::P71F_12, Qcode::P71F_13} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs10,{Qcode::P71G_01, Qcode::P71G_02, Qcode::P71G_03, Qcode::P71G_04, Qcode::P71G_05, Qcode::P71G_06, Qcode::P71G_07, Qcode::P71G_08, Qcode::P71G_09, Qcode::P71G_10, Qcode::P71G_11, Qcode::P71G_12, Qcode::P71G_13} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs11,{Qcode::P71H_01, Qcode::P71H_02, Qcode::P71H_03, Qcode::P71H_04, Qcode::P71H_05, Qcode::P71H_06, Qcode::P71H_07, Qcode::P71H_08, Qcode::P71H_09, Qcode::P71H_10, Qcode::P71H_11, Qcode::P71H_12, Qcode::P71H_13} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs12,{Qcode::P71J_01, Qcode::P71J_02, Qcode::P71J_03, Qcode::P71J_04, Qcode::P71J_05, Qcode::P71J_06, Qcode::P71J_07, Qcode::P71J_08, Qcode::P71J_09, Qcode::P71J_10, Qcode::P71J_11, Qcode::P71J_12, Qcode::P71J_13} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs13,{Qcode::P72A_14, Qcode::P72A_15, Qcode::P72A_16, Qcode::P72A_17, Qcode::P72A_18, Qcode::P72A_19, Qcode::P72A_20, Qcode::P72A_21, Qcode::P72A_22, Qcode::P72A_23, Qcode::P72A_24, Qcode::P72A_25, Qcode::P72A_26} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs14,{Qcode::P72B_14, Qcode::P72B_15, Qcode::P72B_16, Qcode::P72B_17, Qcode::P72B_18, Qcode::P72B_19, Qcode::P72B_20, Qcode::P72B_21, Qcode::P72B_22, Qcode::P72B_23, Qcode::P72B_24, Qcode::P72B_25, Qcode::P72B_26} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs15,{Qcode::P72C_14, Qcode::P72C_15, Qcode::P72C_16, Qcode::P72C_17, Qcode::P72C_18, Qcode::P72C_19, Qcode::P72C_20, Qcode::P72C_21, Qcode::P72C_22, Qcode::P72C_23, Qcode::P72C_24, Qcode::P72C_25, Qcode::P72C_26} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs16,{Qcode::P72D_14, Qcode::P72D_15, Qcode::P72D_16, Qcode::P72D_17, Qcode::P72D_18, Qcode::P72D_19, Qcode::P72D_20, Qcode::P72D_21, Qcode::P72D_22, Qcode::P72D_23, Qcode::P72D_24, Qcode::P72D_25, Qcode::P72D_25} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs17,{Qcode::P72E_14, Qcode::P72E_15, Qcode::P72E_16, Qcode::P72E_17, Qcode::P72E_18, Qcode::P72E_19, Qcode::P72E_20, Qcode::P72E_21, Qcode::P72E_23, Qcode::P72E_23, Qcode::P72E_24, Qcode::P72E_25, Qcode::P72E_26} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs18,{Qcode::P72F_14, Qcode::P72F_15, Qcode::P72F_16, Qcode::P72F_17, Qcode::P72F_18, Qcode::P72F_19, Qcode::P72F_20, Qcode::P72F_21, Qcode::P72F_22, Qcode::P72F_23, Qcode::P72F_24, Qcode::P72F_25, Qcode::P72F_26} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs19,{Qcode::P72G_14, Qcode::P72G_15, Qcode::P72G_16, Qcode::P72G_17, Qcode::P72G_18, Qcode::P72G_19, Qcode::P72G_20, Qcode::P72G_21, Qcode::P72G_22, Qcode::P72G_23, Qcode::P72G_24, Qcode::P72G_25, Qcode::P72G_26} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs20,{Qcode::P72H_14, Qcode::P72H_15, Qcode::P72H_16, Qcode::P72H_17, Qcode::P72H_18, Qcode::P72H_19, Qcode::P72H_20, Qcode::P72H_21, Qcode::P72H_22, Qcode::P72H_23, Qcode::P72H_24, Qcode::P72H_25, Qcode::P72H_26} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs21,{Qcode::P72J_14, Qcode::P72J_15, Qcode::P72J_16, Qcode::P72J_17, Qcode::P72J_18, Qcode::P72J_19, Qcode::P72J_20, Qcode::P72J_21, Qcode::P72J_22, Qcode::P72J_23, Qcode::P72J_24, Qcode::P72J_25, Qcode::P72J_26} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs22,{Qcode::P61C_01, Qcode::P61C_02, Qcode::P61C_03, Qcode::P61C_04, Qcode::P61C_05, Qcode::P61C_06, Qcode::P61C_07, Qcode::P61C_08, Qcode::P61C_09} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs23,{Qcode::P61D_01, Qcode::P61D_02, Qcode::P61D_03, Qcode::P61D_04, Qcode::P61D_05, Qcode::P61D_06, Qcode::P61D_07, Qcode::P61D_08, Qcode::P61D_09} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs24,{Qcode::P61E_01, Qcode::P61E_02, Qcode::P61E_03, Qcode::P61E_04, Qcode::P61E_05, Qcode::P61E_06, Qcode::P61E_07, Qcode::P61E_08, Qcode::P61E_09} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs25,{Qcode::P61F_01, Qcode::P61F_02, Qcode::P61F_03, Qcode::P61F_04, Qcode::P61F_05, Qcode::P61F_06, Qcode::P61F_07, Qcode::P61F_08, Qcode::P61F_09} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs26,{Qcode::P61G_01, Qcode::P61G_02, Qcode::P61G_03, Qcode::P61G_04, Qcode::P61G_05, Qcode::P61G_06, Qcode::P61G_07, Qcode::P61G_08, Qcode::P61G_09} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs27,{Qcode::P61H_01, Qcode::P61H_02, Qcode::P61H_03, Qcode::P61H_04, Qcode::P61H_05, Qcode::P61H_06, Qcode::P61H_07, Qcode::P61H_08, Qcode::P61H_09} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs28,{Qcode::P61J_01, Qcode::P61J_02, Qcode::P61J_03, Qcode::P61J_04, Qcode::P61J_05, Qcode::P61J_06, Qcode::P61J_07, Qcode::P61J_08, Qcode::P61J_09} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs29,{Qcode::P61K_01, Qcode::P61K_02, Qcode::P61K_03, Qcode::P61K_04, Qcode::P61K_05, Qcode::P61K_06, Qcode::P61K_07, Qcode::P61K_08, Qcode::P61K_09} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs30,{Qcode::P61L_01, Qcode::P61L_02, Qcode::P61L_03, Qcode::P61L_04, Qcode::P61L_05, Qcode::P61L_06, Qcode::P61L_07, Qcode::P61L_08, Qcode::P61L_09} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs31,{Qcode::P62C_10, Qcode::P62C_11, Qcode::P62C_12, Qcode::P62C_13, Qcode::P62C_14, Qcode::P62C_15, Qcode::P62C_16, Qcode::P62C_17, Qcode::P62C_18} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs32,{Qcode::P62D_10, Qcode::P62D_11, Qcode::P62D_12, Qcode::P62D_13, Qcode::P62D_14, Qcode::P62D_15, Qcode::P62D_16, Qcode::P62D_17, Qcode::P62D_18} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs33,{Qcode::P62E_10, Qcode::P62E_11, Qcode::P62E_12, Qcode::P62E_13, Qcode::P62E_14, Qcode::P62E_15, Qcode::P62E_16, Qcode::P62E_17, Qcode::P62E_18} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs34,{Qcode::P62F_10, Qcode::P62F_11, Qcode::P62F_12, Qcode::P62F_13, Qcode::P62F_14, Qcode::P62F_15, Qcode::P62F_16, Qcode::P62F_17, Qcode::P62F_18} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs35,{Qcode::P62G_10, Qcode::P62G_11, Qcode::P62G_12, Qcode::P62G_13, Qcode::P62G_14, Qcode::P62G_15, Qcode::P62G_16, Qcode::P62G_17, Qcode::P62G_18} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs36,{Qcode::P62H_10, Qcode::P62H_11, Qcode::P62H_12, Qcode::P62H_13, Qcode::P62H_14, Qcode::P62H_15, Qcode::P62H_16, Qcode::P62H_17, Qcode::P62H_18} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs37,{Qcode::P62J_10, Qcode::P62J_11, Qcode::P62J_12, Qcode::P62J_13, Qcode::P62J_14, Qcode::P62J_15, Qcode::P62J_16, Qcode::P62J_17, Qcode::P62J_18} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs38,{Qcode::P62K_10, Qcode::P62K_11, Qcode::P62K_12, Qcode::P62K_13, Qcode::P62K_14, Qcode::P62K_15, Qcode::P62K_16, Qcode::P62K_17, Qcode::P62K_18} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs39,{Qcode::P62L_10, Qcode::P62L_11, Qcode::P62L_12, Qcode::P62L_13, Qcode::P62L_14, Qcode::P62L_15, Qcode::P62L_16, Qcode::P62L_17, Qcode::P62L_18} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs40,{Qcode::P63C_19, Qcode::P63C_20, Qcode::P63C_21, Qcode::P63C_22, Qcode::P63C_23, Qcode::P63C_24, Qcode::P63C_25, Qcode::P63C_26, Qcode::P63C_27} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs41,{Qcode::P63D_19, Qcode::P63D_20, Qcode::P63D_21, Qcode::P63D_22, Qcode::P63D_23, Qcode::P63D_24, Qcode::P63D_25, Qcode::P63D_26, Qcode::P63D_27} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs42,{Qcode::P63E_19, Qcode::P63E_20, Qcode::P63E_21, Qcode::P63E_22, Qcode::P63E_23, Qcode::P63E_24, Qcode::P63E_25, Qcode::P63E_26, Qcode::P63E_27} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs43,{Qcode::P63F_19, Qcode::P63F_20, Qcode::P63F_21, Qcode::P63F_22, Qcode::P63F_23, Qcode::P63F_24, Qcode::P63F_25, Qcode::P63F_26, Qcode::P63F_27} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs44,{Qcode::P63G_19, Qcode::P63G_20, Qcode::P63G_21, Qcode::P63G_22, Qcode::P63G_23, Qcode::P63G_24, Qcode::P63G_25, Qcode::P63G_26, Qcode::P63G_27} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs45,{Qcode::P63H_19, Qcode::P63H_20, Qcode::P63H_21, Qcode::P63H_22, Qcode::P63H_23, Qcode::P63H_24, Qcode::P63H_25, Qcode::P63H_26, Qcode::P63H_27} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs46,{Qcode::P63J_19, Qcode::P63J_20, Qcode::P63J_21, Qcode::P63J_22, Qcode::P63J_23, Qcode::P63J_24, Qcode::P63J_25, Qcode::P63J_26, Qcode::P63J_27} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs47,{Qcode::P63K_19, Qcode::P63K_20, Qcode::P63K_21, Qcode::P63K_22, Qcode::P63K_23, Qcode::P63K_24, Qcode::P63K_25, Qcode::P63K_26, Qcode::P63K_27} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs48,{Qcode::P63L_19, Qcode::P63L_20, Qcode::P63L_21, Qcode::P63L_22, Qcode::P63L_23, Qcode::P63L_24, Qcode::P63L_25, Qcode::P63L_26, Qcode::P63L_27} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs49,{Qcode::P64C_28, Qcode::P64C_29, Qcode::P64C_30, Qcode::P64C_31, Qcode::P64C_32, Qcode::P64C_33, Qcode::P64C_34, Qcode::P64C_35, Qcode::P64C_36} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs50,{Qcode::P64D_28, Qcode::P64D_29, Qcode::P64D_30, Qcode::P64D_31, Qcode::P64D_32, Qcode::P64D_33, Qcode::P64D_34, Qcode::P64D_35, Qcode::P64D_36} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs51,{Qcode::P64E_28, Qcode::P64E_29, Qcode::P64E_30, Qcode::P64E_31, Qcode::P64E_32, Qcode::P64E_33, Qcode::P64E_34, Qcode::P64E_35, Qcode::P64E_36} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs52,{Qcode::P64F_28, Qcode::P64F_29, Qcode::P64F_30, Qcode::P64F_31, Qcode::P64F_32, Qcode::P64F_33, Qcode::P64F_34, Qcode::P64F_35, Qcode::P64F_36} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs53,{Qcode::P64G_28, Qcode::P64G_29, Qcode::P64G_30, Qcode::P64G_31, Qcode::P64G_32, Qcode::P64G_33, Qcode::P64G_34, Qcode::P64G_35, Qcode::P64G_36} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs54,{Qcode::P64H_28, Qcode::P64H_29, Qcode::P64H_30, Qcode::P64H_31, Qcode::P64H_32, Qcode::P64H_33, Qcode::P64H_34, Qcode::P64H_35, Qcode::P64H_36} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs55,{Qcode::P64J_28, Qcode::P64J_29, Qcode::P64J_30, Qcode::P64J_31, Qcode::P64J_32, Qcode::P64J_33, Qcode::P64J_34, Qcode::P64J_35, Qcode::P64J_36} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs56,{Qcode::P64K_28, Qcode::P64K_29, Qcode::P64K_30, Qcode::P64K_31, Qcode::P64K_32, Qcode::P64K_33, Qcode::P64K_34, Qcode::P64K_35, Qcode::P64K_36} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs57,{Qcode::P64L_28, Qcode::P64L_29, Qcode::P64L_30, Qcode::P64L_31, Qcode::P64L_32, Qcode::P64L_33, Qcode::P64L_34, Qcode::P64L_35, Qcode::P64L_36} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs58,{Qcode::P65A_01, Qcode::P65A_02, Qcode::P65A_03, Qcode::P65A_04, Qcode::P65A_05, Qcode::P65A_06, Qcode::P65A_07, Qcode::P65A_08, Qcode::P65A_09,
									Qcode::P65A_10, Qcode::P65A_11, Qcode::P65A_12, Qcode::P65A_13, Qcode::P65A_14, Qcode::P65A_15, Qcode::P65A_16, Qcode::P65A_17, Qcode::P65A_18} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs59,{Qcode::P65B_01, Qcode::P65B_02, Qcode::P65B_03, Qcode::P65B_04, Qcode::P65B_05, Qcode::P65B_06, Qcode::P65B_07, Qcode::P65B_08, Qcode::P65B_09,
									Qcode::P65B_10, Qcode::P65B_11, Qcode::P65B_12, Qcode::P65B_13, Qcode::P65B_14, Qcode::P65B_15, Qcode::P65B_16, Qcode::P65B_17, Qcode::P65B_18} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs60,{Qcode::P66A_19, Qcode::P66A_20, Qcode::P66A_21, Qcode::P66A_22, Qcode::P66A_23, Qcode::P66A_24, Qcode::P66A_25, Qcode::P66A_26, Qcode::P66A_27,
									Qcode::P66A_28, Qcode::P66A_29, Qcode::P66A_30, Qcode::P66A_31, Qcode::P66A_32, Qcode::P66A_33, Qcode::P66A_34, Qcode::P66A_35, Qcode::P66A_36} });
		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs61,{Qcode::P66B_19, Qcode::P66B_20, Qcode::P66B_21, Qcode::P66B_22, Qcode::P66B_23, Qcode::P66B_24, Qcode::P66B_25, Qcode::P66B_26, Qcode::P66B_27,
									Qcode::P66B_28, Qcode::P66B_29, Qcode::P66B_30, Qcode::P66B_31, Qcode::P66B_32, Qcode::P66B_33, Qcode::P66B_34, Qcode::P66B_35, Qcode::P66B_36} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs104,{Qcode::TFRCp, Qcode::WxCp, Qcode::WxTCp, Qcode::MAPCp, Qcode::GCCp, Qcode::WxAUTO,
									Qcode::WxLR, Qcode::WxTEST, Qcode::TFRFo,Qcode::WxFo, Qcode::WxTFo, Qcode::MAPFo, Qcode::GCFo} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs105,{Qcode::TiltCp, Qcode::GainCpMin, Qcode::GainCpM6, Qcode::GainCpM4, Qcode::GainCpM3, Qcode::GainCpM2,
									Qcode::GainCpM1, Qcode::GainCpNeu, Qcode::GainCp2, Qcode::GainCp4, Qcode::GainCpMax,
                  Qcode::TiltFo, Qcode::GainFoMin, Qcode::GainFoM6, Qcode::GainFoM4, Qcode::GainFoM3, Qcode::GainFoM2,
                  Qcode::GainFoM1, Qcode::GainFoNeu, Qcode::GainFo2, Qcode::GainFo4, Qcode::GainFoMax}});

    if (*g_CfgDualRCPLeft) {
      internalvar_defs_.emplace_back(InternalVarStruct{Qcode::Qs109,{Qcode::HFSensL, Qcode::BigFreqKnobL, Qcode::SmallFreqKnobL}});
    } else {
      internalvar_defs_.emplace_back(InternalVarStruct{Qcode::Qs109,{Qcode::BigSmallFreqKnobL, Qcode::BigSmallFreqSwitchL}});
    }
    if (*g_CfgDualRCPCenter) {
      internalvar_defs_.emplace_back(InternalVarStruct{Qcode::Qs110,{Qcode::HFSensC, Qcode::BigFreqKnobC, Qcode::SmallFreqKnobC}});
    } else {
      internalvar_defs_.emplace_back(InternalVarStruct{Qcode::Qs110,{Qcode::BigSmallFreqKnobC, Qcode::BigSmallFreqSwitchC}});
    }
    if (*g_CfgDualRCPRight) {
      internalvar_defs_.emplace_back(InternalVarStruct{Qcode::Qs111,{Qcode::HFSensR, Qcode::BigFreqKnobR, Qcode::SmallFreqKnobR}});
    } else {
      internalvar_defs_.emplace_back(InternalVarStruct{Qcode::Qs111,{Qcode::BigSmallFreqKnobR, Qcode::BigSmallFreqSwitchR}});
    }

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs113,{Qcode::AcpRotSelVHF_L_L, Qcode::AcpRotSelVHF_C_L, Qcode::AcpRotSelVHF_R_L,
									Qcode::AcpRotSelFLT_L, Qcode::AcpRotSelCAB_L,Qcode::AcpRotSelPA_L, Qcode::AcpRotSelHF_L_L,
									Qcode::AcpRotSelHF_R_L, Qcode::AcpRotSelSAT_L_L, Qcode::AcpRotSelSAT_R_L, Qcode::AcpRotSelSPKR_L, Qcode::AcpRotSelVolVA_L, Qcode::AcpRotSelVolAPP_L,
									Qcode::AcpRotSelVORADF_L, Qcode::AcpRotVBR_L, Qcode::AcpRotIM_L} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs114,{Qcode::AcpRotSelVHF_L_C, Qcode::AcpRotSelVHF_C_C, Qcode::AcpRotSelVHF_R_C,
									Qcode::AcpRotSelFLT_C, Qcode::AcpRotSelCAB_C,Qcode::AcpRotSelPA_C, Qcode::AcpRotSelHF_L_C,
									Qcode::AcpRotSelHF_R_C, Qcode::AcpRotSelSAT_L_C, Qcode::AcpRotSelSAT_R_C, Qcode::AcpRotSelSPKR_C, Qcode::AcpRotSelVolVA_C, Qcode::AcpRotSelVolAPP_C,
									Qcode::AcpRotSelVORADF_C, Qcode::AcpRotVBR_C, Qcode::AcpRotIM_C} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs115,{Qcode::AcpRotSelVHF_L_R, Qcode::AcpRotSelVHF_C_R, Qcode::AcpRotSelVHF_R_R,
									Qcode::AcpRotSelFLT_R, Qcode::AcpRotSelCAB_R,Qcode::AcpRotSelPA_R, Qcode::AcpRotSelHF_L_R,
									Qcode::AcpRotSelHF_R_R, Qcode::AcpRotSelSAT_L_R, Qcode::AcpRotSelSAT_R_R, Qcode::AcpRotSelSPKR_R, Qcode::AcpRotSelVolVA_R, Qcode::AcpRotSelVolAPP_R,
									Qcode::AcpRotSelVORADF_R, Qcode::AcpRotVBR_R, Qcode::AcpRotIM_R} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs117,{Qcode::TcasPanOff, Qcode::TcasPanOn, Qcode::TcasPanL,
									Qcode::TcasPanR, Qcode::TcasPanAb, Qcode::TcasPanNo, Qcode::TcasPanBe, Qcode::TcasPanStby,
									Qcode::TcasPanXpdr, Qcode::TcasPanTA, Qcode::TcasPanRATA} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs120,{Qcode::ElevatorStick, Qcode::AileronControlW, Qcode::RudderPedals} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs357,{Qcode::ToeBrakeLeft, Qcode::ToeBrakeRight} });

		internalvar_defs_.emplace_back(InternalVarStruct{ Qcode::Qs436,{Qcode::Tla1, Qcode::Tla2, Qcode::Tla3, Qcode::Tla4} });
	}

	auto VarPoolImpl::createInternalVarElements() -> void { // NOLINT
		// Qh
		ch_OnOffOnSwitch(Qcode::EcpAdfLCpOnL, "EcpAdfLCpOnL"s, Qcode::Qh13, 0);
		ch_OnOffOnSwitch(Qcode::EcpAdfLCpOnR, "EcpAdfLCpOnR"s, Qcode::Qh13, 1);
		ch_OnOffOnSwitch(Qcode::EcpAdfRCpOnL, "EcpAdfRCpOnL"s, Qcode::Qh14, 0);
		ch_OnOffOnSwitch(Qcode::EcpAdfRCpOnR, "EcpAdfRCpOnR"s, Qcode::Qh14, 1);

		ch_CollRotSwitchTerminal1(Qcode::EcpNdModeCpAp, "EcpNdModeCpAp"s, Qcode::Qh32, 0);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdModeCpVo, "EcpNdModeCpVo"s, Qcode::Qh32, 1);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdModeCpMa, "EcpNdModeCpMa"s, Qcode::Qh32, 2);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdModeCpPL, "EcpNdModeCpPL"s, Qcode::Qh32, 3);

		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeCp5, "EcpNdRangeCp5"s, Qcode::Qh33, 0);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeCp10, "EcpNdRangeCp10"s, Qcode::Qh33, 1);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeCp20, "EcpNdRangeCp20"s, Qcode::Qh33, 2);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeCp40, "EcpNdRangeCp40"s, Qcode::Qh33, 3);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeCp80, "EcpNdRangeCp80"s, Qcode::Qh33, 4);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeCp160, "EcpNdRangeCp160"s, Qcode::Qh33, 5);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeCp320, "EcpNdRangeCp320"s, Qcode::Qh33, 6);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeCp640, "EcpNdRangeCp640"s, Qcode::Qh33, 7);

		ch_OnOffOnSwitch(Qcode::EcpAdfLFoOnL, "EcpAdfLFoOnL"s, Qcode::Qh34, 0);
		ch_OnOffOnSwitch(Qcode::EcpAdfLFoOnR, "EcpAdfLFoOnR"s, Qcode::Qh34, 1);
		ch_OnOffOnSwitch(Qcode::EcpAdfRFoOnL, "EcpAdfRFoOnL"s, Qcode::Qh35, 0);
		ch_OnOffOnSwitch(Qcode::EcpAdfRFoOnR, "EcpAdfRFoOnR"s, Qcode::Qh35, 1);

		ch_CollRotSwitchTerminal1(Qcode::EcpNdModeFoAp, "EcpNdModeFoAp"s, Qcode::Qh53, 0);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdModeFoVo, "EcpNdModeFoVo"s, Qcode::Qh53, 1);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdModeFoMa, "EcpNdModeFoMa"s, Qcode::Qh53, 2);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdModeFoPL, "EcpNdModeFoPL"s, Qcode::Qh53, 3);

		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeFo5, "EcpNdRangeFo5"s, Qcode::Qh54, 0);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeFo10, "EcpNdRangeFo10"s, Qcode::Qh54, 1);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeFo20, "EcpNdRangeFo20"s, Qcode::Qh54, 2);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeFo40, "EcpNdRangeFo40"s, Qcode::Qh54, 3);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeFo80, "EcpNdRangeFo80"s, Qcode::Qh54, 4);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeFo160, "EcpNdRangeFo160"s, Qcode::Qh54, 5);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeFo320, "EcpNdRangeFo320"s, Qcode::Qh54, 6);
		ch_CollRotSwitchTerminal1(Qcode::EcpNdRangeFo640, "EcpNdRangeFo640"s, Qcode::Qh54, 7);

		ch_CollRotSwitchTerminal1(Qcode::McpBankLimAuto, "McpBankLimAuto"s, Qcode::Qh76, 0);
		ch_CollRotSwitchTerminal1(Qcode::McpBankLim5, "McpBankLim5"s, Qcode::Qh76, 1);
		ch_CollRotSwitchTerminal1(Qcode::McpBankLim10, "McpBankLim10"s, Qcode::Qh76, 2);
		ch_CollRotSwitchTerminal1(Qcode::McpBankLim15, "McpBankLim15"s, Qcode::Qh76, 3);
		ch_CollRotSwitchTerminal1(Qcode::McpBankLim20, "McpBankLim20"s, Qcode::Qh76, 4);
		ch_CollRotSwitchTerminal1(Qcode::McpBankLim25, "McpBankLim25"s, Qcode::Qh76, 5);

		ch_CollRotSwitchTerminal1(Qcode::SrceFltDirCpL, "SrceFltDirCpL"s, Qcode::Qh116, 0);
		ch_CollRotSwitchTerminal1(Qcode::SrceFltDirCpC, "SrceFltDirCpC"s, Qcode::Qh116, 1);
		ch_CollRotSwitchTerminal1(Qcode::SrceFltDirCpR, "SrceFltDirCpR"s, Qcode::Qh116, 2);

		ch_CollRotSwitchTerminal1(Qcode::SrceNavCpFL, "SrceNavCpFL"s, Qcode::Qh117, 0);
		ch_CollRotSwitchTerminal1(Qcode::SrceNavCpFR, "SrceNavCpFR"s, Qcode::Qh117, 1);
		ch_CollRotSwitchTerminal1(Qcode::SrceNavCpCL, "SrceNavCpCL"s, Qcode::Qh117, 2);
		ch_CollRotSwitchTerminal1(Qcode::SrceNavCpCC, "SrceNavCpCC"s, Qcode::Qh117, 3);

		ch_CollRotSwitchTerminal1(Qcode::SrceEiuCpL, "SrceEiuCpL"s, Qcode::Qh118, 0);
		ch_CollRotSwitchTerminal1(Qcode::SrceEiuCpA, "SrceEiuCpA"s, Qcode::Qh118, 1);
		ch_CollRotSwitchTerminal1(Qcode::SrceEiuCpC, "SrceEiuCpC"s, Qcode::Qh118, 2);
		ch_CollRotSwitchTerminal1(Qcode::SrceEiuCpR, "SrceEiuCpR"s, Qcode::Qh118, 3);

		ch_CollRotSwitchTerminal1(Qcode::SrceIrsCp0, "SrceIrsCp0"s, Qcode::Qh119, 0);
		ch_CollRotSwitchTerminal1(Qcode::SrceIrsCp1, "SrceIrsCp1"s, Qcode::Qh119, 1);
		ch_CollRotSwitchTerminal1(Qcode::SrceIrsCp2, "SrceIrsCp2"s, Qcode::Qh119, 2);
		ch_CollRotSwitchTerminal1(Qcode::SrceIrsCp3, "SrceIrsCp3"s, Qcode::Qh119, 3);

		ch_CollRotSwitchTerminal1(Qcode::SrceAdcCp0, "SrceAdcCp0"s, Qcode::Qh120, 0);
		ch_CollRotSwitchTerminal1(Qcode::SrceAdcCp1, "SrceAdcCp1"s, Qcode::Qh120, 1);
		ch_CollRotSwitchTerminal1(Qcode::SrceAdcCp2, "SrceAdcCp2"s, Qcode::Qh120, 2);
		ch_CollRotSwitchTerminal1(Qcode::SrceAdcCp3, "SrceAdcCp3"s, Qcode::Qh120, 3);

		ch_CollRotSwitchTerminal1(Qcode::SrceFltDirFoL, "SrceFltDirFoL"s, Qcode::Qh121, 0);
		ch_CollRotSwitchTerminal1(Qcode::SrceFltDirFoC, "SrceFltDirFoC"s, Qcode::Qh121, 1);
		ch_CollRotSwitchTerminal1(Qcode::SrceFltDirFoR, "SrceFltDirFoR"s, Qcode::Qh121, 2);

		ch_CollRotSwitchTerminal1(Qcode::SrceNavFoCC, "SrceNavFoCC"s, Qcode::Qh122, 0);
		ch_CollRotSwitchTerminal1(Qcode::SrceNavFoCR, "SrceNavFoCR"s, Qcode::Qh122, 1);
		ch_CollRotSwitchTerminal1(Qcode::SrceNavFoFL, "SrceNavFoFL"s, Qcode::Qh122, 2);
		ch_CollRotSwitchTerminal1(Qcode::SrceNavFoFR, "SrceNavFoFR"s, Qcode::Qh122, 3);

		ch_CollRotSwitchTerminal1(Qcode::SrceEiuFoL, "SrceEiuFoL"s, Qcode::Qh123, 0);
		ch_CollRotSwitchTerminal1(Qcode::SrceEiuFoC, "SrceEiuFoC"s, Qcode::Qh123, 1);
		ch_CollRotSwitchTerminal1(Qcode::SrceEiuFoA, "SrceEiuFoA"s, Qcode::Qh123, 2);
		ch_CollRotSwitchTerminal1(Qcode::SrceEiuFoR, "SrceEiuFoR"s, Qcode::Qh123, 3);

		ch_CollRotSwitchTerminal1(Qcode::SrceIrsFo0, "SrceIrsFo0"s, Qcode::Qh124, 0);
		ch_CollRotSwitchTerminal1(Qcode::SrceIrsFo1, "SrceIrsFo1"s, Qcode::Qh124, 1);
		ch_CollRotSwitchTerminal1(Qcode::SrceIrsFo2, "SrceIrsFo2"s, Qcode::Qh124, 2);
		ch_CollRotSwitchTerminal1(Qcode::SrceIrsFo3, "SrceIrsFo3"s, Qcode::Qh124, 3);

		ch_CollRotSwitchTerminal1(Qcode::SrceAdcFo0, "SrceAdcFo0"s, Qcode::Qh125, 0);
		ch_CollRotSwitchTerminal1(Qcode::SrceAdcFo1, "SrceAdcFo1"s, Qcode::Qh125, 1);
		ch_CollRotSwitchTerminal1(Qcode::SrceAdcFo2, "SrceAdcFo2"s, Qcode::Qh125, 2);
		ch_CollRotSwitchTerminal1(Qcode::SrceAdcFo3, "SrceAdcFo3"s, Qcode::Qh125, 3);

		ch_CollRotSwitchTerminal1(Qcode::IndbCrtCpEicas, "IndbCrtCpEicas"s, Qcode::Qh126, 0);
		ch_CollRotSwitchTerminal1(Qcode::IndbCrtCpNorm, "IndbCrtCpNorm"s, Qcode::Qh126, 1);
		ch_CollRotSwitchTerminal1(Qcode::IndbCrtCpPfd, "IndbCrtCpPfd"s, Qcode::Qh126, 2);

		ch_CollRotSwitchTerminal1(Qcode::LwrCrtCpIcasPri, "LwrCrtCpIcasPri"s, Qcode::Qh127, 0);
		ch_CollRotSwitchTerminal1(Qcode::LwrCrtCpNorm, "LwrCrtCpNorm"s, Qcode::Qh127, 1);
		ch_CollRotSwitchTerminal1(Qcode::LwrCrtCpNd, "LwrCrtCpNd"s, Qcode::Qh127, 2);

		ch_CollRotSwitchTerminal1(Qcode::IndbCrtFoPfd, "IndbCrtFoPfd"s, Qcode::Qh129, 0);
		ch_CollRotSwitchTerminal1(Qcode::IndbCrtFoNorm, "IndbCrtFoNorm"s, Qcode::Qh129, 1);
		ch_CollRotSwitchTerminal1(Qcode::IndbCrtFoEicas, "IndbCrtFoEicas"s, Qcode::Qh129, 2);

		ch_CollRotSwitchTerminal1(Qcode::LwrCrtFoNd, "LwrCrtFoNd"s, Qcode::Qh130, 0);
		ch_CollRotSwitchTerminal1(Qcode::LwrCrtFoNorm, "LwrCrtFoNorm"s, Qcode::Qh130, 1);
		ch_CollRotSwitchTerminal1(Qcode::LwrCrtFoIcasPri, "LwrCrtFoIcasPri"s, Qcode::Qh130, 2);

		ch_CollRotSwitchTerminal1(Qcode::EiuEicasL, "EiuEicasL"s, Qcode::Qh131, 0);
		ch_CollRotSwitchTerminal1(Qcode::EiuEicasA, "EiuEicasA"s, Qcode::Qh131, 1);
		ch_CollRotSwitchTerminal1(Qcode::EiuEicasC, "EiuEicasC"s, Qcode::Qh131, 2);
		ch_CollRotSwitchTerminal1(Qcode::EiuEicasR, "EiuEicasR"s, Qcode::Qh131, 3);

		ch_CollRotSwitchTerminal1(Qcode::AltnFlapsRet, "AltnFlapsRet"s, Qcode::Qh133, 0);
		ch_CollRotSwitchTerminal1(Qcode::AltnFlapsOff, "AltnFlapsOff"s, Qcode::Qh133, 1);
		ch_CollRotSwitchTerminal1(Qcode::AltnFlapsExt, "AltnFlapsExt"s, Qcode::Qh133, 2);

		ch_CollRotSwitchTerminal1(Qcode::StbyIlsSw0, "StbyIlsSw0"s, Qcode::Qh137, 0);
		ch_CollRotSwitchTerminal1(Qcode::StbyIlsSw1, "StbyIlsSw1"s, Qcode::Qh137, 1);
		ch_CollRotSwitchTerminal1(Qcode::StbyIlsSw2, "StbyIlsSw2"s, Qcode::Qh137, 2);

		ch_CollRotSwitchTerminal1(Qcode::GearLeverUp, "GearLeverUp"s, Qcode::Qh170, 1);
		ch_CollRotSwitchTerminal1(Qcode::GearLeverOff, "GearLeverOff"s, Qcode::Qh170, 2);
		ch_CollRotSwitchTerminal1(Qcode::GearLeverDn, "GearLeverDn"s, Qcode::Qh170, 3);

		ch_OnOffOnSwitch(Qcode::LtBeaconOnL, "LtBeaconOnL"s, Qcode::Qh178, 0);
		ch_OnOffOnSwitch(Qcode::LtBeaconOnR, "LtBeaconOnR"s, Qcode::Qh178, 1);

		ch_OnOffOnSwitch(Qcode::LtIndTestOnL, "LtIndTestOnL"s, Qcode::Qh183, 0);
		ch_OnOffOnSwitch(Qcode::LtIndTestOnR, "LtIndTestOnR"s, Qcode::Qh183, 1);

		ch_CollRotSwitchTerminal1(Qcode::HydDem10, "HydDem10"s, Qcode::Qh184, 0);
		ch_CollRotSwitchTerminal1(Qcode::HydDem11, "HydDem11"s, Qcode::Qh184, 1);
		ch_CollRotSwitchTerminal1(Qcode::HydDem12, "HydDem12"s, Qcode::Qh184, 2);
		ch_CollRotSwitchTerminal1(Qcode::HydDem13, "HydDem13"s, Qcode::Qh184, 3);

		ch_CollRotSwitchTerminal1(Qcode::HydDem2Off, "HydDem2Off"s, Qcode::Qh185, 1);
		ch_CollRotSwitchTerminal1(Qcode::HydDem2Auto, "HydDem2Auto"s, Qcode::Qh185, 2);
		ch_CollRotSwitchTerminal1(Qcode::HydDem2On, "HydDem2On"s, Qcode::Qh185, 3);

		ch_CollRotSwitchTerminal1(Qcode::HydDem3Off, "HydDem3Off"s, Qcode::Qh186, 1);
		ch_CollRotSwitchTerminal1(Qcode::HydDem3Auto, "HydDem3Auto"s, Qcode::Qh186, 2);
		ch_CollRotSwitchTerminal1(Qcode::HydDem3On, "HydDem3On"s, Qcode::Qh186, 3);

		ch_CollRotSwitchTerminal1(Qcode::HydDem4Aux, "HydDem4Aux"s, Qcode::Qh187, 0);
		ch_CollRotSwitchTerminal1(Qcode::HydDem4_1, "HydDem4_1"s, Qcode::Qh187, 1);
		ch_CollRotSwitchTerminal1(Qcode::HydDem4Auto, "HydDem4Auto"s, Qcode::Qh187, 2);
		ch_CollRotSwitchTerminal1(Qcode::HydDem4On, "HydDem4On"s, Qcode::Qh187, 3);

		ch_CollRotSwitchTerminal1(Qcode::StbyPowerOff, "StbyPowerOff"s, Qcode::Qh192, 0);
		ch_CollRotSwitchTerminal1(Qcode::StbyPowerAuto, "StbyPowerAuto"s, Qcode::Qh192, 1);
		ch_CollRotSwitchTerminal1(Qcode::StbyPowerBat, "StbyPowerBat"s, Qcode::Qh192, 2);

		ch_CollRotSwitchTerminal1(Qcode::ApuStartOff, "ApuStartOff"s, Qcode::Qh193, 0);
		ch_CollRotSwitchTerminal1(Qcode::ApuStartOn, "ApuStartOn"s, Qcode::Qh193, 1);
		ch_CollRotSwitchTerminal1(Qcode::ApuStartStart, "ApuStartStart"s, Qcode::Qh193, 2);

		ch_CollRotSwitchTerminal1(Qcode::IrsLOff, "IrsLOff"s, Qcode::Qh213, 0);
		ch_CollRotSwitchTerminal1(Qcode::IrsLAlign, "IrsLAlign"s, Qcode::Qh213, 1);
		ch_CollRotSwitchTerminal1(Qcode::IrsLNav, "IrsLNav"s, Qcode::Qh213, 2);
		ch_CollRotSwitchTerminal1(Qcode::IrsLAtt, "IrsLAtt"s, Qcode::Qh213, 3);
		ch_CollRotSwitchTerminal1(Qcode::IrsCOff, "IrsCOff"s, Qcode::Qh214, 0);
		ch_CollRotSwitchTerminal1(Qcode::IrsCAlign, "IrsCAlign"s, Qcode::Qh214, 1);
		ch_CollRotSwitchTerminal1(Qcode::IrsCNav, "IrsCNav"s, Qcode::Qh214, 2);
		ch_CollRotSwitchTerminal1(Qcode::IrsCAtt, "IrsCAtt"s, Qcode::Qh214, 3);
		ch_CollRotSwitchTerminal1(Qcode::IrsROff, "IrsROff"s, Qcode::Qh215, 0);
		ch_CollRotSwitchTerminal1(Qcode::IrsRAlign, "IrsRAlign"s, Qcode::Qh215, 1);
		ch_CollRotSwitchTerminal1(Qcode::IrsRNav, "IrsRNav"s, Qcode::Qh215, 2);
		ch_CollRotSwitchTerminal1(Qcode::IrsRAtt, "IrsRAtt"s, Qcode::Qh215, 3);

		ch_OnOffOnSwitch(Qcode::EltL, "EltL"s, Qcode::Qh224, 0);
		ch_OnOffOnSwitch(Qcode::EltR, "EltR"s, Qcode::Qh224, 1);

		ch_OnOffOnSwitch(Qcode::NaiTogg1OnL, "NaiTogg1OnL"s, Qcode::Qh229, 0);
		ch_OnOffOnSwitch(Qcode::NaiTogg1OnR, "NaiTogg1OnR"s, Qcode::Qh229, 1);
		ch_OnOffOnSwitch(Qcode::NaiTogg2OnL, "NaiTogg2OnL"s, Qcode::Qh230, 0);
		ch_OnOffOnSwitch(Qcode::NaiTogg2OnR, "NaiTogg2OnR"s, Qcode::Qh230, 1);
		ch_OnOffOnSwitch(Qcode::NaiTogg3OnL, "NaiTogg3OnL"s, Qcode::Qh231, 0);
		ch_OnOffOnSwitch(Qcode::NaiTogg3OnR, "NaiTogg3OnR"s, Qcode::Qh231, 1);
		ch_OnOffOnSwitch(Qcode::NaiTogg4OnL, "NaiTogg4OnL"s, Qcode::Qh232, 0);
		ch_OnOffOnSwitch(Qcode::NaiTogg4OnR, "NaiTogg4OnR"s, Qcode::Qh232, 1);

		ch_OnOffOnSwitch(Qcode::WaiToggOnL, "WaiToggOnL"s, Qcode::Qh234, 0);
		ch_OnOffOnSwitch(Qcode::WaiToggOnR, "WaiToggOnR"s, Qcode::Qh234, 1);

		ch_CollRotSwitchTerminal1(Qcode::WiperL0, "WiperL0"s, Qcode::Qh235, 0);
		ch_CollRotSwitchTerminal1(Qcode::WiperL1, "WiperL1"s, Qcode::Qh235, 1);
		ch_CollRotSwitchTerminal1(Qcode::WiperL2, "WiperL2"s, Qcode::Qh235, 2);
		ch_CollRotSwitchTerminal1(Qcode::WiperL3, "WiperL3"s, Qcode::Qh235, 3);

		ch_CollRotSwitchTerminal1(Qcode::WiperR0, "WiperR0"s, Qcode::Qh236, 0);
		ch_CollRotSwitchTerminal1(Qcode::WiperR1, "WiperR1"s, Qcode::Qh236, 1);
		ch_CollRotSwitchTerminal1(Qcode::WiperR2, "WiperR2"s, Qcode::Qh236, 2);
		ch_CollRotSwitchTerminal1(Qcode::WiperR3, "WiperR3"s, Qcode::Qh236, 3);

		ch_CollRotSwitchTerminal1(Qcode::IgnitSelStby1, "IgnitSelStby1"s, Qcode::Qh263, 0);
		ch_CollRotSwitchTerminal1(Qcode::IgnitSelStbyN, "IgnitSelStbyN"s, Qcode::Qh263, 1);
		ch_CollRotSwitchTerminal1(Qcode::IgnitSelStby2, "IgnitSelStby2"s, Qcode::Qh263, 2);

		ch_CollRotSwitchTerminal1(Qcode::IgnitSelAuto0, "IgnitSelAuto0"s, Qcode::Qh265, 0);
		ch_CollRotSwitchTerminal1(Qcode::IgnitSelAut1, "IgnitSelAut1"s, Qcode::Qh265, 1);
		ch_CollRotSwitchTerminal1(Qcode::IgnitSelAuto2, "IgnitSelAuto2"s, Qcode::Qh265, 2);
		ch_CollRotSwitchTerminal1(Qcode::IgnitSelAut3, "IgnitSelAut3"s, Qcode::Qh265, 3);

		ch_CollOnOffSwitch(Qcode::FireEng1Pulled, "FireEng1Pulled"s, Qcode::Qh275, 0);
		ch_CollOnOffSwitch(Qcode::FireEng1Override, "FireEng1Override"s, Qcode::Qh275, 5);
		ch_CollOnOffSwitch(Qcode::FireEng1M22, "FireEng1M22"s, Qcode::Qh275, 7);
		ch_CollOnOffSwitch(Qcode::FireEng1M15, "FireEng1M15"s, Qcode::Qh275, 8);
		ch_CollOnOffSwitch(Qcode::FireEng1P15, "FireEng1P15"s, Qcode::Qh275, 9);
		ch_CollOnOffSwitch(Qcode::FireEng1P22, "FireEng1P22"s, Qcode::Qh275, 10);

		ch_CollOnOffSwitch(Qcode::FireEng2Pulled, "FireEng2Pulled"s, Qcode::Qh276, 0);
		ch_CollOnOffSwitch(Qcode::FireEng2Override, "FireEng2Override"s, Qcode::Qh276, 5);
		ch_CollOnOffSwitch(Qcode::FireEng2M22, "FireEng2M22"s, Qcode::Qh276, 7);
		ch_CollOnOffSwitch(Qcode::FireEng2M15, "FireEng2M15"s, Qcode::Qh276, 8);
		ch_CollOnOffSwitch(Qcode::FireEng2P15, "FireEng2P15"s, Qcode::Qh276, 9);
		ch_CollOnOffSwitch(Qcode::FireEng2P22, "FireEng2P22"s, Qcode::Qh276, 10);

		ch_CollOnOffSwitch(Qcode::FireEng3Pulled, "FireEng3Pulled"s, Qcode::Qh277, 0);
		ch_CollOnOffSwitch(Qcode::FireEng3Override, "FireEng3Override"s, Qcode::Qh277, 5);
		ch_CollOnOffSwitch(Qcode::FireEng3M22, "FireEng3M22"s, Qcode::Qh277, 7);
		ch_CollOnOffSwitch(Qcode::FireEng3M15, "FireEng3M15"s, Qcode::Qh277, 8);
		ch_CollOnOffSwitch(Qcode::FireEng3P15, "FireEng3P15"s, Qcode::Qh277, 9);
		ch_CollOnOffSwitch(Qcode::FireEng3P22, "FireEng3P22"s, Qcode::Qh277, 10);

		ch_CollOnOffSwitch(Qcode::FireEng4Pulled, "FireEng4Pulled"s, Qcode::Qh278, 0);
		ch_CollOnOffSwitch(Qcode::FireEng4Override, "FireEng4Override"s, Qcode::Qh278, 5);
		ch_CollOnOffSwitch(Qcode::FireEng4M22, "FireEng4M22"s, Qcode::Qh278, 7);
		ch_CollOnOffSwitch(Qcode::FireEng4M15, "FireEng4M15"s, Qcode::Qh278, 8);
		ch_CollOnOffSwitch(Qcode::FireEng4P15, "FireEng4P15"s, Qcode::Qh278, 9);
		ch_CollOnOffSwitch(Qcode::FireEng4P22, "FireEng4P22"s, Qcode::Qh278, 10);

		ch_CollOnOffSwitch(Qcode::FireApuPulled, "FireApuPulled"s, Qcode::Qh279, 0);
		ch_CollOnOffSwitch(Qcode::FireApuOverride, "FireApuOverride"s, Qcode::Qh279, 5);
		ch_CollOnOffSwitch(Qcode::FireApuM22, "FireApuM22"s, Qcode::Qh279, 7);
		ch_CollOnOffSwitch(Qcode::FireApuM15, "FireApuM15"s, Qcode::Qh279, 8);
		ch_CollOnOffSwitch(Qcode::FireApuP15, "FireApuP15"s, Qcode::Qh279, 9);
		ch_CollOnOffSwitch(Qcode::FireApuP22, "FireApuP22"s, Qcode::Qh279, 10);

		ch_OnOffOnSwitch(Qcode::EmerLtL, "EmerLtL"s, Qcode::Qh286, 0);
		ch_OnOffOnSwitch(Qcode::EmerLtR, "EmerLtR"s, Qcode::Qh286, 1);

		ch_OnOffOnSwitch(Qcode::ObsAudioL, "ObsAudioL"s, Qcode::Qh288, 0);
		ch_OnOffOnSwitch(Qcode::ObsAudioR, "ObsAudioR"s, Qcode::Qh288, 1);

		ch_OnOffOnSwitch(Qcode::PassOxygenL, "PassOxygenL"s, Qcode::Qh293, 0);
		ch_OnOffOnSwitch(Qcode::PassOxygenR, "PassOxygenR"s, Qcode::Qh293, 1);

		ch_OnOffOnSwitch(Qcode::OutflowToggOnL, "OutflowToggOnL"s, Qcode::Qh300, 0);
		ch_OnOffOnSwitch(Qcode::OutflowToggOnR, "OutflowToggOnR"s, Qcode::Qh300, 1);

		ch_CollRotSwitchTerminal1(Qcode::CabAltContSelN, "CabAltContSelN"s, Qcode::Qh301, 0);
		ch_CollRotSwitchTerminal1(Qcode::CabAltContSelA, "CabAltContSelA"s, Qcode::Qh301, 1);
		ch_CollRotSwitchTerminal1(Qcode::CabAltContSelB, "CabAltContSelB"s, Qcode::Qh301, 2);

		ch_CollRotSwitchTerminal1(Qcode::EquipCoolingS, "EquipCoolingS"s, Qcode::Qh319, 0);
		ch_CollRotSwitchTerminal1(Qcode::EquipCoolingN, "EquipCoolingN"s, Qcode::Qh319, 1);
		ch_CollRotSwitchTerminal1(Qcode::EquipCoolingO, "EquipCoolingO"s, Qcode::Qh319, 2);

		ch_CollRotSwitchTerminal1(Qcode::PackSel1Off, "PackSel1Off"s, Qcode::Qh320, 0);
		ch_CollRotSwitchTerminal1(Qcode::PackSel1Norm, "PackSel1Norm"s, Qcode::Qh320, 1);
		ch_CollRotSwitchTerminal1(Qcode::PackSel1A, "PackSel1A"s, Qcode::Qh320, 2);
		ch_CollRotSwitchTerminal1(Qcode::PackSel1B, "PackSel1B"s, Qcode::Qh320, 3);

		ch_CollRotSwitchTerminal1(Qcode::PackSel2Off, "PackSel2Off"s, Qcode::Qh321, 0);
		ch_CollRotSwitchTerminal1(Qcode::PackSel2Norm, "PackSel2Norm"s, Qcode::Qh321, 1);
		ch_CollRotSwitchTerminal1(Qcode::PackSel2A, "PackSel2A"s, Qcode::Qh321, 2);
		ch_CollRotSwitchTerminal1(Qcode::PackSel2B, "PackSel2B"s, Qcode::Qh321, 3);

		ch_CollRotSwitchTerminal1(Qcode::PackSel3Off, "PackSel3Off"s, Qcode::Qh322, 0);
		ch_CollRotSwitchTerminal1(Qcode::PackSel3Norm, "PackSel3Norm"s, Qcode::Qh322, 1);
		ch_CollRotSwitchTerminal1(Qcode::PackSel3A, "PackSel3A"s, Qcode::Qh322, 2);
		ch_CollRotSwitchTerminal1(Qcode::PackSel3B, "PackSel3B"s, Qcode::Qh322, 3);

		ch_CollRotSwitchTerminal1(Qcode::CargoCondAirSelBOTH, "CargoCondAirSelBOTH"s, Qcode::Qh358, 0);
		ch_CollRotSwitchTerminal1(Qcode::CargoCondAirSelLA, "CargoCondAirSelLA"s, Qcode::Qh358, 1);
		ch_CollRotSwitchTerminal1(Qcode::CargoCondAirSelLF, "CargoCondAirSelLF"s, Qcode::Qh358, 2);
		ch_CollRotSwitchTerminal1(Qcode::CargoCondAirSelOFF, "CargoCondAirSelOFF"s, Qcode::Qh358, 3);
		ch_CollRotSwitchTerminal1(Qcode::CargoCondAirSelHF, "CargoCondAirSelHF"s, Qcode::Qh358, 4);
		ch_CollRotSwitchTerminal1(Qcode::CargoCondAirSelHA, "CargoCondAirSelHA"s, Qcode::Qh358, 5);

		ch_CollMomActionSwitch(Qcode::ThrustDiscLeft, "ThrustDiscLeft"s, Qcode::Qh386, 0);
		ch_CollMomActionSwitch(Qcode::ThrustDiscRight, "ThrustDiscRight"s, Qcode::Qh386, 1);

		ch_CollMomActionSwitch(Qcode::ThrustTOGALeft, "ThrustTOGALeft"s, Qcode::Qh387, 0);
		ch_CollMomActionSwitch(Qcode::ThrustTOGARight, "ThrustTOGARight"s, Qcode::Qh387, 1);

		ch_OnOffOnSwitch(Qcode::StabTrimCut2On, "StabTrimCut2On"s, Qcode::Qh390, 0);
		ch_OnOffOnSwitch(Qcode::StabTrimCut2Off, "StabTrimCut2Off"s, Qcode::Qh390, 1);
		ch_OnOffOnSwitch(Qcode::StabTrimCut3On, "StabTrimCut3On"s, Qcode::Qh391, 0);
		ch_OnOffOnSwitch(Qcode::StabTrimCut3Off, "StabTrimCut3Off"s, Qcode::Qh391, 1);

		ch_OnOffOnSwitch(Qcode::StabTrimCpOnL, "StabTrimCpOnL"s, Qcode::Qh398, 0);
		ch_OnOffOnSwitch(Qcode::StabTrimCpOnR, "StabTrimCpOnR"s, Qcode::Qh398, 1);

		ch_OnOffOnSwitch(Qcode::StabTrimCFoOnL, "StabTrimCFoOnL"s, Qcode::Qh399, 0);
		ch_OnOffOnSwitch(Qcode::StabTrimFoOnR, "StabTrimFoOnR"s, Qcode::Qh399, 1);

		ch_CollRotSwitchTerminal1(Qcode::NoSmokingOff, "NoSmokingOff"s, Qcode::Qh420, 0);
		ch_CollRotSwitchTerminal1(Qcode::NoSmokingAuto, "NoSmokingAuto"s, Qcode::Qh420, 1);
		ch_CollRotSwitchTerminal1(Qcode::NoSmokingOn, "NoSmokingOn"s, Qcode::Qh420, 2);

		ch_CollRotSwitchTerminal1(Qcode::SeatbeltsOff, "SeatbeltsOff"s, Qcode::Qh421, 0);
		ch_CollRotSwitchTerminal1(Qcode::SeatbeltsAuto, "SeatbeltsAuto"s, Qcode::Qh421, 1);
		ch_CollRotSwitchTerminal1(Qcode::SeatbeltsOn, "SeatbeltsOn"s, Qcode::Qh421, 2);
		ch_OnOffOnSwitch(Qcode::EvacArmL, "EvacArmL"s, Qcode::Qh422, 0);
		ch_OnOffOnSwitch(Qcode::EvacArmR, "EvacArmR"s, Qcode::Qh422, 1);
		ch_CollMomActionSwitch(Qcode::EvacHornPush, "EvacHornPush"s, Qcode::Qh423, 0);
		ch_CollOnOffSwitch(Qcode::EvacTestPress, "EvacTestPress"s, Qcode::Qh423, 1);
		ch_CollOnOffSwitch(Qcode::EvacSigPilot, "EvacSigPilot"s, Qcode::Qh424, 0);
		ch_CollOnOffSwitch(Qcode::EvacSigFA, "EvacSigFA"s, Qcode::Qh424, 1);

		// Qs
		ch_CollOnOffSwitch(Qcode::P71A_01, "P71A_01"s, Qcode::Qs4, 0);
		ch_CollOnOffSwitch(Qcode::P71A_02, "P71A_02"s, Qcode::Qs4, 1);
		ch_CollOnOffSwitch(Qcode::P71A_03, "P71A_03"s, Qcode::Qs4, 2);
		ch_CollOnOffSwitch(Qcode::P71A_04, "P71A_04"s, Qcode::Qs4, 3);
		ch_CollOnOffSwitch(Qcode::P71A_05, "P71A_05"s, Qcode::Qs4, 4);
		ch_CollOnOffSwitch(Qcode::P71A_06, "P71A_06"s, Qcode::Qs4, 5);
		ch_CollOnOffSwitch(Qcode::P71A_07, "P71A_07"s, Qcode::Qs4, 6);
		ch_CollOnOffSwitch(Qcode::P71A_08, "P71A_08"s, Qcode::Qs4, 7);
		ch_CollOnOffSwitch(Qcode::P71A_09, "P71A_09"s, Qcode::Qs4, 8);
		ch_CollOnOffSwitch(Qcode::P71A_10, "P71A_10"s, Qcode::Qs4, 9);
		ch_CollOnOffSwitch(Qcode::P71A_11, "P71A_11"s, Qcode::Qs4, 10);
		ch_CollOnOffSwitch(Qcode::P71A_12, "P71A_12"s, Qcode::Qs4, 11);
		ch_CollOnOffSwitch(Qcode::P71A_13, "P71A_13"s, Qcode::Qs4, 12);

		ch_CollOnOffSwitch(Qcode::P71B_01, "P71B_01"s, Qcode::Qs5, 0);
		ch_CollOnOffSwitch(Qcode::P71B_02, "P71B_02"s, Qcode::Qs5, 1);
		ch_CollOnOffSwitch(Qcode::P71B_03, "P71B_03"s, Qcode::Qs5, 2);
		ch_CollOnOffSwitch(Qcode::P71B_04, "P71B_04"s, Qcode::Qs5, 3);
		ch_CollOnOffSwitch(Qcode::P71B_05, "P71B_05"s, Qcode::Qs5, 4);
		ch_CollOnOffSwitch(Qcode::P71B_06, "P71B_06"s, Qcode::Qs5, 5);
		ch_CollOnOffSwitch(Qcode::P71B_07, "P71B_07"s, Qcode::Qs5, 6);
		ch_CollOnOffSwitch(Qcode::P71B_08, "P71B_08"s, Qcode::Qs5, 7);
		ch_CollOnOffSwitch(Qcode::P71B_09, "P71B_09"s, Qcode::Qs5, 8);
		ch_CollOnOffSwitch(Qcode::P71B_10, "P71B_10"s, Qcode::Qs5, 9);
		ch_CollOnOffSwitch(Qcode::P71B_11, "P71B_11"s, Qcode::Qs5, 10);
		ch_CollOnOffSwitch(Qcode::P71B_12, "P71B_12"s, Qcode::Qs5, 11);
		ch_CollOnOffSwitch(Qcode::P71B_13, "P71B_13"s, Qcode::Qs5, 12);

		ch_CollOnOffSwitch(Qcode::P71C_01, "P71C_01"s, Qcode::Qs6, 0);
		ch_CollOnOffSwitch(Qcode::P71C_02, "P71C_02"s, Qcode::Qs6, 1);
		ch_CollOnOffSwitch(Qcode::P71C_03, "P71C_03"s, Qcode::Qs6, 2);
		ch_CollOnOffSwitch(Qcode::P71C_04, "P71C_04"s, Qcode::Qs6, 3);
		ch_CollOnOffSwitch(Qcode::P71C_05, "P71C_05"s, Qcode::Qs6, 4);
		ch_CollOnOffSwitch(Qcode::P71C_06, "P71C_06"s, Qcode::Qs6, 5);
		ch_CollOnOffSwitch(Qcode::P71C_07, "P71C_07"s, Qcode::Qs6, 6);
		ch_CollOnOffSwitch(Qcode::P71C_08, "P71C_08"s, Qcode::Qs6, 7);
		ch_CollOnOffSwitch(Qcode::P71C_09, "P71C_09"s, Qcode::Qs6, 8);
		ch_CollOnOffSwitch(Qcode::P71C_10, "P71C_10"s, Qcode::Qs6, 9);
		ch_CollOnOffSwitch(Qcode::P71C_11, "P71C_11"s, Qcode::Qs6, 10);
		ch_CollOnOffSwitch(Qcode::P71C_12, "P71C_12"s, Qcode::Qs6, 11);
		ch_CollOnOffSwitch(Qcode::P71C_13, "P71C_13"s, Qcode::Qs6, 12);

		ch_CollOnOffSwitch(Qcode::P71D_01, "P71D_01"s, Qcode::Qs7, 0);
		ch_CollOnOffSwitch(Qcode::P71D_02, "P71D_02"s, Qcode::Qs7, 1);
		ch_CollOnOffSwitch(Qcode::P71D_03, "P71D_03"s, Qcode::Qs7, 2);
		ch_CollOnOffSwitch(Qcode::P71D_04, "P71D_04"s, Qcode::Qs7, 3);
		ch_CollOnOffSwitch(Qcode::P71D_05, "P71D_05"s, Qcode::Qs7, 4);
		ch_CollOnOffSwitch(Qcode::P71D_06, "P71D_06"s, Qcode::Qs7, 5);
		ch_CollOnOffSwitch(Qcode::P71D_07, "P71D_07"s, Qcode::Qs7, 6);
		ch_CollOnOffSwitch(Qcode::P71D_08, "P71D_08"s, Qcode::Qs7, 7);
		ch_CollOnOffSwitch(Qcode::P71D_09, "P71D_09"s, Qcode::Qs7, 8);
		ch_CollOnOffSwitch(Qcode::P71D_10, "P71D_10"s, Qcode::Qs7, 9);
		ch_CollOnOffSwitch(Qcode::P71D_11, "P71D_11"s, Qcode::Qs7, 10);
		ch_CollOnOffSwitch(Qcode::P71D_12, "P71D_12"s, Qcode::Qs7, 11);
		ch_CollOnOffSwitch(Qcode::P71D_13, "P71D_13"s, Qcode::Qs7, 12);

		ch_CollOnOffSwitch(Qcode::P71E_01, "P71E_01"s, Qcode::Qs8, 0);
		ch_CollOnOffSwitch(Qcode::P71E_02, "P71E_02"s, Qcode::Qs8, 1);
		ch_CollOnOffSwitch(Qcode::P71E_03, "P71E_03"s, Qcode::Qs8, 2);
		ch_CollOnOffSwitch(Qcode::P71E_04, "P71E_04"s, Qcode::Qs8, 3);
		ch_CollOnOffSwitch(Qcode::P71E_05, "P71E_05"s, Qcode::Qs8, 4);
		ch_CollOnOffSwitch(Qcode::P71E_06, "P71E_06"s, Qcode::Qs8, 5);
		ch_CollOnOffSwitch(Qcode::P71E_07, "P71E_07"s, Qcode::Qs8, 6);
		ch_CollOnOffSwitch(Qcode::P71E_08, "P71E_08"s, Qcode::Qs8, 7);
		ch_CollOnOffSwitch(Qcode::P71E_09, "P71E_09"s, Qcode::Qs8, 8);
		ch_CollOnOffSwitch(Qcode::P71E_10, "P71E_10"s, Qcode::Qs8, 9);
		ch_CollOnOffSwitch(Qcode::P71E_11, "P71E_11"s, Qcode::Qs8, 10);
		ch_CollOnOffSwitch(Qcode::P71E_12, "P71E_12"s, Qcode::Qs8, 11);
		ch_CollOnOffSwitch(Qcode::P71E_13, "P71E_13"s, Qcode::Qs8, 12);

		ch_CollOnOffSwitch(Qcode::P71F_01, "P71F_01"s, Qcode::Qs9, 0);
		ch_CollOnOffSwitch(Qcode::P71F_02, "P71F_02"s, Qcode::Qs9, 1);
		ch_CollOnOffSwitch(Qcode::P71F_03, "P71F_03"s, Qcode::Qs9, 2);
		ch_CollOnOffSwitch(Qcode::P71F_04, "P71F_04"s, Qcode::Qs9, 3);
		ch_CollOnOffSwitch(Qcode::P71F_05, "P71F_05"s, Qcode::Qs9, 4);
		ch_CollOnOffSwitch(Qcode::P71F_06, "P71F_06"s, Qcode::Qs9, 5);
		ch_CollOnOffSwitch(Qcode::P71F_07, "P71F_07"s, Qcode::Qs9, 6);
		ch_CollOnOffSwitch(Qcode::P71F_08, "P71F_08"s, Qcode::Qs9, 7);
		ch_CollOnOffSwitch(Qcode::P71F_09, "P71F_09"s, Qcode::Qs9, 8);
		ch_CollOnOffSwitch(Qcode::P71F_10, "P71F_10"s, Qcode::Qs9, 9);
		ch_CollOnOffSwitch(Qcode::P71F_11, "P71F_11"s, Qcode::Qs9, 10);
		ch_CollOnOffSwitch(Qcode::P71F_12, "P71F_12"s, Qcode::Qs9, 11);
		ch_CollOnOffSwitch(Qcode::P71F_13, "P71F_13"s, Qcode::Qs9, 12);

		ch_CollOnOffSwitch(Qcode::P71G_01, "P71G_01"s, Qcode::Qs10, 0);
		ch_CollOnOffSwitch(Qcode::P71G_02, "P71G_02"s, Qcode::Qs10, 1);
		ch_CollOnOffSwitch(Qcode::P71G_03, "P71G_03"s, Qcode::Qs10, 2);
		ch_CollOnOffSwitch(Qcode::P71G_04, "P71G_04"s, Qcode::Qs10, 3);
		ch_CollOnOffSwitch(Qcode::P71G_05, "P71G_05"s, Qcode::Qs10, 4);
		ch_CollOnOffSwitch(Qcode::P71G_06, "P71G_06"s, Qcode::Qs10, 5);
		ch_CollOnOffSwitch(Qcode::P71G_07, "P71G_07"s, Qcode::Qs10, 6);
		ch_CollOnOffSwitch(Qcode::P71G_08, "P71G_08"s, Qcode::Qs10, 7);
		ch_CollOnOffSwitch(Qcode::P71G_09, "P71G_09"s, Qcode::Qs10, 8);
		ch_CollOnOffSwitch(Qcode::P71G_10, "P71G_10"s, Qcode::Qs10, 9);
		ch_CollOnOffSwitch(Qcode::P71G_11, "P71G_11"s, Qcode::Qs10, 10);
		ch_CollOnOffSwitch(Qcode::P71G_12, "P71G_12"s, Qcode::Qs10, 11);
		ch_CollOnOffSwitch(Qcode::P71G_13, "P71G_13"s, Qcode::Qs10, 12);

		ch_CollOnOffSwitch(Qcode::P71H_01, "P71H_01"s, Qcode::Qs11, 0);
		ch_CollOnOffSwitch(Qcode::P71H_02, "P71H_02"s, Qcode::Qs11, 1);
		ch_CollOnOffSwitch(Qcode::P71H_03, "P71H_03"s, Qcode::Qs11, 2);
		ch_CollOnOffSwitch(Qcode::P71H_04, "P71H_04"s, Qcode::Qs11, 3);
		ch_CollOnOffSwitch(Qcode::P71H_05, "P71H_05"s, Qcode::Qs11, 4);
		ch_CollOnOffSwitch(Qcode::P71H_06, "P71H_06"s, Qcode::Qs11, 5);
		ch_CollOnOffSwitch(Qcode::P71H_07, "P71H_07"s, Qcode::Qs11, 6);
		ch_CollOnOffSwitch(Qcode::P71H_08, "P71H_08"s, Qcode::Qs11, 7);
		ch_CollOnOffSwitch(Qcode::P71H_09, "P71H_09"s, Qcode::Qs11, 8);
		ch_CollOnOffSwitch(Qcode::P71H_10, "P71H_10"s, Qcode::Qs11, 9);
		ch_CollOnOffSwitch(Qcode::P71H_11, "P71H_11"s, Qcode::Qs11, 10);
		ch_CollOnOffSwitch(Qcode::P71H_12, "P71H_12"s, Qcode::Qs11, 11);
		ch_CollOnOffSwitch(Qcode::P71H_13, "P71H_13"s, Qcode::Qs11, 12);

		ch_CollOnOffSwitch(Qcode::P71J_01, "P71J_01"s, Qcode::Qs12, 0);
		ch_CollOnOffSwitch(Qcode::P71J_02, "P71J_02"s, Qcode::Qs12, 1);
		ch_CollOnOffSwitch(Qcode::P71J_03, "P71J_03"s, Qcode::Qs12, 2);
		ch_CollOnOffSwitch(Qcode::P71J_04, "P71J_04"s, Qcode::Qs12, 3);
		ch_CollOnOffSwitch(Qcode::P71J_05, "P71J_05"s, Qcode::Qs12, 4);
		ch_CollOnOffSwitch(Qcode::P71J_06, "P71J_06"s, Qcode::Qs12, 5);
		ch_CollOnOffSwitch(Qcode::P71J_07, "P71J_07"s, Qcode::Qs12, 6);
		ch_CollOnOffSwitch(Qcode::P71J_08, "P71J_08"s, Qcode::Qs12, 7);
		ch_CollOnOffSwitch(Qcode::P71J_09, "P71J_09"s, Qcode::Qs12, 8);
		ch_CollOnOffSwitch(Qcode::P71J_10, "P71J_10"s, Qcode::Qs12, 9);
		ch_CollOnOffSwitch(Qcode::P71J_11, "P71J_11"s, Qcode::Qs12, 10);
		ch_CollOnOffSwitch(Qcode::P71J_12, "P71J_12"s, Qcode::Qs12, 11);
		ch_CollOnOffSwitch(Qcode::P71J_13, "P71J_13"s, Qcode::Qs12, 12);

		ch_CollOnOffSwitch(Qcode::P72A_14, "P72A_14"s, Qcode::Qs13, 0);
		ch_CollOnOffSwitch(Qcode::P72A_15, "P72A_15"s, Qcode::Qs13, 1);
		ch_CollOnOffSwitch(Qcode::P72A_16, "P72A_16"s, Qcode::Qs13, 2);
		ch_CollOnOffSwitch(Qcode::P72A_17, "P72A_17"s, Qcode::Qs13, 3);
		ch_CollOnOffSwitch(Qcode::P72A_18, "P72A_18"s, Qcode::Qs13, 4);
		ch_CollOnOffSwitch(Qcode::P72A_19, "P72A_19"s, Qcode::Qs13, 5);
		ch_CollOnOffSwitch(Qcode::P72A_20, "P72A_20"s, Qcode::Qs13, 6);
		ch_CollOnOffSwitch(Qcode::P72A_21, "P72A_21"s, Qcode::Qs13, 7);
		ch_CollOnOffSwitch(Qcode::P72A_22, "P72A_22"s, Qcode::Qs13, 8);
		ch_CollOnOffSwitch(Qcode::P72A_23, "P72A_23"s, Qcode::Qs13, 9);
		ch_CollOnOffSwitch(Qcode::P72A_24, "P72A_24"s, Qcode::Qs13, 10);
		ch_CollOnOffSwitch(Qcode::P72A_25, "P72A_25"s, Qcode::Qs13, 11);
		ch_CollOnOffSwitch(Qcode::P72A_26, "P72A_26"s, Qcode::Qs13, 12);

		ch_CollOnOffSwitch(Qcode::P72B_14, "P72B_14"s, Qcode::Qs14, 0);
		ch_CollOnOffSwitch(Qcode::P72B_15, "P72B_15"s, Qcode::Qs14, 1);
		ch_CollOnOffSwitch(Qcode::P72B_16, "P72B_16"s, Qcode::Qs14, 2);
		ch_CollOnOffSwitch(Qcode::P72B_17, "P72B_17"s, Qcode::Qs14, 3);
		ch_CollOnOffSwitch(Qcode::P72B_18, "P72B_18"s, Qcode::Qs14, 4);
		ch_CollOnOffSwitch(Qcode::P72B_19, "P72B_19"s, Qcode::Qs14, 5);
		ch_CollOnOffSwitch(Qcode::P72B_20, "P72B_20"s, Qcode::Qs14, 6);
		ch_CollOnOffSwitch(Qcode::P72B_21, "P72B_21"s, Qcode::Qs14, 7);
		ch_CollOnOffSwitch(Qcode::P72B_22, "P72B_22"s, Qcode::Qs14, 8);
		ch_CollOnOffSwitch(Qcode::P72B_23, "P72B_23"s, Qcode::Qs14, 9);
		ch_CollOnOffSwitch(Qcode::P72B_24, "P72B_24"s, Qcode::Qs14, 10);
		ch_CollOnOffSwitch(Qcode::P72B_25, "P72B_25"s, Qcode::Qs14, 11);
		ch_CollOnOffSwitch(Qcode::P72B_26, "P72B_26"s, Qcode::Qs14, 12);

		ch_CollOnOffSwitch(Qcode::P72C_14, "P72C_14"s, Qcode::Qs15, 0);
		ch_CollOnOffSwitch(Qcode::P72C_15, "P72C_15"s, Qcode::Qs15, 1);
		ch_CollOnOffSwitch(Qcode::P72C_16, "P72C_16"s, Qcode::Qs15, 2);
		ch_CollOnOffSwitch(Qcode::P72C_17, "P72C_17"s, Qcode::Qs15, 3);
		ch_CollOnOffSwitch(Qcode::P72C_18, "P72C_18"s, Qcode::Qs15, 4);
		ch_CollOnOffSwitch(Qcode::P72C_19, "P72C_19"s, Qcode::Qs15, 5);
		ch_CollOnOffSwitch(Qcode::P72C_20, "P72C_20"s, Qcode::Qs15, 6);
		ch_CollOnOffSwitch(Qcode::P72C_21, "P72C_21"s, Qcode::Qs15, 7);
		ch_CollOnOffSwitch(Qcode::P72C_22, "P72C_22"s, Qcode::Qs15, 8);
		ch_CollOnOffSwitch(Qcode::P72C_23, "P72C_23"s, Qcode::Qs15, 9);
		ch_CollOnOffSwitch(Qcode::P72C_24, "P72C_24"s, Qcode::Qs15, 10);
		ch_CollOnOffSwitch(Qcode::P72C_25, "P72C_25"s, Qcode::Qs15, 11);
		ch_CollOnOffSwitch(Qcode::P72C_26, "P72C_26"s, Qcode::Qs15, 12);

		ch_CollOnOffSwitch(Qcode::P72D_14, "P72D_14"s, Qcode::Qs16, 0);
		ch_CollOnOffSwitch(Qcode::P72D_15, "P72D_15"s, Qcode::Qs16, 1);
		ch_CollOnOffSwitch(Qcode::P72D_16, "P72D_16"s, Qcode::Qs16, 2);
		ch_CollOnOffSwitch(Qcode::P72D_17, "P72D_17"s, Qcode::Qs16, 3);
		ch_CollOnOffSwitch(Qcode::P72D_18, "P72D_18"s, Qcode::Qs16, 4);
		ch_CollOnOffSwitch(Qcode::P72D_19, "P72D_19"s, Qcode::Qs16, 5);
		ch_CollOnOffSwitch(Qcode::P72D_20, "P72D_20"s, Qcode::Qs16, 6);
		ch_CollOnOffSwitch(Qcode::P72D_21, "P72D_21"s, Qcode::Qs16, 7);
		ch_CollOnOffSwitch(Qcode::P72D_22, "P72D_22"s, Qcode::Qs16, 8);
		ch_CollOnOffSwitch(Qcode::P72D_23, "P72D_23"s, Qcode::Qs16, 9);
		ch_CollOnOffSwitch(Qcode::P72D_24, "P72D_24"s, Qcode::Qs16, 10);
		ch_CollOnOffSwitch(Qcode::P72D_25, "P72D_25"s, Qcode::Qs16, 11);
		ch_CollOnOffSwitch(Qcode::P72D_26, "P72D_26"s, Qcode::Qs16, 12);

		ch_CollOnOffSwitch(Qcode::P72E_14, "P72E_14"s, Qcode::Qs17, 0);
		ch_CollOnOffSwitch(Qcode::P72E_15, "P72E_15"s, Qcode::Qs17, 1);
		ch_CollOnOffSwitch(Qcode::P72E_16, "P72E_16"s, Qcode::Qs17, 2);
		ch_CollOnOffSwitch(Qcode::P72E_17, "P72E_17"s, Qcode::Qs17, 3);
		ch_CollOnOffSwitch(Qcode::P72E_18, "P72E_18"s, Qcode::Qs17, 4);
		ch_CollOnOffSwitch(Qcode::P72E_19, "P72E_19"s, Qcode::Qs17, 5);
		ch_CollOnOffSwitch(Qcode::P72E_20, "P72E_20"s, Qcode::Qs17, 6);
		ch_CollOnOffSwitch(Qcode::P72E_21, "P72E_21"s, Qcode::Qs17, 7);
		ch_CollOnOffSwitch(Qcode::P72E_22, "P72E_22"s, Qcode::Qs17, 8);
		ch_CollOnOffSwitch(Qcode::P72E_23, "P72E_23"s, Qcode::Qs17, 9);
		ch_CollOnOffSwitch(Qcode::P72E_24, "P72E_24"s, Qcode::Qs17, 10);
		ch_CollOnOffSwitch(Qcode::P72E_25, "P72E_25"s, Qcode::Qs17, 11);
		ch_CollOnOffSwitch(Qcode::P72E_26, "P72E_26"s, Qcode::Qs17, 12);

		ch_CollOnOffSwitch(Qcode::P72F_14, "P72F_14"s, Qcode::Qs18, 0);
		ch_CollOnOffSwitch(Qcode::P72F_15, "P72F_15"s, Qcode::Qs18, 1);
		ch_CollOnOffSwitch(Qcode::P72F_16, "P72F_16"s, Qcode::Qs18, 2);
		ch_CollOnOffSwitch(Qcode::P72F_17, "P72F_17"s, Qcode::Qs18, 3);
		ch_CollOnOffSwitch(Qcode::P72F_18, "P72F_18"s, Qcode::Qs18, 4);
		ch_CollOnOffSwitch(Qcode::P72F_19, "P72F_19"s, Qcode::Qs18, 5);
		ch_CollOnOffSwitch(Qcode::P72F_20, "P72F_20"s, Qcode::Qs18, 6);
		ch_CollOnOffSwitch(Qcode::P72F_21, "P72F_21"s, Qcode::Qs18, 7);
		ch_CollOnOffSwitch(Qcode::P72F_22, "P72F_22"s, Qcode::Qs18, 8);
		ch_CollOnOffSwitch(Qcode::P72F_23, "P72F_23"s, Qcode::Qs18, 9);
		ch_CollOnOffSwitch(Qcode::P72F_24, "P72F_24"s, Qcode::Qs18, 10);
		ch_CollOnOffSwitch(Qcode::P72F_25, "P72F_25"s, Qcode::Qs18, 11);
		ch_CollOnOffSwitch(Qcode::P72F_26, "P72F_26"s, Qcode::Qs18, 12);

		ch_CollOnOffSwitch(Qcode::P72G_14, "P72G_14"s, Qcode::Qs19, 0);
		ch_CollOnOffSwitch(Qcode::P72G_15, "P72G_15"s, Qcode::Qs19, 1);
		ch_CollOnOffSwitch(Qcode::P72G_16, "P72G_16"s, Qcode::Qs19, 2);
		ch_CollOnOffSwitch(Qcode::P72G_17, "P72G_17"s, Qcode::Qs19, 3);
		ch_CollOnOffSwitch(Qcode::P72G_18, "P72G_18"s, Qcode::Qs19, 4);
		ch_CollOnOffSwitch(Qcode::P72G_19, "P72G_19"s, Qcode::Qs19, 5);
		ch_CollOnOffSwitch(Qcode::P72G_20, "P72G_20"s, Qcode::Qs19, 6);
		ch_CollOnOffSwitch(Qcode::P72G_21, "P72G_21"s, Qcode::Qs19, 7);
		ch_CollOnOffSwitch(Qcode::P72G_22, "P72G_22"s, Qcode::Qs19, 8);
		ch_CollOnOffSwitch(Qcode::P72G_23, "P72G_23"s, Qcode::Qs19, 9);
		ch_CollOnOffSwitch(Qcode::P72G_24, "P72G_24"s, Qcode::Qs19, 10);
		ch_CollOnOffSwitch(Qcode::P72G_25, "P72G_25"s, Qcode::Qs19, 11);
		ch_CollOnOffSwitch(Qcode::P72G_26, "P72G_26"s, Qcode::Qs19, 12);

		ch_CollOnOffSwitch(Qcode::P72H_14, "P72H_14"s, Qcode::Qs20, 0);
		ch_CollOnOffSwitch(Qcode::P72H_15, "P72H_15"s, Qcode::Qs20, 1);
		ch_CollOnOffSwitch(Qcode::P72H_16, "P72H_16"s, Qcode::Qs20, 2);
		ch_CollOnOffSwitch(Qcode::P72H_17, "P72H_17"s, Qcode::Qs20, 3);
		ch_CollOnOffSwitch(Qcode::P72H_18, "P72H_18"s, Qcode::Qs20, 4);
		ch_CollOnOffSwitch(Qcode::P72H_19, "P72H_19"s, Qcode::Qs20, 5);
		ch_CollOnOffSwitch(Qcode::P72H_20, "P72H_20"s, Qcode::Qs20, 6);
		ch_CollOnOffSwitch(Qcode::P72H_21, "P72H_21"s, Qcode::Qs20, 7);
		ch_CollOnOffSwitch(Qcode::P72H_22, "P72H_22"s, Qcode::Qs20, 8);
		ch_CollOnOffSwitch(Qcode::P72H_23, "P72H_23"s, Qcode::Qs20, 9);
		ch_CollOnOffSwitch(Qcode::P72H_24, "P72H_24"s, Qcode::Qs20, 10);
		ch_CollOnOffSwitch(Qcode::P72H_25, "P72H_25"s, Qcode::Qs20, 11);
		ch_CollOnOffSwitch(Qcode::P72H_26, "P72H_26"s, Qcode::Qs20, 12);

		ch_CollOnOffSwitch(Qcode::P72J_14, "P72J_14"s, Qcode::Qs21, 0);
		ch_CollOnOffSwitch(Qcode::P72J_15, "P72J_15"s, Qcode::Qs21, 1);
		ch_CollOnOffSwitch(Qcode::P72J_16, "P72J_16"s, Qcode::Qs21, 2);
		ch_CollOnOffSwitch(Qcode::P72J_17, "P72J_17"s, Qcode::Qs21, 3);
		ch_CollOnOffSwitch(Qcode::P72J_18, "P72J_18"s, Qcode::Qs21, 4);
		ch_CollOnOffSwitch(Qcode::P72J_19, "P72J_19"s, Qcode::Qs21, 5);
		ch_CollOnOffSwitch(Qcode::P72J_20, "P72J_20"s, Qcode::Qs21, 6);
		ch_CollOnOffSwitch(Qcode::P72J_21, "P72J_21"s, Qcode::Qs21, 7);
		ch_CollOnOffSwitch(Qcode::P72J_22, "P72J_22"s, Qcode::Qs21, 8);
		ch_CollOnOffSwitch(Qcode::P72J_23, "P72J_23"s, Qcode::Qs21, 9);
		ch_CollOnOffSwitch(Qcode::P72J_24, "P72J_24"s, Qcode::Qs21, 10);
		ch_CollOnOffSwitch(Qcode::P72J_25, "P72J_25"s, Qcode::Qs21, 11);
		ch_CollOnOffSwitch(Qcode::P72J_26, "P72J_26"s, Qcode::Qs21, 12);

		ch_CollOnOffSwitch(Qcode::P61C_01, "P61C_01"s, Qcode::Qs22, 0);
		ch_CollOnOffSwitch(Qcode::P61C_02, "P61C_02"s, Qcode::Qs22, 1);
		ch_CollOnOffSwitch(Qcode::P61C_03, "P61C_03"s, Qcode::Qs22, 2);
		ch_CollOnOffSwitch(Qcode::P61C_04, "P61C_04"s, Qcode::Qs22, 3);
		ch_CollOnOffSwitch(Qcode::P61C_05, "P61C_05"s, Qcode::Qs22, 4);
		ch_CollOnOffSwitch(Qcode::P61C_06, "P61C_06"s, Qcode::Qs22, 5);
		ch_CollOnOffSwitch(Qcode::P61C_07, "P61C_07"s, Qcode::Qs22, 6);
		ch_CollOnOffSwitch(Qcode::P61C_08, "P61C_08"s, Qcode::Qs22, 7);
		ch_CollOnOffSwitch(Qcode::P61C_09, "P61C_09"s, Qcode::Qs22, 8);

		ch_CollOnOffSwitch(Qcode::P61D_01, "P61D_01"s, Qcode::Qs23, 0);
		ch_CollOnOffSwitch(Qcode::P61D_02, "P61D_02"s, Qcode::Qs23, 1);
		ch_CollOnOffSwitch(Qcode::P61D_03, "P61D_03"s, Qcode::Qs23, 2);
		ch_CollOnOffSwitch(Qcode::P61D_04, "P61D_04"s, Qcode::Qs23, 3);
		ch_CollOnOffSwitch(Qcode::P61D_05, "P61D_05"s, Qcode::Qs23, 4);
		ch_CollOnOffSwitch(Qcode::P61D_06, "P61D_06"s, Qcode::Qs23, 5);
		ch_CollOnOffSwitch(Qcode::P61D_07, "P61D_07"s, Qcode::Qs23, 6);
		ch_CollOnOffSwitch(Qcode::P61D_08, "P61D_08"s, Qcode::Qs23, 7);
		ch_CollOnOffSwitch(Qcode::P61D_09, "P61D_09"s, Qcode::Qs23, 8);

		ch_CollOnOffSwitch(Qcode::P61E_01, "P61E_01"s, Qcode::Qs24, 0);
		ch_CollOnOffSwitch(Qcode::P61E_02, "P61E_02"s, Qcode::Qs24, 1);
		ch_CollOnOffSwitch(Qcode::P61E_03, "P61E_03"s, Qcode::Qs24, 2);
		ch_CollOnOffSwitch(Qcode::P61E_04, "P61E_04"s, Qcode::Qs24, 3);
		ch_CollOnOffSwitch(Qcode::P61E_05, "P61E_05"s, Qcode::Qs24, 4);
		ch_CollOnOffSwitch(Qcode::P61E_06, "P61E_06"s, Qcode::Qs24, 5);
		ch_CollOnOffSwitch(Qcode::P61E_07, "P61E_07"s, Qcode::Qs24, 6);
		ch_CollOnOffSwitch(Qcode::P61E_08, "P61E_08"s, Qcode::Qs24, 7);
		ch_CollOnOffSwitch(Qcode::P61E_09, "P61E_09"s, Qcode::Qs24, 8);

		ch_CollOnOffSwitch(Qcode::P61F_01, "P61F_01"s, Qcode::Qs25, 0);
		ch_CollOnOffSwitch(Qcode::P61F_02, "P61F_02"s, Qcode::Qs25, 1);
		ch_CollOnOffSwitch(Qcode::P61F_03, "P61F_03"s, Qcode::Qs25, 2);
		ch_CollOnOffSwitch(Qcode::P61F_04, "P61F_04"s, Qcode::Qs25, 3);
		ch_CollOnOffSwitch(Qcode::P61F_05, "P61F_05"s, Qcode::Qs25, 4);
		ch_CollOnOffSwitch(Qcode::P61F_06, "P61F_06"s, Qcode::Qs25, 5);
		ch_CollOnOffSwitch(Qcode::P61F_07, "P61F_07"s, Qcode::Qs25, 6);
		ch_CollOnOffSwitch(Qcode::P61F_08, "P61F_08"s, Qcode::Qs25, 7);
		ch_CollOnOffSwitch(Qcode::P61F_09, "P61F_09"s, Qcode::Qs25, 8);

		ch_CollOnOffSwitch(Qcode::P61G_01, "P61G_01"s, Qcode::Qs26, 0);
		ch_CollOnOffSwitch(Qcode::P61G_02, "P61G_02"s, Qcode::Qs26, 1);
		ch_CollOnOffSwitch(Qcode::P61G_03, "P61G_03"s, Qcode::Qs26, 2);
		ch_CollOnOffSwitch(Qcode::P61G_04, "P61G_04"s, Qcode::Qs26, 3);
		ch_CollOnOffSwitch(Qcode::P61G_05, "P61G_05"s, Qcode::Qs26, 4);
		ch_CollOnOffSwitch(Qcode::P61G_06, "P61G_06"s, Qcode::Qs26, 5);
		ch_CollOnOffSwitch(Qcode::P61G_07, "P61G_07"s, Qcode::Qs26, 6);
		ch_CollOnOffSwitch(Qcode::P61G_08, "P61G_08"s, Qcode::Qs26, 7);
		ch_CollOnOffSwitch(Qcode::P61G_09, "P61G_09"s, Qcode::Qs26, 8);

		ch_CollOnOffSwitch(Qcode::P61H_01, "P61H_01"s, Qcode::Qs27, 0);
		ch_CollOnOffSwitch(Qcode::P61H_02, "P61H_02"s, Qcode::Qs27, 1);
		ch_CollOnOffSwitch(Qcode::P61H_03, "P61H_03"s, Qcode::Qs27, 2);
		ch_CollOnOffSwitch(Qcode::P61H_04, "P61H_04"s, Qcode::Qs27, 3);
		ch_CollOnOffSwitch(Qcode::P61H_05, "P61H_05"s, Qcode::Qs27, 4);
		ch_CollOnOffSwitch(Qcode::P61H_06, "P61H_06"s, Qcode::Qs27, 5);
		ch_CollOnOffSwitch(Qcode::P61H_07, "P61H_07"s, Qcode::Qs27, 6);
		ch_CollOnOffSwitch(Qcode::P61H_08, "P61H_08"s, Qcode::Qs27, 7);
		ch_CollOnOffSwitch(Qcode::P61H_09, "P61H_09"s, Qcode::Qs27, 8);

		ch_CollOnOffSwitch(Qcode::P61J_01, "P61J_01"s, Qcode::Qs28, 0);
		ch_CollOnOffSwitch(Qcode::P61J_02, "P61J_02"s, Qcode::Qs28, 1);
		ch_CollOnOffSwitch(Qcode::P61J_03, "P61J_03"s, Qcode::Qs28, 2);
		ch_CollOnOffSwitch(Qcode::P61J_04, "P61J_04"s, Qcode::Qs28, 3);
		ch_CollOnOffSwitch(Qcode::P61J_05, "P61J_05"s, Qcode::Qs28, 4);
		ch_CollOnOffSwitch(Qcode::P61J_06, "P61J_06"s, Qcode::Qs28, 5);
		ch_CollOnOffSwitch(Qcode::P61J_07, "P61J_07"s, Qcode::Qs28, 6);
		ch_CollOnOffSwitch(Qcode::P61J_08, "P61J_08"s, Qcode::Qs28, 7);
		ch_CollOnOffSwitch(Qcode::P61J_09, "P61J_09"s, Qcode::Qs28, 8);

		ch_CollOnOffSwitch(Qcode::P61K_01, "P61K_01"s, Qcode::Qs29, 0);
		ch_CollOnOffSwitch(Qcode::P61K_02, "P61K_02"s, Qcode::Qs29, 1);
		ch_CollOnOffSwitch(Qcode::P61K_03, "P61K_03"s, Qcode::Qs29, 2);
		ch_CollOnOffSwitch(Qcode::P61K_04, "P61K_04"s, Qcode::Qs29, 3);
		ch_CollOnOffSwitch(Qcode::P61K_05, "P61K_05"s, Qcode::Qs29, 4);
		ch_CollOnOffSwitch(Qcode::P61K_06, "P61K_06"s, Qcode::Qs29, 5);
		ch_CollOnOffSwitch(Qcode::P61K_07, "P61K_07"s, Qcode::Qs29, 6);
		ch_CollOnOffSwitch(Qcode::P61K_08, "P61K_08"s, Qcode::Qs29, 7);
		ch_CollOnOffSwitch(Qcode::P61K_09, "P61K_09"s, Qcode::Qs29, 8);

		ch_CollOnOffSwitch(Qcode::P61L_01, "P61L_01"s, Qcode::Qs30, 0);
		ch_CollOnOffSwitch(Qcode::P61L_02, "P61L_02"s, Qcode::Qs30, 1);
		ch_CollOnOffSwitch(Qcode::P61L_03, "P61L_03"s, Qcode::Qs30, 2);
		ch_CollOnOffSwitch(Qcode::P61L_04, "P61L_04"s, Qcode::Qs30, 3);
		ch_CollOnOffSwitch(Qcode::P61L_05, "P61L_05"s, Qcode::Qs30, 4);
		ch_CollOnOffSwitch(Qcode::P61L_06, "P61L_06"s, Qcode::Qs30, 5);
		ch_CollOnOffSwitch(Qcode::P61L_07, "P61L_07"s, Qcode::Qs30, 6);
		ch_CollOnOffSwitch(Qcode::P61L_08, "P61L_08"s, Qcode::Qs30, 7);
		ch_CollOnOffSwitch(Qcode::P61L_09, "P61L_09"s, Qcode::Qs30, 8);

		ch_CollOnOffSwitch(Qcode::P62C_10, "P62C_10"s, Qcode::Qs31, 0);
		ch_CollOnOffSwitch(Qcode::P62C_11, "P62C_11"s, Qcode::Qs31, 1);
		ch_CollOnOffSwitch(Qcode::P62C_12, "P62C_12"s, Qcode::Qs31, 2);
		ch_CollOnOffSwitch(Qcode::P62C_13, "P62C_13"s, Qcode::Qs31, 3);
		ch_CollOnOffSwitch(Qcode::P62C_14, "P62C_14"s, Qcode::Qs31, 4);
		ch_CollOnOffSwitch(Qcode::P62C_15, "P62C_15"s, Qcode::Qs31, 5);
		ch_CollOnOffSwitch(Qcode::P62C_16, "P62C_16"s, Qcode::Qs31, 6);
		ch_CollOnOffSwitch(Qcode::P62C_17, "P62C_17"s, Qcode::Qs31, 7);
		ch_CollOnOffSwitch(Qcode::P62C_18, "P62C_18"s, Qcode::Qs31, 8);

		ch_CollOnOffSwitch(Qcode::P62D_10, "P62D_10"s, Qcode::Qs32, 0);
		ch_CollOnOffSwitch(Qcode::P62D_11, "P62D_11"s, Qcode::Qs32, 1);
		ch_CollOnOffSwitch(Qcode::P62D_12, "P62D_12"s, Qcode::Qs32, 2);
		ch_CollOnOffSwitch(Qcode::P62D_13, "P62D_13"s, Qcode::Qs32, 3);
		ch_CollOnOffSwitch(Qcode::P62D_14, "P62D_14"s, Qcode::Qs32, 4);
		ch_CollOnOffSwitch(Qcode::P62D_15, "P62D_15"s, Qcode::Qs32, 5);
		ch_CollOnOffSwitch(Qcode::P62D_16, "P62D_16"s, Qcode::Qs32, 6);
		ch_CollOnOffSwitch(Qcode::P62D_17, "P62D_17"s, Qcode::Qs32, 7);
		ch_CollOnOffSwitch(Qcode::P62D_18, "P62D_18"s, Qcode::Qs32, 8);

		ch_CollOnOffSwitch(Qcode::P62E_10, "P62E_10"s, Qcode::Qs33, 0);
		ch_CollOnOffSwitch(Qcode::P62E_11, "P62E_11"s, Qcode::Qs33, 1);
		ch_CollOnOffSwitch(Qcode::P62E_12, "P62E_12"s, Qcode::Qs33, 2);
		ch_CollOnOffSwitch(Qcode::P62E_13, "P62E_13"s, Qcode::Qs33, 3);
		ch_CollOnOffSwitch(Qcode::P62E_14, "P62E_14"s, Qcode::Qs33, 4);
		ch_CollOnOffSwitch(Qcode::P62E_15, "P62E_15"s, Qcode::Qs33, 5);
		ch_CollOnOffSwitch(Qcode::P62E_16, "P62E_16"s, Qcode::Qs33, 6);
		ch_CollOnOffSwitch(Qcode::P62E_17, "P62E_17"s, Qcode::Qs33, 7);
		ch_CollOnOffSwitch(Qcode::P62E_18, "P62E_18"s, Qcode::Qs33, 8);

		ch_CollOnOffSwitch(Qcode::P62F_10, "P62F_10"s, Qcode::Qs34, 0);
		ch_CollOnOffSwitch(Qcode::P62F_11, "P62F_11"s, Qcode::Qs34, 1);
		ch_CollOnOffSwitch(Qcode::P62F_12, "P62F_12"s, Qcode::Qs34, 2);
		ch_CollOnOffSwitch(Qcode::P62F_13, "P62F_13"s, Qcode::Qs34, 3);
		ch_CollOnOffSwitch(Qcode::P62F_14, "P62F_14"s, Qcode::Qs34, 4);
		ch_CollOnOffSwitch(Qcode::P62F_15, "P62F_15"s, Qcode::Qs34, 5);
		ch_CollOnOffSwitch(Qcode::P62F_16, "P62F_16"s, Qcode::Qs34, 6);
		ch_CollOnOffSwitch(Qcode::P62F_17, "P62F_17"s, Qcode::Qs34, 7);
		ch_CollOnOffSwitch(Qcode::P62F_18, "P62F_18"s, Qcode::Qs34, 8);

		ch_CollOnOffSwitch(Qcode::P62G_10, "P62G_10"s, Qcode::Qs35, 0);
		ch_CollOnOffSwitch(Qcode::P62G_11, "P62G_11"s, Qcode::Qs35, 1);
		ch_CollOnOffSwitch(Qcode::P62G_12, "P62G_12"s, Qcode::Qs35, 2);
		ch_CollOnOffSwitch(Qcode::P62G_13, "P62G_13"s, Qcode::Qs35, 3);
		ch_CollOnOffSwitch(Qcode::P62G_14, "P62G_14"s, Qcode::Qs35, 4);
		ch_CollOnOffSwitch(Qcode::P62G_15, "P62G_15"s, Qcode::Qs35, 5);
		ch_CollOnOffSwitch(Qcode::P62G_16, "P62G_16"s, Qcode::Qs35, 6);
		ch_CollOnOffSwitch(Qcode::P62G_17, "P62G_17"s, Qcode::Qs35, 7);
		ch_CollOnOffSwitch(Qcode::P62G_18, "P62G_18"s, Qcode::Qs35, 8);

		ch_CollOnOffSwitch(Qcode::P62H_10, "P62H_10"s, Qcode::Qs36, 0);
		ch_CollOnOffSwitch(Qcode::P62H_11, "P62H_11"s, Qcode::Qs36, 1);
		ch_CollOnOffSwitch(Qcode::P62H_12, "P62H_12"s, Qcode::Qs36, 2);
		ch_CollOnOffSwitch(Qcode::P62H_13, "P62H_13"s, Qcode::Qs36, 3);
		ch_CollOnOffSwitch(Qcode::P62H_14, "P62H_14"s, Qcode::Qs36, 4);
		ch_CollOnOffSwitch(Qcode::P62H_15, "P62H_15"s, Qcode::Qs36, 5);
		ch_CollOnOffSwitch(Qcode::P62H_16, "P62H_16"s, Qcode::Qs36, 6);
		ch_CollOnOffSwitch(Qcode::P62H_17, "P62H_17"s, Qcode::Qs36, 7);
		ch_CollOnOffSwitch(Qcode::P62H_18, "P62H_18"s, Qcode::Qs36, 8);

		ch_CollOnOffSwitch(Qcode::P62J_10, "P62J_10"s, Qcode::Qs37, 0);
		ch_CollOnOffSwitch(Qcode::P62J_11, "P62J_11"s, Qcode::Qs37, 1);
		ch_CollOnOffSwitch(Qcode::P62J_12, "P62J_12"s, Qcode::Qs37, 2);
		ch_CollOnOffSwitch(Qcode::P62J_13, "P62J_13"s, Qcode::Qs37, 3);
		ch_CollOnOffSwitch(Qcode::P62J_14, "P62J_14"s, Qcode::Qs37, 4);
		ch_CollOnOffSwitch(Qcode::P62J_15, "P62J_15"s, Qcode::Qs37, 5);
		ch_CollOnOffSwitch(Qcode::P62J_16, "P62J_16"s, Qcode::Qs37, 6);
		ch_CollOnOffSwitch(Qcode::P62J_17, "P62J_17"s, Qcode::Qs37, 7);
		ch_CollOnOffSwitch(Qcode::P62J_18, "P62J_18"s, Qcode::Qs37, 8);

		ch_CollOnOffSwitch(Qcode::P62K_10, "P62K_10"s, Qcode::Qs38, 0);
		ch_CollOnOffSwitch(Qcode::P62K_11, "P62K_11"s, Qcode::Qs38, 1);
		ch_CollOnOffSwitch(Qcode::P62K_12, "P62K_12"s, Qcode::Qs38, 2);
		ch_CollOnOffSwitch(Qcode::P62K_13, "P62K_13"s, Qcode::Qs38, 3);
		ch_CollOnOffSwitch(Qcode::P62K_14, "P62K_14"s, Qcode::Qs38, 4);
		ch_CollOnOffSwitch(Qcode::P62K_15, "P62K_15"s, Qcode::Qs38, 5);
		ch_CollOnOffSwitch(Qcode::P62K_16, "P62K_16"s, Qcode::Qs38, 6);
		ch_CollOnOffSwitch(Qcode::P62K_17, "P62K_17"s, Qcode::Qs38, 7);
		ch_CollOnOffSwitch(Qcode::P62K_18, "P62K_18"s, Qcode::Qs38, 8);

		ch_CollOnOffSwitch(Qcode::P62L_10, "P62L_10"s, Qcode::Qs39, 0);
		ch_CollOnOffSwitch(Qcode::P62L_11, "P62L_11"s, Qcode::Qs39, 1);
		ch_CollOnOffSwitch(Qcode::P62L_12, "P62L_12"s, Qcode::Qs39, 2);
		ch_CollOnOffSwitch(Qcode::P62L_13, "P62L_13"s, Qcode::Qs39, 3);
		ch_CollOnOffSwitch(Qcode::P62L_14, "P62L_14"s, Qcode::Qs39, 4);
		ch_CollOnOffSwitch(Qcode::P62L_15, "P62L_15"s, Qcode::Qs39, 5);
		ch_CollOnOffSwitch(Qcode::P62L_16, "P62L_16"s, Qcode::Qs39, 6);
		ch_CollOnOffSwitch(Qcode::P62L_17, "P62L_17"s, Qcode::Qs39, 7);
		ch_CollOnOffSwitch(Qcode::P62L_18, "P62L_18"s, Qcode::Qs39, 8);

		ch_CollOnOffSwitch(Qcode::P63C_19, "P63C_19"s, Qcode::Qs40, 0);
		ch_CollOnOffSwitch(Qcode::P63C_20, "P63C_20"s, Qcode::Qs40, 1);
		ch_CollOnOffSwitch(Qcode::P63C_21, "P63C_21"s, Qcode::Qs40, 2);
		ch_CollOnOffSwitch(Qcode::P63C_22, "P63C_22"s, Qcode::Qs40, 3);
		ch_CollOnOffSwitch(Qcode::P63C_23, "P63C_23"s, Qcode::Qs40, 4);
		ch_CollOnOffSwitch(Qcode::P63C_24, "P63C_24"s, Qcode::Qs40, 5);
		ch_CollOnOffSwitch(Qcode::P63C_25, "P63C_25"s, Qcode::Qs40, 6);
		ch_CollOnOffSwitch(Qcode::P63C_26, "P63C_26"s, Qcode::Qs40, 7);
		ch_CollOnOffSwitch(Qcode::P63C_27, "P63C_27"s, Qcode::Qs40, 8);

		ch_CollOnOffSwitch(Qcode::P63D_19, "P63D_19"s, Qcode::Qs41, 0);
		ch_CollOnOffSwitch(Qcode::P63D_20, "P63D_20"s, Qcode::Qs41, 1);
		ch_CollOnOffSwitch(Qcode::P63D_21, "P63D_21"s, Qcode::Qs41, 2);
		ch_CollOnOffSwitch(Qcode::P63D_22, "P63D_22"s, Qcode::Qs41, 3);
		ch_CollOnOffSwitch(Qcode::P63D_23, "P63D_23"s, Qcode::Qs41, 4);
		ch_CollOnOffSwitch(Qcode::P63D_24, "P63D_24"s, Qcode::Qs41, 5);
		ch_CollOnOffSwitch(Qcode::P63D_25, "P63D_25"s, Qcode::Qs41, 6);
		ch_CollOnOffSwitch(Qcode::P63D_26, "P63D_26"s, Qcode::Qs41, 7);
		ch_CollOnOffSwitch(Qcode::P63D_27, "P63D_27"s, Qcode::Qs41, 8);

		ch_CollOnOffSwitch(Qcode::P63E_19, "P63E_19"s, Qcode::Qs42, 0);
		ch_CollOnOffSwitch(Qcode::P63E_20, "P63E_20"s, Qcode::Qs42, 1);
		ch_CollOnOffSwitch(Qcode::P63E_21, "P63E_21"s, Qcode::Qs42, 2);
		ch_CollOnOffSwitch(Qcode::P63E_22, "P63E_22"s, Qcode::Qs42, 3);
		ch_CollOnOffSwitch(Qcode::P63E_23, "P63E_23"s, Qcode::Qs42, 4);
		ch_CollOnOffSwitch(Qcode::P63E_24, "P63E_24"s, Qcode::Qs42, 5);
		ch_CollOnOffSwitch(Qcode::P63E_25, "P63E_25"s, Qcode::Qs42, 6);
		ch_CollOnOffSwitch(Qcode::P63E_26, "P63E_26"s, Qcode::Qs42, 7);
		ch_CollOnOffSwitch(Qcode::P63E_27, "P63E_27"s, Qcode::Qs42, 8);

		ch_CollOnOffSwitch(Qcode::P63F_19, "P63F_19"s, Qcode::Qs43, 0);
		ch_CollOnOffSwitch(Qcode::P63F_20, "P63F_20"s, Qcode::Qs43, 1);
		ch_CollOnOffSwitch(Qcode::P63F_21, "P63F_21"s, Qcode::Qs43, 2);
		ch_CollOnOffSwitch(Qcode::P63F_22, "P63F_22"s, Qcode::Qs43, 3);
		ch_CollOnOffSwitch(Qcode::P63F_23, "P63F_23"s, Qcode::Qs43, 4);
		ch_CollOnOffSwitch(Qcode::P63F_24, "P63F_24"s, Qcode::Qs43, 5);
		ch_CollOnOffSwitch(Qcode::P63F_25, "P63F_25"s, Qcode::Qs43, 6);
		ch_CollOnOffSwitch(Qcode::P63F_26, "P63F_26"s, Qcode::Qs43, 7);
		ch_CollOnOffSwitch(Qcode::P63F_27, "P63F_27"s, Qcode::Qs43, 8);

		ch_CollOnOffSwitch(Qcode::P63G_19, "P63G_19"s, Qcode::Qs44, 0);
		ch_CollOnOffSwitch(Qcode::P63G_20, "P63G_20"s, Qcode::Qs44, 1);
		ch_CollOnOffSwitch(Qcode::P63G_21, "P63G_21"s, Qcode::Qs44, 2);
		ch_CollOnOffSwitch(Qcode::P63G_22, "P63G_22"s, Qcode::Qs44, 3);
		ch_CollOnOffSwitch(Qcode::P63G_23, "P63G_23"s, Qcode::Qs44, 4);
		ch_CollOnOffSwitch(Qcode::P63G_24, "P63G_24"s, Qcode::Qs44, 5);
		ch_CollOnOffSwitch(Qcode::P63G_25, "P63G_25"s, Qcode::Qs44, 6);
		ch_CollOnOffSwitch(Qcode::P63G_26, "P63G_26"s, Qcode::Qs44, 7);
		ch_CollOnOffSwitch(Qcode::P63G_27, "P63G_27"s, Qcode::Qs44, 8);

		ch_CollOnOffSwitch(Qcode::P63H_19, "P63H_19"s, Qcode::Qs45, 0);
		ch_CollOnOffSwitch(Qcode::P63H_20, "P63H_20"s, Qcode::Qs45, 1);
		ch_CollOnOffSwitch(Qcode::P63H_21, "P63H_21"s, Qcode::Qs45, 2);
		ch_CollOnOffSwitch(Qcode::P63H_22, "P63H_22"s, Qcode::Qs45, 3);
		ch_CollOnOffSwitch(Qcode::P63H_23, "P63H_23"s, Qcode::Qs45, 4);
		ch_CollOnOffSwitch(Qcode::P63H_24, "P63H_24"s, Qcode::Qs45, 5);
		ch_CollOnOffSwitch(Qcode::P63H_25, "P63H_25"s, Qcode::Qs45, 6);
		ch_CollOnOffSwitch(Qcode::P63H_26, "P63H_26"s, Qcode::Qs45, 7);
		ch_CollOnOffSwitch(Qcode::P63H_27, "P63H_27"s, Qcode::Qs45, 8);

		ch_CollOnOffSwitch(Qcode::P63J_19, "P63J_19"s, Qcode::Qs46, 0);
		ch_CollOnOffSwitch(Qcode::P63J_20, "P63J_20"s, Qcode::Qs46, 1);
		ch_CollOnOffSwitch(Qcode::P63J_21, "P63J_21"s, Qcode::Qs46, 2);
		ch_CollOnOffSwitch(Qcode::P63J_22, "P63J_22"s, Qcode::Qs46, 3);
		ch_CollOnOffSwitch(Qcode::P63J_23, "P63J_23"s, Qcode::Qs46, 4);
		ch_CollOnOffSwitch(Qcode::P63J_24, "P63J_24"s, Qcode::Qs46, 5);
		ch_CollOnOffSwitch(Qcode::P63J_25, "P63J_25"s, Qcode::Qs46, 6);
		ch_CollOnOffSwitch(Qcode::P63J_26, "P63J_26"s, Qcode::Qs46, 7);
		ch_CollOnOffSwitch(Qcode::P63J_27, "P63J_27"s, Qcode::Qs46, 8);

		ch_CollOnOffSwitch(Qcode::P63K_19, "P63K_19"s, Qcode::Qs47, 0);
		ch_CollOnOffSwitch(Qcode::P63K_20, "P63K_20"s, Qcode::Qs47, 1);
		ch_CollOnOffSwitch(Qcode::P63K_21, "P63K_21"s, Qcode::Qs47, 2);
		ch_CollOnOffSwitch(Qcode::P63K_22, "P63K_22"s, Qcode::Qs47, 3);
		ch_CollOnOffSwitch(Qcode::P63K_23, "P63K_23"s, Qcode::Qs47, 4);
		ch_CollOnOffSwitch(Qcode::P63K_24, "P63K_24"s, Qcode::Qs47, 5);
		ch_CollOnOffSwitch(Qcode::P63K_25, "P63K_25"s, Qcode::Qs47, 6);
		ch_CollOnOffSwitch(Qcode::P63K_26, "P63K_26"s, Qcode::Qs47, 7);
		ch_CollOnOffSwitch(Qcode::P63K_27, "P63K_27"s, Qcode::Qs47, 8);

		ch_CollOnOffSwitch(Qcode::P63L_19, "P63L_19"s, Qcode::Qs48, 0);
		ch_CollOnOffSwitch(Qcode::P63L_20, "P63L_20"s, Qcode::Qs48, 1);
		ch_CollOnOffSwitch(Qcode::P63L_21, "P63L_21"s, Qcode::Qs48, 2);
		ch_CollOnOffSwitch(Qcode::P63L_22, "P63L_22"s, Qcode::Qs48, 3);
		ch_CollOnOffSwitch(Qcode::P63L_23, "P63L_23"s, Qcode::Qs48, 4);
		ch_CollOnOffSwitch(Qcode::P63L_24, "P63L_24"s, Qcode::Qs48, 5);
		ch_CollOnOffSwitch(Qcode::P63L_25, "P63L_25"s, Qcode::Qs48, 6);
		ch_CollOnOffSwitch(Qcode::P63L_26, "P63L_26"s, Qcode::Qs48, 7);
		ch_CollOnOffSwitch(Qcode::P63L_27, "P63L_27"s, Qcode::Qs48, 8);

		ch_CollOnOffSwitch(Qcode::P64C_28, "P64C_28"s, Qcode::Qs49, 0);
		ch_CollOnOffSwitch(Qcode::P64C_29, "P64C_29"s, Qcode::Qs49, 1);
		ch_CollOnOffSwitch(Qcode::P64C_30, "P64C_30"s, Qcode::Qs49, 2);
		ch_CollOnOffSwitch(Qcode::P64C_31, "P64C_31"s, Qcode::Qs49, 3);
		ch_CollOnOffSwitch(Qcode::P64C_32, "P64C_32"s, Qcode::Qs49, 4);
		ch_CollOnOffSwitch(Qcode::P64C_33, "P64C_33"s, Qcode::Qs49, 5);
		ch_CollOnOffSwitch(Qcode::P64C_34, "P64C_34"s, Qcode::Qs49, 6);
		ch_CollOnOffSwitch(Qcode::P64C_35, "P64C_35"s, Qcode::Qs49, 7);
		ch_CollOnOffSwitch(Qcode::P64C_36, "P64C_36"s, Qcode::Qs49, 8);

		ch_CollOnOffSwitch(Qcode::P64D_28, "P64D_28"s, Qcode::Qs50, 0);
		ch_CollOnOffSwitch(Qcode::P64D_29, "P64D_29"s, Qcode::Qs50, 1);
		ch_CollOnOffSwitch(Qcode::P64D_30, "P64D_30"s, Qcode::Qs50, 2);
		ch_CollOnOffSwitch(Qcode::P64D_31, "P64D_31"s, Qcode::Qs50, 3);
		ch_CollOnOffSwitch(Qcode::P64D_32, "P64D_32"s, Qcode::Qs50, 4);
		ch_CollOnOffSwitch(Qcode::P64D_33, "P64D_33"s, Qcode::Qs50, 5);
		ch_CollOnOffSwitch(Qcode::P64D_34, "P64D_34"s, Qcode::Qs50, 6);
		ch_CollOnOffSwitch(Qcode::P64D_35, "P64D_35"s, Qcode::Qs50, 7);
		ch_CollOnOffSwitch(Qcode::P64D_36, "P64D_36"s, Qcode::Qs50, 8);

		ch_CollOnOffSwitch(Qcode::P64E_28, "P64E_28"s, Qcode::Qs51, 0);
		ch_CollOnOffSwitch(Qcode::P64E_29, "P64E_29"s, Qcode::Qs51, 1);
		ch_CollOnOffSwitch(Qcode::P64E_30, "P64E_30"s, Qcode::Qs51, 2);
		ch_CollOnOffSwitch(Qcode::P64E_31, "P64E_31"s, Qcode::Qs51, 3);
		ch_CollOnOffSwitch(Qcode::P64E_32, "P64E_32"s, Qcode::Qs51, 4);
		ch_CollOnOffSwitch(Qcode::P64E_33, "P64E_33"s, Qcode::Qs51, 5);
		ch_CollOnOffSwitch(Qcode::P64E_34, "P64E_34"s, Qcode::Qs51, 6);
		ch_CollOnOffSwitch(Qcode::P64E_35, "P64E_35"s, Qcode::Qs51, 7);
		ch_CollOnOffSwitch(Qcode::P64E_36, "P64E_36"s, Qcode::Qs51, 8);

		ch_CollOnOffSwitch(Qcode::P64F_28, "P64F_28"s, Qcode::Qs52, 0);
		ch_CollOnOffSwitch(Qcode::P64F_29, "P64F_29"s, Qcode::Qs52, 1);
		ch_CollOnOffSwitch(Qcode::P64F_30, "P64F_30"s, Qcode::Qs52, 2);
		ch_CollOnOffSwitch(Qcode::P64F_31, "P64F_31"s, Qcode::Qs52, 3);
		ch_CollOnOffSwitch(Qcode::P64F_32, "P64F_32"s, Qcode::Qs52, 4);
		ch_CollOnOffSwitch(Qcode::P64F_33, "P64F_33"s, Qcode::Qs52, 5);
		ch_CollOnOffSwitch(Qcode::P64F_34, "P64F_34"s, Qcode::Qs52, 6);
		ch_CollOnOffSwitch(Qcode::P64F_35, "P64F_35"s, Qcode::Qs52, 7);
		ch_CollOnOffSwitch(Qcode::P64F_36, "P64F_36"s, Qcode::Qs52, 8);

		ch_CollOnOffSwitch(Qcode::P64G_28, "P64G_28"s, Qcode::Qs53, 0);
		ch_CollOnOffSwitch(Qcode::P64G_29, "P64G_29"s, Qcode::Qs53, 1);
		ch_CollOnOffSwitch(Qcode::P64G_30, "P64G_30"s, Qcode::Qs53, 2);
		ch_CollOnOffSwitch(Qcode::P64G_31, "P64G_31"s, Qcode::Qs53, 3);
		ch_CollOnOffSwitch(Qcode::P64G_32, "P64G_32"s, Qcode::Qs53, 4);
		ch_CollOnOffSwitch(Qcode::P64G_33, "P64G_33"s, Qcode::Qs53, 5);
		ch_CollOnOffSwitch(Qcode::P64G_34, "P64G_34"s, Qcode::Qs53, 6);
		ch_CollOnOffSwitch(Qcode::P64G_35, "P64G_35"s, Qcode::Qs53, 7);
		ch_CollOnOffSwitch(Qcode::P64G_36, "P64G_36"s, Qcode::Qs53, 8);

		ch_CollOnOffSwitch(Qcode::P64H_28, "P64H_28"s, Qcode::Qs54, 0);
		ch_CollOnOffSwitch(Qcode::P64H_29, "P64H_29"s, Qcode::Qs54, 1);
		ch_CollOnOffSwitch(Qcode::P64H_30, "P64H_30"s, Qcode::Qs54, 2);
		ch_CollOnOffSwitch(Qcode::P64H_31, "P64H_31"s, Qcode::Qs54, 3);
		ch_CollOnOffSwitch(Qcode::P64H_32, "P64H_32"s, Qcode::Qs54, 4);
		ch_CollOnOffSwitch(Qcode::P64H_33, "P64H_33"s, Qcode::Qs54, 5);
		ch_CollOnOffSwitch(Qcode::P64H_34, "P64H_34"s, Qcode::Qs54, 6);
		ch_CollOnOffSwitch(Qcode::P64H_35, "P64H_35"s, Qcode::Qs54, 7);
		ch_CollOnOffSwitch(Qcode::P64H_36, "P64H_36"s, Qcode::Qs54, 8);

		ch_CollOnOffSwitch(Qcode::P64J_28, "P64J_28"s, Qcode::Qs55, 0);
		ch_CollOnOffSwitch(Qcode::P64J_29, "P64J_29"s, Qcode::Qs55, 1);
		ch_CollOnOffSwitch(Qcode::P64J_30, "P64J_30"s, Qcode::Qs55, 2);
		ch_CollOnOffSwitch(Qcode::P64J_31, "P64J_31"s, Qcode::Qs55, 3);
		ch_CollOnOffSwitch(Qcode::P64J_32, "P64J_32"s, Qcode::Qs55, 4);
		ch_CollOnOffSwitch(Qcode::P64J_33, "P64J_33"s, Qcode::Qs55, 5);
		ch_CollOnOffSwitch(Qcode::P64J_34, "P64J_34"s, Qcode::Qs55, 6);
		ch_CollOnOffSwitch(Qcode::P64J_35, "P64J_35"s, Qcode::Qs55, 7);
		ch_CollOnOffSwitch(Qcode::P64J_36, "P64J_36"s, Qcode::Qs55, 8);

		ch_CollOnOffSwitch(Qcode::P64K_28, "P64K_28"s, Qcode::Qs56, 0);
		ch_CollOnOffSwitch(Qcode::P64K_29, "P64K_29"s, Qcode::Qs56, 1);
		ch_CollOnOffSwitch(Qcode::P64K_30, "P64K_30"s, Qcode::Qs56, 2);
		ch_CollOnOffSwitch(Qcode::P64K_31, "P64K_31"s, Qcode::Qs56, 3);
		ch_CollOnOffSwitch(Qcode::P64K_32, "P64K_32"s, Qcode::Qs56, 4);
		ch_CollOnOffSwitch(Qcode::P64K_33, "P64K_33"s, Qcode::Qs56, 5);
		ch_CollOnOffSwitch(Qcode::P64K_34, "P64K_34"s, Qcode::Qs56, 6);
		ch_CollOnOffSwitch(Qcode::P64K_35, "P64K_35"s, Qcode::Qs56, 7);
		ch_CollOnOffSwitch(Qcode::P64K_36, "P64K_36"s, Qcode::Qs56, 8);

		ch_CollOnOffSwitch(Qcode::P64L_28, "P64L_28"s, Qcode::Qs57, 0);
		ch_CollOnOffSwitch(Qcode::P64L_29, "P64L_29"s, Qcode::Qs57, 1);
		ch_CollOnOffSwitch(Qcode::P64L_30, "P64L_30"s, Qcode::Qs57, 2);
		ch_CollOnOffSwitch(Qcode::P64L_31, "P64L_31"s, Qcode::Qs57, 3);
		ch_CollOnOffSwitch(Qcode::P64L_32, "P64L_32"s, Qcode::Qs57, 4);
		ch_CollOnOffSwitch(Qcode::P64L_33, "P64L_33"s, Qcode::Qs57, 5);
		ch_CollOnOffSwitch(Qcode::P64L_34, "P64L_34"s, Qcode::Qs57, 6);
		ch_CollOnOffSwitch(Qcode::P64L_35, "P64L_35"s, Qcode::Qs57, 7);
		ch_CollOnOffSwitch(Qcode::P64L_36, "P64L_36"s, Qcode::Qs57, 8);

		ch_CollOnOffSwitch(Qcode::P65A_01, "P65A_01"s, Qcode::Qs58, 0);
		ch_CollOnOffSwitch(Qcode::P65A_02, "P65A_02"s, Qcode::Qs58, 1);
		ch_CollOnOffSwitch(Qcode::P65A_03, "P65A_03"s, Qcode::Qs58, 2);
		ch_CollOnOffSwitch(Qcode::P65A_04, "P65A_04"s, Qcode::Qs58, 3);
		ch_CollOnOffSwitch(Qcode::P65A_05, "P65A_05"s, Qcode::Qs58, 4);
		ch_CollOnOffSwitch(Qcode::P65A_06, "P65A_06"s, Qcode::Qs58, 5);
		ch_CollOnOffSwitch(Qcode::P65A_07, "P65A_07"s, Qcode::Qs58, 6);
		ch_CollOnOffSwitch(Qcode::P65A_08, "P65A_08"s, Qcode::Qs58, 7);
		ch_CollOnOffSwitch(Qcode::P65A_09, "P65A_09"s, Qcode::Qs58, 8);
		ch_CollOnOffSwitch(Qcode::P65A_10, "P65A_10"s, Qcode::Qs58, 9);
		ch_CollOnOffSwitch(Qcode::P65A_11, "P65A_11"s, Qcode::Qs58, 10);
		ch_CollOnOffSwitch(Qcode::P65A_12, "P65A_12"s, Qcode::Qs58, 11);
		ch_CollOnOffSwitch(Qcode::P65A_13, "P65A_13"s, Qcode::Qs58, 12);
		ch_CollOnOffSwitch(Qcode::P65A_14, "P65A_14"s, Qcode::Qs58, 13);
		ch_CollOnOffSwitch(Qcode::P65A_15, "P65A_15"s, Qcode::Qs58, 14);
		ch_CollOnOffSwitch(Qcode::P65A_16, "P65A_16"s, Qcode::Qs58, 15);
		ch_CollOnOffSwitch(Qcode::P65A_17, "P65A_17"s, Qcode::Qs58, 16);
		ch_CollOnOffSwitch(Qcode::P65A_18, "P65A_18"s, Qcode::Qs58, 17);

		ch_CollOnOffSwitch(Qcode::P65B_01, "P65B_01"s, Qcode::Qs59, 0);
		ch_CollOnOffSwitch(Qcode::P65B_02, "P65B_02"s, Qcode::Qs59, 1);
		ch_CollOnOffSwitch(Qcode::P65B_03, "P65B_03"s, Qcode::Qs59, 2);
		ch_CollOnOffSwitch(Qcode::P65B_04, "P65B_04"s, Qcode::Qs59, 3);
		ch_CollOnOffSwitch(Qcode::P65B_05, "P65B_05"s, Qcode::Qs59, 4);
		ch_CollOnOffSwitch(Qcode::P65B_06, "P65B_06"s, Qcode::Qs59, 5);
		ch_CollOnOffSwitch(Qcode::P65B_07, "P65B_07"s, Qcode::Qs59, 6);
		ch_CollOnOffSwitch(Qcode::P65B_08, "P65B_08"s, Qcode::Qs59, 7);
		ch_CollOnOffSwitch(Qcode::P65B_09, "P65B_09"s, Qcode::Qs59, 8);
		ch_CollOnOffSwitch(Qcode::P65B_10, "P65B_10"s, Qcode::Qs59, 9);
		ch_CollOnOffSwitch(Qcode::P65B_11, "P65B_11"s, Qcode::Qs59, 10);
		ch_CollOnOffSwitch(Qcode::P65B_12, "P65B_12"s, Qcode::Qs59, 11);
		ch_CollOnOffSwitch(Qcode::P65B_13, "P65B_13"s, Qcode::Qs59, 12);
		ch_CollOnOffSwitch(Qcode::P65B_14, "P65B_14"s, Qcode::Qs59, 13);
		ch_CollOnOffSwitch(Qcode::P65B_15, "P65B_15"s, Qcode::Qs59, 14);
		ch_CollOnOffSwitch(Qcode::P65B_16, "P65B_16"s, Qcode::Qs59, 15);
		ch_CollOnOffSwitch(Qcode::P65B_17, "P65B_17"s, Qcode::Qs59, 16);
		ch_CollOnOffSwitch(Qcode::P65B_18, "P65B_18"s, Qcode::Qs59, 17);

		ch_CollOnOffSwitch(Qcode::P66A_19, "P66A_19"s, Qcode::Qs60, 0);
		ch_CollOnOffSwitch(Qcode::P66A_20, "P66A_20"s, Qcode::Qs60, 1);
		ch_CollOnOffSwitch(Qcode::P66A_21, "P66A_21"s, Qcode::Qs60, 2);
		ch_CollOnOffSwitch(Qcode::P66A_22, "P66A_22"s, Qcode::Qs60, 3);
		ch_CollOnOffSwitch(Qcode::P66A_23, "P66A_23"s, Qcode::Qs60, 4);
		ch_CollOnOffSwitch(Qcode::P66A_24, "P66A_24"s, Qcode::Qs60, 5);
		ch_CollOnOffSwitch(Qcode::P66A_25, "P66A_25"s, Qcode::Qs60, 6);
		ch_CollOnOffSwitch(Qcode::P66A_26, "P66A_26"s, Qcode::Qs60, 7);
		ch_CollOnOffSwitch(Qcode::P66A_27, "P66A_27"s, Qcode::Qs60, 8);
		ch_CollOnOffSwitch(Qcode::P66A_28, "P66A_28"s, Qcode::Qs60, 9);
		ch_CollOnOffSwitch(Qcode::P66A_29, "P66A_29"s, Qcode::Qs60, 10);
		ch_CollOnOffSwitch(Qcode::P66A_30, "P66A_30"s, Qcode::Qs60, 11);
		ch_CollOnOffSwitch(Qcode::P66A_31, "P66A_31"s, Qcode::Qs60, 12);
		ch_CollOnOffSwitch(Qcode::P66A_32, "P66A_32"s, Qcode::Qs60, 13);
		ch_CollOnOffSwitch(Qcode::P66A_33, "P66A_33"s, Qcode::Qs60, 14);
		ch_CollOnOffSwitch(Qcode::P66A_34, "P66A_34"s, Qcode::Qs60, 15);
		ch_CollOnOffSwitch(Qcode::P66A_35, "P66A_35"s, Qcode::Qs60, 16);
		ch_CollOnOffSwitch(Qcode::P66A_36, "P66A_36"s, Qcode::Qs60, 17);

		ch_CollOnOffSwitch(Qcode::P66B_19, "P66B_19"s, Qcode::Qs61, 0);
		ch_CollOnOffSwitch(Qcode::P66B_20, "P66B_20"s, Qcode::Qs61, 1);
		ch_CollOnOffSwitch(Qcode::P66B_21, "P66B_21"s, Qcode::Qs61, 2);
		ch_CollOnOffSwitch(Qcode::P66B_22, "P66B_22"s, Qcode::Qs61, 3);
		ch_CollOnOffSwitch(Qcode::P66B_23, "P66B_23"s, Qcode::Qs61, 4);
		ch_CollOnOffSwitch(Qcode::P66B_24, "P66B_24"s, Qcode::Qs61, 5);
		ch_CollOnOffSwitch(Qcode::P66B_25, "P66B_25"s, Qcode::Qs61, 6);
		ch_CollOnOffSwitch(Qcode::P66B_26, "P66B_26"s, Qcode::Qs61, 7);
		ch_CollOnOffSwitch(Qcode::P66B_27, "P66B_27"s, Qcode::Qs61, 8);
		ch_CollOnOffSwitch(Qcode::P66B_28, "P66B_28"s, Qcode::Qs61, 9);
		ch_CollOnOffSwitch(Qcode::P66B_29, "P66B_29"s, Qcode::Qs61, 10);
		ch_CollOnOffSwitch(Qcode::P66B_30, "P66B_30"s, Qcode::Qs61, 11);
		ch_CollOnOffSwitch(Qcode::P66B_31, "P66B_31"s, Qcode::Qs61, 12);
		ch_CollOnOffSwitch(Qcode::P66B_32, "P66B_32"s, Qcode::Qs61, 13);
		ch_CollOnOffSwitch(Qcode::P66B_33, "P66B_33"s, Qcode::Qs61, 14);
		ch_CollOnOffSwitch(Qcode::P66B_34, "P66B_34"s, Qcode::Qs61, 15);
		ch_CollOnOffSwitch(Qcode::P66B_35, "P66B_35"s, Qcode::Qs61, 16);
		ch_CollOnOffSwitch(Qcode::P66B_36, "P66B_36"s, Qcode::Qs61, 17);

		ch_CollRotSwitchTerminal(Qcode::TFRCp, "TFRCp"s, Qcode::Qs104, 0, 0);
		ch_CollRotSwitchTerminal(Qcode::WxCp, "WxCp"s, Qcode::Qs104, 0, 1);
		ch_CollRotSwitchTerminal(Qcode::WxTCp, "WxTCp"s, Qcode::Qs104, 0, 2);
		ch_CollRotSwitchTerminal(Qcode::MAPCp, "MAPCp"s, Qcode::Qs104, 0, 3);
		ch_CollOnOffSwitch(Qcode::GCCp, "GCCp"s, Qcode::Qs104, 1);
		ch_CollOnOffSwitch(Qcode::WxAUTO, "WxAUTO"s, Qcode::Qs104, 2);
		ch_CollOnOffSwitch(Qcode::WxLR, "WxLR"s, Qcode::Qs104, 3);
		ch_CollOnOffSwitch(Qcode::WxTEST, "WxTEST"s, Qcode::Qs104, 4);
		ch_CollRotSwitchTerminal(Qcode::TFRFo, "TFRFo"s, Qcode::Qs104, 5, 0);
		ch_CollRotSwitchTerminal(Qcode::WxFo, "WxFo"s, Qcode::Qs104, 5, 1);
		ch_CollRotSwitchTerminal(Qcode::WxTFo, "WxTFo"s, Qcode::Qs104, 5, 2);
		ch_CollRotSwitchTerminal(Qcode::MAPFo, "MAPFo"s, Qcode::Qs104, 5, 3);
		ch_CollOnOffSwitch(Qcode::GCFo, "GCFo"s, Qcode::Qs104, 6);

		ch_CollPotentiometer270(Qcode::TiltCp, "TiltCp"s, Qcode::Qs105, 0);
		ch_CollRotSwitchTerminal(Qcode::GainCpMin, "GainCpMin"s, Qcode::Qs105, 1, 0);
		ch_CollRotSwitchTerminal(Qcode::GainCpM6, "GainCpM6"s, Qcode::Qs105, 1, 1);
		ch_CollRotSwitchTerminal(Qcode::GainCpM4, "GainCpM4"s, Qcode::Qs105, 1, 2);
		ch_CollRotSwitchTerminal(Qcode::GainCpM3, "GainCpM3"s, Qcode::Qs105, 1, 3);
		ch_CollRotSwitchTerminal(Qcode::GainCpM2, "GainCpM2"s, Qcode::Qs105, 1, 4);
		ch_CollRotSwitchTerminal(Qcode::GainCpM1, "GainCpM1"s, Qcode::Qs105, 1, 5);
		ch_CollRotSwitchTerminal(Qcode::GainCpNeu, "GainCpNeu"s, Qcode::Qs105, 1, 6);
		ch_CollRotSwitchTerminal(Qcode::GainCp2, "GainCp2"s, Qcode::Qs105, 1, 7);
		ch_CollRotSwitchTerminal(Qcode::GainCp4, "GainCp4"s, Qcode::Qs105, 1, 8);
		ch_CollRotSwitchTerminal(Qcode::GainCpMax, "GainCpMax"s, Qcode::Qs105, 1, 9);
		ch_CollPotentiometer270(Qcode::TiltFo, "TiltFo"s, Qcode::Qs105, 2);
		ch_CollRotSwitchTerminal(Qcode::GainFoMin, "GainFoMin"s, Qcode::Qs105, 3, 0);
		ch_CollRotSwitchTerminal(Qcode::GainFoM6, "GainFoM6"s, Qcode::Qs105, 3, 1);
		ch_CollRotSwitchTerminal(Qcode::GainFoM4, "GainFoM4"s, Qcode::Qs105, 3, 2);
		ch_CollRotSwitchTerminal(Qcode::GainFoM3, "GainFoM3"s, Qcode::Qs105, 3, 3);
		ch_CollRotSwitchTerminal(Qcode::GainFoM2, "GainFoM2"s, Qcode::Qs105, 3, 4);
		ch_CollRotSwitchTerminal(Qcode::GainFoM1, "GainFoM1"s, Qcode::Qs105, 3, 5);
		ch_CollRotSwitchTerminal(Qcode::GainFoNeu, "GainFoNeu"s, Qcode::Qs105, 3, 6);
		ch_CollRotSwitchTerminal(Qcode::GainFo2, "GainFo2"s, Qcode::Qs105, 3, 7);
		ch_CollRotSwitchTerminal(Qcode::GainFo4, "GainFo4"s, Qcode::Qs105, 3, 8);
		ch_CollRotSwitchTerminal(Qcode::GainFoMax, "GainFoMax"s, Qcode::Qs105, 3, 9);

		if (*g_CfgDualRCPLeft) {
			ch_CollInfiniteRotarySelector(Qcode::HFSensL, "HFSensL"s, 8, Qcode::Qs109, 0, *g_CfgInvertHfSensL);
			ch_CollInfiniteRotarySelector(Qcode::BigFreqKnobL, "BigFreqKnobL"s, 8, Qcode::Qs109, 1, *g_CfgInvertBigFreqKnobL);
			ch_CollInfiniteRotarySelector(Qcode::SmallFreqKnobL, "SmallFreqKnobL"s, 8, Qcode::Qs109, 2, *g_CfgInvertSmallFreqKnobL);
		} else {
			ch_CollInfiniteRotarySelector(Qcode::BigSmallFreqKnobL, "BigSmallFreqKnobL"s, 8, Qcode::Qs109, 0, *g_CfgInvertBigFreqKnobL);
			ch_CollOnOffSwitch(Qcode::BigSmallFreqSwitchL, "BigSmallFreqSwitchL"s, Qcode::Qs109, 1);
		}

		if (*g_CfgDualRCPCenter) {
			ch_CollInfiniteRotarySelector(Qcode::HFSensC, "HFSensC"s, 8, Qcode::Qs110, 0, *g_CfgInvertHfSensC);
			ch_CollInfiniteRotarySelector(Qcode::BigFreqKnobC, "BigFreqKnobC"s, 8, Qcode::Qs110, 1, *g_CfgInvertBigFreqKnobC);
			ch_CollInfiniteRotarySelector(Qcode::SmallFreqKnobC, "SmallFreqKnobC"s, 8, Qcode::Qs110, 2, *g_CfgInvertSmallFreqKnobC);
		} else {
			ch_CollInfiniteRotarySelector(Qcode::BigSmallFreqKnobC, "BigSmallFreqKnobC"s, 8, Qcode::Qs110, 0, *g_CfgInvertBigFreqKnobC);
			ch_CollOnOffSwitch(Qcode::BigSmallFreqSwitchC, "BigSmallFreqSwitchC"s, Qcode::Qs110, 1);
		}

		if (*g_CfgDualRCPRight) {
			ch_CollInfiniteRotarySelector(Qcode::HFSensR, "HFSensR"s, 8, Qcode::Qs111, 0, *g_CfgInvertHfSensR);
			ch_CollInfiniteRotarySelector(Qcode::BigFreqKnobR, "BigFreqKnobR"s, 8, Qcode::Qs111, 1, *g_CfgInvertBigFreqKnobR);
			ch_CollInfiniteRotarySelector(Qcode::SmallFreqKnobR, "SmallFreqKnobR"s, 8, Qcode::Qs111, 2, *g_CfgInvertSmallFreqKnobR);
		} else {
			ch_CollInfiniteRotarySelector(Qcode::BigSmallFreqKnobR, "BigSmallFreqKnobR"s, 8, Qcode::Qs111, 0, *g_CfgInvertBigFreqKnobR);
			ch_CollOnOffSwitch(Qcode::BigSmallFreqSwitchR, "BigSmallFreqSwitchR"s, Qcode::Qs111, 1);
		}

		ch_CollRotSwitchTerminal(Qcode::TcasPanOff, "TcasPanSelOff"s, Qcode::Qs117, 0, 0);
		ch_CollRotSwitchTerminal(Qcode::TcasPanOn, "TcasPanSelOn"s, Qcode::Qs117, 0, 1);
		ch_CollRotSwitchTerminal(Qcode::TcasPanL, "TcasPanSelL"s, Qcode::Qs117, 1, 0);
		ch_CollRotSwitchTerminal(Qcode::TcasPanR, "TcasPanSelR"s, Qcode::Qs117, 1, 1);
		ch_CollRotSwitchTerminal(Qcode::TcasPanAb, "TcasPanSelAb"s, Qcode::Qs117, 2, 0);
		ch_CollRotSwitchTerminal(Qcode::TcasPanNo, "TcasPanSelNo"s, Qcode::Qs117, 2, 1);
		ch_CollRotSwitchTerminal(Qcode::TcasPanBe, "TcasPanSelBe"s, Qcode::Qs117, 2, 2);
		ch_CollRotSwitchTerminal(Qcode::TcasPanStby, "TcasPanSelStby"s, Qcode::Qs117, 3, 0);
		ch_CollRotSwitchTerminal(Qcode::TcasPanXpdr, "TcasPanSelXpdr"s, Qcode::Qs117, 3, 1);
		ch_CollRotSwitchTerminal(Qcode::TcasPanTA, "TcasPanSelTA"s, Qcode::Qs117, 3, 2);
		ch_CollRotSwitchTerminal(Qcode::TcasPanRATA, "TcasPanSelRATA"s, Qcode::Qs117, 3, 3);

		ch_CollValueControlVar(Qcode::AcpRotSelVHF_L_L, "AcpRotSelVHF_L_L"s, MinMax{ 99 }, Qcode::Qs113, 0);
		ch_CollValueControlVar(Qcode::AcpRotSelVHF_C_L, "AcpRotSelVHF_C_L"s, MinMax{ 99 }, Qcode::Qs113, 1);
		ch_CollValueControlVar(Qcode::AcpRotSelVHF_R_L, "AcpRotSelVHF_R_L"s, MinMax{ 99 }, Qcode::Qs113, 2);
		ch_CollValueControlVar(Qcode::AcpRotSelFLT_L, "AcpRotSelFLT_L"s, MinMax{ 99 }, Qcode::Qs113, 3);
		ch_CollValueControlVar(Qcode::AcpRotSelCAB_L, "AcpRotSelCAB_L"s, MinMax{ 99 }, Qcode::Qs113, 4);
		ch_CollValueControlVar(Qcode::AcpRotSelPA_L, "AcpRotSelPA_L"s, MinMax{ 99 }, Qcode::Qs113, 5);
		ch_CollValueControlVar(Qcode::AcpRotSelHF_L_L, "AcpRotSelHF_L_L"s, MinMax{ 99 }, Qcode::Qs113, 6);
		ch_CollValueControlVar(Qcode::AcpRotSelHF_R_L, "AcpRotSelHF_R_L"s, MinMax{ 99 }, Qcode::Qs113, 7);
		ch_CollValueControlVar(Qcode::AcpRotSelSAT_L_L, "AcpRotSelSAT_L_L"s, MinMax{ 99 }, Qcode::Qs113, 8);
		ch_CollValueControlVar(Qcode::AcpRotSelSAT_R_L, "AcpRotSelSAT_R_L"s, MinMax{ 99 }, Qcode::Qs113, 9);
		ch_CollValueControlVar(Qcode::AcpRotSelSPKR_L, "AcpRotSelSPKR_L"s, MinMax{ 99 }, Qcode::Qs113, 10);
		ch_CollValueControlVar(Qcode::AcpRotSelVolVA_L, "AcpRotSelVolVA_L"s, MinMax{ 99 }, Qcode::Qs113, 11);
		ch_CollValueControlVar(Qcode::AcpRotSelVolAPP_L, "AcpRotSelVolAPP_L"s, MinMax{ 99 }, Qcode::Qs113, 12);
		ch_CollValueControlVar(Qcode::AcpRotSelVORADF_L, "AcpRotSelVORADF_L"s, MinMax{ 3 }, Qcode::Qs113, 13);
		ch_CollValueControlVar(Qcode::AcpRotVBR_L, "AcpRotVBR_L"s, MinMax{ 2 }, Qcode::Qs113, 14);
		ch_CollValueControlVar(Qcode::AcpRotIM_L, "AcpRotIM_L"s, MinMax{ 3 }, Qcode::Qs113, 15);

		ch_CollValueControlVar(Qcode::AcpRotSelVHF_L_C, "AcpRotSelVHF_L_C"s, MinMax{ 99 }, Qcode::Qs114, 0);
		ch_CollValueControlVar(Qcode::AcpRotSelVHF_C_C, "AcpRotSelVHF_C_C"s, MinMax{ 99 }, Qcode::Qs114, 1);
		ch_CollValueControlVar(Qcode::AcpRotSelVHF_R_C, "AcpRotSelVHF_R_C"s, MinMax{ 99 }, Qcode::Qs114, 2);
		ch_CollValueControlVar(Qcode::AcpRotSelFLT_C, "AcpRotSelFLT_C"s, MinMax{ 99 }, Qcode::Qs114, 3);
		ch_CollValueControlVar(Qcode::AcpRotSelCAB_C, "AcpRotSelCAB_C", MinMax{ 99 }, Qcode::Qs114, 4);
		ch_CollValueControlVar(Qcode::AcpRotSelPA_C, "AcpRotSelPA_C"s, MinMax{ 99 }, Qcode::Qs114, 5);
		ch_CollValueControlVar(Qcode::AcpRotSelHF_L_C, "AcpRotSelHF_L_C"s, MinMax{ 99 }, Qcode::Qs114, 6);
		ch_CollValueControlVar(Qcode::AcpRotSelHF_R_C, "AcpRotSelHF_R_C"s, MinMax{ 99 }, Qcode::Qs114, 7);
		ch_CollValueControlVar(Qcode::AcpRotSelSAT_L_C, "AcpRotSelSAT_L_C"s, MinMax{ 99 }, Qcode::Qs114, 8);
		ch_CollValueControlVar(Qcode::AcpRotSelSAT_R_C, "AcpRotSelSAT_R_C"s, MinMax{ 99 }, Qcode::Qs114, 9);
		ch_CollValueControlVar(Qcode::AcpRotSelSPKR_C, "AcpRotSelSPKR_C"s, MinMax{ 99 }, Qcode::Qs114, 10);
		ch_CollValueControlVar(Qcode::AcpRotSelVolVA_C, "AcpRotSelVolVA_C"s, MinMax{ 99 }, Qcode::Qs114, 11);
		ch_CollValueControlVar(Qcode::AcpRotSelVolAPP_C, "AcpRotSelVolAPP_C"s, MinMax{ 99 }, Qcode::Qs114, 12);
		ch_CollValueControlVar(Qcode::AcpRotSelVORADF_C, "AcpRotSelVORADF_C"s, MinMax{ 3 }, Qcode::Qs114, 13);
		ch_CollValueControlVar(Qcode::AcpRotVBR_C, "AcpRotVBR_C"s, MinMax{ 2 }, Qcode::Qs114, 14);
		ch_CollValueControlVar(Qcode::AcpRotIM_C, "AcpRotIM_C"s, MinMax{ 3 }, Qcode::Qs114, 15);

		ch_CollValueControlVar(Qcode::AcpRotSelVHF_L_R, "AcpRotSelVHF_L_R"s, MinMax{ 99 }, Qcode::Qs115, 0);
		ch_CollValueControlVar(Qcode::AcpRotSelVHF_C_R, "AcpRotSelVHF_C_R"s, MinMax{ 99 }, Qcode::Qs115, 1);
		ch_CollValueControlVar(Qcode::AcpRotSelVHF_R_R, "AcpRotSelVHF_R_R"s, MinMax{ 99 }, Qcode::Qs115, 2);
		ch_CollValueControlVar(Qcode::AcpRotSelFLT_R, "AcpRotSelFLT_R"s, MinMax{ 99 }, Qcode::Qs115, 3);
		ch_CollValueControlVar(Qcode::AcpRotSelCAB_R, "AcpRotSelCAB_R"s, MinMax{ 99 }, Qcode::Qs115, 4);
		ch_CollValueControlVar(Qcode::AcpRotSelPA_R, "AcpRotSelPA_R"s, MinMax{ 99 }, Qcode::Qs115, 5);
		ch_CollValueControlVar(Qcode::AcpRotSelHF_L_R, "AcpRotSelHF_L_R"s, MinMax{ 99 }, Qcode::Qs115, 6);
		ch_CollValueControlVar(Qcode::AcpRotSelHF_R_R, "AcpRotSelHF_R_R"s, MinMax{ 99 }, Qcode::Qs115, 7);
		ch_CollValueControlVar(Qcode::AcpRotSelSAT_L_R, "AcpRotSelSAT_L_R", MinMax{ 99 }, Qcode::Qs115, 8);
		ch_CollValueControlVar(Qcode::AcpRotSelSAT_R_R, "AcpRotSelSAT_R_R"s, MinMax{ 99 }, Qcode::Qs115, 9);
		ch_CollValueControlVar(Qcode::AcpRotSelSPKR_R, "AcpRotSelSPKR_R"s, MinMax{ 99 }, Qcode::Qs115, 10);
		ch_CollValueControlVar(Qcode::AcpRotSelVolVA_R, "AcpRotSelVolVA_R"s, MinMax{ 99 }, Qcode::Qs115, 11);
		ch_CollValueControlVar(Qcode::AcpRotSelVolAPP_R, "AcpRotSelVolAPP_R"s, MinMax{ 99 }, Qcode::Qs115, 12);
		ch_CollValueControlVar(Qcode::AcpRotSelVORADF_R, "AcpRotSelVORADF_R"s, MinMax{ 3 }, Qcode::Qs115, 13);
		ch_CollValueControlVar(Qcode::AcpRotVBR_R, "AcpRotVBR_R"s, MinMax{ 2 }, Qcode::Qs115, 14);
		ch_CollValueControlVar(Qcode::AcpRotIM_R, "AcpRotIM_R"s, MinMax{ 3 }, Qcode::Qs115, 15);

		ch_CollContinuousValueControlVar(Qcode::ElevatorStick, "ElevatorStick"s, MinMax{ -999, 999 }, Qcode::Qs120, 0);
		ch_CollContinuousValueControlVar(Qcode::AileronControlW, "AileronControlW"s, MinMax{ -999, 999 }, Qcode::Qs120, 1);
		ch_CollContinuousValueControlVar(Qcode::RudderPedals, "RudderPedals"s, MinMax{ -999, 999 }, Qcode::Qs120, 2);

		ch_CollContinuousValueControlVar(Qcode::ToeBrakeLeft, "ToeBrakeLeft"s, MinMax{ 1'000 }, Qcode::Qs357, 0);
		ch_CollContinuousValueControlVar(Qcode::ToeBrakeRight, "ToeBrakeRight"s, MinMax{ 1'000 }, Qcode::Qs357, 1);

		ch_CollContinuousValueControlVar(Qcode::Tla1, "Tla1"s, MinMax{ -8'925, 5'000 }, Qcode::Qs436, 0);
		ch_CollContinuousValueControlVar(Qcode::Tla2, "Tla2"s, MinMax{ -8'925, 5'000 }, Qcode::Qs436, 1);
		ch_CollContinuousValueControlVar(Qcode::Tla3, "Tla3"s, MinMax{ -8'925, 5'000 }, Qcode::Qs436, 2);
		ch_CollContinuousValueControlVar(Qcode::Tla4, "Tla4"s, MinMax{ -8'925, 5'000 }, Qcode::Qs436, 3);
	}


	auto VarPoolImpl::getVarPtr(const Qcode q) const -> SharedVarPtr {
		//
		// ASSUMPTION:    object with key var may not exit! 
		// SPECIFICATION: return nullptr if object does not exist.
		//     (so varpool[var] is not possible here, it would insert a new object at position var...)
		//
		if (const auto it{ pool_.find(q) }; it != pool_.end()) {  
			return (*it).second;
		}
    return static_cast<SharedVarPtr>(nullptr);
	}

	auto VarPoolImpl::findAndEraseInactives(QcodeSet const& actives) -> void {
		QcodeSet inactives;
		std::ranges::for_each(std::as_const(pool_), [&actives, &inactives](auto const& p) -> void { 
			if (!actives.contains(p.first)) { inactives.insert(p.first); }
													});
		std::ranges::for_each(inactives, [this](auto q) -> void {
			pool_.erase(q); }
		);
		inactives.clear();
	}

	static auto lf_addSp(std::string const& s, uint32_t num) -> std::string	{
		std::string q{ s };
		while (q.size() < num) {
			q += gkSpaceChar;
		}
		return q;
	}

	static auto lf_logVarNumbers(SharedVarPtrVector const& v, std::list<std::string>& li) -> void {
		std::string line;
		auto counter{ 0 };
		for (const auto& p : v) {
			line += gf_Qcode2Str((*p).getQcode()) + gkSpaceStr + gkDoubleQuoteStr + lf_addSp((*p).getName() + gkDoubleQuoteStr, 20);
			++counter;
			if (0 == (counter % 5)) {
				counter = 0;
				li.push_back(line);
				line.clear();
			}
		};
		if (counter != 0) {
			li.push_back(line);
		}
	}

	auto VarPoolImpl::logVarnumbers() -> void {
		SharedVarPtrVector v;
		std::ranges::for_each(std::as_const(pool_), [&v](auto const& sp) -> auto {v.push_back(sp.second); });
		lf_logVarNumbers(v, varnumbers_);
	}

#ifdef LIST_SIOC_VARS
	static auto lf_Qcode3Dig(std::string const& s) -> std::string {
		// change Qi2 => Qi002  and so on
		std::string r;
		const auto l{ s.length() };
		if (l == 3) {
			r = s.substr(0, 2) + gkDigit0Str + gkDigit0Str + s.substr(2, 1);
		} else if (l == 4) {
			r = s.substr(0, 2) + gkDigit0Str + s.substr(2, 2);
		} else {
			r = s;
		}
		return r;
	}



	auto VarPoolImpl::getSIOCVarsList(std::list<std::string>& li) const -> void {
		// Qcode as order
			const auto kc{ ", "s };
			const int32_t knameL{ 20 };
			const int32_t kQcL{ 5 };
			const int32_t kSIOCL{ 4 };
			const int32_t kCntrlL{ 11 };
			for (auto const& sp : pool_) {
				const auto p{ sp.second };
				if (const auto q{ (*p).getQcode() }; (q >= Qcode::Qh0) && (!gf_isInternalQhvar(q))) {
					auto qcode{ gf_var2Qcode(q) };
					if (qcode.empty()) {
						qcode = (*p).getExpandedQcode();
					}
					const bool infov{ (*p).isInfoVar() };
					li.emplace_back(lf_addSp(lf_Qcode3Dig(qcode), kQcL) + kc + lf_addSp((*p).getName(), knameL) + kc + lf_addSp(gf_Qcode2Str(q), kSIOCL) + kc +
						lf_addSp((infov ? gkEmptyStr : (*p).getType()), kCntrlL) + kc +
						(infov ? (*p).getType() : gkEmptyStr));
				}
			}
	}
#endif

	auto VarPoolImpl::createInicioVars() -> void {
    std::ranges::for_each(std::as_const(pool_), [this](auto const& sp) -> auto {	
			if (!(*(sp.second)).isInfoVar()) {  // is it a ControlVar?
				inicio_.insert(sp.first);
			};});
	}

	auto VarPoolImpl::createInicioPsxVars() -> void {
    std::ranges::for_each(std::as_const(pool_), [this](auto const& sp) -> auto {	inicio_psx_.insert(sp.first); });
	}

	auto VarPoolImpl::preInitAllVars() const -> void {
		std::ranges::for_each(std::as_const(pool_), [](auto const& sp) -> auto {	(*(sp.second)).preInitialise();});
	}

	VarPool::VarPool()
		:
		pImpl_(std::make_unique<VarPoolImpl>()) {
	}

#ifdef LIST_SIOC_VARS
	auto VarPool::generateSIOCVarsList() -> void {
		std::list<std::string> m1;
    g_CfgDualRCPLeft.value_ = true;
    g_CfgDualRCPCenter.value_ = true;
    g_CfgDualRCPRight.value_ = true;
		(*pImpl_).createPsxseeconVars();
    (*pImpl_).getSIOCVarsList(m1);
		(*pImpl_).pool_.clear();
    g_CfgDualRCPLeft.value_ = false;
    g_CfgDualRCPCenter.value_ = false;
    g_CfgDualRCPRight.value_ = false;
    (*pImpl_).createPsxseeconVars();
    (*pImpl_).createInternalVars();
    (*pImpl_).getSIOCVarsList(m1);
    m1.sort();
    m1.unique();
    const auto numvars{m1.size()};
    for (auto& s : m1) {
      // remove leading zeroes in Qcodes (they were only inserted there for sorting)
      if ((s.at(2) == gk0Char) && (s.at(3) == gk0Char)) {
        s.at(2) = s.at(4);
        s.at(3) = gkSpaceChar;
        s.at(4) = gkSpaceChar;
      } else if (s.at(2) == gk0Char) {
        s.at(2) = s.at(3);
        s.at(3) = s.at(4);
        s.at(4) = gkSpaceChar;
      }
    };
    m1.push_front("[" + std::format("{}", numvars) + " SIOC variables for PSX, by Qcode, version " + std::string(VER_FILEVERSION_STR) + "]");
    const auto file{std::filesystem::path("SIOC_variables_by_Qcode.txt"s)};
    if (TxtFileWriter w; w.openFile(file)) {
      w.writeStringList(m1);
      g_log.line(file.string() + " written, with " + std::format("{}", numvars) + " SIOC variables.");
    }
	}
#endif


	auto VarPool::configurePool(QcodeSet const& vars_found_in_configfile) const -> void {
		(*pImpl_).createPsxseeconVars();

		QcodeSet actives(vars_found_in_configfile);   // remove Qs117?  No not necessary, ssifile reader does not recognize Qhs varnums! Why should it?
		(*pImpl_).findAndEraseInactives(actives);
		if ((*pImpl_).pool_.empty()) {  // in case actives were invalid lpxseecon vars
			throw UserErrorException(1, "No variables found in your CONFIG_FILE"s);
		}
		(*pImpl_).createInicioVars();
		(*pImpl_).total_vars_ = static_cast<uint32_t>((*pImpl_).pool_.size());
		(*pImpl_).logVarnumbers();

    (*pImpl_).createInternalVarDefs();
    (*pImpl_).createInternalVars();
		for (auto const& t : (*pImpl_).internalvar_defs_) {
			const auto ivar{ t.internalvar_number };
			for (auto q : t.terminals) {
				if (actives.find(q) != actives.end()) {
					actives.insert(ivar);
					(*pImpl_).internalvars_.insert(ivar);
					break;
				}
			};
		};
		(*pImpl_).findAndEraseInactives(actives);
		(*pImpl_).createInicioPsxVars();
		actives.clear();
	}

	auto VarPool::getVarPtr(const Qcode q) const -> const SharedVarPtr {
		return (*pImpl_).getVarPtr(q);
	}

	auto VarPool::getInicio() const noexcept -> QcodeSet const& {
		return (*pImpl_).inicio_;
	}

	auto VarPool::getInicioPsx() const noexcept -> QcodeSet const& {
		return (*pImpl_).inicio_psx_;
	}

	auto VarPool::getNumberOfVars() const noexcept -> uint32_t {
		return (*pImpl_).total_vars_;
	}

	auto VarPool::getInternalvarsAdded()const noexcept -> QcodeSet const& {
		return (*pImpl_).internalvars_;
	}

	auto VarPool::preInitAllVars() const -> void {
		(*pImpl_).preInitAllVars();
	}

	auto VarPool::getVarNumbersForLog() const noexcept -> const std::list<std::string>& {
		return (*pImpl_).varnumbers_;
	}

}  
