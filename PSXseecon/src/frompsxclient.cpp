//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
//
#include "stdafx.hpp"

namespace nwk {
  using namespace std::literals;
 
namespace {
  constinit auto l_dominanceInterrupted{false};

  template<typename T>
  [[nodiscard]] auto lf_fromChars(std::string_view s) noexcept -> std::optional<T> {
    T result{};
    const auto* const first{s.data()};
    const auto* const last{s.data() + s.size()};
    const auto [ptr, ec] = std::from_chars(first, last, result);
    if (ec != std::errc{} || ptr != last) {
      return std::nullopt;
    }
    return result;
  }
} // unnamed space

  //	constexpr auto kDefaultTL{6'000};

  FromPSXClient::FromPSXClient()
    :
    FromTCPIPClient("FromPSX"s) {
  }

  auto FromPSXClient::canBeSkipped(std::string const& s) -> bool {
    // is dit wel nodig?  alleen crash gemeld door JP, niet reproduceerbaar, is format Qs375 etc. wel altijd goed door Hardy?
    if (s.size() > 100U) {
      if (s.starts_with("Qs375"s) || s.starts_with("Qs376"s) || s.starts_with("Qs377"s) ||
          s.starts_with("Qs407"s) || s.starts_with("Qs408"s) || s.starts_with("Qs409"s) ||
          s.starts_with("Qs410"s) || s.starts_with("Qs411"s) || s.starts_with("Qs412"s)) {
        // strings of up to 50000 chars, it is efficient not to process these, unless there are other Q message following:
        if (s.find("Qs"s, 6) == std::string::npos && s.find("Qi"s, 6) == std::string::npos && s.find("Qh"s, 6) == std::string::npos) {
          return true;
        }
      }
    }
    return false;
  }

  auto FromPSXClient::decodeMessage(std::string const& line) -> void {
    static const std::regex kEverythingBeforeCrLf{R"((.+)\r?\n)"};  // prepare for \r\n (Windows) and \n (Mac) !!!! Jan 16, 2015
    if (!FromPSXClient::canBeSkipped(line)) {
      try {
        const std::regex_token_iterator<std::string::const_iterator> rend; // end-of-sequence iterator
        std::regex_token_iterator<std::string::const_iterator> rit(line.cbegin(), line.cend(), kEverythingBeforeCrLf, 1);
        while (rit != rend) {
          decodeOneMessage(*rit);
          ++rit;
        }
      }
      catch (const ExitPSXReceivedException& e) {
        UNREFERENCED_PARAMETER(e);
        throw;  // re-throw,  main will handle this
      }
      catch (const std::exception& e) {
        g_log.line(gkQuestionmarkStr + gkSpaceChar + __func__ + gkSpaceChar + line + gkSpaceChar + e.what() + gkNewLineStr);
        throw;
      }
    }
  }

