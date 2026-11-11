#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
	auto gft_logline(std::string const& s) -> void;

	template<typename T>
	class Cf {
	public:
		std::string arg_;
		T init_val_, min_val_, max_val_;   // min and max only for integer configuration parameters
		T value_;
		Cf();
    Cf(std::string arg, T i_val)
      requires (std::is_same<bool, T>::value) || (std::is_same<std::string, T>::value);
    Cf(std::string arg, T i_val, T min_val, T max_val)
      requires (std::is_same<bool, T>::value) || (std::is_same<int32_t, T>::value) || (std::is_same<std::string, T>::value);

		virtual ~Cf() = default;
		Cf(Cf const& v) = default;
		Cf(Cf && v) = default;
		auto operator=(Cf const& v) -> Cf& = default;
		auto operator=(Cf && v) -> Cf& = default;

		auto setValue(const T v) -> void;  // set value and write to PSXseecon.cfg
		auto readValue() -> void;      // read value from PSXseecon.cfg (if available) 
		inline auto operator*() const -> T;

	private:
    auto updateCfgPar(const T value) const -> void requires (std::is_same<bool, T>::value) || (std::is_same<int32_t, T>::value) || (std::is_same<std::string, T>::value) ;
    auto log() const -> void requires (std::is_same<bool, T>::value) || (std::is_same<int32_t, T>::value) || (std::is_same<std::string, T>::value) ;
	};

#pragma warning(push)
#pragma warning(push)
#pragma warning(disable:26455)
#pragma warning(disable:26495)
	template<typename T>
	Cf<T>::Cf()
		:
		arg_{ gkEmptyStr } {
	}
#pragma warning(pop)
#pragma warning(pop)

	template<typename T>
  Cf<T>::Cf(std::string arg, T i_val)  
		requires (std::is_same<bool, T>::value) || (std::is_same<std::string, T>::value)
		:
		arg_{ std::move(arg) },
		init_val_{ i_val },
		min_val_{ i_val },
		max_val_{ i_val },
		value_{ i_val } {
	}

	template<typename T>
  Cf<T>::Cf(std::string arg, T i_val, T min_val, T max_val) 
		requires (std::is_same<bool, T>::value) || (std::is_same<int32_t, T>::value) || (std::is_same<std::string, T>::value)
		:
		arg_{ std::move(arg) },
		init_val_{ i_val },
		min_val_{ min_val },
		max_val_{ max_val },
		value_{ i_val } {
	}

	template<typename T>
	auto Cf<T>::setValue(const T val) -> void {
		if constexpr (std::is_same<int32_t, T>::value) {
			if (val < min_val_) {
				value_ = min_val_;
			} else if (val > max_val_) {
				value_ = max_val_;
			} else {
				value_ = val;
			}
		} else {
			value_ = val;
		}
		updateCfgPar(value_);
	}

	template<typename T>
	auto Cf<T>::operator*() const -> T {
		return value_;
	}

	template<typename T>
	auto Cf<T>::readValue() -> void {
		if (TxtFileReader r; r.openFile(gkProductNameCfgPath)) {
			if constexpr (std::is_same<bool, T>::value) {
				if (const auto h{ r.findArgIntAsBool(arg_) }; !h.empty()) {
					value_ = h == gkDigit1Str;
				} else {
					value_ = init_val_;
				}
			} else if constexpr (std::is_same<int32_t, T>::value) {
				if (const auto h{ r.findArgNumber(arg_) }; !h.empty()) {
					auto res{ value_ };
					try {
						const auto [ptr, ec] = std::from_chars(h.data(), h.data() + h.size(), res);
						if (ec != std::errc{} || ptr != h.data() + h.size()) {
							throw std::invalid_argument{ "invalid integer input" };
						}
					} catch (...) {
						gft_logline("read in value EXCEPTION " + h);
					}
					if (res < min_val_) {
						res = min_val_;
					} else {
						if (res > max_val_) {
							res = max_val_;
						}
					}
					value_ = res;
				} else {
					value_ = init_val_;
				}
			} else {
				if constexpr (std::is_same<std::string, T>::value) {
					if (const auto h{r.findArgString(arg_)}; !h.empty()) {
						value_ = h;
					} else {
						value_ = init_val_;
					}
				}
			}
			r.closeFile();
		}
		log();
	}

	auto gf_updateCfgPar(std::string const& arg, std::string const& value) -> void;

	template<typename T>
	auto Cf<T>::updateCfgPar(const T value) const -> void
    requires (std::is_same<bool, T>::value) || (std::is_same<int32_t, T>::value) || (std::is_same<std::string, T>::value) {
		if constexpr (std::is_same<bool, T>::value) {
			gf_updateCfgPar(arg_, value ? gkDigit1Str : gkDigit0Str);
		} else if constexpr (std::is_same<int32_t, T>::value) {
			gf_updateCfgPar(arg_, std::format("{}", value));
		} else {
			if constexpr (std::is_same<std::string, T>::value) {
				gf_updateCfgPar(arg_, value);
			}
		}
	}

	template<typename T>
	auto Cf<T>::log() const -> void
    requires (std::is_same<bool, T>::value) || (std::is_same<int32_t, T>::value) || (std::is_same<std::string, T>::value) {
		if constexpr (std::is_same<bool, T>::value) {
			gft_logline(arg_ + gkEqualStr + (value_ ? gkDigit1Str : gkDigit0Str));
		} else if constexpr (std::is_same<int32_t, T>::value) {
			gft_logline(arg_ + std::format("={}", value_));
		} else {
			if constexpr (std::is_same<std::string, T>::value) {
				gft_logline(arg_ + gkEqualStr + value_);
			}
		}
	}
	using CfgBool = Cf<bool>;
	using CfgInt = Cf<int32_t>;
	using CfgString = Cf<std::string>;
}
  
