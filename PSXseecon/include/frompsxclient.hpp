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
	class FromPSXClient final : public FromTCPIPClient {
	public:
		FromPSXClient();
		~FromPSXClient() = default;
		auto initInicio(QcodeSet const& registeredvars) -> void;
		auto decodeMessage(std::string const& line) -> void override;

	protected:
		auto flushElement(QcodeValueData const& e) -> void override;

	private:
		const std::regex kSemiColonSeparator{ R"([^;]+)" };
		const static RefToSharedVarPtrFunctionWithQcodePar varptrfie_;
		auto decodeOneMessage(std::string const& msg) -> void;  // decodes the PSX router protocol std::string in parameter line 

		static auto canBeSkipped(std::string const& s) -> bool;
		auto decodeQs(const Qcode q, std::string const& s) -> void;
		auto decodeNonQcode(std::string const& line) -> void;
		auto decodeQsn(std::string const& s, const Qcode q, const uint32_t n) -> void;
		auto decodeQsnSv(const std::vector<std::string>& sv, const Qcode q) -> void;
		auto decodeQsEfisActL(std::string const& s) -> void;
		auto decodeQsEfisActR(std::string const& s) -> void;
		auto decodeQs106_108(std::string const& s, const uint32_t index, const Qcode start) -> void;
		auto decodeQs118(std::string const& s) -> void;
		auto decodeQs120(std::string const& s) -> void;
		auto decodeQs121(std::string const& s) -> void;
		auto decodeQs357(std::string const& s) -> void;
		auto decodeQs434(std::string const& s) -> void;
		auto decodeQs436(std::string const& s) -> void;
    auto decodeQs457(std::string const& s) -> void;
    auto decodeQs458_460(std::string const& s, Qcode rcpa, Qcode rcps) -> void;
    auto decodeQs480(std::string const& s) -> void;
		static auto decodeRcpDisp(std::string const& s) -> int32_t;
		auto handleVirtualVar(QcodeValueData const& e) -> void;
		bool Qs106_ = false;
		bool Qs107_ = false;
		bool Qs108_ = false;
		bool Qs118_ = false;
		bool Qs120_0_ = false;
		bool Qs120_1_ = false;
		bool Qs120_2_ = false;
		bool Qs120_ = false;
		bool Qs121_ = false;
		bool Qs124_ = false;
		bool Qs125_ = false;
		bool Qs126_ = false;
		bool Qs127_ = false;
		bool Qs128_ = false;
		bool Qs129_ = false;
    bool Qs321_ = false;
    bool Qs328_ = false;
		bool Qs329_ = false;
		bool Qs330_ = false;
		bool Qs331_ = false;
		bool Qs332_ = false;
		bool Qs333_ = false;
		bool Qs334_ = false;
		bool Qs335_ = false;
		bool Qs357_0_ = false;
		bool Qs357_1_ = false;
		bool Qs357_ = false;
		bool Qs428_ = false;
		bool Qs429_ = false;
		bool Qs431_ = false;
		bool Qs432_ = false;
		bool Qs434_ = false;
		bool Qs436_ = false;
		bool Qs436Virtual_ = false;
		bool Qs438_ = false;
		bool Qs448_ = false;
		bool Qs449_ = false;
    bool Qs457_ = false;
    bool Qs458_ = false;
		bool Qs459_ = false;
		bool Qs460_ = false;
		bool QiCallFr_ = false;
		bool Qi175Needed_ = false;
		bool AutobrVirtual_ = false;
		bool GearLockOvrdVirtual_ = false;
		bool EngStartSw1Virtual_ = false;
		bool EngStartSw2Virtual_ = false;
		bool EngStartSw3Virtual_ = false;
		bool EngStartSw4Virtual_ = false;
		bool Tla1Virtual_ = false;
		bool Tla2Virtual_ = false;
		bool Tla3Virtual_ = false;
		bool Tla4Virtual_ = false;
		bool TlaMotor_ = false;
		bool TlaMotorOrQs436Virtual_ = false;
		bool TlaMotorOrQs434OrQs436_ = false;
		bool FireEng1Virtual_ = false;
		bool FireEng2Virtual_ = false;
		bool FireEng3Virtual_ = false;
		bool FireEng4Virtual_ = false;
		bool FireApuVirtual_ = false;
		bool CallPanelVirtual_ = false;
		bool LtIndTestVirtual_ = false;
		bool AileronsVirtual_ = false;
		bool ElevatorVirtual_ = false;
		bool RudderVirtual_ = false;
		bool ToeBrakeLeftVirtual_ = false;
    bool ToeBrakeRightVirtual_ = false;
    bool ParkBrkLevVirtual_ = false;
		bool Qs120T_ = false;
		bool Qs357T_ = false;

		int32_t ATActive_ = 0;
		std::array<SharedVarPtr, static_cast<uint32_t>(Qcode::QCODE_DUMMY_END)> varpool_;
	};

  inline std::array<int32_t, static_cast<uint32_t>(Qcode::QCODE_DUMMY_END)> g_dbp;
  inline std::array<std::string, static_cast<uint32_t>(Qcode::QsSIZE)> g_str;

  inline constinit bool g_TcasMDT{ false };
  inline constinit bool g_TcasBlank{ false };
  inline constinit bool g_TcasPass{ false };
  inline constinit bool g_XpdrFail{ false };
  inline constinit int32_t g_TcasNumDigits{ 0 };
  inline constinit bool g_EtCpBlank{ true };
  inline constinit bool g_EtFoBlank{ true };
  inline constinit bool g_gearUp{ false };
  inline constinit bool g_DemandForQi211{ false };
  inline constinit bool g_DemandForQi214{ false };
  inline constinit bool g_DemandForQi271{ false };
	inline constinit bool g_DemandForQi273{ false };
	inline constinit bool g_DemandForQi274{ false };
	inline constinit bool g_DemandForQs480{ false };
  inline constinit bool g_DemandForQs483{ false };
  inline constinit std::array<bool, 3> g_amL{ false, false, false };  // Left, Center, Right
  inline constinit std::array<bool, 3> g_amR{ false, false, false };  // ,,

	inline constexpr int32_t gkNotUsedControl = 3'737;

  inline constinit int32_t g_ElevatorStick{ gkNotUsedControl };
  inline constinit int32_t g_AileronControlW{ gkNotUsedControl };
  inline constinit int32_t g_RudderPedals{ gkNotUsedControl };
  inline constinit int32_t g_ToeBrakeLeft{ gkNotUsedControl };
  inline constinit int32_t g_ToeBrakeRight{ gkNotUsedControl };

}  // namespace nwk