  auto FromPSXClient::initInicio(QcodeSet const& registeredvars) -> void {
    // Get the pointers to the Vars in use
    for (auto q : registeredvars) {
      auto ptr{varptrfie_(q)};
      if (!ptr) {  // not necessary but defensive programming
        throw std::runtime_error(std::string(__func__));
      }
      varpool_[static_cast<uint32_t>(q)] = ptr;
    };

    Qs106_ = (
      (varpool_[static_cast<uint32_t>(Qcode::Qi92)] != nullptr) ||
      varpool_[static_cast<uint32_t>(Qcode::L_ActVHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_StbVHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_ActVHFC)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_StbVHFC)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_ActVHLR)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_StbVHFR)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_ActHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_StbHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_ActHFR)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_StbHFR)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_SenHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_SenHFR)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_AMActHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::L_AMActHFR)]
      );

    Qs107_ =
      ((varpool_[static_cast<uint32_t>(Qcode::Qi93)] != nullptr) ||
       varpool_[static_cast<uint32_t>(Qcode::C_ActVHFL)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_StbVHFL)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_ActVHFC)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_StbVHFC)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_ActVHLR)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_StbVHFR)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_ActHFL)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_StbHFL)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_ActHFR)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_StbHFR)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_SenHFL)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_SenHFR)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_AMActHFL)] ||
       varpool_[static_cast<uint32_t>(Qcode::C_AMActHFR)]
       );

    Qs108_ = (
      (varpool_[static_cast<uint32_t>(Qcode::Qi94)] != nullptr) ||
      varpool_[static_cast<uint32_t>(Qcode::R_ActVHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_StbVHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_ActVHFC)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_StbVHFC)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_ActVHLR)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_StbVHFR)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_ActHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_StbHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_ActHFR)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_StbHFR)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_SenHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_SenHFR)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_AMActHFL)] ||
      varpool_[static_cast<uint32_t>(Qcode::R_AMActHFR)]
      );

    Qs118_ = (varpool_[static_cast<uint32_t>(Qcode::SquawkDispl)] || varpool_[static_cast<uint32_t>(Qcode::XpdrFail)] || varpool_[static_cast<uint32_t>(Qcode::Qi204)]);

    Qs120_0_ = varpool_[static_cast<uint32_t>(Qcode::ElevatorStick)] != nullptr;
    Qs120_1_ = varpool_[static_cast<uint32_t>(Qcode::AileronControlW)] != nullptr;
    Qs120_2_ = varpool_[static_cast<uint32_t>(Qcode::RudderPedals)] != nullptr;
    Qs120_ = (Qs120_0_ || Qs120_1_ || Qs120_2_);

    Qs121_ = 
      varpool_[static_cast<uint32_t>(Qcode::PitchRad)] ||
      varpool_[static_cast<uint32_t>(Qcode::BankRad)] ||
      varpool_[static_cast<uint32_t>(Qcode::Altitude)] ||
      varpool_[static_cast<uint32_t>(Qcode::TrueHeading)] ||
      varpool_[static_cast<uint32_t>(Qcode::TAS)] ||
      varpool_[static_cast<uint32_t>(Qcode::Latitude)] ||
      varpool_[static_cast<uint32_t>(Qcode::Longitude)]
      ;
    Qs124_ = 
      varpool_[static_cast<uint32_t>(Qcode::TimeClockL)] ||
      varpool_[static_cast<uint32_t>(Qcode::TimeClockL2)] ||
      varpool_[static_cast<uint32_t>(Qcode::TimeClockL3)]
      ;
    Qs125_ = 
      varpool_[static_cast<uint32_t>(Qcode::TimeClockR)] ||
      varpool_[static_cast<uint32_t>(Qcode::TimeClockR2)] ||
      varpool_[static_cast<uint32_t>(Qcode::TimeClockR3)]
      ;
    Qs126_ = varpool_[static_cast<uint32_t>(Qcode::ElapMinsL)] != nullptr;
    Qs127_ = varpool_[static_cast<uint32_t>(Qcode::ElapMinsR)] != nullptr;
    Qs128_ = varpool_[static_cast<uint32_t>(Qcode::ChroSecsL)] != nullptr;
    Qs129_ = varpool_[static_cast<uint32_t>(Qcode::ChroSecsR)] != nullptr;
   
    Qs321_ =
      varpool_[static_cast<uint32_t>(Qcode::pitchErrAdd)] ||
      varpool_[static_cast<uint32_t>(Qcode::bankErrAdd)] ||
      varpool_[static_cast<uint32_t>(Qcode::gyroRpm)];

    Qs328_ = 
      varpool_[static_cast<uint32_t>(Qcode::WxB_hiCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_hiCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_hiCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_loCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_loCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_loCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_turbIntensity)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_turbTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_turbBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_cbCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_cbCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_cbCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_microburstMode)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_microburstRandom)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_microburstOutflow)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_inversionOn)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_inversionTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_inversionTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_arptWindVarEtc)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_arptWindGust)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_visibMtrs)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_precipLevel)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_surfaceTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::WxB_QNH)]
      ;

    Qs329_ = 
      varpool_[static_cast<uint32_t>(Qcode::Wx1_hiCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_hiCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_hiCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_loCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_loCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_loCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_turbIntensity)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_turbTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_turbBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_cbCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_cbCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_cbCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_microburstMode)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_microburstRandom)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_microburstOutflow)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_inversionOn)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_inversionTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_inversionTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_arptWindVarEtc)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_arptWindGust)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_visibMtrs)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_precipLevel)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_surfaceTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx1_QNH)]
      ;

    Qs330_ = 
      varpool_[static_cast<uint32_t>(Qcode::Wx2_hiCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_hiCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_hiCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_loCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_loCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_loCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_turbIntensity)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_turbTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_turbBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_cbCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_cbCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_cbCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_microburstMode)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_microburstRandom)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_microburstOutflow)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_inversionOn)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_inversionTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_inversionTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_arptWindVarEtc)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_arptWindGust)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_visibMtrs)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_precipLevel)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_surfaceTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx2_QNH)]
      ;

    Qs331_ = 
      varpool_[static_cast<uint32_t>(Qcode::Wx3_hiCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_hiCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_hiCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_loCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_loCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_loCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_turbIntensity)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_turbTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_turbBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_cbCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_cbCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_cbCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_microburstMode)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_microburstRandom)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_microburstOutflow)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_inversionOn)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_inversionTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_inversionTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_arptWindVarEtc)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_arptWindGust)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_visibMtrs)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_precipLevel)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_surfaceTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx3_QNH)]
      ;

    Qs332_ = 
      varpool_[static_cast<uint32_t>(Qcode::Wx4_hiCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_hiCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_hiCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_loCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_loCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_loCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_turbIntensity)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_turbTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_turbBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_cbCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_cbCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_cbCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_microburstMode)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_microburstRandom)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_microburstOutflow)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_inversionOn)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_inversionTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_inversionTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_arptWindVarEtc)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_arptWindGust)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_visibMtrs)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_precipLevel)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_surfaceTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx4_QNH)]
      ;

    Qs333_ = 
      varpool_[static_cast<uint32_t>(Qcode::Wx5_hiCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_hiCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_hiCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_loCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_loCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_loCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_turbIntensity)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_turbTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_turbBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_cbCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_cbCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_cbCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_microburstMode)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_microburstRandom)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_microburstOutflow)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_inversionOn)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_inversionTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_inversionTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_arptWindVarEtc)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_arptWindGust)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_visibMtrs)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_precipLevel)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_surfaceTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx5_QNH)]
      ;

    Qs334_ = 
      varpool_[static_cast<uint32_t>(Qcode::Wx6_hiCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_hiCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_hiCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_loCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_loCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_loCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_turbIntensity)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_turbTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_turbBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_cbCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_cbCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_cbCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_microburstMode)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_microburstRandom)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_microburstOutflow)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_inversionOn)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_inversionTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_inversionTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_arptWindVarEtc)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_arptWindGust)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_visibMtrs)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_precipLevel)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_surfaceTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx6_QNH)]
      ;

    Qs335_ = 
      varpool_[static_cast<uint32_t>(Qcode::Wx7_hiCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_hiCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_hiCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_loCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_loCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_loCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_turbIntensity)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_turbTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_turbBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_cbCloudCov)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_cbCloudTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_cbCloudBase)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_microburstMode)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_microburstRandom)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_microburstOutflow)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_inversionOn)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_inversionTop)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_inversionTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_arptWindVarEtc)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_arptWindGust)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_visibMtrs)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_precipLevel)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_surfaceTmp)] ||
      varpool_[static_cast<uint32_t>(Qcode::Wx7_QNH)]
      ;

    Qs357_0_ = varpool_[static_cast<uint32_t>(Qcode::ToeBrakeLeft)] != nullptr;
    Qs357_1_ = varpool_[static_cast<uint32_t>(Qcode::ToeBrakeRight)] != nullptr;
    Qs357_ = Qs357_0_ || Qs357_1_;

    Qs428_ = 
      varpool_[static_cast<uint32_t>(Qcode::BldAirDuctL)] ||
      varpool_[static_cast<uint32_t>(Qcode::BldAirDuctC)] ||
      varpool_[static_cast<uint32_t>(Qcode::BldAirDuctR)]
      ;
    Qs429_ = 
      varpool_[static_cast<uint32_t>(Qcode::IceWingL)] ||
      varpool_[static_cast<uint32_t>(Qcode::IceWingR)] ||
      varpool_[static_cast<uint32_t>(Qcode::IceNac1)] ||
      varpool_[static_cast<uint32_t>(Qcode::IceNac2)] ||
      varpool_[static_cast<uint32_t>(Qcode::IceNac3)] ||
      varpool_[static_cast<uint32_t>(Qcode::IceNac4)] ||
      varpool_[static_cast<uint32_t>(Qcode::Iced)]
      ;
    Qs431_ = 
      varpool_[static_cast<uint32_t>(Qcode::OutFloValveL)] ||
      varpool_[static_cast<uint32_t>(Qcode::OutFloValveR)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabinAltitude)]
      ;
    Qs432_ = 
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarFD)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActFD)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarUD)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActUD)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarCR)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActCR)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarMdCarFWD)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActMdCarFWD)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarMdCarAFT)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActMdCarAFT)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarLdCarFWD)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActLdCarFWD)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarLdCarAFT)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActLdCarAFT)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarA)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActA)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarB)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActB)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarC)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActC)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarD)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActD)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarE)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActE)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempTarLdCarB)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempActLdCarB)] ||
      varpool_[static_cast<uint32_t>(Qcode::CabTempMasterSet)]
      ;
    Qs434_ = varpool_[static_cast<uint32_t>(Qcode::ATActive)] != nullptr;
    Qs436_ = varpool_[static_cast<uint32_t>(Qcode::Qs436)] != nullptr;

    Qs438_ = 
  varpool_[static_cast<uint32_t>(Qcode::TankMain1)] ||
  varpool_[static_cast<uint32_t>(Qcode::TankMain2)] ||
  varpool_[static_cast<uint32_t>(Qcode::TankMain3)] ||
  varpool_[static_cast<uint32_t>(Qcode::TankMain4)] ||
  varpool_[static_cast<uint32_t>(Qcode::TankReserve2)] ||
  varpool_[static_cast<uint32_t>(Qcode::TankReserve3)] ||
  varpool_[static_cast<uint32_t>(Qcode::TankCenter)] ||
  varpool_[static_cast<uint32_t>(Qcode::TankStab)] ||
  varpool_[static_cast<uint32_t>(Qcode::TankAux)] ||
  varpool_[static_cast<uint32_t>(Qcode::RefgPres)] ||
  varpool_[static_cast<uint32_t>(Qcode::FuelTemp)]
    ;

    Qs448_ =
      varpool_[static_cast<uint32_t>(Qcode::EfisActLhPa)] ||
      varpool_[static_cast<uint32_t>(Qcode::EfisActLBaro)];

    Qs449_ =
      varpool_[static_cast<uint32_t>(Qcode::EfisActRhPa)] ||
      varpool_[static_cast<uint32_t>(Qcode::EfisActRBaro)];


    Qs457_ = varpool_[static_cast<uint32_t>(Qcode::CallPanelDisp)] != nullptr;
    Qs458_ = (varpool_[static_cast<uint32_t>(Qcode::RcpLa)] || varpool_[static_cast<uint32_t>(Qcode::RcpLs)]);
    Qs459_ = (varpool_[static_cast<uint32_t>(Qcode::RcpCa)] || varpool_[static_cast<uint32_t>(Qcode::RcpCs)]);
    Qs460_ = (varpool_[static_cast<uint32_t>(Qcode::RcpRa)] || varpool_[static_cast<uint32_t>(Qcode::RcpRs)]);

    QiCallFr_ = 
      varpool_[static_cast<uint32_t>(Qcode::CallFrVhfL_L)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrVhfL_C)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrVhfL_R)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrVhfC_L)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrVhfC_C)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrVhfC_R)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrVhfR_L)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrVhfR_C)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrVhfR_R)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrIntph_L)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrIntph_C)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrIntph_R)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrCabin_L)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrCabin_C)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrCabin_R)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrHfL_L)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrHfL_C)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrHfL_R)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrHfR_L)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrHfR_C)] ||
      varpool_[static_cast<uint32_t>(Qcode::CallFrHfR_R)]
    ;

    Qi175Needed_ = 
      varpool_[static_cast<uint32_t>(Qcode::Qi106)] ||
      varpool_[static_cast<uint32_t>(Qcode::Qi107)] ||
      varpool_[static_cast<uint32_t>(Qcode::Qi109)] ||
      varpool_[static_cast<uint32_t>(Qcode::Qi98)] ||
      varpool_[static_cast<uint32_t>(Qcode::Qi99)] ||
      varpool_[static_cast<uint32_t>(Qcode::Qi100)] ||
      varpool_[static_cast<uint32_t>(Qcode::Qi101)] ||
      varpool_[static_cast<uint32_t>(Qcode::Qi102)] ||
      varpool_[static_cast<uint32_t>(Qcode::Qi103)] ||
      varpool_[static_cast<uint32_t>(Qcode::Qi104)] ||
      QiCallFr_
      ;

    AutobrVirtual_ = varpool_[static_cast<uint32_t>(Qcode::AutobrVirtual)] != nullptr;
    GearLockOvrdVirtual_ = varpool_[static_cast<uint32_t>(Qcode::GearLockOvrdVirtual)] != nullptr;
    EngStartSw1Virtual_ = varpool_[static_cast<uint32_t>(Qcode::EngStartSw1Virtual)] != nullptr;
    EngStartSw2Virtual_ = varpool_[static_cast<uint32_t>(Qcode::EngStartSw2Virtual)] != nullptr;
    EngStartSw3Virtual_ = varpool_[static_cast<uint32_t>(Qcode::EngStartSw3Virtual)] != nullptr;
    EngStartSw4Virtual_ = varpool_[static_cast<uint32_t>(Qcode::EngStartSw4Virtual)] != nullptr;
    Tla1Virtual_ = varpool_[static_cast<uint32_t>(Qcode::Tla1Virtual)] != nullptr;
    Tla2Virtual_ = varpool_[static_cast<uint32_t>(Qcode::Tla2Virtual)] != nullptr;
    Tla3Virtual_ = varpool_[static_cast<uint32_t>(Qcode::Tla3Virtual)] != nullptr;
    Tla4Virtual_ = varpool_[static_cast<uint32_t>(Qcode::Tla4Virtual)] != nullptr;
    Qs436Virtual_ = Tla1Virtual_ || Tla2Virtual_ || Tla3Virtual_ || Tla4Virtual_;
    TlaMotor_ = varpool_[static_cast<uint32_t>(Qcode::TlaMotor)] != nullptr;
    TlaMotorOrQs436Virtual_ = TlaMotor_ || Qs436Virtual_;
    TlaMotorOrQs434OrQs436_ = TlaMotor_ || Qs434_ || Qs436_;
    FireEng1Virtual_ = varpool_[static_cast<uint32_t>(Qcode::FireEng1Virtual)] != nullptr;
    FireEng2Virtual_ = varpool_[static_cast<uint32_t>(Qcode::FireEng2Virtual)] != nullptr;
    FireEng3Virtual_ = varpool_[static_cast<uint32_t>(Qcode::FireEng3Virtual)] != nullptr;
    FireEng4Virtual_ = varpool_[static_cast<uint32_t>(Qcode::FireEng4Virtual)] != nullptr;
    FireApuVirtual_ = varpool_[static_cast<uint32_t>(Qcode::FireApuVirtual)] != nullptr;
    CallPanelVirtual_ = varpool_[static_cast<uint32_t>(Qcode::CallPanelVirtual)] != nullptr;
    LtIndTestVirtual_ = varpool_[static_cast<uint32_t>(Qcode::LtIndTestVirtual)] != nullptr;
    AileronsVirtual_ = varpool_[static_cast<uint32_t>(Qcode::AileronsVirtual)] != nullptr;
    ElevatorVirtual_ = varpool_[static_cast<uint32_t>(Qcode::ElevatorVirtual)] != nullptr;
    RudderVirtual_ = varpool_[static_cast<uint32_t>(Qcode::RudderVirtual)] != nullptr;
    ToeBrakeLeftVirtual_ = varpool_[static_cast<uint32_t>(Qcode::ToeBrakeLeftVirtual)] != nullptr;
    ToeBrakeRightVirtual_ = varpool_[static_cast<uint32_t>(Qcode::ToeBrakeRightVirtual)] != nullptr;
    ParkBrkLevVirtual_ = varpool_[static_cast<uint32_t>(Qcode::ParkBrkLevVirtual)] != nullptr;
    
    Qs120T_ = Qs120_ || AileronsVirtual_ || ElevatorVirtual_ || RudderVirtual_;
    Qs357T_ = Qs357_ || ToeBrakeLeftVirtual_ || ToeBrakeRightVirtual_;

    g_DemandForQi211 = varpool_[static_cast<uint32_t>(Qcode::Qi211)] != nullptr;
    g_DemandForQi214 = varpool_[static_cast<uint32_t>(Qcode::Qi214)] != nullptr;
    g_DemandForQi271 = varpool_[static_cast<uint32_t>(Qcode::Qi271)] != nullptr;
    g_DemandForQi273 = varpool_[static_cast<uint32_t>(Qcode::Qi273)] != nullptr;
    g_DemandForQi274 = varpool_[static_cast<uint32_t>(Qcode::Qi274)] != nullptr;
    g_DemandForQs480 = {
      varpool_[static_cast<uint32_t>(Qcode::AilOutbLeft)] ||
      varpool_[static_cast<uint32_t>(Qcode::AilInbLeft)] ||
      varpool_[static_cast<uint32_t>(Qcode::AilInbRight)] ||
      varpool_[static_cast<uint32_t>(Qcode::AilOutbRight)] ||
      varpool_[static_cast<uint32_t>(Qcode::ElevOutbLeft)] ||
      varpool_[static_cast<uint32_t>(Qcode::ElevInbLeft)] ||
      varpool_[static_cast<uint32_t>(Qcode::ElevInbRight)] ||
      varpool_[static_cast<uint32_t>(Qcode::ElevOutbRight)] ||
      varpool_[static_cast<uint32_t>(Qcode::RudderUp)] ||
      varpool_[static_cast<uint32_t>(Qcode::RudderLow)]
    };

    g_DemandForQs483 = { 
       varpool_[static_cast<uint32_t>(Qcode::PhysIAS)] ||
       varpool_[static_cast<uint32_t>(Qcode::PhysOAT)] ||
       varpool_[static_cast<uint32_t>(Qcode::PhysTAT)] ||
       varpool_[static_cast<uint32_t>(Qcode::CompWndDir)] ||
       varpool_[static_cast<uint32_t>(Qcode::CompWndSpd)] ||
       varpool_[static_cast<uint32_t>(Qcode::MagVar)]
    };

    g_dbp[static_cast<uint32_t>(Qcode::TlaMotor)] = 0;

    l_dominanceInterrupted = !(*g_cfgPTTSwitchesDominant && *g_cfgAutobrakesDominant && *g_cfgParkBrkLevDominant &&
                               *g_cfgGearLockOverrideDominant && *g_cfgFireEng1Dominant && *g_cfgFireEng2Dominant &&
                               *g_cfgFireEng3Dominant && *g_cfgFireEng4Dominant && *g_cfgFireApuDominant && *g_cfgEngStartersDominant);

  }

  const std::set<uint32_t> l_VirtualQhvars{
    134U, 169U, 183U, 267U, 268U, 269U, 270U, 275U, 276U, 277U, 278U, 279U, 413U
  };

  [[nodiscard]] auto lf_isVirtualQhvar(const uint32_t n) {
    return l_VirtualQhvars.find(n) != l_VirtualQhvars.cend();
  }

  [[nodiscard]] auto lf_isQiLeadingCtrlVar(const uint32_t num) {
    // Qi Cfg variables or Door command variables
    return num <= 31 || num == 38 || (num >= 179 && num <= 181);
  }

  auto FromPSXClient::decodeOneMessage(std::string const& msg) -> void {
    // ASSUMPTION: std::string msg is not empty.
    std::smatch m;
    if (msg.at(0) == gkQChar) {
      if (const std::regex r{R"(Q(h|i|s)(\d+)=(.*))"}; regex_match(msg, m, r)) {
        const auto num{static_cast<uint32_t>(str2int32(m[2U]))};
        if (msg.at(1) == gksChar) {
          if (*g_CfgMONITOR_PSX_IN_Qs && gf_isQsvarlist(num)) {
            g_log.line(kRecv + msg);
          }
          decodeQs(static_cast<Qcode>(num), m[3U]);
        } else if (msg.at(1) == gkhChar) {
          if (*g_CfgMONITOR_PSX_IN_Qh && gf_isQhvarlist(num)) {
            g_log.line(kRecv + msg);
          }
          auto store{false};
          const auto varnum{num + static_cast<uint32_t>(Qcode::Qh0)};
          if (varpool_[varnum]) {  // are we using this Qh var?
            store = true;
          } else if (lf_isVirtualQhvar(num)) {  // possible use of a virtual Qh var
            store = true;
          } else if (varpool_[varnum + 2'000U]) {  // July 16, 2021, version 3.23  
            // are we using an ih_IndicatorLightBigActionSwitch for this control-var that we have not implemented?
            store = true;
          } else {
            if (num == 424U) {  // bug fix for Balt's EVAC light. Sep 19, 2017
              store = true;
            }
          }
          if (store) {
            writeToBuffer({ static_cast<Qcode>(varnum), str2int32(m[3U]) });
          }
        } else {
          if (*g_CfgMONITOR_PSX_IN_Qi && gf_isQivarlist(num)) {
            g_log.line(kRecv + msg);
          }
          auto store{false};
          const auto val{str2int32(m[3U])};
          const auto varnum{num + static_cast<uint32_t>(Qcode::Qi0)};
          if (varpool_[varnum]) {  // are we using this Qi var?
            store = true;
          } else if (lf_isQiLeadingCtrlVar(num)) {
            if (varpool_[static_cast<size_t>(varnum) + 3'000]) {  // are we using this control var?   
              store = true;  // this way, in FlushBuffer the control var will send its value ...
            }
          } else if (num >= 98 && num <= 104) {
            if (QiCallFr_) {
              store = true;
            }
          } else {
            if (num == 175) {
              if (Qi175Needed_) {
                store = true;
              }
            }
          }
          if (num == 89) {  // CDU blank variable
            if (*g_CfgCduLScreen) {
              g_gui.setCduLBlankWrite(val > 0);
            }
          } else if (num == 90) {  // CDU blank variable
            if (*g_CfgCduCScreen) {
              g_gui.setCduCBlankWrite(val > 0);
            }
          } else {
            if (num == 91) {  // CDU blank variable
              if (*g_CfgCduRScreen) {
                g_gui.setCduRBlankWrite(val > 0);
              }
            }
          }
          if (store) {
            writeToBuffer({ static_cast<Qcode>(varnum), val });
          }
        }
      }
    } else {
      decodeNonQcode(msg);
    }
  }

  auto FromPSXClient::decodeQs(const Qcode q, std::string const& s) -> void {
    const auto var{static_cast<uint32_t>(q)};
    if (q >= Qcode::Qs4 && q <= Qcode::Qs61) {
      if (varpool_[var]) {  // are we using this Circuit Breaker var?
        g_str[var] = s;
        (*varpool_[var]).act();   // yes, let it re-send its value (representing our SIOC variable state), NOTE: tested upon use was already done
      }
    } else if (q >= Qcode::Qs62 && q <= Qcode::Qs75) {
      g_str[var] = s;
      if (*g_CfgCduLScreen) {
        g_gui.setCduTextLine(var - static_cast<uint32_t>(Qcode::Qs62), s);
      }
    } else if (q >= Qcode::Qs76 && q <= Qcode::Qs89) {
      g_str[var] = s;
      if (*g_CfgCduCScreen) {
        g_gui.setCduTextLine(var - static_cast<uint32_t>(Qcode::Qs76), s);
      }
    } else if (q >= Qcode::Qs90 && q <= Qcode::Qs103) {
      g_str[var] = s;
      if (*g_CfgCduRScreen) {
        g_gui.setCduTextLine(var - static_cast<uint32_t>(Qcode::Qs90), s);
      }
    } else if (q >= Qcode::Qs137 && q <= Qcode::Qs320) {
      constexpr auto offset{static_cast<uint32_t>(Qcode::MfAilLockout) - static_cast<uint32_t>(Qcode::Qs137)};
      if (varpool_[static_cast<size_t>(var) + static_cast<size_t>(offset)]) {  // are we using this Malfunction var?
        g_str[var] = s;
      }
    } else if (q >= Qcode::Qs500 && q <= Qcode::Qs513) {
      if (*g_CfgLCDCdu) {
        g_str[var] = s;
        if (*g_CfgCduLScreen) {
          g_gui.setCduColorLine(var - static_cast<uint32_t>(Qcode::Qs500), s);
        }
      }
    } else if (q >= Qcode::Qs514 && q <= Qcode::Qs527) {
      if (*g_CfgLCDCdu) {
        g_str[var] = s;
        if (*g_CfgCduCScreen) {
          g_gui.setCduColorLine(var - static_cast<uint32_t>(Qcode::Qs514), s);
        }
      }
    } else if (q >= Qcode::Qs528 && q <= Qcode::Qs541) {
      if (*g_CfgLCDCdu) {
        g_str[var] = s;
        if (*g_CfgCduRScreen) {
          g_gui.setCduColorLine(var - static_cast<uint32_t>(Qcode::Qs528), s);
        }
      }
    } else {
      switch (q) {
      case Qcode::Qs106:
        if (Qs106_) {
          FromPSXClient::decodeQs106_108(s, 0, Qcode::L_ActVHFL);
        }
        break;
      case Qcode::Qs107:
        if (Qs107_) {
          FromPSXClient::decodeQs106_108(s, 1, Qcode::C_ActVHFL);
        }
        break;
      case Qcode::Qs108:
        if (Qs108_) {
          FromPSXClient::decodeQs106_108(s, 2, Qcode::R_ActVHFL);
        }
        break;
      case Qcode::Qs112:
        decodeQsn(s, Qcode::FreqsAntennaVHF_L, 5U);
        break;
      case Qcode::Qs117:
        if (varpool_[var]) {  // are we using this var?
          g_str[var] = s;
          (*varpool_[var]).act();   // yes, let it resent its value (representing our hardware state)
        }
        break;
      case Qcode::Qs118:
        if (Qs118_) {
          decodeQs118(s);
        }
        break;
      case Qcode::Qs120:
        if (Qs120T_) {
          decodeQs120(s);
        }
        break;
      case Qcode::Qs121:
        if (Qs121_) {
          decodeQs121(s);
        }
        break;
      case Qcode::Qs124:
        if (Qs124_) {
          if (const auto millis{lf_fromChars<int64_t>(s)}) {
            const auto u{gf_decode_utc(static_cast<int32_t>(*millis / 1'000))};
            //            g_log.line(format("{}:{} {} {} {}", u.hour, u.min, u.month, u.day, u.year));
            writeToBuffer({ Qcode::TimeClockL, u.hour * 100 + u.min });
            writeToBuffer({ Qcode::TimeClockL2, u.month * 100 + u.day });
            writeToBuffer({ Qcode::TimeClockL3, u.year } );
          }
        }
        break;
      case Qcode::Qs125:
        if (Qs125_) {
          if (const auto millis{lf_fromChars<int64_t>(s)}) {
            const auto u{gf_decode_utc(static_cast<int32_t>(*millis / 1'000))};
            writeToBuffer({ Qcode::TimeClockR, u.hour * 100 + u.min });
            writeToBuffer({ Qcode::TimeClockR2, u.month * 100 + u.day });
            writeToBuffer({ Qcode::TimeClockR3, u.year });
          }
        }
        break;
      case Qcode::Qs126:
        if (Qs126_) {
          g_EtCpBlank = ((s == gkDigit0Str));
          if (const auto millis{lf_fromChars<int64_t>(s)}) {
            writeToBuffer({ Qcode::ElapMinsL, static_cast<int32_t>(*millis / 60'000) });
          }
        }
        break;
      case Qcode::Qs127:
        if (Qs127_) {
          g_EtFoBlank = (s == gkDigit0Str);
          if (const auto millis{lf_fromChars<int64_t>(s)}) {
            writeToBuffer({Qcode::ElapMinsR, static_cast<int32_t>(*millis / 60'000)});
          }
        }
        break;
      case Qcode::Qs128:
        if (Qs128_) {
          if (const auto millis{lf_fromChars<int32_t>(s)}) {
            writeToBuffer({Qcode::ChroSecsL, *millis / 1'000});
          }
        }
        break;
      case Qcode::Qs129:
        if (Qs129_) {
          if (const auto millis{lf_fromChars<int32_t>(s)}) {
            writeToBuffer({Qcode::ChroSecsR, *millis / 1'000});
          }
        }
        break;

      case Qcode::Qs321:
        if (Qs321_) {
          decodeQsn(s, Qcode::Dummy1, 4);
        }
        break;

      case Qcode::Qs323:
        decodeQsn(s, Qcode::nonDimC358, 16);
        break;

      case Qcode::Qs324:
        decodeQsn(s, Qcode::domeLight, 22);
        break;

      case Qcode::Qs328:
        if (Qs328_) {
          decodeQsn(s, Qcode::WxB_hiCloudCov, 24);
        }
        break;

      case Qcode::Qs329:
        if (Qs329_) {
          decodeQsn(s, Qcode::Wx1_hiCloudCov, 24);
        }
        break;

      case Qcode::Qs330:
        if (Qs330_) {
          decodeQsn(s, Qcode::Wx2_hiCloudCov, 24);
        }
        break;

      case Qcode::Qs331:
        if (Qs331_) {
          decodeQsn(s, Qcode::Wx3_hiCloudCov, 24);
        }
        break;

      case Qcode::Qs332:
        if (Qs332_) {
          decodeQsn(s, Qcode::Wx4_hiCloudCov, 24);
        }
        break;

      case Qcode::Qs333:
        if (Qs333_) {
          decodeQsn(s, Qcode::Wx5_hiCloudCov, 24);
        }
        break;

      case Qcode::Qs334:
        if (Qs334_) {
          decodeQsn(s, Qcode::Wx6_hiCloudCov, 24);
        }
        break;

      case Qcode::Qs335:
        if (Qs335_) {
          decodeQsn(s, Qcode::Wx7_hiCloudCov, 24);
        }
        break;


      case Qcode::Qs357:
        if (Qs357T_) {
          decodeQs357(s);
        }
        break;

      case Qcode::Qs431:
        if (Qs431_) {
          decodeQsn(s, Qcode::OutFloValveL, 3);
        }
        break;
      case Qcode::Qs432:
        if (Qs432_) {
          //   Cabin temperature indications
          //    Code : Qs432
          //    Name : CabTemps
          //    Mode : ECON
          //    Length : 55 .. 95
          //    Add - ons can : Read only
          //    Example :
          //    73, 73;77, 77;70, 70;62, 62;62, 62;60, 38;60, 41;75, 75;75, 75;75, 75;75, 75;75, 75;72, 41;73;
          //    Description:
          //    Values are always in Fahrenheit. Data records are separated by semicolons.
          //    The last record is a single value(master temperature setting); every other record consists of two values separated by a comma, 
          //    whereas the first value is a target temperature and the second value an actual temperature of a specific cabin zone temperature controller.
          //    The sequence of the value pairs(cabin zones) from left to right in the std::string is :
          //    1 - F / D
          //    2 - U / D
          //    3 - C / R
          //    4 - Main deck cargo FWD
          //    5 - Main deck cargo AFT
          //    6 - Lower deck cargo FWD
          //    7 - Lower deck cargo AFT
          //    8 - A
          //    9 - B
          //    10 - C
          //    11 - D
          //    12 - E
          //    13 - Lower deck cargo bulk
          decodeQsn(s, Qcode::CabTempTarFD, 27);
        }
        break;

      case Qcode::Qs428:
        if (Qs428_) {
          decodeQsn(s, Qcode::BldAirDuctL, 3);
        }
        break;

      case Qcode::Qs429:
        if (Qs429_) {
          decodeQsn(s, Qcode::IceWingL, 7);
        }
        break;

      case Qcode::Qs434:
        if (TlaMotorOrQs434OrQs436_) {
          decodeQs434(s);
        }
        break;
      case Qcode::Qs436:
        if (Qs436_) {
          if (0 == ATActive_) { // AT not active
            (*varpool_[var]).act();   // send variables from hardware throttle
          }
        }
        if (1 == ATActive_ && TlaMotorOrQs436Virtual_) {
          decodeQs436(s);
        }
        break;

      case Qcode::Qs438:
        if (Qs438_) {
          auto s1{s};
          s1.at(0) = gkSpaceChar;
          decodeQsn(s1, Qcode::TankMain1, 11);
        }
        break;

      case Qcode::Qs448:
        if (Qs448_) {
          decodeQsEfisActL(s);
        }
        break;

      case Qcode::Qs449:
        if (Qs449_) {
          decodeQsEfisActR(s);
        }
        break;

      case Qcode::Qs457:
        if (Qs457_) {
          decodeQs457(s);
        }
        break;
      case Qcode::Qs458:
        if (Qs458_) {
          decodeQs458_460(s, Qcode::RcpLa, Qcode::RcpLs);
        }
        break;
      case Qcode::Qs459:
        if (Qs459_) {
          decodeQs458_460(s, Qcode::RcpCa, Qcode::RcpCs);
        }
        break;
      case Qcode::Qs460:
        if (Qs460_) {
          decodeQs458_460(s, Qcode::RcpRa, Qcode::RcpRs);
        }
        break;
      case Qcode::Qs480:
        decodeQs480(s);
        break;
      case Qcode::Qs483:
        decodeQsn(s, Qcode::PhysIAS, 6);
        break;
      case Qcode::Qs104:
      case Qcode::Qs105:
      case Qcode::Qs109:
      case Qcode::Qs110:
      case Qcode::Qs111:
      case Qcode::Qs113:
      case Qcode::Qs114:
      case Qcode::Qs115:
        if (varpool_[var]) {  // are we using this var?
          g_str[var] = s;
          (*varpool_[var]).act();   // yes, let it resent its value (representing our hardware state)
        }
        break;
      default:
        // do nothing
        break;
      }
    }
  }

  auto FromPSXClient::decodeNonQcode(std::string const& line) -> void {
    if (*g_CfgMONITOR_PSX_IN_NONQ) {
      g_log.line(kRecv + line);
    }
    if (line == kexitStr) {
      throw ExitPSXReceivedException();
    }
    std::smatch m;
    if (line.front() == gkvChar) {
      if (const std::regex rv{R"(version=(.+))"}; regex_match(line, m, rv)) {
        g_gui.setPSXVersion(m[1]);
      }
    }
  }

  auto FromPSXClient::decodeQsnSv(const std::vector<std::string>& sv, const Qcode q) -> void {
    const auto var{ static_cast<uint32_t>(q) };
    uint32_t index{ 0 };
    g_log.newLine();   
    try {
      for (auto const& s : sv) {
       writeToBuffer({static_cast<Qcode>(var + index), str2int32(s)});
        ++index;
      }
    } catch (...) {
      gf_logUnexpectedAndUnhandledException(__func__);
    }
  }

  auto FromPSXClient::decodeQsn(std::string const& s, const Qcode q, const uint32_t n) -> void {
    if (const auto sv{gt_string2Container<std::vector<std::string>>(s, gkSemiColonChar)}; n == sv.size()) {
      decodeQsnSv(sv, q);
    } else {
      if (*g_CfgMONITOR_PSX_IN_Qs) {
        g_log.line("error: "s + std::string(__func__) + std::format(" var: {}, s: {}, n:{}, sv size:{}", static_cast<uint32_t>(q), s, n, sv.size()));
      }
    }
  }

 /* append(isDhArmed ? 'R' : 'r')
    .append(dh).append(';').append(mda).append(';')
    .append(minsMode).append(';')
    .append(hPa).append(';').append(baroMode).append(';')
    .append(adfL).append(';').append(adfR).append(';')
    .append(ndMode).append(ndRange)
    .append(fpv).append(mtrs).append(ctr).append(tfc)
    .append(mWxr).append(mSta).append(mWpt).append(mArpt)
    .append(mData).append(mPos).append(mTerr)
    .append(cduOn ? 'C' : 'c');

  Qs448 = R-1;3530;1;101693;-1;1;1;2310110000001c
 
    The last section "2310110000001c" is a sequence of single characters where each character indicates a specific mode.For most of them it's just "on" or "off" (1 or 0).
    hPa vierde element
    baroMode vijfde element
*/

  auto FromPSXClient::decodeQsEfisActL(std::string const& s) -> void {
    const auto sv = gt_string2Container<std::vector<std::string>>(s, gkSemiColonChar);
    if (sv.size() >= 5) {
      writeToBuffer({ Qcode::EfisActLhPa, str2int32(sv[3]) });
      writeToBuffer({ Qcode::EfisActLBaro, str2int32(sv[4]) });
    }
  }

  auto FromPSXClient::decodeQsEfisActR(std::string const& s) -> void {
    const auto sv = gt_string2Container<std::vector<std::string>>(s, gkSemiColonChar);
    if (sv.size() >= 5) {
      writeToBuffer({ Qcode::EfisActRhPa, str2int32(sv.at(3)) });
      writeToBuffer({ Qcode::EfisActRBaro, str2int32(sv.at(4)) });
    }
  }


  // Qs106 = "MemRcpL"; Mode = ECON; Min = 71; Max = 77;
  // Qs107 = "MemRcpC"; Mode = ECON; Min = 71; Max = 77;
  // Qs108 = "MemRcpR"; Mode = ECON; Min = 71; Max = 77;

  //A MemRcp... std::string stores the following variables of the respective RCP memory, separated by semicolons(there must be a semicolon at the end):

  //  actFreqVhfL
  //  stbyFreqVhfL
  //  actFreqVhfC
  //  stbyFreqVhfC
  //  actFreqVhfR
  //  stbyFreqVhfR
  //  actFreqHfL
  //  stbyFreqHfL
  //  actFreqHfR
  //  stbyFreqHfR
  //  hfSenL
  //  hfSenR
  //  amL
  //  amR

  //  If amL is not zero, then HF L is in AM mode.
  //  If amR is not zero, then HF R is in AM mode.

  //  The AM light contact is closed if ...
  //  ... the HF L light is contact closed and amL is not zero
  //  ... or ...
  //  ... the HF R light is contact closed and amR is not zero

  auto FromPSXClient::decodeQs106_108(std::string const& s, const uint32_t index, const Qcode start) -> void {
    const auto sv{gt_string2Container<std::vector<std::string>>(s, gkSemiColonChar)};
    g_amL.at(index) = sv.at(12) != gkDigit0Str;
    g_amR.at(index) = sv.at(13) != gkDigit0Str;
    uint32_t i{0};
    for (const auto& str : sv) {
      writeToBuffer({ static_cast<Qcode>((static_cast<uint32_t>(start) + i)), str2int32(str) });
      ++i;
    }
  }

  auto FromPSXClient::decodeQs118(std::string const& s) -> void {
    //  Code : Qs118
    //  Name : TcasPanStat
    //  Mode : ECON
    //  Length : 11
    //  Add - ons can : Read only
    //  Example :
    //  77007700NNN
    //  Description :

    // There are no semicolons.The digits at position 1 through 4 are the active squawk(but ATC add - ons should read Qi204 instead of Qs118).
    // The digits at position 5 through 8 are the ones that are displayed on the LCD; those should be used by hardware panel add - ons.
    // The displayed value may be cleared and is not necessarily equal to the the active squawk.

    //  Another example :
    //  77005___NNF
    //  In this example, only the first digit of the tansponder code should be displayed on the LCD, the "5" in this case.
    //  The other three digits are blank on the LCD(an underscore indicates a blank digit).

    //  The last three characters refer to certain system modes :

    // Character at position 9 :
    //  L : MD & T test on(all LCD segments on and XPDR FAIL light on)
    //  N : MD & T test off
    //  E : Transponder fail

    //  Character at position 10 :
    //  B : Blank display
    //  P : PASS displayed
    //  N : Normal display

    //  Character at position 11 (this is not relevant to add - ons) :
    //  F : F / L mode on
    //  N : F / L mode off

    // May 2023, Frederic:
    // Als ik kijk wat er dan gebeurt(via PSXrouter), zie ik dat de Qs118 verandert naar 56555655NEN(ipv 56555655NNN als de linker
    // unit is geselecteerd).Ik heb dit ook aan Hardy gevraagd of dit juist is, en hij bevestigde mij dat ‘E’ inderdaad de
    // FAIL light aangeeft. Hij zal de network documentatie aanpassen.

    g_TcasMDT = s.at(8) == gkLChar;
    g_TcasBlank = s.at(9) == gkBChar;
    g_TcasPass = s.at(9) == gkPChar;
    g_XpdrFail = s.at(9) == gkEChar;

    auto sq{-1L};
    if (gkUnderscoreChar == s.at(4)) {
      g_TcasNumDigits = 0;
    } else {
      if (gkUnderscoreChar == s.at(5)) {
        g_TcasNumDigits = 1;
      } else if (gkUnderscoreChar == s.at(6)) {
        g_TcasNumDigits = 2;
      } else if (gkUnderscoreChar == s.at(7)) {
        g_TcasNumDigits = 3;
      } else {
        g_TcasNumDigits = 4;
      }
      sq = str2int32(s.substr(4, g_TcasNumDigits));
    }
   writeToBuffer({Qcode::SquawkDispl, sq});
  }

  auto FromPSXClient::decodeQs120(std::string const& s) -> void {
    // For the not available items, save the value, so in the act() it can be re-sent (instead of 0)
    // This is needed if not available items are controlled via USB devices instead of via TCP network.
    const auto sv{gt_string2Container<std::vector<std::string>>(s, gkSemiColonChar)};
    const auto ele{str2int32(sv.at(0))};
    const auto ail{str2int32(sv.at(1))};
    const auto rud{str2int32(sv.at(2))};
    if (ElevatorVirtual_) {
      (*varpool_[static_cast<uint32_t>(Qcode::ElevatorVirtual)]).setValue(ele);
    }
    if (AileronsVirtual_) {
      (*varpool_[static_cast<uint32_t>(Qcode::AileronsVirtual)]).setValue(ail);
    }
    if (RudderVirtual_) {
      (*varpool_[static_cast<uint32_t>(Qcode::RudderVirtual)]).setValue(rud);
    }
    if (Qs120_) {
      if (!Qs120_0_) {
        g_ElevatorStick = ele;
      }
      if (!Qs120_1_) {
        g_AileronControlW = ail;
      }
      if (!Qs120_2_) {
        g_RudderPedals = rud;
      }
      (*varpool_[static_cast<uint32_t>(Qcode::Qs120)]).act();
    }
  }

  auto FromPSXClient::decodeQs121(std::string const& s) -> void {
    //Variable name: PiBaHeAlTas
    //Category: Qs
    //Mode: CONT
    //Min std::string length: 9
    //Max std::string length: 200
    //Read and write
    //
    // This variable is continuously updated at 5 Hz. The name PiBaHeAlTas helps memorizing the sequence 
    // of the variables that this std::string provides: it is pitch, bank, heading, altitude, TAS. 
    // There must be no semicolon at the end. The values are formatted as follows:
    //
    //1. Target pitch in radians * 100000 (32 bit integer)
    //2. Target bank in radians * 100000 (32 bit integer)
    //3. Target true heading in radians (64 bit float)
    //4. Target true altitude of aircraft center above MSL in feet * 1000 (32 bit integer)
    //5. Target TAS * 1000 (32 bit integer)
    //6. Target latitude of aircraft center in radians (64 bit float)
    //7. Target longitude of aircraft center in radians (64 bit float)
    //
    // All values are target values. When a value is injected that it not equal to the aircraft's 
    // current value, the aircraft's current value will be smoothly changed to the new target value.
    auto sv{gt_string2Container<std::vector<std::string>>(s, gkSemiColonChar)};
   writeToBuffer({Qcode::PitchRad, str2int32(sv.at(0))});
   writeToBuffer({Qcode::BankRad, str2int32(sv.at(1))});
    if (const auto trueHeading{lf_fromChars<float>(sv.at(2))}) {
      writeToBuffer({Qcode::TrueHeading, std::bit_cast<int32_t>(*trueHeading)});
    }
   writeToBuffer({Qcode::Altitude, str2int32(sv.at(3))});
   writeToBuffer({Qcode::TAS, str2int32(sv.at(4))});
    if (const auto latitude{lf_fromChars<float>(sv.at(5))}) {
      writeToBuffer({Qcode::Latitude, std::bit_cast<int32_t>(*latitude)});
    }
    if (const auto longitude{lf_fromChars<float>(sv.at(6))}) {
      writeToBuffer({Qcode::Longitude, std::bit_cast<int32_t>(*longitude)});
    }
  }

  auto FromPSXClient::decodeQs357(std::string const& s) -> void {
    // For the not available items, save the value, so in the act() it can be re-sent (instead of 0)
    // This is needed if not available items are controlled via USB devices instead of via TCP network.
    const auto sv{gt_string2Container<std::vector<std::string>>(s, gkSemiColonChar)};
    const auto rl{str2int32(sv.at(0))};
    const auto rr{str2int32(sv.at(1))};
    if (ToeBrakeLeftVirtual_) {
      (*varpool_[static_cast<uint32_t>(Qcode::ToeBrakeLeftVirtual)]).setValue(rl);
    }
    if (ToeBrakeRightVirtual_) {
      (*varpool_[static_cast<uint32_t>(Qcode::ToeBrakeRightVirtual)]).setValue(rr);
    }
    if (Qs357_) {
      if (!Qs357_0_) {
        g_ToeBrakeLeft = rl;
      }
      if (!Qs357_1_) {
        g_ToeBrakeRight = rr;
      }
      (*varpool_[static_cast<uint32_t>(Qcode::Qs357)]).act();
    }
  }

  auto FromPSXClient::decodeQs434(std::string const& s) -> void {
    if (const auto found{s.find(gkSemiColonChar)}; found != std::string::npos) {
      const auto ATMode{str2int32(s.substr(0, found))};
      ATActive_ = ((ATMode == 0) || (ATMode == 21L)) ? 0 : 1;
      const auto curval{g_dbp.at(static_cast<uint32_t>(Qcode::ATActive))};
     writeToBuffer({Qcode::ATActive, ATActive_});
      if (Qs436_ && 1 == curval && 0 == ATActive_) {  // AT Disc
        (*varpool_[static_cast<uint32_t>(Qcode::Qs436)]).act();   // send variables from hardware throttle to align the virtual ones.
      }
    } else {
      g_log.line("! Syserror: "s + std::string(__func__));
    }
  }

  auto FromPSXClient::decodeQs436(std::string const& s) -> void {
    //  Thrust levers
    //  Code : Qs436
    //  Name : Tla
    //  Mode : ECON
    //  Length : 7 .. 23
    //  Add - ons can : Read & write
    //  Example :
    //  5000;5000;4999;5000
    //  Description:

    //  This variable contains data for all four forward thrust lever angles and all four reverser lever angles.
    //  If an engine's value is zero, its forward lever is at idle and its reverser lever is stowed. 
    //  If the value is negative, the forward lever is at idle and the reverser lever is not stowed. 
    //  If the value is positive, the forward lever is out of idle and the reverser lever is stowed. 
    //  The values represent hundredths of a degree; that is, 5000 means 50 degree TLA (thrust lever angle). 
    //  Each value is an integer ranging from 5000 to -8925. The four values are separated by semicolons. 
    //  There must be no semicolon at the end of the std::string.

    //  If you run autothrottle servo hardware, you may do this : When the autothrottle mode annunciation 
    //  is not blank and not HOLD, feed your servo with the TLA values sent by Precision Simulator, and
    //  re - inject your current hardware TLA values into Precision Simulator every 1 second.
    //  Every re - injection must contain the latest TLA(not a value from 1 second ago).
    //  These re - injections allow pilot interventions on the levers.The 1 - second delay is necessary
    //  to allow the autothrottle to move freely between each re - injection.
    //  When the autothrottle mode annunciation is blank or HOLD, just inject your current 
    //  hardware TLA values into Precision Simulator whenever they have changed, 
    //  and do not read the TLA values from Precision Simulator.
    //  The current autothrottle mode can be read from the variable Afds(Qs434).
    const auto sv{gt_string2Container<std::vector<std::string>>(s, gkSemiColonChar)};
    const auto tla1{str2int32(sv.at(0))};
    const auto tla2{str2int32(sv.at(1))};
    const auto tla3{str2int32(sv.at(2))};
    const auto tla4{str2int32(sv.at(3))};
    if (Tla1Virtual_) {
      (*varpool_[static_cast<uint32_t>(Qcode::Tla1Virtual)]).setValue(tla1);
    }
    if (Tla2Virtual_) {
      (*varpool_[static_cast<uint32_t>(Qcode::Tla2Virtual)]).setValue(tla2);
    }
    if (Tla3Virtual_) {
      (*varpool_[static_cast<uint32_t>(Qcode::Tla3Virtual)]).setValue(tla3);
    }
    if (Tla4Virtual_) {
      (*varpool_[static_cast<uint32_t>(Qcode::Tla4Virtual)]).setValue(tla4);
    }
    if (TlaMotor_) {
      static auto allth_avg_previous{0};
      const auto allth_avg{(max(0, tla1) + max(0, tla2) + max(0, tla3) + max(0, tla4)) / 4};  // 0 .. 5000, negative values mean lever is at idle.
      constexpr auto delta{25}; // try to get rid of noise
      auto motor{0};
      if ((allth_avg - allth_avg_previous) > delta) {
        motor = 1;
      } else  if ((allth_avg - allth_avg_previous) < -delta) {
        motor = -1;
      }
      if (constexpr auto var{static_cast<uint32_t>(Qcode::TlaMotor)}; motor != g_dbp.at(var)) {
        static auto sync{ false };
        if (sync) {
          g_dbp.at(var) = motor;
          (*varpool_[var]).setValue(motor);
        } else {
          sync = true;
        }
      }
      allth_avg_previous = allth_avg;
    }
  }


  //  Call panel display on passenger and combi aircraft
  //   Code : Qs457
  //   Name : CallPanelDisp
  //   Mode : XECON
  //   Length : 1 .. 16
  //   Add - ons can : Read only
  //  Example :
  //     U / D CREW REST P3
  //   Description :

  //  When the display is on, the string is always 16 characters long(including blank spaces).
  //		When the display is off, the string consists of just one underscore character.

  auto FromPSXClient::decodeQs457(std::string const& s) -> void {
    // from Hardy:
    static std::array<std::string, 78 > cabinStation{
     "",                  // 00
     "DIRECTORY"s,        // 01
     "VIDEO IN USE"s,     // 02
     "PA IN USE"s,        // 03
     "CABIN READY"s,      // 04
     "PILOT ALERT"s,      // 05
     "PRIORITY"s,         // 06
     "PARTY LINE"s,       // 07
     ""s,                 // 08
     ""s,                 // 09
     ""s,                 // 10
     "DOOR 1 LEFT"s,      // 11
     "DOOR 2 LEFT"s,      // 12
     "DOOR 3 LEFT"s,      // 13
     "DOOR 4 LEFT"s,      // 14
     "DOOR 5 LEFT"s,      // 15
     "U/D LEFT DOOR"s,    // 16
     ""s,                 // 17
     ""s,                 // 18
     ""s,                 // 19
     ""s,                 // 20
     "DOOR 1 RIGHT"s,     // 21
     "DOOR 2 RIGHT"s,     // 22
     "DOOR 3 RIGHT"s,     // 23
     "DOOR 4 RIGHT"s,     // 24
     "DOOR 5 RIGHT"s,     // 25
     "U/D RGHT DOOR"s,    // 26 LH only
     ""s,                 // 27
     ""s,                 // 28
     ""s,                 // 29
     ""s,                 // 30
     "PILOT"s,            // 31 LH only
     "PRIORITY STA"s,     // 32 LH only
     "PRIORTY PILOT"s,    // 33
     "PURSER"s,           // 34 QF, LH
     ""s,                 // 35
     "U/D STAIR"s,        // 36 UAL only
     ""s,                 // 37
     ""s,                 // 38
     ""s,                 // 39
     ""s,                 // 40
     "PA FIRST CL"s,      // 41
     "PA BUSINESS"s,      // 42
     "PA ECONOMY CL"s,    // 43
     "PA CREW REST"s,     // 44
     ""s,                 // 45
     "PA ALL AREAS"s,     // 46
     "PA PRIORITY"s,      // 47
     ""s,                 // 48
     ""s,                 // 49
     ""s,                 // 50
     "DOOR 2 GALLEY"s,    // 51 QF, UAL
     "M/D GALLEY"s,       // 52
     "DOOR 4 GALLEY"s,    // 53 QF, UAL
     ""s,                 // 54 attendants only
     "ALL CALL"s,         // 55 QF, UAL
     "U/D GALLEY"s,       // 56
     ""s,                 // 57
     ""s,                 // 58
     ""s,                 // 59
     ""s,                 // 60
     "DR5 CREW REST"s,    // 61 Pax only
     "MCR CREW REST"s,    // 62 Combi only
     ""s,                 // 63
     ""s,                 // 64
     ""s,                 // 65
     "CR REST PILOT"s,    // 66 UAL only
     "CABIN READY"s,      // 67 LH only
     ""s,                 // 68
     ""s,                 // 69
     ""s,                 // 70
     "GND CREW CALL"s,    // 71
     "CARGO CALL"s,       // 72 Combi only
     "U/D CREW REST"s,    // 73 LH only
     "M/D CREW REST"s,    // 74 LH only
     ""s,                 // 75
     ""s,                 // 76
     "PILOT ALERT"s,      // 77 LH only
    };
    int32_t index{-1};
    if (s != "_"s) {
      if (auto it{std::ranges::find_if(std::as_const(cabinStation), [&index, &s](auto const& m) noexcept -> auto {
        ++index;
        return !m.empty() && s.find(m) != std::string::npos; })}; it == cend(cabinStation)) {
        index = -1;
      }
    }
    writeToBuffer(QcodeValueData{Qcode::CallPanelDisp, index});
  }


  //  PSX models seven aircraft that can be seen in the windshield and on the TCAS display.
  //  Aircraft that are more than 40 nm away from the windshield are invisible.
  //  Add - ons can read and write the data of the seven aircraft.
  //  When an injecting add - on is in use,  the option "Externally controlled" must be selected 
  //  on Instructor / Situation / Human / Traffic.

  //  There are two kinds of data : Position data and velocity data.

  //  Velocity data should be injected only when an aircraft changes its V / S, track or groundspeed, 
  //  but at an injection rate not higher than ca. 5 Hz.
  //  When injecting accurate and constant velocity data, 
  //  position data can be injected at rates below 1 Hz. If velocity data are not injected, 
  //  PSX will generate velocity data on its own by vectorizing the last two position injections.

  //  Pitch and bank are computed by PSX.


  auto FromPSXClient::decodeQs458_460(std::string const& s, Qcode rcpa, Qcode rcps) -> void {
    //    RCP LCD
    //    Code : For Qs indices, refer to Variables.txt.
    //    Name : RcpDispL, RcpDispC, RcpDispR.
    //    Mode : XECON
    //    Length : 15 .. 15
    //    Add - ons can : Read only
    //    Example :
    //    18.700 SEN 85;
    //    Description:
    //    An RcpDisp std::string is always 15 characters long.The last one is a semicolon.
    //    The first 7 characters represent the active display, the next 7 characters are for the standby display.
   writeToBuffer({rcpa, FromPSXClient::decodeRcpDisp(s.substr(0, 7))});
   writeToBuffer({rcps, FromPSXClient::decodeRcpDisp(s.substr(7, 7))});
  }

  auto FromPSXClient::decodeQs480(std::string const& s) -> void {
    // True aileron, elevator, and rudder surface positions

    //  Code : Qs480
    //  Name : SurfAilEleRud
    //  Mode : DEMAND
    //  Length : 20
    //  Add - ons can : Read only
    //  Example :
    //  15221815151717153232
    //  Description :

    //  The output service for this variable can be activated by sending the message "demand=Qs480" to the server.One demand message is sufficient; 
    //  the service will remain active until the demander disconnects from the network.If an add - on does not require this service,
    //  the add - on should not demand it(relieve the network).The server will send data whenever any aileron, elevator,
    //  or rudder surface position has changed, but at an update rate not higher than 10 Hz.The std::string incorporates 10 integer values, 
    //  indicating true (EICAS independent) surface position angles in degrees.A value is never negative, and it always consists of a
    //  2 - digit pair, hence it may contain a leading zero.The pairs from left to right are :

    //  1 - Aileron outboard left(neutral at 15)
    //  2 - Aileron inboard left(neutral at 20)
    //  3 - Aileron inboard right(neutral at 20)
    //  4 - Aileron outboard right(neutral at 15)
    //  5 - Elevator outboard left(neutral at 15)
    //  6 - Elevator inboard left(neutral at 17)
    //  7 - Elevator inboard right(neutral at 17)
    //  8 - Elevator outboard right(neutral at 15)
    //  9 - Rudder upper(neutral at 32)
    //  10 - Rudder lower(neutral at 32)

    if (s.length() == 20) {
      if (s.front() != '#') {
        size_t cnt{0};
        std::string s1{gkEmptyStr};
        std::vector<std::string> sv;
        for (auto const& c : s) {
          s1.push_back(c);
          ++cnt;
          if (cnt % 2 == 0) {
            sv.push_back(s1);
            s1.clear();
          }
        }
        if (sv.size() == 10) {
          decodeQsnSv(sv, Qcode::AilOutbLeft);
        } else {
          g_log.line("! Syserror: sv not 10 long "s + std::string(__func__));
        }
      }
    } else {
      g_log.line("QS480 not 20 characters?:"s + s);
    }
  }

//  Code: Qs483
//    Name : MiscFltData
//    Mode : DEMAND
//    Length : 12 .. 300
//    Add - ons can : Read only
//    Example :
//    315; -71; -40; 309; 10; -14;
//  Description:
//
//  The output service for this variable can be activated by sending the message "demand=Qs483" to the server.
//   One demand message is sufficient; the service will remain active until the demander disconnects from the network.
//   If an add - on does not require this service, the add - on should not demand it(relieve the network).The server will send data whenever any related value has changed, but at an update rate not higher than 0.3 Hz 1 Hz.The string includes the following integers from left to right, all separated by semicolons :
//
//    1 - Physical IAS x 10 (avionic independent)
//    2 - Physical OAT x 10 (avionic independent)
//    3 - Physical TAT x 10 (avionic independent)
//    4 - Computed wind direction(captain's selected ADC/IRU)
//    5 - Computed wind speed(captain's selected ADC/IRU)
//    6 - Magnetic variation x 10


  auto FromPSXClient::decodeRcpDisp(std::string const& s) -> int32_t {
    auto f{0};
    if (s.at(3) == gkDotChar) {  // VHF
      f = (str2int32(s.substr(0, 3)) * 1'000) + str2int32(s.substr(4, 3));
    } else {
      if (s.at(2) == gkDotChar) {  // HF
        f = (str2int32(s.substr(0, 2)) * 1'000) + str2int32(s.substr(3, 3));
      }
    }
    return f;
  }


  auto FromPSXClient::handleVirtualVar(QcodeValueData const& e) -> void {
    //		const auto var{ static_cast<uint32_t>(e.qcode) };
    switch (e.qcode) {
    case Qcode::Qh134:
      if (AutobrVirtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::AutobrVirtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh169:
      if (GearLockOvrdVirtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::GearLockOvrdVirtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh183:
      if (LtIndTestVirtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::LtIndTestVirtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh267:
      if (EngStartSw1Virtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::EngStartSw1Virtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh268:
      if (EngStartSw2Virtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::EngStartSw2Virtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh269:
      if (EngStartSw3Virtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::EngStartSw3Virtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh270:
      if (EngStartSw4Virtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::EngStartSw4Virtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh275:
      if (FireEng1Virtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::FireEng1Virtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh276:
      if (FireEng2Virtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::FireEng2Virtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh277:
      if (FireEng3Virtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::FireEng3Virtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh278:
      if (FireEng4Virtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::FireEng4Virtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh279:
      if (FireApuVirtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::FireApuVirtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh397:
      if (ParkBrkLevVirtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::ParkBrkLevVirtual)]).setValue(e.value);
      }
      break;
    case Qcode::Qh413:
      if (CallPanelVirtual_) {
        (*varpool_[static_cast<uint32_t>(Qcode::CallPanelVirtual)]).setValue(e.value);
      }
      break;
    default:
      break;
    }
  }

  auto lf_dominanceWanted(Qcode const& qcode) {
    return !(
      (qcode >= Qcode::Qh401 && qcode <= Qcode::Qh403)
      ||
      (l_dominanceInterrupted &&
       ((!*g_cfgPTTSwitchesDominant && (qcode == Qcode::Qh82 || qcode == Qcode::Qh93)) ||
        (!*g_cfgAutobrakesDominant && qcode == Qcode::Qh134) ||
        (!*g_cfgParkBrkLevDominant && qcode == Qcode::Qh397) ||
        (!*g_cfgGearLockOverrideDominant && qcode == Qcode::Qh169) ||
        (!*g_cfgFireEng1Dominant && qcode == Qcode::Qh275) ||
        (!*g_cfgFireEng2Dominant && qcode == Qcode::Qh276) ||
        (!*g_cfgFireEng3Dominant && qcode == Qcode::Qh277) ||
        (!*g_cfgFireEng4Dominant && qcode == Qcode::Qh278) ||
        (!*g_cfgFireApuDominant && qcode == Qcode::Qh279) ||
        (!*g_cfgEngStartersDominant && qcode >= Qcode::Qh267 && qcode <= Qcode::Qh270))
       )
      );
  }

  auto FromPSXClient::flushElement(QcodeValueData const& e) -> void {
    const auto var{static_cast<uint32_t>(e.qcode)};
    g_dbp.at(var) = e.value;  // always update database, you never know.
    const auto ptr{varpool_[var]};
    const auto isQh{e.qcode >= Qcode::Qh0 && e.qcode < Qcode::Qi0};
    if (isQh) {
      handleVirtualVar(e);
    }
    if (ptr) {  // are we using this var?
      if (isQh) {
        if (lf_dominanceWanted(e.qcode)) {
          (*ptr).act();  // re-send, momentary action switches too, except for the keyboard vars above
        }
      } else {  // Qi or Qs exp
        if (!(*ptr).needsCheck()) {
          (*ptr).setValue(e.value);
        } // else: the object will check against the value in g_dbp[e.qcode];
      }
    }
    if (lf_isQiLeadingCtrlVar(var - static_cast<uint32_t>(Qcode::Qi0))) {
      if (const auto ptr2{varpool_[var + 3'000U]}; ptr2) {
        (*ptr2).act();  // send SIOC value   (use has been tested earlier)
      }
    }
  }
}
