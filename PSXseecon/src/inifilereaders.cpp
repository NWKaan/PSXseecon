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

  namespace {
    constexpr auto kSiociniStr{ "sioc.ini"s };
    constexpr auto kSsiExt{ ".ssi"s };
    constexpr auto kLstExt{ ".lst"s };
    const std::filesystem::path kQuestionMarkDrive{ "?:/"s };
    const std::filesystem::path kSIOCPath{ "SIOC"s };
    const std::filesystem::path kIOCardsPath{ "IOCards"s };
    std::set<char> l_valid_driveletters;
    std::filesystem::path l_pathToSIOCFolder;

    [[nodiscard]] auto lf_findSIOCFolderRecursive(std::filesystem::path const& directory, std::filesystem::path& result) {
      auto r{ false };
      for (auto it{ std::filesystem::recursive_directory_iterator(directory) }; it != std::filesystem::recursive_directory_iterator(); ++it) {
        // file object contains absolute path in the case of recursive iterators
        auto const& file{ it->path() };
        if (const auto matches_mask = [](std::filesystem::path const& file) -> bool {return regex_search(file.filename().string(), std::regex("^SIOC$"s)); }; is_directory(file) && matches_mask(file)) {
          result = file;
          r = true;
          break;
        }
      }
      return r;
    }

    [[nodiscard]] auto lf_tryDrives(std::filesystem::path const& pathtosiocfolder) {
      auto b{ false };
      for (auto const& c : l_valid_driveletters) {
        auto p{ pathtosiocfolder.string() };
        p[0] = c;  // replace question mark by a valid drive letter !
        if (TxtFileReader r; r.openFile(std::filesystem::path(p) / std::filesystem::path(kSiociniStr))) {
          l_pathToSIOCFolder = std::filesystem::path(p);
          b = true;
          break;
        }
      }
      return b;
    }

    [[nodiscard]] auto lf_getSIOCFolderQuick() {
      // check present and valid drives first, skip CD ROM, RAMDISK, REMOVABLE, REMOTE ...
      // loop over possible drives:
      const auto driveletters{ "CDEFGHIJKLMNOPQRSTUVWXYZ"s };
      for (const auto c : driveletters) {
        std::string d{ R"(?:\)" };
        d.erase(0, 1);
        d.insert(0, 1, c);
        if (GetDriveType(utf8_to_wide(d).c_str()) == static_cast<UINT>(DRIVE_FIXED)) {
          l_valid_driveletters.insert(c);
        }
      }
      std::vector<std::filesystem::path> paths;
      paths.emplace_back(kQuestionMarkDrive / kIOCardsPath / kSIOCPath);
      paths.emplace_back(kQuestionMarkDrive / kSIOCPath);
      if (const auto pf(gf_myProgramFilesPath()); !pf.empty()) {
        paths.emplace_back(pf / kIOCardsPath / kSIOCPath);
        paths.emplace_back(pf / kSIOCPath);
      }
      if (const auto pfx86(gf_myProgramFilesX86Path()); !pfx86.empty()) {
        paths.emplace_back(pfx86 / kIOCardsPath / kSIOCPath);
        paths.emplace_back(pfx86 / kSIOCPath);
      }
#pragma warning(push)
#pragma warning(push)
#pragma warning(disable:4834)
#pragma warning(disable:6031)
      std::ranges::find_if(std::as_const(paths), [](auto const& p) -> auto { return lf_tryDrives(p); });
#pragma warning(pop)
#pragma warning(pop)
      paths.clear();
      l_valid_driveletters.clear();
      auto t{ l_pathToSIOCFolder };
      l_pathToSIOCFolder.clear();
      return t;
    }

    [[nodiscard]] auto lf_getSIOCFolderSlow() {
      std::filesystem::path res;
      std::list<std::string> drives;
      const auto driveletters{ "CDEFGHIJKLMNOPQRSTUVWXYZ"s };
      for (const auto c : driveletters) {
        std::string d{ R"(?:\)" };
        d.erase(0, 1);
        d.insert(0, 1, c);
        if (GetDriveType(utf8_to_wide(d).c_str()) == static_cast<UINT>(DRIVE_FIXED)) {
          std::string ds{ R"(?:\)" };
          ds.erase(0, 1);
          ds.insert(0, 1, c);
          drives.push_back(ds);
        }
      }
#pragma warning(push)
#pragma warning(push)
#pragma warning(disable:4834)
#pragma warning(disable:6031)
      std::ranges::find_if(std::as_const(drives), [&res](auto const& d) -> auto {
        return lf_findSIOCFolderRecursive(std::filesystem::path(d), res) && exists(res / std::filesystem::path(kSiociniStr)); });
#pragma warning(pop)
#pragma warning(pop)
      return res;
    }

  } // namespace unnamed


  class IniFileReaders : public TxtFileReader {
  public:
    ~IniFileReaders() = default;
    virtual auto readParameters() -> SiocParameters const& = 0;

  protected:
    IniFileReaders() = default;
  };





  class SiocIniFileReader final : public IniFileReaders {
  public:
    explicit SiocIniFileReader(std::filesystem::path pathtosiocfolder)
      :
      pathtosiocfolder_{ std::move(pathtosiocfolder) } {
    }
    virtual ~SiocIniFileReader() = default;

    SiocIniFileReader(SiocIniFileReader const& v) = delete;
    auto operator=(SiocIniFileReader const& v) -> SiocIniFileReader& = delete;
    SiocIniFileReader(SiocIniFileReader && v) = delete;
    auto operator=(SiocIniFileReader && v) -> SiocIniFileReader& = delete;

    auto readParameters() -> SiocParameters const& override;

  private:
    const std::filesystem::path pathtosiocfolder_;
    SiocParameters siocpars_;
  };



  auto SiocIniFileReader::readParameters() -> SiocParameters const& {
    siocpars_.hostaddress.value_ = "127.0.0.1"s;  // by default, if sioc found at same pc as your program
    siocpars_.hostaddress.isSet_ = true;

    const auto p{ findArgNumber(siocpars_.port.text_) };
    if (!p.empty()) {
      try {
        siocpars_.port.value_ = static_cast<uint16_t>(str2int32(p));
        siocpars_.port.isSet_ = true;
      } catch (...) {
        siocpars_.port.value_ = 0;
        siocpars_.port.isSet_ = false;
      }
    }

    if (auto temp{ findArgString(siocpars_.configfile.text_) }; !temp.empty()) {
      if (const auto loc{ temp.find(R"(.\)") }; loc != std::string::npos) {
        temp.erase(loc, static_cast<size_t>(2U));
        siocpars_.configfile.value_ = (pathtosiocfolder_ / std::filesystem::path(temp)).string();
      } else if (temp.find(R"(:\)", static_cast<size_t>(0)) != std::string::npos) {
        // take what the user has supplied, it is probably a full file name already
        siocpars_.configfile.value_ = temp;
      } else {
        siocpars_.configfile.value_ = (pathtosiocfolder_ / std::filesystem::path(temp)).string();
      }
      siocpars_.configfile.isSet_ = true;
    }
    return siocpars_;
  }







  class PSXseeconCfgFileReader final : public IniFileReaders {
  public:
    PSXseeconCfgFileReader() = default;
    virtual ~PSXseeconCfgFileReader() = default;

    PSXseeconCfgFileReader(PSXseeconCfgFileReader const& v) = delete;
    auto operator=(PSXseeconCfgFileReader const& v) -> PSXseeconCfgFileReader& = delete;
    PSXseeconCfgFileReader(PSXseeconCfgFileReader && v) = delete;
    auto operator=(PSXseeconCfgFileReader && v) -> PSXseeconCfgFileReader& = delete;

    auto readParameters() -> SiocParameters const& override;

  private:
    SiocParameters siocpars_;
  };

  [[nodiscard]] auto PSXseeconCfgFileReader::readParameters() -> SiocParameters const& {
    if (const auto h{ findArgIPaddress(siocpars_.hostaddress.text_) }; !h.empty()) {  // hard coded hostaddress (ip_address)
      siocpars_.hostaddress.value_ = h;
      siocpars_.hostaddress.isSet_ = true;
    }
    if (const auto p{ findArgNumber(siocpars_.port.text_) }; !p.empty()) {
      try {
        siocpars_.port.value_ = static_cast<uint16_t>(str2int32(p));
        siocpars_.port.isSet_ = true;
      } catch (...) {
        siocpars_.port.value_ = 0;
        siocpars_.port.isSet_ = false;
      }
    }
    if (const auto s{ findArgString(siocpars_.configfile.text_) }; !s.empty()) {
      siocpars_.configfile.value_ = s;
      siocpars_.configfile.isSet_ = true;
    }
    return siocpars_;
  }

  namespace {

    auto lf_InfoIsMissing(std::string const& file) {
      return " information is missing in "s + file;
    }

    auto lf_WrongExtension() {
      return " has no "s + kSsiExt + " or "s + kLstExt + " extension"s;
    }

    auto lf_complete(SiocParameters& p, std::string const& file) {
      if (!p.hostaddress.isSet_) {
        throw UserErrorException(14, p.hostaddress.text_ + lf_InfoIsMissing(file));
      }
      if (!p.port.isSet_) {
        throw UserErrorException(15, p.port.text_ + lf_InfoIsMissing(file));
      }
      if (!p.configfile.isSet_) {
        throw UserErrorException(16, p.configfile.text_ + lf_InfoIsMissing(file));
      }
      if (!p.configfile.value_.ends_with(kSsiExt)) {
        const auto loc(p.configfile.value_.find(kLstExt));
        if (loc == std::string::npos) {
          throw UserErrorException(17, p.configfile.value_ + lf_WrongExtension());
        }
        p.configfile.value_.replace(loc, 4, kSsiExt);
      }
    }

    [[nodiscard]] auto lf_fullAndFineCfg(SiocParameters& s) {
      auto r{ false };
      if (s.hostaddress.isSet_ && s.port.isSet_ && s.configfile.isSet_) {
        r = true;
        if (!s.configfile.value_.ends_with(kSsiExt)) {
          const auto loc{ s.configfile.value_.find(kLstExt) };
          if (loc == std::string::npos) {
            throw UserErrorException(17, s.configfile.value_ + lf_WrongExtension());
          }
          s.configfile.value_.replace(loc, 4, kSsiExt);
        }
      }
      return r;
    }

    auto lf_merge(SiocParameters& cfgpar, SiocParameters const& siocpar) {
      // DESIGN: values in PSXsecon.cfg take precedence over the ones in sioc.ini
      if (!cfgpar.hostaddress.isSet_) {
        cfgpar.hostaddress = siocpar.hostaddress;
      }
      if (siocpar.port.isSet_ && !cfgpar.port.isSet_) {
        cfgpar.port = siocpar.port;
      }
      if (siocpar.configfile.isSet_ && !cfgpar.configfile.isSet_) {
        cfgpar.configfile = siocpar.configfile;
      }
    }

    [[nodiscard]] auto lf_get(IniFileReaders& ir, std::filesystem::path const& file) {
      SiocParametersPtr p{ nullptr };
      if (ir.openFile(file)) {
        auto const& par{ ir.readParameters() };
        p = std::make_shared<SiocParameters>();
        (*p) = par;
      }
      return p;
    }
  } // namespace unnamed

  [[nodiscard]] auto gf_determineSiocParameters() -> SiocParametersPtr {
    SiocParametersPtr result = nullptr;
    PSXseeconCfgFileReader cfg;
    auto cfgptr{ lf_get(cfg, gkProductNameCfgPath) };
    if (cfgptr && lf_fullAndFineCfg(*cfgptr)) {
      result = cfgptr; // don't look further for sioc.ini, because cfg has priority over sioc.ini
    } else {
      auto t{ lf_getSIOCFolderQuick() };
      if (t.empty()) {
        t = lf_getSIOCFolderSlow();
      }
      SiocIniFileReader sio(t);
      auto sioptr{ lf_get(sio, t / std::filesystem::path(kSiociniStr)) };
      if ((!cfgptr) && (!sioptr)) {
        throw UserErrorException(11, kSiociniStr + " and "s + gkProductNameCfgStr + " not found"s);
      }
      if (!cfgptr) {
        lf_complete(*sioptr, kSiociniStr);
        result = sioptr;
      } else if (!sioptr) {
        lf_complete(*cfgptr, gkProductNameCfgStr);
        result = cfgptr;
      } else {
        lf_merge(*cfgptr, *sioptr);
        const std::string mergestring{ "merge of "s + kSiociniStr + " and  "s + gkProductNameCfgStr };
        lf_complete(*cfgptr, mergestring);
        result = cfgptr;
      }
    }
    return result;
  }
}  // namespace nwk
