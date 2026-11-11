FturnS//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
#include "stdafx.hpp"

namespace nwk {
	using namespace std::literals;

	Var::Var(const Qcode var, std::string name)
		:
		name_(std::move(name)),
		qcode_(var) {
	}

	auto Var::getName() const noexcept -> std::string const& {
		return name_;
	}

	auto Var::getQcode() const noexcept -> Qcode {
		return qcode_;
	}

	auto Var::getValue() const noexcept -> int32_t {
		return val_;
	}

	auto Var::getType() const -> std::string {
		return gkEmptyStr;
	}

	auto Var::getExpandedQcode() const -> std::string {
		return gkEmptyStr;
	}

	auto gf_var2Qcode(Qcode q) -> std::string {
		const auto qui{static_cast<uint32_t>(q)};
		std::string s;
		if (q < Qcode::Qh0) {
			s = std::format("Qs{}", qui - static_cast<uint32_t>(Qcode::Qs0));
		} else if (q < Qcode::Qi0) {
			s = std::format("Qh{}", qui - static_cast<uint32_t>(Qcode::Qh0));
		} else {
			if (q < Qcode::Qexpanded) {
				s = std::format("Qi{}", qui - static_cast<uint32_t>(Qcode::Qi0));
			}
		}
		return s;
	}

	auto gf_Qcode2Str(const Qcode qcode) -> std::string {
		return std::format("{}", static_cast<uint32_t>(qcode));
	}

	CDUArray l_OCCdu_keys{ // codes for USB-Keys in Opencockpits CDU , assembled or kit
		70, 39, 38, 37, 36, 35, 68, 0, 67, 34, 33, 32, 31, 30, 9, 8, 7, 29, 28, 27, 26, 25, 6, 5, 4, 24, 23, 22, 21, 20, 3, 2, 1,
		70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,70, 70, 70, 70, 19,18, 17, 16, 15, 50, 49, 41, 14, 13, 12, 11, 10,
		48, 47, 42, 70, 40, 66, 65, 64, 63, 62, 43, 70, 56, 61, 60, 59, 58, 57, 44, 70, 55, 54, 53, 52, 51, 46, 45
	};

	std::queue<QcodeValueData> l_InfiniteRotaryQueue_;
	std::queue<DeltaColVal> l_DeltaCollectionQueue_;
	std::mutex if_mutex;
	std::mutex dc_mutex;

	MinMax::MinMax(const int32_t min, const int32_t max) noexcept
		:
		min{min},
		max{max} {
		static_assert(sizeof(MinMax) == 8);
	}

	MinMax::MinMax(const int32_t max) noexcept
		:
		min{0},
		max{max} {
		static_assert(sizeof(MinMax) == 8);
	}

	auto lf_putIFRInQueue(QcodeValueData&& e) {
		const std::lock_guard mut(if_mutex);
		l_InfiniteRotaryQueue_.push(std::move(e));
	}

	auto gf_getIFRData(QcodeValueData& e) -> bool {
		auto b{false};
		const std::lock_guard mut(if_mutex);
		if (!l_InfiniteRotaryQueue_.empty()) {
			e = l_InfiniteRotaryQueue_.front();
			l_InfiniteRotaryQueue_.pop();
			b = true;
		}
		return b;
	}

	auto lf_putDCVInQueue(const DeltaColVal& e) {
		const std::lock_guard mut(dc_mutex);
		l_DeltaCollectionQueue_.push(e);
	}

	auto gf_getDCVData(DeltaColVal& e) -> bool {
		auto b{false};
		const std::lock_guard mut(dc_mutex);
		if (!l_DeltaCollectionQueue_.empty()) {
			e = l_DeltaCollectionQueue_.front();
			l_DeltaCollectionQueue_.pop();
			b = true;
		}
		return b;
	}

	auto updateOCCduKeys(const CDUArray& codes) noexcept -> void {
		l_OCCdu_keys = codes;
	}

	ControlVar::ControlVar(const Qcode q, std::string const& name)
		:
		Var(q, name) {
	}

	auto ControlVar::preInitialise() -> void {
	}

	auto ControlVar::check() -> void {
		// by default a control var needs no polling
	}

	auto ControlVar::act() -> void {
		(*toPSXPtr_).writeToBuffer(QcodeValueData(qcode_, val_));  // ControlVar::sendToPSX(val_);
	}

	auto ControlVar::setValue(const int32_t v) -> void {
		val_ = v;
		act();
	}

	auto ControlVar::isInfoVar() const noexcept -> bool {
		return false;
	}

	auto ControlVar::needsCheck() const noexcept -> bool {
		return false;
	}

	auto ControlVar::sendToPSX() const -> void {
		(*toPSXPtr_).writeToBuffer(qcode_);
	}

	auto ControlVar::sendToPSX(const int32_t value) const -> void {
		(*toPSXPtr_).writeToBuffer(QcodeValueData(qcode_, value));
	}

	auto ControlVar::controllingObjectError(Qcode q) -> void {
		throw UserErrorException(100, "Controlling_object "s + gf_Qcode2Str(q) + " is not present"s);
	}







	ValueControlVar::ValueControlVar(const Qcode q, std::string const& name, const MinMax minmax)
		:
		ControlVar(q, name),
		minmax_{minmax} {
	}

	auto ValueControlVar::setValue(const int32_t v) -> void {
		auto va{v};
		if (va > minmax_.max) {
			va = minmax_.max;
		} else {
			if (va < minmax_.min) {
				va = minmax_.min;
			}
		}
		ControlVar::setValue(va);
	}

	auto ValueControlVar::getType() const -> std::string {
		return std::format("[{}..{}]", minmax_.min, minmax_.max);
	}



	ContinuousValueControlVar::ContinuousValueControlVar(const Qcode q, std::string const& name, const MinMax minmax)
		:
		ControlVar(q, name),
		minmax_{minmax},
		delta_{(minmax_.max - minmax_.min) / 100} {
		val_ = minmax_.max + delta_;
	}

	auto ContinuousValueControlVar::setValue(const int32_t v) -> void {
		auto va{v};
		if (va > minmax_.max) {
			va = minmax_.max;
		} else {
			if (va < minmax_.min) {
				va = minmax_.min;
			}
		}
		if (abs(va - val_) >= delta_) {  // suppress jitter
			ControlVar::setValue(va);
		}
	}

	auto ContinuousValueControlVar::getType() const -> std::string {
		return std::format("[{}.%.{}]", minmax_.min, minmax_.max);
	}

	Potentiometer270ControlVar::Potentiometer270ControlVar(const Qcode q, std::string const& name)
		:
		ControlVar(q, name) {
		val_ = kMax_ + kDelta_;
	}

	auto Potentiometer270ControlVar::setValue(const int32_t v) -> void {
		auto va{v};
		if (va > kMax_) {
			va = kMax_;
		} else {
			if (va < kMin_) {
				va = kMin_;
			}
		}
		if (abs(va - val_) >= kDelta_) {  // suppress jitter
			ControlVar::setValue(va);
		}
	}

	auto Potentiometer270ControlVar::getType() const -> std::string {
		return "P270"s;
	}




	MomentaryActionSwitch::MomentaryActionSwitch(const Qcode q, std::string const& name)
		:
		ControlVar(q, name) {
	}

	auto MomentaryActionSwitch::setValue(const int32_t v) -> void {
		if (!syncPhase_) {
			if (0 == v || 1 == v) {
				ControlVar::setValue(1);
			}
		}
	}

	auto MomentaryActionSwitch::act() -> void {
		ControlVar::sendToPSX(val_);
		val_ = 0;
	}

	auto MomentaryActionSwitch::getType() const -> std::string {
		return gkMStr;
	}








	AlternateActionSwitch::AlternateActionSwitch(const Qcode q, std::string const& name)
		:
		ControlVar(q, name) {
	}

	auto AlternateActionSwitch::setValue(const int32_t v) -> void {
		if (0 == v || 1 == v) {
			ControlVar::setValue(v);
		}
	}

	auto AlternateActionSwitch::getType() const -> std::string {
		return gkAStr;
	}





