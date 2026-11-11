#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {


  // === General powers and digits ===
  inline constexpr uint32_t gkTenU{ 10 };
  inline constexpr uint32_t gkElevenU{ 11 };
  inline constexpr uint32_t gkHundredU{ 100 };
  inline constexpr uint32_t gkThousandU{ 1'000 };
  inline constexpr uint32_t gkTenThousandU{ 10'000 };
  inline constexpr uint32_t gkOneHundredThousandU{ 100'000 };
  inline constexpr uint32_t gkOneMillionU{ 1'000'000 };

  inline constexpr int32_t gkThousand{ 1'000 };
  inline constexpr int32_t gkTenThousand{ 10'000 };
  inline constexpr int32_t gkHundredThousand{ 100'000 };

  inline constexpr double gkHundredD{ 100.0 };

  inline constexpr uint32_t gk16U{ 16 };
  inline constexpr uint32_t gk128U{ 128 };
  inline constexpr uint32_t gk256U{ 256 };
  inline constexpr uint32_t gk512U{ 512 };
  inline constexpr uint32_t gk4KU{ 4'096 };
  inline constexpr uint32_t gk8KU{ 8'192 };
  inline constexpr uint32_t gk16KU{ 16'384 };
  inline constexpr uint32_t gk64KU{ 65'536 };
  inline constexpr uint32_t gk1MU{ 1'048'576 };
  inline constexpr uint32_t gk16MU{ 16'777'216 };

  inline constexpr int32_t gk16{ 16 };
  inline constexpr int32_t gk128{ 128 };
  inline constexpr int32_t gk256{ 256 };
  inline constexpr int32_t gk4K{ 4'096 };
  inline constexpr int32_t gk8K{ 8'192 };
  inline constexpr int32_t gk16K{ 16'384 };
  inline constexpr int32_t gk64K{ 65'536 };
  inline constexpr int32_t gk1M{ 1'048'576 };
  inline constexpr int32_t gk16M{ 16'777'216 };

  // === Bit masks ===
  inline constexpr int32_t gkBit0 = 0b1;
  inline constexpr int32_t gkBit1 = 0b10;
  inline constexpr int32_t gkBit2 = 0b100;
  inline constexpr int32_t gkBit3 = 0b1000;
  inline constexpr int32_t gkBit4 = 0b1'0000;
  inline constexpr int32_t gkBit5 = 0b10'0000;
  inline constexpr int32_t gkBit6 = 0b100'0000;
  inline constexpr int32_t gkBit7 = 0b1000'0000;
  inline constexpr int32_t gkBit8 = 0b1'0000'0000;
  inline constexpr int32_t gkBit9 = 0b10'0000'0000;
  inline constexpr int32_t gkBit10 = 0b100'0000'0000;
  inline constexpr int32_t gkBit11 = 0b1000'0000'0000;
  inline constexpr int32_t gkBit12 = 0b1'0000'0000'0000;
  inline constexpr int32_t gkBit13 = 0b10'0000'0000'0000;
  inline constexpr int32_t gkBit14 = 0b100'0000'0000'0000;
  inline constexpr int32_t gkBit15 = 0b1000'0000'0000'0000;
  inline constexpr int32_t gkBit16 = 0b1'0000'0000'0000'0000;
  inline constexpr int32_t gkBit17 = 0b10'0000'0000'0000'0000;
  inline constexpr int32_t gkBit18 = 0b100'0000'0000'0000'0000;
  inline constexpr int32_t gkBit19 = 0b1000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit20 = 0b1'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit21 = 0b10'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit22 = 0b100'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit23 = 0b1000'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit24 = 0b1'0000'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit25 = 0b10'0000'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit26 = 0b100'0000'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit27 = 0b1000'0000'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit28 = 0b1'0000'0000'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit29 = 0b10'0000'0000'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit30 = 0b100'0000'0000'0000'0000'0000'0000'0000;
  inline constexpr int32_t gkBit31 = 0b1000'0000'0000'0000'0000'0000'0000'0000;

  inline constexpr std::array<int32_t, 32> gk32Bits = {
    gkBit0, gkBit1, gkBit2, gkBit3, gkBit4, gkBit5, gkBit6, gkBit7,
    gkBit8, gkBit9, gkBit10, gkBit11, gkBit12, gkBit13, gkBit14, gkBit15,
    gkBit16, gkBit17, gkBit18, gkBit19, gkBit20, gkBit21, gkBit22, gkBit23,
    gkBit24, gkBit25, gkBit26, gkBit27, gkBit28, gkBit29, gkBit30, gkBit31
  };

 
  // === BCD ===
  using bcd = uint32_t;

  constexpr auto gf_ui2BCD2(const uint32_t i) noexcept -> bcd {
    return i % gkTenU + ((i % gkHundredU) / gkTenU) * gk16U;
  }

  constexpr auto gf_ui2BCD3(const uint32_t i) noexcept -> bcd {
    return gf_ui2BCD2(i) + ((i % gkThousandU) / gkHundredU) * gk256U;
  }

  constexpr auto gf_ui2BCD4(const uint32_t i) noexcept -> bcd {
    return gf_ui2BCD3(i) + ((i % gkTenThousandU) / gkThousandU) * gk4KU;
  }

  constexpr auto gf_ui2BCD5(const uint32_t i) noexcept -> bcd {
    return gf_ui2BCD4(i) + ((i % gkOneHundredThousandU) / gkTenThousandU) * gk64KU;
  }

  constexpr auto gf_ui2BCD6(const uint32_t i) noexcept -> bcd {
    return gf_ui2BCD5(i) + ((i % gkOneMillionU) / gkOneHundredThousandU) * gk1MU;
  }

  constexpr auto gf_ui2BCD3b(const uint32_t i) noexcept -> bcd {
    const auto d0 = i % gkTenU;
    auto d1 = (i % gkHundredU) / gkTenU;
    auto d2 = (i % gkThousandU) / gkHundredU;
    if (d2 == 0) {
      d2 = gkTenU;
      if (d1 == 0) {
        d1 = gkTenU;
      }
    }
    return d0 + d1 * gk16U + d2 * gk256U;
  }

  constexpr auto gf_ui2BCD4b(const uint32_t i) noexcept -> bcd {
    const auto d0 = i % gkTenU;
    auto d1 = (i % gkHundredU) / gkTenU;
    auto d2 = (i % gkThousandU) / gkHundredU;
    auto d3 = (i % gkTenThousandU) / gkThousandU;
    if (d3 == 0) {
      d3 = gkTenU;
      if (d2 == 0) {
        d2 = gkTenU;
        if (d1 == 0) {
          d1 = gkTenU;
        }
      }
    }
    return d0 + d1 * gk16U + d2 * gk256U + d3 * gk4KU;
  }

  constexpr auto gf_ui2BCD5b(const uint32_t i) noexcept -> bcd {
    const auto d0 = i % gkTenU;
    auto d1 = (i % gkHundredU) / gkTenU;
    auto d2 = (i % gkThousandU) / gkHundredU;
    auto d3 = (i % gkTenThousandU) / gkThousandU;
    auto d4 = (i % gkOneHundredThousandU) / gkTenThousandU;
    if (d4 == 0) {
      d4 = gkTenU;
      if (d3 == 0) {
        d3 = gkTenU;
        if (d2 == 0) {
          d2 = gkTenU;
          if (d1 == 0) {
            d1 = gkTenU;
          }
        }
      }
    }
    return d0 + d1 * gk16U + d2 * gk256U + d3 * gk4KU + d4 * gk64KU;
  }

  constexpr auto gf_ui2BCD6b(const uint32_t i) noexcept -> bcd {
    const auto d0 = i % gkTenU;
    auto d1 = (i % gkHundredU) / gkTenU;
    auto d2 = (i % gkThousandU) / gkHundredU;
    auto d3 = (i % gkTenThousandU) / gkThousandU;
    auto d4 = (i % gkOneHundredThousandU) / gkTenThousandU;
    auto d5 = (i % gkOneMillionU) / gkOneHundredThousandU;
    if (d5 == 0) {
      d5 = gkTenU;
      if (d4 == 0) {
        d4 = gkTenU;
        if (d3 == 0) {
          d3 = gkTenU;
          if (d2 == 0) {
            d2 = gkTenU;
            if (d1 == 0) {
              d1 = gkTenU;
            }
          }
        }
      }
    }
    return d0 + d1 * gk16U + d2 * gk256U + d3 * gk4KU + d4 * gk64KU + d5 * gk1MU;
  }

  // Signed version declaration
  auto gf_i2BCD3bn(const int32_t i) -> bcd;

  namespace {

    static_assert(gf_ui2BCD2(42) == 0x42, "BCD2(42) should be 0x42");
    static_assert(gf_ui2BCD3(123) == 0x123, "BCD3(123) should be 0x123");
    static_assert(gf_ui2BCD4(5678) == 0x5678, "BCD4(5678) should be 0x5678");
    static_assert(gf_ui2BCD5(12345) == 0x12345, "BCD5(12345) should be 0x12345");
    static_assert(gf_ui2BCD6(999999) == (9 + 9 * gk16U + 9 * gk256U + 9 * gk4KU + 9 * gk64KU + 9 * gk1MU),
                  "BCD6(999999) should match manual encoding");

    // Blanked versions: leading zeros replaced by 10
    static_assert(gf_ui2BCD3b(5) == (5 + 10 * gk16U + 10 * gk256U), "BCD3b(5) blanks correctly");
    static_assert(gf_ui2BCD4b(42) == (2 + 4 * gk16U + 10 * gk256U + 10 * gk4KU), "BCD4b(42) blanks correctly");
    static_assert(gf_ui2BCD5b(7) == (7 + 10 * gk16U + 10 * gk256U + 10 * gk4KU + 10 * gk64KU), "BCD5b(7) blanks correctly");
    static_assert(gf_ui2BCD6b(0) == (0 + 10 * gk16U + 10 * gk256U + 10 * gk4KU + 10 * gk64KU + 10 * gk1MU),
                  "BCD6b(0) blanks all digits");

  }
 

} // namespace nwk

