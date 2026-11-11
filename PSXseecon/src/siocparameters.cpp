//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
#include "stdafx.hpp"

namespace nwk {
  auto SiocParameters::toString() const -> std::string {
    std::string result;
    if (hostaddress.isSet_) {
      result += std::format(" {}={}\n",  hostaddress.text_ , *hostaddress);
    }
    if (port.isSet_) {
      result += std::format(" {}={}\n", port.text_, *port);
    }
    if (configfile.isSet_) {
      result += std::format(" {}={}\n", configfile.text_, *configfile);
    }
    return result;
  }
}  
