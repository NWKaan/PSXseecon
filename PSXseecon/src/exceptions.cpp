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

	UserErrorException::UserErrorException(int32_t error_num, std::string const& what_arg)
		:
		std::runtime_error(std::format("{}: ", error_num) + what_arg) {
	}

	SysErrorException::SysErrorException(int32_t error_num, std::string const& what_arg)
		:
		std::runtime_error(std::format("sys error {}: ", error_num) + what_arg) {
	}


  UserIPAddressErrorException::UserIPAddressErrorException(std::string const& what_arg) 
    :
    UserErrorException(77, what_arg) {
  }
#pragma warning(push)
#pragma warning(disable:26455)

  SIOC_FD_CLOSE_Exception::SIOC_FD_CLOSE_Exception() 
    :
    std::runtime_error("SIOC server closed the connection"s) {
  }

  PSX_FD_CLOSE_Exception::PSX_FD_CLOSE_Exception() 
    :
    std::runtime_error("PSX main server closed the connection"s) {
  }

  PSXBOOST_FD_CLOSE_Exception::PSXBOOST_FD_CLOSE_Exception() 
    :
    std::runtime_error("PSX boost server closed the connection"s) {
  }

  FinReceivedException::FinReceivedException() 
    :
    std::runtime_error("Fin received from SIOC server"s) {
  }

  ExitPSXReceivedException::ExitPSXReceivedException() 
    :
    std::runtime_error("Exit received from PSX server"s) {
  }

  ExitPSXBoostReceivedException::ExitPSXBoostReceivedException() 
    :
    std::runtime_error("Exit received from PSX Boost server"s) {
  }

  InternetAccessException::InternetAccessException() 
    :
    std::runtime_error("No internet access"s) {
  }

  InternetUrlException::InternetUrlException(std::string const& what_arg) 
    :
    std::runtime_error("Url not found: "s + what_arg) {
  }
#pragma warning(pop)
 }  
