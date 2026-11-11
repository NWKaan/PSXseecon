#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
	class UserErrorException : public std::runtime_error {
	public:
		explicit UserErrorException(int32_t error_num, std::string const& what_arg);
	};

	class SysErrorException : public std::runtime_error {
	public:
		explicit SysErrorException(int32_t error_num, std::string const& what_arg);
	};

	class UserIPAddressErrorException final : public UserErrorException {
	public:
		explicit UserIPAddressErrorException(std::string const& what_arg);
	};


	class SIOC_FD_CLOSE_Exception final : public std::runtime_error {
	public:
		SIOC_FD_CLOSE_Exception();
	};

	class PSX_FD_CLOSE_Exception final : public std::runtime_error {
	public:
		PSX_FD_CLOSE_Exception();
	};

	class PSXBOOST_FD_CLOSE_Exception final : public std::runtime_error {
	public:
		PSXBOOST_FD_CLOSE_Exception();
	};

	class FinReceivedException final : public std::runtime_error {
	public:
		FinReceivedException();
	};

	class ExitPSXReceivedException final : public std::runtime_error {
	public:
		ExitPSXReceivedException();
	};

	class ExitPSXBoostReceivedException final : public std::runtime_error {
	public:
		ExitPSXBoostReceivedException();
	};

	class InternetAccessException final : public std::runtime_error {
	public:
		InternetAccessException();
	};

	class InternetUrlException final : public std::runtime_error {
	public:
		InternetUrlException(std::string const& what_arg);
	};

} 

