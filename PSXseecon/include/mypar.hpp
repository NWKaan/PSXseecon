#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
  template <class T>
  struct MyPar {
    bool isSet_ = false;
    T value_;
    std::string text_;
    MyPar(void) = default;
    MyPar(const T value, const std::string text) {
      isSet_ = false;
      value_ = value;
      text_ = text;
    }

    ~MyPar() {}

    auto operator=(const MyPar& m) -> MyPar& {
      if (this != &m) {
        isSet_ = m.isSet_;
        value_ = m.value_;
        text_ = m.text_;
      }
      return (*this);
    }

    inline auto operator*() const -> T {
      return value_;
    }
  };

  using StringPar = MyPar<std::string>;
  using IntPar = MyPar<int32_t>;
  using Uint16Par = MyPar<uint16_t>;
  using BoolPar = MyPar<bool>;
} 