	ExpControlVar::ExpControlVar(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		ControlVar(q, name),
		expqcode_{expqcode} {
	}

	auto ExpControlVar::sendToPSX(const int32_t value) const -> void {
		(*toPSXPtr_).writeToBuffer(QcodeValueData(expqcode_, value));
	}

	auto ExpControlVar::act() -> void {
		ExpControlVar::sendToPSX(val_);
	}

	auto ExpControlVar::getExpandedQcode() const -> std::string {
		return gf_var2Qcode(expqcode_);
	}




	// MalFunction

	// the first character in the std::string sets the "severe" mode.
	//
	// To repair a malfunction, send this std::string:
	//
	// 1;6;-99
	//
	// Or this std::string:
	//
	// 0;6;-99
	//
	// To activate a malfunction in "severe" mode, send this std::string:
	//
	// 1;6;220812
	//
	// To activate a malfunction in "non-severe" mode, send this std::string:
	//
	// 0;6;220812
	//
	// In this example, 220812 stands for 22h 08m 12sec. That's the time stamp displayed in red on the Instructor. 
	// It shows when the malfunction has occured. This is just a cosmetic function; you can write any
	// time stamp you want (6 digit format). The malfunction is triggered when the second value is 6 and the last value is not -99.
	//
	// The first character just sets the "Severe" checkbox. The "Severe" function is relevant only in the moment when the malfunction is triggered.

	MalFunctionControlVar::MalFunctionControlVar(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		ExpControlVar(q, name, expqcode) {
	}

	auto MalFunctionControlVar::act() -> void {
		const auto eqi{static_cast<uint32_t>(expqcode_)};
		const auto severity{val_ % 4};
		auto res{gkDigit0Str};
		if (severity > 0) {
			// severity determined by SIOC
			if (severity == 2) {
				res = gkDigit1Str;
			}
		} else {
			// take severity from instructor 
			if (!g_str[eqi].empty()) {  // defensive programming
				res = g_str[eqi].at(0);
			}
		}
		res += ";6;"s;
		if ((val_ / 4) != 0) {
			res += gf_UTChhmmss();
		} else {
			res += "-99"s;
		}
		g_str[eqi] = res;
		ExpControlVar::sendToPSX(eqi);
		// send Qi130 MfEvent;
		(*toPSXPtr_).writeToBuffer(QcodeValueData(Qcode::Qi130, 1));
	}

	auto MalFunctionControlVar::getType() const -> std::string {
		return "bits 0 & 1: Severity, b 2: Set/Clear MF"s;
	}







	QiControlVar::QiControlVar(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		ExpControlVar(q, name, expqcode) {
	}

	auto QiControlVar::getType() const -> std::string {
		return "Qcode value"s;
	}








	QiValueControlVar::QiValueControlVar(const Qcode q, std::string const& name, const Qcode expqcode, const MinMax minmax)
		:
		QiControlVar(q, name, expqcode),
		minmax_(minmax) {
	}

	auto QiValueControlVar::setValue(const int32_t v) -> void {
		auto va{v};
		if (va > minmax_.max) {
			va = minmax_.max;
		} else {
			if (va < minmax_.min) {
				va = minmax_.min;
			}
		}
		ControlVar::setValue(va);
	}

	auto QiValueControlVar::getType() const -> std::string {
		return std::format("[{}..{}]", minmax_.min, minmax_.max);
	}





	QiMomentaryActionSwitch::QiMomentaryActionSwitch(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		QiControlVar(q, name, expqcode) {
	}


	auto QiMomentaryActionSwitch::setValue(const int32_t v) -> void {
		if (!syncPhase_) {
			if (0 == v || 1 == v) {
				ControlVar::setValue(1);
			}
		}
	}

	auto QiMomentaryActionSwitch::act() -> void {
		ExpControlVar::act();
		val_ = 0;
	}

	auto QiMomentaryActionSwitch::getType() const -> std::string {
		return gkMStr;
	}






	QiAlternateActionSwitch::QiAlternateActionSwitch(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		QiControlVar(q, name, expqcode) {
	}


	auto QiAlternateActionSwitch::setValue(const int32_t v) -> void {
		if (0 == v || 1 == v) {
			ControlVar::setValue(v);
		}
	}

	auto QiAlternateActionSwitch::getType() const -> std::string {
		return gkAStr;
	}










	QiDeltaControlVar::QiDeltaControlVar(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		QiMomentaryActionSwitch(q, name, expqcode) {
	}


	auto QiDeltaControlVar::setValue(const int32_t v) -> void {
		if (!syncPhase_) {
			ControlVar::setValue(v);
		}
	}







	AutoStart1234Var::AutoStart1234Var(const Qcode q, std::string const& name)
		:
		AlternateActionSwitch(q, name) {
	}


	auto AutoStart1234Var::act() -> void {
		if (1 == val_) {
			sendToPSX(2);   // ON and guard open
		} else {
			sendToPSX(1);   // OFF and closed guard
		}
	}







	CduKeyboardVar::CduKeyboardVar(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		ExpControlVar(q, name, expqcode) {
	}

	auto CduKeyboardVar::setValue(const int32_t v) -> void {
		if (v >= -1 && v <= 68) {
			ControlVar::setValue(v);
		}
	}

	auto CduKeyboardVar::getType() const -> std::string {
		return "-1..68"s;
	}





	GuaMonToggleType2::GuaMonToggleType2(const Qcode q, std::string const& name)
		:
		AlternateActionSwitch(q, name) {
	}


	auto GuaMonToggleType2::act() -> void {
		if (1 == val_) {
			sendToPSX(2);
		} else {
			sendToPSX(1);  // plus closed guard
		}
	}























	InfiniteRotarySelector::InfiniteRotarySelector(const Qcode q, std::string const& name, const int32_t range, const bool direction)
		:
		ControlVar(q, name),
		max_delta_low_{-range},
		max_delta_high_{range},
		direction_{!direction} {  // inverted because that matches my hardware best, no need to add parameters to PSXseecon.cfg  ;-)
	}

	auto InfiniteRotarySelector::act() -> void {
		if (val_ != 0) {
			auto v{val_};
			if (v > max_delta_high_) {
				v = max_delta_high_;
			} else {
				if (v < max_delta_low_) {
					v = max_delta_low_;
				}
			}
			lf_putIFRInQueue(QcodeValueData{qcode_, (direction_ ? -1 : 1) * v});
			val_ = 0;
		}
	}

	auto InfiniteRotarySelector::getType() const -> std::string {
		return gkEStr;
	}



















	CollectionVar::CollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements)
		:
		ControlVar(q, name),
		num_elements_{num_elements}	{
	}

	auto CollectionVar::preInitialise() -> void {
		for (auto i{ 1U }; i <= num_elements_; ++i) {
			elements_.push_back(0);
		}
	}

	auto CollectionVar::setElementValue(const uint32_t element_number, const int32_t value) -> void {
		elements_.at(element_number) = value;
		act(); // ControlVar::setValue(val_);  // dummy val, only the act is important and the syncphase check
	}

	auto CollectionVar::getType() const -> std::string {
		return std::format("Collection of {}...", num_elements_);
	}





	QsECONCollectionVar::QsECONCollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements)
		:
		CollectionVar(q, name, num_elements) {
	}


	auto QsECONCollectionVar::act() -> void {
		std::string s;
		for (auto term : elements_) {
			s += std::format("{};", term);
		};
		if (!s.empty()) { //  defensive programming added, 14 May 2016
			s.pop_back();  // last semicolon
		}
		g_str[static_cast<uint32_t>(qcode_)] = s;
		sendToPSX();
	}








	Qs120Var::Qs120Var(const Qcode q, std::string const& name, const uint32_t num_elements)
		:
		QsECONCollectionVar(q, name, num_elements) {
	}


	auto Qs120Var::act() -> void {
		// for the items not in use, take the PSX value (because they might be controlled via USB and not via the Network
		if (g_ElevatorStick != gkNotUsedControl) {
			elements_.at(0) = g_ElevatorStick;
		}
		if (g_AileronControlW != gkNotUsedControl) {
			elements_.at(1) = g_AileronControlW;
		}
		if (g_RudderPedals != gkNotUsedControl) {
			elements_.at(2) = g_RudderPedals;
		}
		QsECONCollectionVar::act();
	}








	Qs357Var::Qs357Var(const Qcode q, std::string const& name, const uint32_t num_elements)
		:
		QsECONCollectionVar(q, name, num_elements) {
	}


	auto Qs357Var::act() -> void {
		// for the items not in use, take the PSX value (because they might be controlled via USB and not via the Network
		if (g_ToeBrakeLeft != gkNotUsedControl) {
			elements_.at(0) = g_ToeBrakeLeft;
		}
		if (g_ToeBrakeRight != gkNotUsedControl) {
			elements_.at(1) = g_ToeBrakeRight;
		}
		QsECONCollectionVar::act();
	}





	QsCBLineVar::QsCBLineVar(const Qcode q, std::string const& name, const uint32_t num_elements)
		:
		CollectionVar(q, name, num_elements) {
	}


	auto QsCBLineVar::act() -> void {
		auto panel{ g_str[static_cast<uint32_t>(qcode_)] };
		for (auto i{ 0U }; i < num_elements_; ++i) {
			const char p{ panel.at(i) };
			const auto c{ elements_.at(i) };
			if (p == gkxChar || p == gkRChar) {
				// do nothing
			} else if (p == gknChar) {
				if (1 == c) {
					panel.at(i) = gkNChar;
				}
			} else if (p == gkNChar) {
				if (0 == c) {
					panel.at(i) = gknChar;
				}
			} else if (p == gkyChar) {
				if (1 == c) {
					panel.at(i) = gkYChar;
				}
			} else if (p == gkYChar) {
				if (0 == c) {
					panel.at(i) = gkyChar;
				}
			} else if (p == gkbChar) {
				if (1 == c) {
					panel.at(i) = gkBChar;
				}
			} else {
				if (p == gkBChar) {
					if (0 == c) {
						panel.at(i) = gkbChar;
					}
				}
			}
		};
		g_str[static_cast<uint32_t>(qcode_)] = panel;
		sendToPSX();
	}





	QsDeltaCollectionVar::QsDeltaCollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements)
		:
		CollectionVar(q, name, num_elements) {
	}


	auto QsDeltaCollectionVar::act() -> void {
		std::string s;
    std::ranges::for_each(std::as_const(elements_), [&s](const auto term) -> auto {s += std::format("{};", term); });
		if (!s.empty()) {
			s.pop_back();
		}
		std::ranges::for_each(elements_, [](auto& e) -> auto {e = 0; });
		lf_putDCVInQueue(DeltaColVal{ qcode_, s });
	}


  QsDeltaCollectionVar2::QsDeltaCollectionVar2(const Qcode q, std::string const& name)
    :
    CollectionVar(q, name, 2) {
  }

  auto QsDeltaCollectionVar2::act() -> void {
    std::string s;
    if (elements_[1] == 0) {  // the push button of the single rotary encoder
      s = std::format("0;0;{}", elements_[0]);
    } else {
      s = std::format("0;{};0", elements_[0]);
    }
    elements_[0] = 0;
    lf_putDCVInQueue(DeltaColVal{qcode_, s});
  }


	/*
	>>> WXR - weather radar panel pushbuttons

	Code: Qs104
	Name: WxRdrPanelPush
	Mode: ECON
	Length: 13 .. 13
	Add-ons can: Read & write
	Example:
	FWTmGareFWTmG
	Description:

	Each character in the std::string from left to right indicates the "pushed"-status of a certain button. Lower case means "pushed". The order from left to right:

	1 - TFR captain
	2 - WX captain
	3 - WX+T captain
	4 - MAP captain
	5 - GC captain
	6 - AUTO
	7 - L/R
	8 - TEST
	9 - TFR F/O
	10 - WX F/O
	11 - WX+T F/O
	12 - MAP F/O
	13 - GC F/O
	<<<
	*/

	WxRdrPanelPushVar::WxRdrPanelPushVar(const Qcode q, std::string const& name, const uint32_t num_elements)
		:
		CollectionVar(q, name, num_elements) {
	}


	auto WxRdrPanelPushVar::act() -> void {
		// for the definition of the elements, see Qs104 in varpool.cpp 
		auto s{WxRdrPanelPushVar::sw2Str(elements_.at(0))};
		if (1 == elements_.at(1)) {
			s += gkgChar;
		} else {
			s += gkGChar;
		}
		if (1 == elements_.at(2)) {
			s += gkaChar;
		} else {
			s += gkAChar;
		}
		if (1 == elements_.at(3)) {
			s += gkrChar;
		} else {
			s += gkRChar;
		}
		if (1 == elements_.at(4)) {
			s += gkeChar;
		} else {
			s += gkEChar;
		}
		s += WxRdrPanelPushVar::sw2Str(elements_.at(5));
		if (1 == elements_.at(6)) {
			s += gkgChar;
		} else {
			s += gkGChar;
		}
		g_str[static_cast<uint32_t>(qcode_)] = s;
		sendToPSX();
	}

	auto WxRdrPanelPushVar::sw2Str(const int32_t i) -> std::string {
		std::string s;
		switch (i) {
		case 0:
			s = "fWTM"s;
			break;
		case 1:
			s = "FwTM"s;
			break;
		case 2:
			s = "FWtM"s;
			break;
		case 3:
			s = "FWTm"s;
			break;
		default:
			break;
		}
		return s;
	}


	/*
	>>> WXR - weather radar panel rotary selectors

	Code: Qs105
	Name: WxRdrPanelTurn
	Mode: ECON
	Length: 7 .. 15
	Add-ons can: Read & write
	Example:
	4713;3;2197;0
	Description:

	The std::string stores the position values of the two gain rotary selectors and the two tilt potentiometers. All values are separated by semicolons. There must be no semicolon at the end. All values are integer values. The std::string sequence from left to right:

	1 - Tilt captain (range 0 .. 4713)
	2 - Gain captain (range -6 .. 3)
	3 - Tilt F/O (range 0 .. 4713)
	4 - Gain F/O (range -6 .. 3)
	<<<
	*/



	WxRdrPanelTurnVar::WxRdrPanelTurnVar(const Qcode q, std::string const& name, const uint32_t num_elements)
		:
		CollectionVar(q, name, num_elements) {
	}


	auto WxRdrPanelTurnVar::act() -> void {
		const auto s{std::format("{};{};{};{}", elements_.at(0), elements_.at(1) - 6, elements_.at(2), elements_.at(3) - 6)};
		g_str[static_cast<uint32_t>(qcode_)] = s;
		sendToPSX();
	}







	QhMomActionCollectionVar::QhMomActionCollectionVar(const Qcode q, std::string const& name, const uint32_t num_elements)
		:
		CollectionVar(q, name, num_elements) {
	}


	auto QhMomActionCollectionVar::act() -> void {
		// DELTA
		if (1 == elements_.at(0)) {
			val_ = 1;
			elements_.at(0) = 0;
		}
		if (1 == elements_.at(1)) {
			val_ += 2;
			elements_.at(1) = 0;
		}
		sendToPSX(val_);
		val_ = 0;
	}






	QhEVACPshButtonsCollectionVar::QhEVACPshButtonsCollectionVar(const Qcode q, std::string const& name)
		:
		CollectionVar(q, name, 2) {
	}


	auto QhEVACPshButtonsCollectionVar::act() -> void {
		// MIXED:
		if (1 == elements_.at(0)) {
			// DELTA
			val_ = 1;
			elements_.at(0) = 0;
		} else {
			val_ = 0;
		}
		if (1 == elements_.at(1)) {
			// ECON
			val_ += 2;
		}
		sendToPSX(val_);
	}







	QhEVACSignalCmndsCollectionVar::QhEVACSignalCmndsCollectionVar(const Qcode q, std::string const& name)
		:
		CollectionVar(q, name, 2) {
	}


	auto QhEVACSignalCmndsCollectionVar::act() -> void {
		// ECON:
		val_ = elements_.at(0);
		if (1 == elements_.at(1)) {
			val_ += 8;
		}
		sendToPSX(val_);
	}





	QhRotSwitchCollectionVar::QhRotSwitchCollectionVar(const Qcode q, std::string const& name)
		:
		CollectionVar(q, name, 1) {
	}

	auto QhRotSwitchCollectionVar::act() -> void {
		sendToPSX(elements_.at(0));
	}






	QhFireSwitchVar::QhFireSwitchVar(const Qcode q, std::string const& name)
		:
		CollectionVar(q, name, 11) {
	}

	auto QhFireSwitchVar::act() -> void {
		val_ = 0;
		if (1 == elements_.at(0)) {
			val_ = gkBit0;
		}
		if (1 == elements_.at(5)) {
			val_ += gkBit5;
		}
		if (1 == elements_.at(7)) {
			val_ += gkBit7;
		} else if (1 == elements_.at(8)) {
			val_ += gkBit8;
		} else if (1 == elements_.at(9)) {
			val_ += gkBit9;
		} else {
			if (1 == elements_.at(10)) {
				val_ += gkBit10;
			}
		}
		sendToPSX(val_);
	}






	QhOnOffOnSwitchCollectionVar::QhOnOffOnSwitchCollectionVar(const Qcode q, std::string const& name)
		:
		CollectionVar(q, name, 2) {
	}

	auto QhOnOffOnSwitchCollectionVar::act() -> void {
		if (1 == elements_.at(0)) {
			val_ = -1;
		} else if (1 == elements_.at(1)) {
			val_ = 1;
		} else {
			val_ = 0;
		}
		sendToPSX(val_);
	}





	GuaOnOffOnVar::GuaOnOffOnVar(const Qcode q, std::string const& name)
		:
		CollectionVar(q, name, 2) {
	}

	auto GuaOnOffOnVar::act() -> void {
		// ECON
		if (1 == elements_.at(0)) {
			val_ = 2;  // guard open and one outer position
		} else if (1 == elements_.at(1)) {
			val_ = 0;  // guard open and other outer position
		} else {
			val_ = 5;  // centered and guard closed
		}
		sendToPSX(val_);
	}






	CollRotSwitchTerminal::CollRotSwitchTerminal(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number, const uint32_t terminal_number)
		:
		AlternateActionSwitch(q, name),
		terminal_number_{ terminal_number },
		controlling_var_{ controlling_var },
		element_number_{ element_number }	{
	}

	auto CollRotSwitchTerminal::preInitialise() -> void {
		if (collection_object_ = static_pointer_cast<CollectionVar>(varptrfie_(controlling_var_)); !collection_object_) {
			ControlVar::controllingObjectError(controlling_var_);
		}
	}

	auto CollRotSwitchTerminal::act() -> void {
		if (collection_object_ && 1 == val_) {
			// give controlling var a new value only if terminal goes to 1
			(*collection_object_).setElementValue(element_number_, terminal_number_);
		}
	}

	auto CollRotSwitchTerminal::getType() const -> std::string {
		return gkRStr;
	}


	auto CollRotSwitchTerminal::getExpandedQcode() const -> std::string {
		return gf_var2Qcode(controlling_var_);
	}






	CollRotSwitchTerminal1::CollRotSwitchTerminal1(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t terminal_number)
		:
		CollRotSwitchTerminal(q, name, controlling_var, 0, terminal_number) {
	}









	CollOnOffSwitch::CollOnOffSwitch(const Qcode q, std::string const& name, const Qcode controlling_var, uint32_t element_number)
		:
		CollRotSwitchTerminal(q, name, controlling_var, element_number, 0) {
	}

	auto CollOnOffSwitch::act() -> void {
		if (collection_object_) {
			(*collection_object_).setElementValue(element_number_, val_);
		}
	}

	auto CollOnOffSwitch::getType() const -> std::string {
		return gkAStr;
	}





	OnOffOnSwitch::OnOffOnSwitch(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number)
		:
		CollOnOffSwitch(q, name, controlling_var, element_number) {
	}

	auto OnOffOnSwitch::getType() const -> std::string {
		return gkOStr;
	}



	CollInfiniteRotarySelector::CollInfiniteRotarySelector(const Qcode q, std::string const& name, const int32_t range, const Qcode controlling_var, const uint32_t element_number, const bool direction)
		:
		InfiniteRotarySelector(q, name, range, direction),
		controlling_var_{ controlling_var },
		element_number_{ element_number } {
	}

	auto CollInfiniteRotarySelector::preInitialise() -> void {
		if (collection_object_ = static_pointer_cast<CollectionVar>(varptrfie_(controlling_var_)); collection_object_ == nullptr) {
			ControlVar::controllingObjectError(controlling_var_);
		}
	}

	auto CollInfiniteRotarySelector::act() -> void {
		if (val_ != 0) {
			auto v{ val_ };
			if (v > max_delta_high_) {
				v = max_delta_high_;
			} else { 
				if (v < max_delta_low_) {
					v = max_delta_low_;
				}
			}
			(*collection_object_).setElementValue(element_number_, (direction_ ? -1 : 1) * v);
			val_ = 0;
		}
	}

	auto CollInfiniteRotarySelector::getExpandedQcode() const -> std::string {
		return gf_var2Qcode(controlling_var_);
	}









	CollValueControlVar::CollValueControlVar(const Qcode q, std::string const& name, const MinMax minmax, const Qcode controlling_var, const uint32_t element_number)
		:
		ValueControlVar(q, name, minmax),
		controlling_var_{ controlling_var },
		element_number_{ element_number } {
	}

	auto CollValueControlVar::preInitialise() -> void {
		if (collection_object_ = static_pointer_cast<CollectionVar>(varptrfie_(controlling_var_)); !collection_object_) {
			ControlVar::controllingObjectError(controlling_var_);
		}
	}

	auto CollValueControlVar::act() -> void {
		(*collection_object_).setElementValue(element_number_, val_);
	}

	auto CollValueControlVar::getExpandedQcode() const -> std::string {
		return gf_var2Qcode(controlling_var_);
	}




	CollContinuousValueControlVar::CollContinuousValueControlVar(const Qcode q, std::string const& name, const MinMax minmax, const Qcode controlling_var, const uint32_t element_number)
		:
		ContinuousValueControlVar(q, name, minmax),
		controlling_var_{ controlling_var },
		element_number_{ element_number } {
	}

	auto CollContinuousValueControlVar::preInitialise() -> void {
		if (collection_object_ = static_pointer_cast<CollectionVar>(varptrfie_(controlling_var_)); !collection_object_) {
			ControlVar::controllingObjectError(controlling_var_);
		}
	}

	auto CollContinuousValueControlVar::act() -> void {
		(*collection_object_).setElementValue(element_number_, val_);
	}

	auto CollContinuousValueControlVar::getExpandedQcode() const -> std::string {
		return gf_var2Qcode(controlling_var_);
	}



	CollPotentiometer270ControlVar::CollPotentiometer270ControlVar(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number)
		:
		Potentiometer270ControlVar(q, name),
		controlling_var_{ controlling_var },
		element_number_{ element_number } {
	}

	auto CollPotentiometer270ControlVar::preInitialise() -> void {
		if (collection_object_ = static_pointer_cast<CollectionVar>(varptrfie_(controlling_var_)); !collection_object_) {
			ControlVar::controllingObjectError(controlling_var_);
		}
	}

	auto CollPotentiometer270ControlVar::act() -> void {
		(*collection_object_).setElementValue(element_number_, val_);
	}

	auto CollPotentiometer270ControlVar::getExpandedQcode() const -> std::string {
		return gf_var2Qcode(controlling_var_);
	}









	CollMomActionSwitch::CollMomActionSwitch(const Qcode q, std::string const& name, const Qcode controlling_var, const uint32_t element_number)
		:
		MomentaryActionSwitch(q, name),
		controlling_var_{ controlling_var },
		element_number_{ element_number } {
	}

	auto CollMomActionSwitch::preInitialise() -> void {
		if (collection_object_ = static_pointer_cast<CollectionVar>(varptrfie_(controlling_var_)); !collection_object_) {
			ControlVar::controllingObjectError(controlling_var_);
		}
	}

	auto CollMomActionSwitch::act() -> void {
		(*collection_object_).setElementValue(element_number_, 1);
	}

	auto CollMomActionSwitch::getExpandedQcode() const -> std::string {
		return gf_var2Qcode(controlling_var_);
	}











	ExpPshButton::ExpPshButton(const Qcode q, std::string const& name, uint32_t key_number, const Qcode expqcode)
		:
		ExpControlVar(q, name, expqcode),
		key_number_{ key_number } {
	}

	auto ExpPshButton::setValue(const int32_t v) -> void {
		if (0 == v || 1 == v) {
			ControlVar::setValue(v);
		}
	}

	auto ExpPshButton::getType() const -> std::string {
		return gkMStr;
	}




	TransponderKey::TransponderKey(const Qcode q, std::string const& name, const uint32_t key_number, const Qcode expqcode)
		:
		ExpPshButton(q, name, key_number, expqcode) {
	}

	auto TransponderKey::act() -> void {
		if (key_number_ < 10) {
			sendToPSX(key_number_);
		} else { // key 10 and 11
			if (1 == val_) {
				sendToPSX(key_number_);
			} else {
				sendToPSX(-1);
			}
		}
	}

	auto TransponderKey::getType() const -> std::string {
		return (key_number_ < 10) ? gkMStr : gkHStr;
	}







	RcpPushbutton::RcpPushbutton(const Qcode q, std::string const& name, const uint32_t key_number, const Qcode expqcode)
		:
		ExpPshButton(q, name, key_number, expqcode) {
	}

	auto RcpPushbutton::act() -> void {
		if (key_number_ > 0) {
			sendToPSX(key_number_);
		} else { // key = 0
			if (1 == val_) {
				sendToPSX(key_number_);
			} else {
				sendToPSX(-1);
			}
		}
	}

	auto RcpPushbutton::getType() const -> std::string {
		return (key_number_ > 0) ? gkMStr : gkHStr;
	}








	AcpPushbutton::AcpPushbutton(const Qcode q, std::string const& name, const uint32_t key_number, const Qcode expqcode)
		:
		ExpPshButton(q, name, key_number, expqcode) {
	}

	auto AcpPushbutton::act() -> void {
		if (key_number_ <= 22) {
			sendToPSX(key_number_);
		} else {  // 26 or 27
			if (1 == val_) {
				sendToPSX(key_number_);
			} else {
				sendToPSX(-1);
			}
		}
	}

	auto AcpPushbutton::getType() const -> std::string {
		return (key_number_ <= 22) ? gkMStr : gkHStr;
	}








	OCKeyboard::OCKeyboard(const Qcode q, std::string const& name)
		:
		ControlVar(q, name) {
	}


	auto OCKeyboard::act() -> void {
		if (val_ != 0) {
			auto key{ l_OCCdu_keys.at(val_) };
			if (key >= 0 && key <= 68) {
				if (60L == key) {
					if (atc_sent_) {
						sendToPSX(-1);
						atc_sent_ = false;
					} else {
						sendToPSX(key);
						atc_sent_ = true;
					}
				} else {
					sendToPSX(key);
					if (39 == key) {
						gf_sendCDUClearDelayed(qcode_);
					}
				}
			}
		}
	}

	auto OCKeyboard::getType() const -> std::string {
		return gkKStr;
	}














	////////////////////////////////////////////
	// InfoVars



	InfoVar::InfoVar(const Qcode q, std::string const& name)
		:
		Var(q, name) {
		val_ = 373737;   // an unlikely start value  as 'undefined'
	}

	auto InfoVar::preInitialise() -> void {
	}

	auto InfoVar::setValue(const int32_t v) -> void {
		val_ = v;
		sendToSioc();
	}

	auto InfoVar::act() -> void {
	}

	auto InfoVar::checkAct(const int32_t panelVal) -> void {
		if (val_ != panelVal) {
			val_ = panelVal;
			sendToSioc();
		}
	}

	auto InfoVar::sendToSioc() -> void {
		(*tosiocptr_).writeToBuffer(QcodeValueData(qcode_, val_));
	}

	auto InfoVar::isInfoVar() const noexcept -> bool {
		return true;
	}

	auto InfoVar::needsCheck() const noexcept -> bool {
		return true;
	}



	//  Name : IndicatorsRcpL, IndicatorsRcpC, IndicatorsRcpR.
	//  Mode : ECON
	//  Range : 0 .. 62463
	//  Add - ons can : Read only
	//  Description :
	//
	//      Each IndicatorsRcp variable is a bitmask :
	//        bit 0001 : VHF L light contact closed
	//        bit 0002 : VHF C light contact closed
	//        bit 0004 : VHF R light contact closed
	//        bit 0008 : HF L light contact closed
	//        bit 0032 : HF R light contact closed
	//        bit 0064 : Offside tuning light contact closed
	//        bit 0128 : FAIL displayed on the LCDs *
	//        bit 0256 : INOP displayed on the LCDs *
	//        bit 0512 : Panel is on
	//        bit 4096 : LCDs are blank *
	//        bit 8192 : MD&T test is on(all light contacts closed, all LCD segments on *)

	//  amL and amR are in Qs106 .. Qs108, see FromPSXClient

	//  The AM light contact is closed if ...
	//  ... the HF L light is contact closed and amL is not zero
	//  ... or ...
	//  ... the HF R light is contact closed and amR is not zero


	RCPIndicatorLights::RCPIndicatorLights(const Qcode q, std::string const& name, const Qcode volt)
		:
		InfoVar(q, name),
		volt_{ volt } {
	}

	auto RCPIndicatorLights::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(qcode_)) };
		if ((v & gk8KU) != 0) {
			v += gkBit14 + gkBit15 + gkBit16 + gkBit17 + gkBit18 + gkBit19;
		} else if (g_dbp.at(static_cast<uint32_t>(volt_)) > 0) {
			if ((v & 1) != 0) {
				v += gkBit14;
			}
			if ((v & 2) != 0) {
				v += gkBit15;
			}
			if ((v & 4) != 0) {
				v += gkBit16;
			}
			if ((v & 8) != 0) {
				v += gkBit17;
				if (g_amL.at(static_cast<uint32_t>(qcode_) - static_cast<uint32_t>(Qcode::Qi92))) {
					v += gkBit20;
				}
			}
			if ((v & 32) != 0) {
				v += gkBit18;
				if (g_amR.at(static_cast<uint32_t>(qcode_) - static_cast<uint32_t>(Qcode::Qi92))) {
					v += gkBit20;
				}
			}
			if ((v & 64) != 0) {
				v += gkBit19;
			}
		}
		checkAct(v);
	}

	auto RCPIndicatorLights::getType() const -> std::string {
		return "b 14(VHFL) 15(VHFC) 16(VHFR) 17(HFL) 18(HFR) 19(OTL) 20 (AM)"s;  // OTL and AM  added June 6, 2016, version 3.1/3.2
	}


	//  Indicator lights not integrated in switches
	//  Code : For Qi indices, refer to Variables.txt.
	//  Name : HydValveLtTail1, HydValveLtTail2, HydValveLtTail3, HydValveLtTail4, HydValveLtWing1, HydValveLtWing2, HydValveLtWing3, 
	//         HydValveLtWing4, FieldLtGen1, FieldLtGen2, FieldLtGen3, FieldLtGen4, FieldLtApu1, FieldLtApu2, SsbOpenLt, TowOnBatLt, 
	//         IrsOnBatLt, HydSysFaultLt1, HydSysFaultLt2, HydSysFaultLt3, HydSysFaultLt4, HydDemPressLt1, HydDemPressLt2,
	//         HydDemPressLt3, HydDemPressLt4, EngBleedSysFaultLt1, EngBleedSysFaultLt2, EngBleedSysFaultLt3, EngBleedSysFaultLt4.
	//  Mode : ECON
	//  Range : 0 .. 8199
	//  Add - ons can : Read only(but may write bulb - broken bits)
	//  Description :

	//  Every Qi indicator light variable is a bitmask :
	//  bit 0001 : light contact closed by system other than MD & T
	//  bit 0002 : first light bulb broken
	//  bit 0004 : second light bulb broken
	//  bit 8192 : light contact closed by MD & T

	IndicatorLightNotIntegrated::IndicatorLightNotIntegrated(const Qcode q, std::string const& name, const Qcode volt)
		:
		InfoVar(q, name),
		volt_{ volt } {
	}


	auto IndicatorLightNotIntegrated::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(qcode_)) };
		if ((v & gk8KU) != 0) {
			auto b{ false };
			if (0 == (v & 2)) {
				v += gkBit14;
				b = true;
			}
			if (0 == (v & 4)) {
				v += gkBit15;
				b = true;
			}
			if (b) {
				v += gkBit16;
			}
		} else {
			if ((v & 1) != 0 && g_dbp.at(static_cast<uint32_t>(volt_)) > 0) {
				auto b{ false };
				if (0 == (v & 2)) {
					v += gkBit14;
					b = true;
				}
				if (0 == (v & 4)) {
					v += gkBit15;
					b = true;
				}
				if (b) {
					v += gkBit16;
				}
			}
		}
		checkAct(v);
	}


	auto IndicatorLightNotIntegrated::getType() const -> std::string {
		return "b 14 15 16"s;
		//  return "Indicator light: bit0=first bulb, bit1=second bulb";
	}



	// CDU indicator lights :
	//  Q Variables :
	//  Each CDU keyboard has a dedicated Qi bitmask for its five lights :
	//  LightsCduL(Qi86)
	//  LightsCduC(Qi87)
	//  LightsCduR(Qi88)

	//  bit 0001 : EXEC light contact closed by CDU
	//  bit 0002 : DSPY light contact closed by CDU
	//  bit 0004 : FAIL light contact closed by CDU
	//  bit 0008 : MSG light contact closed by CDU
	//  bit 0016 : OFST light contact closed by CDU
	//  bit 8192 : all light contacts closed by MD & T

	CduIndicatorLights::CduIndicatorLights(const Qcode q, std::string const& name, const Qcode volt)
		:
		InfoVar(q, name),
		volt_{ volt } {
	}

	auto CduIndicatorLights::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(qcode_)) };
		if ((v & gk8KU) != 0) {
			v = gkBit14 + gkBit15 + gkBit16 + gkBit17 + gkBit18;
		} else if (g_dbp.at(static_cast<uint32_t>(volt_)) > 0) {
			if ((v & 1) != 0) {
				v += gkBit14;
			}
			if ((v & 2) != 0) {
				v += gkBit15;
			}
			if ((v & 4) != 0) {
				v += gkBit16;
			}
			if ((v & 8) != 0) {
				v += gkBit17;
			}
			if ((v & 16) != 0) {
				v += gkBit18;
			}
		}
		checkAct(v);
	}


	auto CduIndicatorLights::getType() const -> std::string {
		return "b 14(EXEC) 15(DSPY) 16(FAIL) 17(MSG) 18(OFST)"s;
	}





	QcodeInfo::QcodeInfo(const Qcode q, std::string const& name)
		:
		InfoVar(q, name) {
	}

	auto QcodeInfo::setValue(const int32_t v) -> void {
		val_ = v;
		sendToSioc();
	}

	auto QcodeInfo::check() -> void {
		// no check, value is set directly via setvalue in FromPsxClient flushelement
	}

	auto QcodeInfo::needsCheck() const noexcept -> bool {
		return false;
	}

	auto QcodeInfo::getType() const -> std::string {
		return "Qcode value"s;
	}






	StbyCompass::StbyCompass(const Qcode q, std::string const& name)
		:
		QcodeInfo(q, name) {
	}

	auto StbyCompass::setValue(const int32_t v) -> void {
		// v = 0..3600 for 360 degrees
		if (abs(v - val_) > 5) {  // reduce the amount of traffic to SIOC
			val_ = v;
			sendToSioc();
		}
	}




	ExpQscodeInfo::ExpQscodeInfo(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		QcodeInfo(q, name),
		expqcode_{ expqcode } {
	}


	auto ExpQscodeInfo::getExpandedQcode() const -> std::string {
		return gf_var2Qcode(expqcode_);
	}





	QhcodeInfo::QhcodeInfo(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		ExpQscodeInfo(q, name, expqcode) {
	}


	auto QhcodeInfo::getType() const -> std::string {
		return "Qh value (Virtual Switch position)"s;
	}











	ExpQscodeInfoVar::ExpQscodeInfoVar(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		InfoVar(q, name),
		expqcode_{ expqcode } {
	}


	auto ExpQscodeInfoVar::getExpandedQcode() const -> std::string {
		return gf_var2Qcode(expqcode_);
	}








	//  MCP momentary action switches are similar to big momentary actions switches, they just have less features.

	//  bit 0001 : switch pushed(server will internally delete this bit after a short delay)
	//  bit 0002 : first light bulb broken
	//  bit 0004 : second light bulb broken
	//  bit 0008 : light contact closed by AFDS
	//  bit 8192 : light contact closed by MD & T

	//  The MD & T bit is necessary because when the test ends, the simulation needs to remember the light status before the test began, 
	//  i.e.the light contact may have been commanded to close by the AFDS which uses bit 8. For a reading add - on, this means :
	//  if ((value & (8 + 8192)) != 0) then light contact closed

	IndicatorLightMcpIntegrated::IndicatorLightMcpIntegrated(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode) {
	}

	auto IndicatorLightMcpIntegrated::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(expqcode_)) };
		if ((v & gk8KU) != 0) {
			auto b{ false };
			if ((v & 2) == 0) {
				v += gkBit14;
				b = true;
			}
			if ((v & 4) == 0) {
				v += gkBit15;
				b = true;
			}
			if (b) {
				v += gkBit16;
			}
		} else {
			auto b{ false };
			if ((v & 8) != 0 && g_dbp.at(static_cast<uint32_t>(Qcode::dimRelayK4a)) > 0) {
				if ((v & 2) == 0) {
					v += gkBit14;
					b = true;
				}
			}
			if ((v & 8) != 0 && g_dbp.at(static_cast<uint32_t>(Qcode::dimRelayK6b)) > 0) {
				if ((v & 4) == 0) {
					v += gkBit15;
					b = true;
				}
			}
			if (b) {
				v += gkBit16;
			}
		}
		checkAct(v);
	}


	auto IndicatorLightMcpIntegrated::getType() const -> std::string {
		return "b 14 15 16"s;
	}







	//  bit 0001 : switch pushed
	//  bit 0002 : upper(or left) light : first bulb broken
	//  bit 0004 : upper(or left) light : second bulb broken
	//  bit 0008 : lower(or right) light : first bulb broken
	//  bit 0016 : lower(or right) light : second bulb broken
	//  bit 0032 : covered
	//  bit 0064 : wired
	//  bit 0128 : upper(or left) light contact closed by system other than MD & T
	//  bit 0256 : lower(or right) light contact closed by system other than MD & T
	//  bit 8192 : all light contacts closed by MD & T

	//  The MD & T bit is necessary because when the test ends, the simulation needs to remember the light status before the test began, i.e.the light contact may have been commanded to close by a system other than the MD & T.Systems other than the MD & T use bit 128 or 256. For a reading add - on, this means :

	//  if ((value & (128 + 8192)) != 0) then upper(or left) light contact closed
	//    if ((value & (256 + 8192)) != 0) then lower(or right) light contact closed

	IndicatorLightBigActionSwitch::IndicatorLightBigActionSwitch(const Qcode q, std::string const& name, const Qcode volt_up, const Qcode volt_down, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode),
		volt_up_{ volt_up },
		volt_down_{ volt_down } {
	}

	auto IndicatorLightBigActionSwitch::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(expqcode_)) };
		if ((v & gk8KU) != 0) {
			auto b{ false };
			if ((v & 2) == 0) {
				v += gkBit14;
				b = true;
			}
			if ((v & 4) == 0) {
				v += gkBit15;
				b = true;
			}
			if (b) {
				v += gkBit16;
			}
			b = false;
			if ((v & 8) == 0) {
				v += gkBit17;
				b = true;
			}
			if ((v & 16) == 0) {
				v += gkBit18;
				b = true;
			}
			if (b) {
				v += gkBit19;
			}
		} else {
			if (g_dbp.at(static_cast<uint32_t>(volt_up_)) > 0) {
				if ((v & 1) != 0) {
					v += gkBit20;
				}
				if ((v & gk128) != 0) {
					auto b{ false };
					if ((v & 2) == 0) {
						v += gkBit14;
						b = true;
					}
					if ((v & 4) == 0) {
						v += gkBit15;
						b = true;
					}
					if (b) {
						v += gkBit16;
					}
				}
			}
			if (g_dbp.at(static_cast<uint32_t>(volt_down_)) > 0) {
				if ((v & 1) != 0) {
					v += gkBit21;
				}
				if ((v & gk256) != 0) {
					auto b{ false };
					if ((v & 8) == 0) {
						v += gkBit17;
						b = true;
					}
					if ((v & 16) == 0) {
						v += gkBit18;
						b = true;
					}
					if (b) {
						v += gkBit19;
					}
				}
			}
		}
		checkAct(v);
	}


	auto IndicatorLightBigActionSwitch::getType() const -> std::string {
		return "b 14 15 16 17 18 19 20 21"s;
	}





	//  Name: FuelCutoff1, FuelCutoff2, FuelCutoff3, FuelCutoff4.
	//  Mode : ECON
	//  Range : 0 .. 7
	//  Add - ons can : Read & write
	//  Description :
	//
	//     Each fuel cutoff switch is a bitmask :
	//     bit 0001 : Switch set to RUN
	//     bit 0002 : Light bulb broken
	//     bit 0004 : Light contact closed


	IndicatorLightFuelCutoffSwitch::IndicatorLightFuelCutoffSwitch(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode),
		volt_{ volt } {
	}

	auto IndicatorLightFuelCutoffSwitch::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(expqcode_)) };
		if ((v & 4) != 0 && g_dbp.at(static_cast<uint32_t>(volt_)) > 0) {
			if ((v & 2) == 0) {
				v += gkBit3;
			}
		}
		checkAct(v);
	}

	auto IndicatorLightFuelCutoffSwitch::getType() const -> std::string {
		return "b 3"s;
	}




	//  Engine start switches
	//  Code : For Qh indices, refer to Variables.txt.
	//  Name : EngStartSw1, EngStartSw2, EngStartSw3, EngStartSw4
	//  Mode : ECON
	//  Range : 0 .. 8199
	//  Add - ons can : Read & write
	//  Description :

	//  Each switch is controlled manually but also by a solenoid.Each switch contains one bulb.Each switch is a Qh bitmask :
	//  bit 0001 : Switch pulled
	//  bit 0002 : Light bulb broken
	//  bit 0004 : Light contact closed by start logic
	//  bit 8192 : Light contact closed by MD & T

	WhiteLightEngStartSwitch::WhiteLightEngStartSwitch(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode),
		volt_{ volt } {
	}

	auto WhiteLightEngStartSwitch::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(expqcode_)) };
		if ((v & gk8KU) != 0) {
			if ((v & 2) == 0) {
				v += gkBit14;
			}
		} else if ((v & 4) != 0 && g_dbp.at(static_cast<uint32_t>(volt_)) > 0) {
			if ((v & 2) == 0) {
				v += gkBit14;
			}
		}
		checkAct(v);
	}

	auto WhiteLightEngStartSwitch::getType() const -> std::string {
		return "b 14"s;
	}





	//  Engine & APU fire switches

	//  Code : For Qh indices, refer to Variables.txt.
	//  Name : FireEng1, FireEng2, FireEng3, FireEng4, FireApu.
	//  Mode : ECON
	//  Range : 0 .. 4095
	//  Add - ons can : Read & write
	//  Description :

	//  Each fire switch is a Qh bitmask :
	//  bit 0001 : pulled
	//  bit 0002 : first light bulb broken
	//  bit 0004 : second light bulb broken
	//  bit 0008 : third light bulb broken
	//  bit 0016 : fourth light bulb broken
	//  bit 0032 : override lock pushed
	//  bit 0064 : light contact closed
	//  bit 0128 : handle angle - 22 degrees
	//  bit 0256 : handle angle - 15 degrees
	//  bit 0512 : handle angle + 15 degrees
	//  bit 1024 : handle angle + 22 degrees

	IndicatorLightFireSwitch::IndicatorLightFireSwitch(const Qcode q, std::string const& name, Qcode volt, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode),
		volt_{ volt } {
	}

	auto IndicatorLightFireSwitch::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(expqcode_)) };
		if ((v & 64) != 0 && g_dbp.at(static_cast<uint32_t>(volt_)) > 0) {
			auto b{ false };
			if ((v & 2) == 0) {
				v += gkBit11;
				b = true;
			}
			if ((v & 4) == 0) {
				v += gkBit12;
				b = true;
			}
			if ((v & 8) == 0) {
				v += gkBit13;
				b = true;
			}
			if ((v & 16) == 0) {
				v += gkBit14;
				b = true;
			}
			if (b) {
				v += gkBit15;
			}
		}
		checkAct(v);
	}

	auto IndicatorLightFireSwitch::getType() const -> std::string {
		return "b 11 12 13 14 15(T)"s;
	}




	//  Green squib pushbuttons

	//  Code : For Qh indices, refer to Variables.txt.
	//  Name : SquibEng1, SquibEng2, SquibEng3, SquibEng4, SquibApu, SquibA, SquibB, SquibC, SquibD,
	//  SquibE, SquibF, SquibAH, SquibBJ, SquibCK, SquibDL, SquibEM, SquibFN, SquibGP.
	//  Mode : ECON
	//  Range : 0 .. 7
	//  Add - ons can : Read & write
	//  Description :

	//  Squib pushbutton installations are aircraft specific.Each squib pushbutton is a Qh bitmask :
	//  bit 0001 : button pushed
	//  bit 0002 : light bulb broken
	//  bit 0004 : light contact closed

	IndicatorLightSquibButton::IndicatorLightSquibButton(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode) {
	}

	auto IndicatorLightSquibButton::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(expqcode_)) };
		if ((v & 4) != 0 && g_dbp.at(static_cast<uint32_t>(Qcode::dimRelayK2b)) > 0) {
			if ((v & 2) == 0) {
				v += gkBit3;
			}
		}
		checkAct(v);
	}

	auto IndicatorLightSquibButton::getType() const -> std::string {
		return "b 3"s;
	}



	//    ELT panel
	//    Code : Qh224
	//    Name : Elt
	//    Mode : ECON
	//    Range : 0 .. 8207
	//    Add - ons can : Read & write
	//    Description :
	//
	//    Qh224 is a bitmask :
	//    bit 0001 : guard closed
	//    bit 0002 : switch up
	//    bit 0004 : switch centered
	//    else : switch down
	//    bit 0008 : LED contact closed by system other than MD & T
	//    bit 8192 : LED contact closed by MD & T

	IndicatorLightELT::IndicatorLightELT(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode) {
	}

	auto IndicatorLightELT::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(expqcode_)) };
		if ((v & gk8KU) != 0) {
			v += gkBit14;
		} else if ((v & 8) != 0 && g_dbp.at(static_cast<uint32_t>(Qcode::nonDimC614)) > 0) {
			v += gkBit14;
		}
		checkAct(v);
	}

	auto IndicatorLightELT::getType() const -> std::string {
		return "b 14"s;
	}



	//  EVAC light
	//  Code : Qh424
	//  Name : EvacLight
	//  Mode : ECON
	//  Range : 0 .. 31
	//  Add - ons can : Read & write
	//  Description :

	//  Qh424 is a bitmask :
	//  bit 0001 : EVAC signal commanded on by pilot
	//  bit 0002 : Light contact closed by flasher
	//  bit 0004 : Light contact closed by test push button
	//  bit 0008 : EVAC signal commanded on by flight attendant

	EvacLight::EvacLight(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode) {
	}

	auto EvacLight::check() -> void {
		auto v{ g_dbp.at(static_cast<uint32_t>(expqcode_)) };
		if ((v & (2 + 4)) != 0 && g_dbp.at(static_cast<uint32_t>(Qcode::nonDimC614)) > 0) {
			v += gkBit5;
		}
		checkAct(v);
	}

	auto EvacLight::getType() const -> std::string {
		return "b 5"s;
	}







	//  ACP MIC 

	//  Code : For Qi indices, refer to Variables.txt.
	//  Name : MicSelectL, MicSelectC, MicSelectR
	//  Mode : ECON
	//  Range : -1 .. 9
	//  Add - ons can : Read only
	//  Description :

	//  This Qi indicates which MIC light contact is closed. 
	//  - 1 means all MIC lights are off. Only one MIC light can illuminate at a time unless the MD & T test 
	//  is running in which case all MIC light contacts are closed(for MD & T status refer to ReceiverSelL, 
	//  ReceiverSelC, ReceiverSelR).The value of each MIC light is :
	//  0 : VHF L
	//  1 : VHF L
	//  2 : VHF L
	//  3 : FLT
	//  4 : CAB
	//  5 : PA
	//  6 : HF L
	//  7 : HF R
	//  8 : SAT L
	//  9 : SAT R

	AcpMICLights::AcpMICLights(const Qcode q, std::string const& name, const LCR lcr)
		:
		InfoVar(q, name),
		lcr_{ lcr } {
	}

  auto AcpMICLights::check() -> void {
    auto power{false};
    auto lt{false};
    if (LCR::left == lcr_) {
      power = g_dbp.at(static_cast<uint32_t>(Qcode::dimRelayK2a)) > 0;
      lt = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi109)) & gk8KU) != 0;
    } else if (LCR::center == lcr_) {
      power = g_dbp.at(static_cast<uint32_t>(Qcode::dimRelayK8b)) > 0;
      lt = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi110)) & gk8KU) != 0;
    } else {  // right
      power = g_dbp.at(static_cast<uint32_t>(Qcode::dimRelayK2b)) > 0;
      lt = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi111)) & gk8KU) != 0;
    }
    if (lt) {
      checkAct(0x3FF);
    } else if (!power) {   // no power
      checkAct(0);
    } else {
      checkAct(g_dbp.at(static_cast<uint32_t>(qcode_)));
    }
  }

	auto AcpMICLights::getType() const -> std::string {
    return "-1: ALL OFF, 0: VHF L, 1: VHF L, 2: VHF L, 3: FLT, 4: CAB, 5: PA, 6: HF L, 7: HF R, 8: SAT L, 9: SAT R"s;
	}






	// ACP - green LEDs

	//  Code : For Qi indices, refer to Variables.txt.
	//  Name : ReceiverSelL, ReceiverSelC, ReceiverSelR.
	//  Mode : ECON
	//  Range : 0 .. 16383
	//  Add - ons can : Read only
	//  Description :

	//Every ReceiverSel variable is a bitmask :
	//  bit 0001 : VHF L light contact closed by ACP
	//  bit 0002 : VHF C light contact closed by ACP
	//  bit 0004 : VHF R light contact closed by ACP
	//  bit 0008 : FLT light contact closed by ACP
	//  bit 0016 : CAB light contact closed by ACP
	//  bit 0032 : PA light contact closed by ACP
	//  bit 0064 : HF L light contact closed by ACP
	//  bit 0128 : HF R light contact closed by ACP
	//  bit 0256 : SAT L light contact closed by ACP
	//  bit 0512 : SAT R light contact closed by ACP
	//  bit 1024 : SPKR light contact closed by ACP
	//  bit 2048 : VOR / ADF light contact closed by ACP
	//  bit 4096 : APP / MKR light contact closed by ACP
	//  bit 8192 : all ACP light contacts closed by MD & T

	//  The LEDs are powered by the respective ACP.During MD & T test, the respective MD & T relay closes 
	//  the contacts for the LED test, but the LEDs are powered by the ACP.


	//>> > Power status of all ACPs

	//  Code : Qi175
	//  Name : AcpPwrBits
	//  Mode : XECON
	//  Range : 0 .. 7
	//  Add - ons can : Read only
	//  Description :

	//  Qi175 is a bitmask :
	//  bit 0001 : ACP L is powered
	//  bit 0002 : ACP R is powered
	//  bit 0004 : ACP C is powered

	//  When an ACP is not powered, its green LEDs cannot illuminate, even when the contacts are closed by the MD & T.
	//  << <

	ACPgreenLEDs::ACPgreenLEDs(const Qcode q, std::string const& name, const LCR lcr)
		:
		InfoVar(q, name),
		lcr_{ lcr } {
	}

	auto ACPgreenLEDs::check() -> void {
		auto power{ false };
		if (LCR::left == lcr_) {
			power = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi175)) & 1) != 0;
		} else if (LCR::right == lcr_) {
			power = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi175)) & 2) != 0;
		} else {  // center
			power = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi175)) & 4) != 0;
		}
		const int32_t v{ g_dbp.at(static_cast<uint32_t>(qcode_)) };
		if (!power) {   // no power
			checkAct(0);
		} else if ((v & gk8KU) != 0) { // lights test
			checkAct(0x1FFF);
		} else {
			checkAct(v);
		}
	}

	auto ACPgreenLEDs::getType() const -> std::string {
		return "b 0(VHFL) 1(VHFC) 2(VHFR) 3(FLT) 4(CAB) 5(PA) 6(HFL) 7(HFR) 8(SATL) 9(SATR) 10(SPKR) 11(VA) 12(AM)"s;
	}







	EtChrVar::EtChrVar(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode),
		volt_{ volt } {
	}

	auto EtChrVar::check() -> void {
		if (0 == g_dbp.at(static_cast<uint32_t>(volt_))) {
			checkAct(0xAAAA);
		} else {
			const auto captain = expqcode_ == Qcode::Qs126;
			const auto chronostate = captain ? Qcode::Qi124 : Qcode::Qi125;
			const auto etBlank = captain ? g_EtCpBlank : g_EtFoBlank;
			if (g_dbp.at(static_cast<uint32_t>(chronostate)) != 0) { // chrono is running
				const auto chronosecs = static_cast<uint32_t>(captain ? g_dbp.at(static_cast<uint32_t>(Qcode::ChroSecsL)) : g_dbp.at(static_cast<uint32_t>(Qcode::ChroSecsR)));
				const auto bcd4 = gf_ui2BCD4b(((chronosecs / 3600U) * 100U) + (chronosecs / 60U));
				const auto dp = (chronosecs / 3600L) >= 1;
				checkAct(bcd4 + (dp ? gkBit16 : 0));
			} else if (!etBlank) {  // ET is running
				const auto etmins = static_cast<uint32_t>(captain ? g_dbp.at(static_cast<uint32_t>(Qcode::ElapMinsL)) : g_dbp.at(static_cast<uint32_t>(Qcode::ElapMinsR)));
				checkAct(gf_ui2BCD4(((etmins / 60U) * 100U) + (etmins % 60U)) + gkBit16);
			} else {
				checkAct(0xAAAA);
			}
		}
	}

	auto EtChrVar::getType() const -> std::string {
		return "BCD4 (for ET & Chrono) + b 16 (DP)"s;
	}





	ClockVar::ClockVar(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode),
		volt_{ volt } {
	}

	auto ClockVar::check() -> void {
		if (0 == g_dbp.at(static_cast<uint32_t>(volt_))) {
			checkAct(0xAAAA);
			hrt100pmin_ = 123456U;
		} else if (hrt100pmin_ != static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)))) {
			hrt100pmin_ = static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)));
			InfoVar::setValue(gf_ui2BCD4(hrt100pmin_));
		}
	}

	auto ClockVar::getType() const -> std::string {
		return "BCD4 HHMM"s;
	}








	ClockVar2::ClockVar2(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode),
		volt_{ volt } {
	}

	auto ClockVar2::check() -> void {
		if (0 == g_dbp.at(static_cast<uint32_t>(volt_))) {
			checkAct(0xAAAA);
			montht100pday_ = 123456U;
		} else if (montht100pday_ != static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)))) {
			montht100pday_ = static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)));
			InfoVar::setValue(gf_ui2BCD4(montht100pday_));
		}
	}

	auto ClockVar2::getType() const -> std::string {
		return "BCD4 MMDD"s;
	}






	ClockVar3::ClockVar3(const Qcode q, std::string const& name, const Qcode volt, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode),
		volt_{ volt } {
	}

	auto ClockVar3::check() -> void {
		if (0 == g_dbp.at(static_cast<uint32_t>(volt_))) {
			checkAct(0xAAAA);
			year_ = 123456U;
		} else if (year_ != static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)))) {
			year_ = static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)));
			InfoVar::setValue(gf_ui2BCD4(year_));
		}
	}

	auto ClockVar3::getType() const -> std::string {
		return "BCD4 YEAR"s;
	}








	RcpDispl::RcpDispl(const Qcode q, std::string const& name, const Qcode expqcode, const Qcode indicatorlightvar, const Qcode volt)
		:
		ExpQscodeInfoVar(q, name, expqcode),
		indicatorlightvar_{ indicatorlightvar },
		volt_{ volt } {
	}

	auto RcpDispl::check() -> void {
		const auto f{ static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_))) };
		if (0 == g_dbp.at(static_cast<uint32_t>(volt_)) || (g_dbp.at(static_cast<uint32_t>(indicatorlightvar_)) & 4096) != 0) {
			checkAct(0x0AA'AAAA);  // radio is blank
		} else if ((g_dbp.at(static_cast<uint32_t>(indicatorlightvar_)) & gk8KU) != 0) {
			checkAct(0x388'8888);  // MD&T test: "888888"
		} else if ((g_dbp.at(static_cast<uint32_t>(indicatorlightvar_)) & (128 + 256)) != 0) {
			checkAct(0xBB'BBBB);  // panel is FAIL or INOP, show "------"
		} else if (0 == f) { // no VHF or HF, possibly DATA std::string
			checkAct(0x0AA'AAAA);  // let radio blank
		} else if ((g_dbp.at(static_cast<uint32_t>(indicatorlightvar_)) & 7) != 0) {  // VHF
			checkAct(gf_ui2BCD6(f) + 0x100'0000);
		} else if ((g_dbp.at(static_cast<uint32_t>(indicatorlightvar_)) & 40) != 0) { // HF
			checkAct(gf_ui2BCD6b(f) + 0x200'0000);
		} else {
			checkAct(0x0AA'AAAA);  // radio is blank
		}
	}

	auto RcpDispl::getType() const -> std::string {
		return "VHF: BCD6 + b 24(DP), HF: BCD6b + b 25(DP)"s;
	}



	// Transponder panel - indications

	//  Code : Qs118
	//  Name : TcasPanStat
	//  Mode : ECON
	//  Length : 11
	//  Add - ons can : Read only
	//  Example :
	//  77007700NNN
	//  Description :

	//  There are no semicolons.The digits at position 1 through 4 are the active squawk (but ATC add - ons should read Qi204 instead of Qs118).
	//  The digits at position 5 through 8 are the ones that are displayed on the LCD; those should be used by hardware panel add - ons
	//  The displayed value may be cleared and is not necessarily equal to the the active squawk.

	//  Another example :
	//  77005___NNF
	//  In this example, only the first digit of the tansponder code should be displayed on the LCD, the "5" in this case.The other three digits are blank on the LCD(an underscore indicates a blank digit).

	//  The last three characters refer to certain system modes :

	//  Character at position 9 :
	//  L : MD & T test on(all LCD segments on and XPDR FAIL light on)
	//  N : MD & T test off

	//  Character at position 10 :
	//  B : Blank display
	//  P : PASS displayed
	//  N : Normal display

	//  Character at position 11 (this is not relevant to add - ons) :
	//  F : F / L mode on
	//  N : F / L mode off

	SquawkDispl::SquawkDispl(const Qcode q, std::string const& name, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode) {
	}

 

	auto SquawkDispl::check() -> void {
		if (g_dbp.at(static_cast<uint32_t>(Qcode::gDivAislePnlT951)) == 0 || g_TcasBlank) {  // Xpndr LCD power off, or Transponder Blank
			checkAct(0x0'AAAA);
		} else if (g_TcasPass) {
			checkAct(0x0'BBBB);  // test shows PASS in PSX, show "----"
		} else if (g_TcasMDT) {
			checkAct(0x0'8888);
		} else {
			auto sq{ g_dbp.at(static_cast<uint32_t>(qcode_)) };
			if (sq < 0) {
				checkAct(0x0'AAAA);
			} else if (0 == sq) {
				if (1 == g_TcasNumDigits) {
					checkAct(0 * gk4K + 10 * gk256 + 10 * gk16 + 10);
				} else if (2 == g_TcasNumDigits) {
					checkAct(0 * gk4K + 0 * gk256 + 10 * gk16 + 10);
				} else if (3 == g_TcasNumDigits) {
					checkAct(0 * gk4K + 0 * gk256 + 0 * gk16 + 10);
				} else {
					checkAct(0 * gk4K + 0 * gk256 + 0 * gk16 + 0);
				}
			} else {
				const auto sqU = static_cast<uint32_t>(sq);
				if (sqU < gkTenU) {
					checkAct(sqU * gk4KU + gkTenU * gk256U + gkTenU * gk16U + gkTenU);
				} else if (sqU < 100U) {
					checkAct(gf_ui2BCD2(sqU) * gk256U + gkTenU * gk16U + gkTenU);
				} else if (sqU < 1000U) {
					checkAct(gf_ui2BCD3(sqU) * gk16U + gkTenU);
				} else {
					checkAct(gf_ui2BCD4(sqU));
				}
			}
		}
	}

	auto SquawkDispl::getType() const -> std::string {
		return "BCD4 (trailing A's possible, such as 1AAA; ---- means PASS)"s;
	}

  XpdrFail::XpdrFail(const Qcode q, std::string const& name, const Qcode expqcode)
    :
    ExpQscodeInfoVar(q, name, expqcode) {
  }

  auto XpdrFail::check() -> void {
    checkAct(g_XpdrFail ? 1 : 0);
  }

  auto XpdrFail::getType() const -> std::string {
    return "integer: 0/1"s;
  }


	// Transponder status
	//  Code : Qi204
	//  Name : Xpndr
	//  Mode : ECON
	//  Range : 10000 .. 317777
	//  Add - ons can : Read only
	//  Description :

	//  1 : Transponder OFF
	//  2 : Transponder ON, and ALT RPTG OFF
	//  3 : Transponder ON, and ALT RPTG ON

	//  The second digit is 1 when the ident is on, else it is 0. Once started, the ident mode remains on for 20 seconds and then deactivates automatically; the ident key need not be held.The last four digits are the active transponder code.

	SquawkIdent::SquawkIdent(const Qcode q, std::string const& name)
		:
		InfoVar(q, name) {
	}

	auto SquawkIdent::check() -> void {
		if (const auto v{ static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_))) }; v >= 200'000) {  // if first digit is a 1, then XPDR is OFF
			const auto xpndr{ static_cast<int32_t>(v % gkTenThousandU) };
			if (*g_CfgSquawkAsInt) {
        checkAct(xpndr);
			} else {
				const auto ident{(((v & gkOneHundredThousandU) / gkTenThousandU) == 1)};
				if (0 == xpndr) {
					checkAct(0x0000 + (ident ? 0x1'0000 : 0));
				} else {
					checkAct(gf_ui2BCD4(xpndr) + (ident ? 0x1'0000 : 0));
				}
			}
		}
	}

	auto SquawkIdent::getType() const -> std::string {
		return "BCD4 + b 16 (ident)"s;
	}





	//  CALL lights
	//  Call lights are activated by other avionic components.
	//  When a call is made, the respective call is indicated on all three ACPs.
	//  The following Qi variables need to be monitored :
	//  CallCargoStat(freighter only)
	//  CallFrVhfL
	//  CallFrVhfC
	//  CallFrVhfR
	//  CallFrIntph
	//  CallFrCabin
	//  CallFrHfL
	//  CallFrHfR

	//  CallCargoStat is a bitmask : if (CallCargoStat & 7) != 0 then CAB CALL light contact is closed.
	//  For all CallFr... variables : if value > 0 then respective CALL light contact is closed.
	//  When the MD & T test is running, all CALL light contacts are closed (for MD & T status refer to ReceiverSelL, ReceiverSelC, ReceiverSelR).

	CallFrLight::CallFrLight(const Qcode q, std::string const& name, const LCR lcr, const Qcode expqcode)
		:
		ExpQscodeInfoVar(q, name, expqcode),
		lcr_{ lcr } {
	}
  auto CallFrLight::check() -> void {
    auto power{false};
    auto lt{false};
    if (LCR::left == lcr_) {
      power = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi175)) & 1) != 0;
      lt = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi106)) & gk8KU) != 0;
    } else if (LCR::center == lcr_) {  // center
      power = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi175)) & 4) != 0;
      lt = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi107)) & gk8KU) != 0;
    } else {
      power = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi175)) & 2) != 0;
      lt = (g_dbp.at(static_cast<uint32_t>(Qcode::Qi108)) & gk8KU) != 0;
    }
    if (lt) {
      checkAct(1);
    } else if (!power) {
      checkAct(0);
    } else {
      checkAct(g_dbp.at(static_cast<uint32_t>(expqcode_)));  // Feb 7, 2017: CallFrCabin values are between 0 and 99, others just 0 or 1
    }
  }

	auto CallFrLight::getType() const -> std::string {
		std::string res;
		if (expqcode_ == Qcode::Qi102) {
			res = "0..99"s;
		} else {
			res = "b 0"s;
		}
		return res;
	}


	//  MCP HDG window
	//  Code : Qi33
	//  Name : McpWdoHdg
	//  Mode : ECON
	//  Range : 0 .. 359
	//  Add - ons can : Read only
	//  Description :
	//  The HDG window may be blanked by the MD & T test.

	McpHdgWin::McpHdgWin(const Qcode q, std::string const& name)
		:
		InfoVar(q, name) {
	}

	auto McpHdgWin::check() -> void {
		const auto lcd{ 1 == g_dbp.at(static_cast<uint32_t>(Qcode::Qi12)) };  // LCD type MCP?
		const auto mdt{ g_dbp.at(static_cast<uint32_t>(Qcode::Qi36)) };  // MCD LCD power and test control
		if (lcd && 0 == mdt) {
			checkAct(0xAAA);
			hdg_ = 12345U;
		} else if (lcd && 2 == mdt) {
			checkAct(0x888);
			hdg_ = 12345U;
		} else {
			if (hdg_ != static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)))) {
				hdg_ = static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)));
				InfoVar::setValue(gf_ui2BCD3(hdg_));
			}
		}
	}

	auto McpHdgWin::getType() const -> std::string {
		return "BCD3"s;
	}



	//  MCP SPD window
	//  Code : Qi32
	//  Name : McpWdoSpd
	//  Mode : ECON
	//  Range : 0 .. 1950
	//  Add - ons can : Read only
	//  Description :
	//  When the value is greater than 950, the SPD window is blanked by the AFDS.
	//  The SPD window may also be blanked by the MD & T test.
	//  The Mach word and the Mach dot should disappear when the value is below 400.

	McpSpdWin::McpSpdWin(const Qcode q, std::string const& name)
		:
		InfoVar(q, name) {
	}

	auto McpSpdWin::check() -> void {
		const auto lcd{ 1 == g_dbp.at(static_cast<uint32_t>(Qcode::Qi12)) };  // LCD type MCP?
		const auto mdt{ g_dbp.at(static_cast<uint32_t>(Qcode::Qi36)) };  // MCD LCD power and test control
		if (lcd && 0 == mdt) {
			checkAct(0xAAA);
			spd_ = 12345U;
		} else if (lcd && 2 == mdt) {
			checkAct(0x1888);
			spd_ = 12345U;
		} else {
			if (spd_ != static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)))) {
				spd_ = static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)));
				if (spd_ > 950U) {
					InfoVar::setValue(0xAAA);
				} else if (spd_ > 400U) {
					InfoVar::setValue(gf_ui2BCD3(spd_) + 0x1000);
				} else {
					InfoVar::setValue(gf_ui2BCD3(spd_));
				}
			}
		}
	}

	auto McpSpdWin::getType() const -> std::string {
		return "BCD3 + b 12 (DP)"s;
	}


	//  MCP ALT window
	//  Code : Qi35
	//  Name : McpWdoAlt
	//  Mode : ECON
	//  Range : 0 .. 500
	//  Add - ons can : Read only
	//  Description :

	//  The value represents hundreds of feet. 
	//  When the value is 0, the hardware display should indicate "00000".
	//  In all other cases, leading zeroes should not be indicated on the display.
	//  The ALT window may be blanked by the MD & T test.

	McpAltWin::McpAltWin(const Qcode q, std::string const& name)
		:
		InfoVar(q, name) {
	}


	auto McpAltWin::check() -> void {
		const auto lcd{ 1 == g_dbp.at(static_cast<uint32_t>(Qcode::Qi12)) };  // LCD type MCP?
		const auto mdt{ g_dbp.at(static_cast<uint32_t>(Qcode::Qi36)) };  // MCD LCD power and test control
		if (lcd && 0 == mdt) {
			checkAct(0xA'AAAA);
			alt_ = 12345U;
		} else if (lcd && 2 == mdt) {
			checkAct(0x8'8888);
			alt_ = 12345U;
		} else {
			if (alt_ != static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)))) {
				alt_ = static_cast<uint32_t>(g_dbp.at(static_cast<uint32_t>(qcode_)));
				if (0 == alt_) {
					InfoVar::setValue(0x0'0000);
				} else {
					InfoVar::setValue(gf_ui2BCD3b(alt_) * gk256);
				}
			}
		}
	}

	auto McpAltWin::getType() const -> std::string {
		return "BCD5"s;
	}


	//  MCP V / S window
	//  Code : Qi34
	//  Name : McpWdoVs
	//  Mode : ECON
	//  Range : -80 .. 60
	//  Add - ons can : Read only
	//  Description :

	// The value represents hundreds of feet. When the value is 0, the hardware display should indicate "0000".
	// In all other cases, leading zeroes should not be indicated on the display.
	// Non - zero values should be preceded by - and +signs.
	// The V / S window should be blank when the V / S mode is not engaged.
	// Mode engagement can be checked by reading the variable McpPshVs(Qh69).
	// This is the V / S switch with the in - built light.
	// If the light contact is closed by the AFDS, the V / S window is not blank :

	//  if (McpPshVs & 8) == 0 then window is blank.
	//    The V / S window may also be blanked by the MD & T test.

	McpVsWin::McpVsWin(const Qcode q, std::string const& name)
		:
		InfoVar(q, name) {
	}

	auto McpVsWin::check() -> void {
		const auto lcd{ 1 == g_dbp.at(static_cast<uint32_t>(Qcode::Qi12)) };  // LCD type MCP?
		const auto mdt{ g_dbp.at(static_cast<uint32_t>(Qcode::Qi36)) };  // MCD LCD power and test control
		if (lcd && 0 == mdt) {
			checkAct(0xA'AAAA);
			vs_ = 12345;
		} else if (lcd && 2 == mdt) {
			checkAct(0x8'8888);
			vs_ = 12345;
		} else if ((g_dbp.at(static_cast<uint32_t>(Qcode::Qh69)) & 8) == 0) { // V/S not engaged
			checkAct(0xA'AAAA);
			vs_ = 12345;
		} else {
			if (vs_ != g_dbp.at(static_cast<uint32_t>(qcode_))) {
				vs_ = g_dbp.at(static_cast<uint32_t>(qcode_));
				if (vs_ == 0) {
					InfoVar::setValue(0xA'0000);
				} else {
					try {
						InfoVar::setValue(gf_i2BCD3bn(vs_) * gk256);
					}
					catch (...) {
						// negative var too big
					}
				}
			}
		}
	}

	auto McpVsWin::getType() const -> std::string {
		return "BCD4bn"s;
	}
}  // namespace nwk
