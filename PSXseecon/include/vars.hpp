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
	class Var {
	public:
		Var() = delete;
		virtual ~Var() = default;
		Var(const Var& v) = delete;
		Var(Var&& v) = delete;
		auto operator=(const Var& v) -> Var& = delete;   // prevent assignments 
		auto operator=(Var&& v) -> Var& = delete;  //  ,,

		virtual auto preInitialise() -> void = 0;

		// Is called at the time all the objects have been created.
		// This gives us an opportunity to set links to other objects
		// (such as Internal Objects)

		virtual auto check() -> void = 0;

		// Is called regularly (in the polling process),
		// this is as well the case for infovars as for leading control vars.


		virtual auto act() -> void = 0;

		// acts upon receiving a new value, either to the PSX (control vars) or to SIOC.


		virtual auto setValue(const int32_t v) -> void = 0;

		// Sets  the new value.
		// This method is virtual because subclasses may overrule to add range checking (for instance).
		// A Var can get a new value with method setValue, either in the
		// check method, or upon receival of a SIOC var-value change, then the
		// Setvalue method is called from the FromSiocClient object.

		virtual auto isInfoVar() const noexcept -> bool = 0;
		// or controlvar if false,  Note: Infovar needs checking at 5Hz (polling)
		virtual auto needsCheck() const noexcept -> bool = 0;   // not all Infovars need checking ...

		auto getName() const noexcept -> std::string const&;
		virtual auto getType() const -> std::string;
		virtual auto getExpandedQcode() const -> std::string;
		auto getQcode() const noexcept -> Qcode;
		auto getValue() const noexcept -> int32_t;

	protected:
		Var(const Qcode var, std::string name);
		const std::string name_;
		const Qcode qcode_;
		int32_t val_{ 0 };
	};

	auto gf_var2Qcode(Qcode q) -> std::string;

	using SharedVarPtr = std::shared_ptr<Var>;
	using SharedVarPtrVector = std::vector<SharedVarPtr>;
	using RefToSharedVarPtrFunctionWithQcodePar = std::function<SharedVarPtr(Qcode)>;

	using CDUArray = std::array<uint32_t, 89>;

	auto updateOCCduKeys(const CDUArray& codes) noexcept -> void;

	auto gf_getIFRData(QcodeValueData& e) -> bool;

	enum class LCR : uint32_t {
		left, center, right
	};

	struct MinMax final {
		int32_t min;
		int32_t max;
		MinMax(const int32_t min, const int32_t max) noexcept;
		MinMax(const int32_t max) noexcept;  // min = 0
	};

	struct DeltaColVal {
		nwk::Qcode qcode;
		std::string val;
	};

	auto gf_getDCVData(DeltaColVal& e) -> bool;

	class ControlVar : public Var {
	public:
		~ControlVar() = default;

		ControlVar(ControlVar const& v) = delete;
		auto operator=(ControlVar const& v) -> ControlVar& = delete;
		ControlVar(ControlVar && v) = delete;
		auto operator=(ControlVar && v) -> ControlVar& = delete;

		auto preInitialise() -> void override;
		auto setValue(const int32_t v) -> void override;
		auto check() -> void override;
		auto act() -> void override;
		auto isInfoVar() const noexcept -> bool final;
		auto needsCheck() const noexcept -> bool final;
		virtual auto sendToPSX() const -> void;
		virtual auto sendToPSX(const int32_t value) const -> void;
		static bool syncPhase_;

	protected:
		ControlVar(const Qcode q, std::string const& name);
		static auto controllingObjectError(Qcode q) -> void;
		static const RefToSharedVarPtrFunctionWithQcodePar varptrfie_;
		static const std::shared_ptr<Client> toPSXPtr_;
	};

	class MomentaryActionSwitch : public ControlVar {
	public:
		MomentaryActionSwitch(const Qcode q, std::string const& name);
		~MomentaryActionSwitch() = default;

		MomentaryActionSwitch(MomentaryActionSwitch const& v) = delete;
		auto operator=(MomentaryActionSwitch const& v) -> MomentaryActionSwitch& = delete;
		MomentaryActionSwitch(MomentaryActionSwitch && v) = delete;
		auto operator=(MomentaryActionSwitch && v) -> MomentaryActionSwitch& = delete;

		auto act() -> void override;
		auto setValue(const int32_t v) -> void override;
		auto getType() const -> std::string override;
	};

	class AlternateActionSwitch : public ControlVar {
	public:
		AlternateActionSwitch(const Qcode q, std::string const& name);
		~AlternateActionSwitch() = default;

		AlternateActionSwitch(AlternateActionSwitch const& v) = delete;
		auto operator=(AlternateActionSwitch const& v) -> AlternateActionSwitch& = delete;
		AlternateActionSwitch(AlternateActionSwitch && v) = delete;
		auto operator=(AlternateActionSwitch && v) -> AlternateActionSwitch& = delete;

		auto setValue(const int32_t v) -> void override;
		auto getType() const -> std::string override;
	};

	class ValueControlVar : public ControlVar {
	public:
		ValueControlVar(const Qcode q, std::string const& name, const MinMax minmax);
		~ValueControlVar() = default;

		ValueControlVar(ValueControlVar const& v) = delete;
		auto operator=(ValueControlVar const& v) -> ValueControlVar& = delete;
		ValueControlVar(ValueControlVar && v) = delete;
		auto operator=(ValueControlVar && v) -> ValueControlVar& = delete;

		auto setValue(const int32_t v) -> void override;
		auto getType() const -> std::string override;

	private:
		const MinMax minmax_;
	};

	class ContinuousValueControlVar : public ControlVar {
	public:
		ContinuousValueControlVar(const Qcode q, std::string const& name, const MinMax minmax);
		~ContinuousValueControlVar() = default;

		ContinuousValueControlVar(ContinuousValueControlVar const& v) = delete;
		auto operator=(ContinuousValueControlVar const& v) -> ContinuousValueControlVar& = delete;
		ContinuousValueControlVar(ContinuousValueControlVar && v) = delete;
		auto operator=(ContinuousValueControlVar && v) -> ContinuousValueControlVar& = delete;

		auto setValue(const int32_t v) -> void override;
		auto getType() const -> std::string override;

	private:
		const MinMax minmax_;
		const int32_t delta_;
	};

	class Potentiometer270ControlVar : public ControlVar {
	public:
		Potentiometer270ControlVar(const Qcode q, std::string const& name);
		~Potentiometer270ControlVar() = default;

		Potentiometer270ControlVar(Potentiometer270ControlVar const& v) = delete;
		auto operator=(Potentiometer270ControlVar const& v) -> Potentiometer270ControlVar& = delete;
		Potentiometer270ControlVar(Potentiometer270ControlVar && v) = delete;
		auto operator=(Potentiometer270ControlVar && v) -> Potentiometer270ControlVar& = delete;

		auto setValue(const int32_t v) -> void override;
		auto getType() const -> std::string override;

	private:
		static constexpr int32_t kMax_ = 4713;
		static constexpr int32_t kMin_ = 0;
		static constexpr int32_t kDelta_ = 47;
	};

	class ExpControlVar : public ControlVar {
	public:
		~ExpControlVar() = default;

		ExpControlVar(ExpControlVar const& v) = delete;
		auto operator=(ExpControlVar const& v) -> ExpControlVar& = delete;
		ExpControlVar(ExpControlVar && v) = delete;
		auto operator=(ExpControlVar && v) -> ExpControlVar& = delete;

		auto act() -> void override;
		auto getExpandedQcode() const -> std::string override;
		auto sendToPSX(const int32_t value) const -> void override;

	protected:
		ExpControlVar(const Qcode q, std::string const& name, const Qcode expqcode);
		const Qcode expqcode_;
	};

	class MalFunctionControlVar final : public ExpControlVar {
	public:
		MalFunctionControlVar(const Qcode q, std::string const& name, const Qcode expqcode);
		~MalFunctionControlVar() = default;

		MalFunctionControlVar(MalFunctionControlVar const& v) = delete;
		auto operator=(MalFunctionControlVar const& v) -> MalFunctionControlVar& = delete;
		MalFunctionControlVar(MalFunctionControlVar && v) = delete;
		auto operator=(MalFunctionControlVar && v) -> MalFunctionControlVar& = delete;

		auto getType() const -> std::string override;
		auto act() -> void override;
	};

	class QiControlVar : public ExpControlVar {
	public:
		QiControlVar(const Qcode q, std::string const& name, const Qcode expqcode);
		~QiControlVar() = default;

		QiControlVar(QiControlVar const& v) = delete;
		auto operator=(QiControlVar const& v) -> QiControlVar& = delete;
		QiControlVar(QiControlVar && v) = delete;
		auto operator=(QiControlVar && v) -> QiControlVar& = delete;

		auto getType() const -> std::string override;
	};

	class QiValueControlVar : public QiControlVar {
	public:
		QiValueControlVar(const Qcode q, std::string const& name, const Qcode expqcode, const MinMax minmax);
		~QiValueControlVar() = default;

		QiValueControlVar(QiValueControlVar const& v) = delete;
		auto operator=(QiValueControlVar const& v) -> QiValueControlVar& = delete;
		QiValueControlVar(QiValueControlVar && v) = delete;
		auto operator=(QiValueControlVar && v) -> QiValueControlVar& = delete;

		auto getType() const -> std::string override;
		auto setValue(const int32_t v) -> void override;

	private:
		const MinMax minmax_;
	};

	class QiMomentaryActionSwitch : public QiControlVar {
	public:
		QiMomentaryActionSwitch(const Qcode q, std::string const& name, const Qcode expqcode);
		~QiMomentaryActionSwitch() = default;

		QiMomentaryActionSwitch(QiMomentaryActionSwitch const& v) = delete;
		auto operator=(QiMomentaryActionSwitch const& v) -> QiMomentaryActionSwitch& = delete;
		QiMomentaryActionSwitch(QiMomentaryActionSwitch && v) = delete;
		auto operator=(QiMomentaryActionSwitch && v) -> QiMomentaryActionSwitch& = delete;

		auto setValue(const int32_t v) -> void override;
		auto act() -> void override;
		auto getType() const -> std::string override;
	};

	class QiAlternateActionSwitch : public QiControlVar {
	public:
		QiAlternateActionSwitch(const Qcode q, std::string const& name, const Qcode expqcode);
		~QiAlternateActionSwitch() = default;

		QiAlternateActionSwitch(QiAlternateActionSwitch const& v) = delete;
		auto operator=(QiAlternateActionSwitch const& v) -> QiAlternateActionSwitch& = delete;
		QiAlternateActionSwitch(QiAlternateActionSwitch && v) = delete;
		auto operator=(QiAlternateActionSwitch && v) -> QiAlternateActionSwitch& = delete;

		auto setValue(const int32_t v) -> void override;
		auto getType() const -> std::string override;
	};

	class QiDeltaControlVar final : public QiMomentaryActionSwitch {
	public:
		QiDeltaControlVar(const Qcode q, std::string const& name, const Qcode expqcode);
		~QiDeltaControlVar() = default;

		QiDeltaControlVar(QiDeltaControlVar const& v) = delete;
		auto operator=(QiDeltaControlVar const& v) -> QiDeltaControlVar& = delete;
		QiDeltaControlVar(QiDeltaControlVar && v) = delete;
		auto operator=(QiDeltaControlVar && v) -> QiDeltaControlVar& = delete;

		auto setValue(const int32_t v) -> void override;
	};

	class CduKeyboardVar final : public ExpControlVar {  // ! can only be used for Qh controls that do not need to be re-sent, such as Keyboard values -1 .. 68
	public:
		CduKeyboardVar(const Qcode q, std::string const& name, const Qcode expqcode);
		~CduKeyboardVar() = default;

		CduKeyboardVar(CduKeyboardVar const& v) = delete;
		auto operator=(CduKeyboardVar const& v) -> CduKeyboardVar& = delete;
		CduKeyboardVar(CduKeyboardVar && v) = delete;
		auto operator=(CduKeyboardVar && v) -> CduKeyboardVar& = delete;

		auto setValue(const int32_t v) -> void override;
		auto getType() const -> std::string override;
	};

	class GuaMonToggleType2 final : public AlternateActionSwitch {
	public:
		GuaMonToggleType2(const Qcode q, std::string const& name);
		~GuaMonToggleType2() = default;

		GuaMonToggleType2(GuaMonToggleType2 const& v) = delete;
		auto operator=(GuaMonToggleType2 const& v) -> GuaMonToggleType2& = delete;
		GuaMonToggleType2(GuaMonToggleType2 && v) = delete;
		auto operator=(GuaMonToggleType2 && v) -> GuaMonToggleType2& = delete;

		auto act() -> void override;
	};

	class InfiniteRotarySelector : public ControlVar {
	public:
		InfiniteRotarySelector(const Qcode q, std::string const& name, const int32_t range, const bool direction);
		~InfiniteRotarySelector() = default;

		InfiniteRotarySelector(InfiniteRotarySelector const& v) = delete;
		auto operator=(InfiniteRotarySelector const& v) -> InfiniteRotarySelector& = delete;
		InfiniteRotarySelector(InfiniteRotarySelector && v) = delete;
		auto operator=(InfiniteRotarySelector && v) -> InfiniteRotarySelector& = delete;

		auto act() -> void override;
		auto getType() const -> std::string override;

	protected:
		const int32_t max_delta_low_;
		const int32_t max_delta_high_;
		const bool direction_;
	};

	class AutoStart1234Var final : public AlternateActionSwitch {
	public:
		AutoStart1234Var(const Qcode q, std::string const& name);
		~AutoStart1234Var() = default;

		AutoStart1234Var(AutoStart1234Var const& v) = delete;
		auto operator=(AutoStart1234Var const& v) -> AutoStart1234Var& = delete;
		AutoStart1234Var(AutoStart1234Var && v) = delete;
		auto operator=(AutoStart1234Var && v) -> AutoStart1234Var& = delete;

		auto act() -> void override;
	};

	class CollectionVar : public ControlVar {
	public:
		CollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements);
		~CollectionVar() = default;

		CollectionVar(CollectionVar const& v) = delete;
		auto operator=(CollectionVar const& v) -> CollectionVar& = delete;
		CollectionVar(CollectionVar && v) = delete;
		auto operator=(CollectionVar && v) -> CollectionVar& = delete;

		auto preInitialise() -> void override;
		virtual auto setElementValue(const uint32_t element_number, const int32_t value) -> void;
		auto getType() const -> std::string override;

	protected:
		const uint32_t num_elements_;
		std::vector<int32_t> elements_;
	};

	class QsECONCollectionVar : public CollectionVar {
	public:
		QsECONCollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements);
		~QsECONCollectionVar() = default;

		QsECONCollectionVar(QsECONCollectionVar const& v) = delete;
		auto operator=(QsECONCollectionVar const& v) -> QsECONCollectionVar& = delete;
		QsECONCollectionVar(QsECONCollectionVar && v) = delete;
		auto operator=(QsECONCollectionVar && v) -> QsECONCollectionVar& = delete;

		auto act() -> void override;
	};

	class Qs120Var final : public QsECONCollectionVar {
	public:
		Qs120Var(const Qcode q, std::string const& name, const uint32_t num_elements);
		~Qs120Var() = default;

		Qs120Var(Qs120Var const& v) = delete;
		auto operator=(Qs120Var const& v) -> Qs120Var& = delete;
		Qs120Var(Qs120Var && v) = delete;
		auto operator=(Qs120Var && v) -> Qs120Var& = delete;

		auto act() -> void override;
	};

	class Qs357Var final : public QsECONCollectionVar {
	public:
		Qs357Var(const Qcode q, std::string const& name, const uint32_t num_elements);
		~Qs357Var() = default;

		Qs357Var(Qs357Var const& v) = delete;
		auto operator=(Qs357Var const& v) -> Qs357Var& = delete;
		Qs357Var(Qs357Var && v) = delete;
		auto operator=(Qs357Var && v) -> Qs357Var& = delete;

		auto act() -> void override;
	};

	class QsCBLineVar final : public CollectionVar {
	public:
		QsCBLineVar(const Qcode q, std::string const& name, const uint32_t num_elements);
		~QsCBLineVar() = default;

		QsCBLineVar(QsCBLineVar const& v) = delete;
		auto operator=(QsCBLineVar const& v) -> QsCBLineVar& = delete;
		QsCBLineVar(QsCBLineVar && v) = delete;
		auto operator=(QsCBLineVar && v) -> QsCBLineVar& = delete;

		auto act() -> void override;
	};

	class QsDeltaCollectionVar final : public CollectionVar {
	public:
		QsDeltaCollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements);
		~QsDeltaCollectionVar() = default;

		QsDeltaCollectionVar(QsDeltaCollectionVar const& v) = delete;
		auto operator=(QsDeltaCollectionVar const& v) -> QsDeltaCollectionVar& = delete;
		QsDeltaCollectionVar(QsDeltaCollectionVar && v) = delete;
		auto operator=(QsDeltaCollectionVar && v) -> QsDeltaCollectionVar& = delete;

		auto act() -> void override;
	};

  class QsDeltaCollectionVar2 final : public CollectionVar {
  public:
    QsDeltaCollectionVar2(const Qcode q, std::string const& name);
    ~QsDeltaCollectionVar2() = default;

    QsDeltaCollectionVar2(QsDeltaCollectionVar2 const& v) = delete;
    auto operator=(QsDeltaCollectionVar2 const& v) -> QsDeltaCollectionVar2& = delete;
    QsDeltaCollectionVar2(QsDeltaCollectionVar2 && v) = delete;
    auto operator=(QsDeltaCollectionVar2 && v) -> QsDeltaCollectionVar2& = delete;

    auto act() -> void override;
  };



	class WxRdrPanelPushVar final : public CollectionVar {
	public:
		WxRdrPanelPushVar(const Qcode q, std::string const& name, const uint32_t num_elements);
		~WxRdrPanelPushVar() = default;

		WxRdrPanelPushVar(WxRdrPanelPushVar const& v) = delete;
		auto operator=(WxRdrPanelPushVar const& v) -> WxRdrPanelPushVar& = delete;
		WxRdrPanelPushVar(WxRdrPanelPushVar && v) = delete;
		auto operator=(WxRdrPanelPushVar && v) -> WxRdrPanelPushVar& = delete;

		auto act() -> void override;

	private:
		static auto sw2Str(const int32_t i) -> std::string;
	};

	class WxRdrPanelTurnVar final : public CollectionVar {
	public:
		WxRdrPanelTurnVar(const Qcode q, std::string const& name, const uint32_t num_elements);
		~WxRdrPanelTurnVar() = default;

		WxRdrPanelTurnVar(WxRdrPanelTurnVar const& v) = delete;
		auto operator=(WxRdrPanelTurnVar const& v) -> WxRdrPanelTurnVar& = delete;
		WxRdrPanelTurnVar(WxRdrPanelTurnVar && v) = delete;
		auto operator=(WxRdrPanelTurnVar && v) -> WxRdrPanelTurnVar& = delete;

		auto act() -> void override;
	};

	class QhMomActionCollectionVar final : public CollectionVar {
	public:
		QhMomActionCollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements);
		~QhMomActionCollectionVar() = default;

		QhMomActionCollectionVar(QhMomActionCollectionVar const& v) = delete;
		auto operator=(QhMomActionCollectionVar const& v) -> QhMomActionCollectionVar& = delete;
		QhMomActionCollectionVar(QhMomActionCollectionVar && v) = delete;
		auto operator=(QhMomActionCollectionVar && v) -> QhMomActionCollectionVar& = delete;

		auto act() -> void override;
	};

	class QhEVACPshButtonsCollectionVar final : public CollectionVar {
	public:
		QhEVACPshButtonsCollectionVar(const Qcode q, std::string const& name);
		~QhEVACPshButtonsCollectionVar() = default;

		QhEVACPshButtonsCollectionVar(QhEVACPshButtonsCollectionVar const& v) = delete;
		auto operator=(QhEVACPshButtonsCollectionVar const& v) -> QhEVACPshButtonsCollectionVar& = delete;
		QhEVACPshButtonsCollectionVar(QhEVACPshButtonsCollectionVar && v) = delete;
		auto operator=(QhEVACPshButtonsCollectionVar && v) -> QhEVACPshButtonsCollectionVar& = delete;

		auto act() -> void override;
	};

	class QhEVACSignalCmndsCollectionVar final : public CollectionVar {
	public:
		QhEVACSignalCmndsCollectionVar(const Qcode q, std::string const& name);
		~QhEVACSignalCmndsCollectionVar() = default;

		QhEVACSignalCmndsCollectionVar(QhEVACSignalCmndsCollectionVar const& v) = delete;
		auto operator=(QhEVACSignalCmndsCollectionVar const& v) -> QhEVACSignalCmndsCollectionVar& = delete;
		QhEVACSignalCmndsCollectionVar(QhEVACSignalCmndsCollectionVar && v) = delete;
		auto operator=(QhEVACSignalCmndsCollectionVar && v) -> QhEVACSignalCmndsCollectionVar& = delete;

		auto act() -> void override;
	};

	class QhRotSwitchCollectionVar final : public CollectionVar {
	public:
		QhRotSwitchCollectionVar(const Qcode q, std::string const& name);
		~QhRotSwitchCollectionVar() = default;

		QhRotSwitchCollectionVar(QhRotSwitchCollectionVar const& v) = delete;
		auto operator=(QhRotSwitchCollectionVar const& v) -> QhRotSwitchCollectionVar& = delete;
		QhRotSwitchCollectionVar(QhRotSwitchCollectionVar && v) = delete;
		auto operator=(QhRotSwitchCollectionVar && v) -> QhRotSwitchCollectionVar& = delete;

		auto act() -> void override;
	};

	class QhFireSwitchVar final : public CollectionVar {
	public:
		QhFireSwitchVar(const Qcode q, std::string const& name);
		~QhFireSwitchVar() = default;

		QhFireSwitchVar(QhFireSwitchVar const& v) = delete;
		auto operator=(QhFireSwitchVar const& v) -> QhFireSwitchVar& = delete;
		QhFireSwitchVar(QhFireSwitchVar && v) = delete;
		auto operator=(QhFireSwitchVar && v) -> QhFireSwitchVar& = delete;

		auto act() -> void override;
	};

	class QhOnOffOnSwitchCollectionVar final : public CollectionVar {
	public:
		QhOnOffOnSwitchCollectionVar(const Qcode q, std::string const& name);
		~QhOnOffOnSwitchCollectionVar() = default;

		QhOnOffOnSwitchCollectionVar(QhOnOffOnSwitchCollectionVar const& v) = delete;
		auto operator=(QhOnOffOnSwitchCollectionVar const& v) -> QhOnOffOnSwitchCollectionVar& = delete;
		QhOnOffOnSwitchCollectionVar(QhOnOffOnSwitchCollectionVar && v) = delete;
		auto operator=(QhOnOffOnSwitchCollectionVar && v) -> QhOnOffOnSwitchCollectionVar& = delete;

		auto act() -> void override;
	};

	class GuaOnOffOnVar final : public CollectionVar {
	public:
		GuaOnOffOnVar(const Qcode q, std::string const& name);
		~GuaOnOffOnVar() = default;

		GuaOnOffOnVar(GuaOnOffOnVar const& v) = delete;
		auto operator=(GuaOnOffOnVar const& v) -> GuaOnOffOnVar& = delete;
		GuaOnOffOnVar(GuaOnOffOnVar && v) = delete;
		auto operator=(GuaOnOffOnVar && v) -> GuaOnOffOnVar& = delete;

		auto act() -> void override;
	};

	class CollRotSwitchTerminal : public AlternateActionSwitch {
	public:
		CollRotSwitchTerminal(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number, const uint32_t terminal_number);
		~CollRotSwitchTerminal() = default;

		CollRotSwitchTerminal(CollRotSwitchTerminal const& v) = delete;
		auto operator=(CollRotSwitchTerminal const& v) -> CollRotSwitchTerminal& = delete;
		CollRotSwitchTerminal(CollRotSwitchTerminal && v) = delete;
		auto operator=(CollRotSwitchTerminal && v) -> CollRotSwitchTerminal& = delete;

		auto preInitialise() -> void override;
		auto act() -> void override;
		auto getType() const -> std::string override;
		auto getExpandedQcode() const -> std::string override;

	protected:
		const uint32_t terminal_number_;
		const Qcode controlling_var_;
		const uint32_t element_number_;
		std::shared_ptr<CollectionVar> collection_object_ = nullptr;
	};

	class CollRotSwitchTerminal1 final : public CollRotSwitchTerminal {
	public:
		CollRotSwitchTerminal1(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t terminal_number);
		~CollRotSwitchTerminal1() = default;

		CollRotSwitchTerminal1(CollRotSwitchTerminal1 const& v) = delete;
		auto operator=(CollRotSwitchTerminal1 const& v) -> CollRotSwitchTerminal1& = delete;
		CollRotSwitchTerminal1(CollRotSwitchTerminal1 && v) = delete;
		auto operator=(CollRotSwitchTerminal1 && v) -> CollRotSwitchTerminal1& = delete;

	};

	class CollOnOffSwitch : public CollRotSwitchTerminal {
	public:
		CollOnOffSwitch(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number);
		~CollOnOffSwitch() = default;

		CollOnOffSwitch(CollOnOffSwitch const& v) = delete;
		auto operator=(CollOnOffSwitch const& v) -> CollOnOffSwitch& = delete;
		CollOnOffSwitch(CollOnOffSwitch && v) = delete;
		auto operator=(CollOnOffSwitch && v) -> CollOnOffSwitch& = delete;

		auto act() -> void override;
		auto getType() const -> std::string override;
	};

	class OnOffOnSwitch : public CollOnOffSwitch {
	public:
		OnOffOnSwitch(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number);

		OnOffOnSwitch(OnOffOnSwitch const& v) = delete;
		auto operator=(OnOffOnSwitch const& v) -> OnOffOnSwitch& = delete;
		OnOffOnSwitch(OnOffOnSwitch && v) = delete;
		auto operator=(OnOffOnSwitch && v) -> OnOffOnSwitch& = delete;

		auto getType() const -> std::string override;
	};

	class CollInfiniteRotarySelector final : public InfiniteRotarySelector {
	public:
		CollInfiniteRotarySelector(const Qcode q, std::string const& name, const int32_t range, const Qcode controlling_var, const uint32_t element_number, const bool direction); // direction 1 or -1
		~CollInfiniteRotarySelector() = default;

		CollInfiniteRotarySelector(CollInfiniteRotarySelector const& v) = delete;
		auto operator=(CollInfiniteRotarySelector const& v) -> CollInfiniteRotarySelector& = delete;
		CollInfiniteRotarySelector(CollInfiniteRotarySelector && v) = delete;
		auto operator=(CollInfiniteRotarySelector && v) -> CollInfiniteRotarySelector& = delete;

		auto preInitialise() -> void override;
		auto act() -> void override;
		auto getExpandedQcode() const -> std::string override;

	private:
		const Qcode controlling_var_;
		const uint32_t element_number_;
		std::shared_ptr<CollectionVar> collection_object_ = nullptr;
	};

	class CollValueControlVar final : public ValueControlVar {
	public:
		CollValueControlVar(const Qcode q, std::string const& name, const MinMax minmax, const Qcode controlling_var, const uint32_t element_number);
		~CollValueControlVar() = default;

		CollValueControlVar(CollValueControlVar const& v) = delete;
		auto operator=(CollValueControlVar const& v) -> CollValueControlVar& = delete;
		CollValueControlVar(CollValueControlVar && v) = delete;
		auto operator=(CollValueControlVar && v) -> CollValueControlVar& = delete;

		auto preInitialise() -> void override;
		auto act() -> void override;
		auto getExpandedQcode() const -> std::string override;

	private:
		const Qcode controlling_var_;
		const uint32_t element_number_;
		std::shared_ptr<CollectionVar> collection_object_ = nullptr;
	};

	class CollContinuousValueControlVar final : public ContinuousValueControlVar {
	public:
		CollContinuousValueControlVar(const Qcode q, std::string const& name, const MinMax minmax, const Qcode controlling_var, const uint32_t element_number);
		~CollContinuousValueControlVar() = default;

		CollContinuousValueControlVar(CollContinuousValueControlVar const& v) = delete;
		auto operator=(CollContinuousValueControlVar const& v) -> CollContinuousValueControlVar& = delete;
		CollContinuousValueControlVar(CollContinuousValueControlVar && v) = delete;
		auto operator=(CollContinuousValueControlVar && v) -> CollContinuousValueControlVar& = delete;

		auto preInitialise() -> void override;
		auto act() -> void override;
		auto getExpandedQcode() const -> std::string override;

	private:
		const Qcode controlling_var_;
		const uint32_t element_number_;
		std::shared_ptr<CollectionVar> collection_object_ = nullptr;
	};

	class CollPotentiometer270ControlVar final : public Potentiometer270ControlVar {
	public:
		CollPotentiometer270ControlVar(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number);
		~CollPotentiometer270ControlVar() = default;

		CollPotentiometer270ControlVar(CollPotentiometer270ControlVar const& v) = delete;
		auto operator=(CollPotentiometer270ControlVar const& v) -> CollPotentiometer270ControlVar& = delete;
		CollPotentiometer270ControlVar(CollPotentiometer270ControlVar && v) = delete;
		auto operator=(CollPotentiometer270ControlVar && v) -> CollPotentiometer270ControlVar& = delete;

		auto preInitialise() -> void override;
		auto act() -> void override;
		auto getExpandedQcode() const -> std::string override;

	private:
		const Qcode controlling_var_;
		const uint32_t element_number_;
		std::shared_ptr<CollectionVar> collection_object_ = nullptr;
	};

	class CollMomActionSwitch final : public MomentaryActionSwitch {
	public:
		CollMomActionSwitch(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number);
		~CollMomActionSwitch() = default;

		CollMomActionSwitch(CollMomActionSwitch const& v) = delete;
		auto operator=(CollMomActionSwitch const& v) -> CollMomActionSwitch& = delete;
		CollMomActionSwitch(CollMomActionSwitch && v) = delete;
		auto operator=(CollMomActionSwitch && v) -> CollMomActionSwitch& = delete;

		auto preInitialise() -> void override;
		auto act() -> void override;
		auto getExpandedQcode() const -> std::string override;

	private:
		const Qcode controlling_var_;
		const uint32_t element_number_;
		std::shared_ptr<CollectionVar> collection_object_ = nullptr;
	};

	class ExpPshButton : public ExpControlVar {
	public:
		~ExpPshButton() = default;

		ExpPshButton(ExpPshButton const& v) = delete;
		auto operator=(ExpPshButton const& v) -> ExpPshButton& = delete;
		ExpPshButton(ExpPshButton && v) = delete;
		auto operator=(ExpPshButton && v) -> ExpPshButton& = delete;

		auto setValue(const int32_t v) -> void final;
		auto getType() const -> std::string override;

	protected:
		ExpPshButton(const Qcode q, std::string const& name, const uint32_t key_number, const Qcode expqcode);
		const uint32_t key_number_;
	};

	class TransponderKey final : public ExpPshButton {
	public:
		TransponderKey(const Qcode q, std::string const& name, const uint32_t key_number, const Qcode expqcode);
		~TransponderKey() = default;

		TransponderKey(TransponderKey const& v) = delete;
		auto operator=(TransponderKey const& v) -> TransponderKey& = delete;
		TransponderKey(TransponderKey && v) = delete;
		auto operator=(TransponderKey && v) -> TransponderKey& = delete;

		auto act() -> void override;
		auto getType() const -> std::string override;
	};

	class RcpPushbutton final : public ExpPshButton {
	public:
		RcpPushbutton(const Qcode q, std::string const& name, const uint32_t key_number, const Qcode expqcode);
		~RcpPushbutton() = default;

		RcpPushbutton(RcpPushbutton const& v) = delete;
		auto operator=(RcpPushbutton const& v) -> RcpPushbutton& = delete;
		RcpPushbutton(RcpPushbutton && v) = delete;
		auto operator=(RcpPushbutton && v) -> RcpPushbutton& = delete;

		auto act() -> void override;
		auto getType() const -> std::string override;
	};

	class AcpPushbutton final : public ExpPshButton {
	public:
		AcpPushbutton(const Qcode q, std::string const& name, uint32_t key_number, const Qcode expqcode);
		~AcpPushbutton() = default;

		AcpPushbutton(AcpPushbutton const& v) = delete;
		auto operator=(AcpPushbutton const& v) -> AcpPushbutton& = delete;
		AcpPushbutton(AcpPushbutton && v) = delete;
		auto operator=(AcpPushbutton && v) -> AcpPushbutton& = delete;

		auto act() -> void override;
		auto getType() const -> std::string override;
	};

	class OCKeyboard final : public ControlVar {
	public:
		OCKeyboard(const Qcode q, std::string const& name);
		~OCKeyboard() = default;

		OCKeyboard(OCKeyboard const& v) = delete;
		auto operator=(OCKeyboard const& v) -> OCKeyboard& = delete;
		OCKeyboard(OCKeyboard && v) = delete;
		auto operator=(OCKeyboard && v) -> OCKeyboard& = delete;

		auto act() -> void override;
		auto getType() const -> std::string override;

	private:
		bool atc_sent_{ false };
	};





	// INFO VARS

	class InfoVar : public Var {
	public:
		~InfoVar() = default;

		InfoVar(InfoVar const& v) = delete;
		auto operator=(InfoVar const& v) -> InfoVar& = delete;
		InfoVar(InfoVar && v) = delete;
		auto operator=(InfoVar && v) -> InfoVar& = delete;

		auto preInitialise() -> void final;
		auto setValue(const int32_t v) -> void override;
		auto act() -> void final;
		auto isInfoVar() const noexcept -> bool final;
		auto needsCheck() const noexcept -> bool override;

	protected:
		InfoVar(const Qcode q, std::string const& name);
		auto checkAct(const int32_t panelVal) -> void;
		auto sendToSioc() -> void;   // and thereby to your home cockpit
		const static RefToSharedVarPtrFunctionWithQcodePar varptrfie_;
		const static std::shared_ptr<Client> tosiocptr_;
	};

	class IndicatorLightNotIntegrated : public InfoVar {
	public:
		IndicatorLightNotIntegrated(const Qcode q, std::string const& name, const Qcode volt);
		~IndicatorLightNotIntegrated() = default;

		IndicatorLightNotIntegrated(IndicatorLightNotIntegrated const& v) = delete;
		auto operator=(IndicatorLightNotIntegrated const& v) -> IndicatorLightNotIntegrated& = delete;
		IndicatorLightNotIntegrated(IndicatorLightNotIntegrated && v) = delete;
		auto operator=(IndicatorLightNotIntegrated && v) -> IndicatorLightNotIntegrated& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	protected:
		const Qcode volt_;
	};

	class CduIndicatorLights final : public InfoVar {
	public:
		CduIndicatorLights(const Qcode q, std::string const& name, const Qcode volt);
		~CduIndicatorLights() = default;

		CduIndicatorLights(CduIndicatorLights const& v) = delete;
		auto operator=(CduIndicatorLights const& v) -> CduIndicatorLights& = delete;
		CduIndicatorLights(CduIndicatorLights && v) = delete;
		auto operator=(CduIndicatorLights && v) -> CduIndicatorLights& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode volt_;
	};

	class AcpMICLights final : public InfoVar {
	public:
		AcpMICLights(const Qcode q, std::string const& name, const LCR lcr);
		~AcpMICLights() = default;

		AcpMICLights(AcpMICLights const& v) = delete;
		auto operator=(AcpMICLights const& v) -> AcpMICLights& = delete;
		AcpMICLights(AcpMICLights && v) = delete;
		auto operator=(AcpMICLights && v) -> AcpMICLights& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const LCR lcr_;
	};

	class ACPgreenLEDs final : public InfoVar {
	public:
		ACPgreenLEDs(const Qcode q, std::string const& name, const LCR lcr);
		~ACPgreenLEDs() = default;

		ACPgreenLEDs(ACPgreenLEDs const& v) = delete;
		auto operator=(ACPgreenLEDs const& v) -> ACPgreenLEDs& = delete;
		ACPgreenLEDs(ACPgreenLEDs && v) = delete;
		auto operator=(ACPgreenLEDs && v) -> ACPgreenLEDs& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const LCR lcr_;
	};

	class QcodeInfo : public InfoVar {
	public:
		QcodeInfo(const Qcode q, std::string const& name);
		~QcodeInfo() = default;

		QcodeInfo(QcodeInfo const& v) = delete;
		auto operator=(QcodeInfo const& v) -> QcodeInfo& = delete;
		QcodeInfo(QcodeInfo && v) = delete;
		auto operator=(QcodeInfo && v) -> QcodeInfo& = delete;

		auto setValue(const int32_t v) -> void override;
		auto check() -> void override;
		auto needsCheck() const noexcept -> bool override;
		auto getType() const -> std::string override;
	};

	class StbyCompass final : public QcodeInfo {
	public:
		StbyCompass(const Qcode q, std::string const& name);
		~StbyCompass() = default;

		StbyCompass(StbyCompass const& v) = delete;
		auto operator=(StbyCompass const& v) -> StbyCompass& = delete;
		StbyCompass(StbyCompass && v) = delete;
		auto operator=(StbyCompass && v) -> StbyCompass& = delete;

		auto setValue(const int32_t v) -> void override;
	};

	class ExpQscodeInfo : public QcodeInfo {
	public:
		ExpQscodeInfo(const Qcode q, std::string const& name, const Qcode expqcode);
		~ExpQscodeInfo() = default;

		ExpQscodeInfo(ExpQscodeInfo const& v) = delete;
		auto operator=(ExpQscodeInfo const& v) -> ExpQscodeInfo& = delete;
		ExpQscodeInfo(ExpQscodeInfo && v) = delete;
		auto operator=(ExpQscodeInfo && v) -> ExpQscodeInfo& = delete;

		auto getExpandedQcode() const -> std::string override;

	protected:
		const Qcode expqcode_;
	};

	class QhcodeInfo final : public ExpQscodeInfo {
	public:
		QhcodeInfo(const Qcode q, std::string const& name, const Qcode expqcode);
		~QhcodeInfo() = default;

		QhcodeInfo(QhcodeInfo const& v) = delete;
		auto operator=(QhcodeInfo const& v) -> QhcodeInfo& = delete;
		QhcodeInfo(QhcodeInfo && v) = delete;
		auto operator=(QhcodeInfo && v) -> QhcodeInfo& = delete;

		auto getType() const -> std::string override;
	};


	class ExpQscodeInfoVar : public InfoVar {
	public:
		ExpQscodeInfoVar(const Qcode q, std::string const& name, const Qcode expqcode);
		~ExpQscodeInfoVar() = default;

		ExpQscodeInfoVar(ExpQscodeInfoVar const& v) = delete;
		auto operator=(ExpQscodeInfoVar const& v) -> ExpQscodeInfoVar& = delete;
		ExpQscodeInfoVar(ExpQscodeInfoVar && v) = delete;
		auto operator=(ExpQscodeInfoVar && v) -> ExpQscodeInfoVar& = delete;

		auto getExpandedQcode() const -> std::string override;

	protected:
		const Qcode expqcode_;
	};

	class RCPIndicatorLights final : public InfoVar {
	public:
		RCPIndicatorLights(const Qcode q, std::string const& name, const Qcode volt);
		~RCPIndicatorLights() = default;

		RCPIndicatorLights(RCPIndicatorLights const& v) = delete;
		auto operator=(RCPIndicatorLights const& v) -> RCPIndicatorLights& = delete;
		RCPIndicatorLights(RCPIndicatorLights && v) = delete;
		auto operator=(RCPIndicatorLights && v) -> RCPIndicatorLights& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode volt_;
	};

	class IndicatorLightMcpIntegrated final : public ExpQscodeInfoVar {
	public:
		IndicatorLightMcpIntegrated(const Qcode q, std::string const& name, const Qcode expqcode);
		~IndicatorLightMcpIntegrated() = default;

		IndicatorLightMcpIntegrated(IndicatorLightMcpIntegrated const& v) = delete;
		auto operator=(IndicatorLightMcpIntegrated const& v) -> IndicatorLightMcpIntegrated& = delete;
		IndicatorLightMcpIntegrated(IndicatorLightMcpIntegrated && v) = delete;
		auto operator=(IndicatorLightMcpIntegrated && v) -> IndicatorLightMcpIntegrated& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;
	};

	class IndicatorLightBigActionSwitch final : public ExpQscodeInfoVar {  // Big Mom Action or Big Alternate Action
	public:
		IndicatorLightBigActionSwitch(const Qcode q, std::string const& name, const Qcode volt_up, const Qcode volt_down, const Qcode expqcode);
		~IndicatorLightBigActionSwitch() = default;

		IndicatorLightBigActionSwitch(IndicatorLightBigActionSwitch const& v) = delete;
		auto operator=(IndicatorLightBigActionSwitch const& v) -> IndicatorLightBigActionSwitch& = delete;
		IndicatorLightBigActionSwitch(IndicatorLightBigActionSwitch && v) = delete;
		auto operator=(IndicatorLightBigActionSwitch && v) -> IndicatorLightBigActionSwitch& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode volt_up_;
		const Qcode volt_down_;
	};

	class IndicatorLightFuelCutoffSwitch final : public ExpQscodeInfoVar {
	public:
		IndicatorLightFuelCutoffSwitch(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode);
		~IndicatorLightFuelCutoffSwitch() = default;

		IndicatorLightFuelCutoffSwitch(IndicatorLightFuelCutoffSwitch const& v) = delete;
		auto operator=(IndicatorLightFuelCutoffSwitch const& v) -> IndicatorLightFuelCutoffSwitch& = delete;
		IndicatorLightFuelCutoffSwitch(IndicatorLightFuelCutoffSwitch && v) = delete;
		auto operator=(IndicatorLightFuelCutoffSwitch && v) -> IndicatorLightFuelCutoffSwitch& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode volt_;
	};

	class WhiteLightEngStartSwitch final : public ExpQscodeInfoVar {
	public:
		WhiteLightEngStartSwitch(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode);
		~WhiteLightEngStartSwitch() = default;

		WhiteLightEngStartSwitch(WhiteLightEngStartSwitch const& v) = delete;
		auto operator=(WhiteLightEngStartSwitch const& v) -> WhiteLightEngStartSwitch& = delete;
		WhiteLightEngStartSwitch(WhiteLightEngStartSwitch && v) = delete;
		auto operator=(WhiteLightEngStartSwitch && v) -> WhiteLightEngStartSwitch& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode volt_;
	};

	class IndicatorLightFireSwitch final : public ExpQscodeInfoVar {
	public:
		IndicatorLightFireSwitch(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode);
		~IndicatorLightFireSwitch() = default;

		IndicatorLightFireSwitch(IndicatorLightFireSwitch const& v) = delete;
		auto operator=(IndicatorLightFireSwitch const& v) -> IndicatorLightFireSwitch& = delete;
		IndicatorLightFireSwitch(IndicatorLightFireSwitch && v) = delete;
		auto operator=(IndicatorLightFireSwitch && v) -> IndicatorLightFireSwitch& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode volt_;
	};

	class IndicatorLightSquibButton final : public ExpQscodeInfoVar {
	public:
		IndicatorLightSquibButton(const Qcode q, std::string const& name, const Qcode expqcode);
		~IndicatorLightSquibButton() = default;

		IndicatorLightSquibButton(IndicatorLightSquibButton const& v) = delete;
		auto operator=(IndicatorLightSquibButton const& v) -> IndicatorLightSquibButton& = delete;
		IndicatorLightSquibButton(IndicatorLightSquibButton && v) = delete;
		auto operator=(IndicatorLightSquibButton && v) -> IndicatorLightSquibButton& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;
	};

	class IndicatorLightELT final : public ExpQscodeInfoVar {
	public:
		IndicatorLightELT(const Qcode q, std::string const& name, const Qcode expqcode);
		~IndicatorLightELT() = default;

		IndicatorLightELT(IndicatorLightELT const& v) = delete;
		auto operator=(IndicatorLightELT const& v) -> IndicatorLightELT& = delete;
		IndicatorLightELT(IndicatorLightELT && v) = delete;
		auto operator=(IndicatorLightELT && v) -> IndicatorLightELT& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;
	};

	class EvacLight final : public ExpQscodeInfoVar {
	public:
		EvacLight(const Qcode q, std::string const& name, const Qcode expqcode);
		~EvacLight() = default;

		EvacLight(EvacLight const& v) = delete;
		auto operator=(EvacLight const& v) -> EvacLight& = delete;
		EvacLight(EvacLight && v) = delete;
		auto operator=(EvacLight && v) -> EvacLight& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;
	};

	class EtChrVar final : public ExpQscodeInfoVar {
	public:
		EtChrVar(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode);
		~EtChrVar() = default;

		EtChrVar(EtChrVar const& v) = delete;
		auto operator=(EtChrVar const& v) -> EtChrVar& = delete;
		EtChrVar(EtChrVar && v) = delete;
		auto operator=(EtChrVar && v) -> EtChrVar& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode volt_;
	};

	class ClockVar final : public ExpQscodeInfoVar {
	public:
		ClockVar(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode);
		~ClockVar() = default;

		ClockVar(ClockVar const& v) = delete;
		auto operator=(ClockVar const& v) -> ClockVar& = delete;
		ClockVar(ClockVar && v) = delete;
		auto operator=(ClockVar && v) -> ClockVar& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode volt_;
		uint32_t hrt100pmin_ = 123456U;
	};

	class ClockVar2 final : public ExpQscodeInfoVar {
	public:
		ClockVar2(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode);
		~ClockVar2() = default;

		ClockVar2(ClockVar2 const& v) = delete;
		auto operator=(ClockVar2 const& v) -> ClockVar2& = delete;
		ClockVar2(ClockVar2 && v) = delete;
		auto operator=(ClockVar2 && v) -> ClockVar2& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode volt_;
		uint32_t montht100pday_ = 123456U;
	};

	class ClockVar3 final : public ExpQscodeInfoVar {
	public:
		ClockVar3(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode);
		~ClockVar3() = default;

		ClockVar3(ClockVar3 const& v) = delete;
		auto operator=(ClockVar3 const& v) -> ClockVar3& = delete;
		ClockVar3(ClockVar3 && v) = delete;
		auto operator=(ClockVar3 && v) -> ClockVar3& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode volt_;
		uint32_t year_ = 123456U;
	};

	class RcpDispl final : public ExpQscodeInfoVar {
	public:
		RcpDispl(const Qcode q, std::string const& name, const Qcode expqcode, const Qcode indicatorlightvar, const Qcode volt);
		~RcpDispl() = default;

		RcpDispl(RcpDispl const& v) = delete;
		auto operator=(RcpDispl const& v) -> RcpDispl& = delete;
		RcpDispl(RcpDispl && v) = delete;
		auto operator=(RcpDispl && v) -> RcpDispl& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const Qcode indicatorlightvar_;
		const Qcode volt_;
	};

  class SquawkDispl final : public ExpQscodeInfoVar {
  public:
    SquawkDispl(const Qcode q, std::string const& name, const Qcode expqcode);
    ~SquawkDispl() = default;

    SquawkDispl(SquawkDispl const& v) = delete;
    auto operator=(SquawkDispl const& v) -> SquawkDispl& = delete;
    SquawkDispl(SquawkDispl && v) = delete;
    auto operator=(SquawkDispl && v) -> SquawkDispl& = delete;

    auto check() -> void override;
    auto getType() const -> std::string override;
  };

  class XpdrFail final : public ExpQscodeInfoVar {
  public:
		XpdrFail(const Qcode q, std::string const& name, const Qcode expqcode);
    ~XpdrFail() = default;

    XpdrFail(XpdrFail const& v) = delete;
    auto operator=(XpdrFail const& v) -> XpdrFail& = delete;
    XpdrFail(XpdrFail && v) = delete;
    auto operator=(XpdrFail && v) -> XpdrFail& = delete;

    auto check() -> void override;
    auto getType() const -> std::string override;
  };

	class CallFrLight final : public ExpQscodeInfoVar {
	public:
		CallFrLight(const Qcode q, std::string const& name, const LCR lcr, const Qcode expqcode);
		~CallFrLight() = default;

		CallFrLight(CallFrLight const& v) = delete;
		auto operator=(CallFrLight const& v) -> CallFrLight& = delete;
		CallFrLight(CallFrLight && v) = delete;
		auto operator=(CallFrLight && v) -> CallFrLight& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		const LCR lcr_;
	};

	class McpHdgWin final : public InfoVar {
	public:
		McpHdgWin(const Qcode q, std::string const& name);
		~McpHdgWin() = default;

		McpHdgWin(McpHdgWin const& v) = delete;
		auto operator=(McpHdgWin const& v) -> McpHdgWin& = delete;
		McpHdgWin(McpHdgWin && v) = delete;
		auto operator=(McpHdgWin && v) -> McpHdgWin& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		uint32_t hdg_ = 123456U;
	};

	class McpSpdWin final : public InfoVar {
	public:
		McpSpdWin(const Qcode q, std::string const& name);
		~McpSpdWin() = default;

		McpSpdWin(McpSpdWin const& v) = delete;
		auto operator=(McpSpdWin const& v) -> McpSpdWin& = delete;
		McpSpdWin(McpSpdWin && v) = delete;
		auto operator=(McpSpdWin && v) -> McpSpdWin& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		uint32_t spd_ = 123456U;
	};

	class McpVsWin final : public InfoVar {
	public:
		McpVsWin(const Qcode q, std::string const& name);
		~McpVsWin() = default;

		McpVsWin(McpVsWin const& v) = delete;
		auto operator=(McpVsWin const& v) -> McpVsWin& = delete;
		McpVsWin(McpVsWin && v) = delete;
		auto operator=(McpVsWin && v) -> McpVsWin& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		int32_t vs_ = 123456;
	};

	class McpAltWin final : public InfoVar {
	public:
		McpAltWin(const Qcode q, std::string const& name);
		~McpAltWin() = default;

		McpAltWin(McpAltWin const& v) = delete;
		auto operator=(McpAltWin const& v) -> McpAltWin& = delete;
		McpAltWin(McpAltWin && v) = delete;
		auto operator=(McpAltWin && v) -> McpAltWin& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;

	private:
		uint32_t alt_ = 123456U;
	};

	class SquawkIdent final : public InfoVar {
	public:
		SquawkIdent(const Qcode q, std::string const& name);
		~SquawkIdent() = default;

		SquawkIdent(SquawkIdent const& v) = delete;
		auto operator=(SquawkIdent const& v) -> SquawkIdent& = delete;
		SquawkIdent(SquawkIdent && v) = delete;
		auto operator=(SquawkIdent && v) -> SquawkIdent& = delete;

		auto check() -> void override;
		auto getType() const -> std::string override;
	};



}  // namespace nwk
