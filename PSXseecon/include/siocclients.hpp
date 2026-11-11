#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
	class ToSiocClient final : public ToTCPIPClient {
	public:
		ToSiocClient();
		~ToSiocClient() = default;

		ToSiocClient(ToSiocClient const& v) = delete;
		auto operator=(ToSiocClient const& v) -> ToSiocClient& = delete;
		ToSiocClient(ToSiocClient && v) = delete;
		auto operator=(ToSiocClient && v) -> ToSiocClient& = delete;

		auto sendInicio(QcodeSet const& registeredvars) -> void;    // registers registeredvars at the IOCP server
		auto exit() -> void override;

	protected:
		auto flushElement(QcodeValueData const& e) -> void override;        // sends Var-Value to SIOC server
		auto endOfFlush() -> void override;

	private:
		std::string flushbuf_;
	};

	class FromSiocClient final : public FromTCPIPClient {
	public:
		FromSiocClient();
		~FromSiocClient() = default;

		FromSiocClient(FromSiocClient const& v) = delete;
		auto operator=(FromSiocClient const& v) -> FromSiocClient& = delete;
		FromSiocClient(FromSiocClient && v) = delete;
		auto operator=(FromSiocClient && v) -> FromSiocClient& = delete;

		auto decodeMessage(std::string const& line) -> void override;
		auto initInicio(QcodeSet const& registeredvars) -> void;

	protected:
		auto flushElement(QcodeValueData const& e) -> void override;        // sends Var-Value to SIOC server

	private:
		static const RefToSharedVarPtrFunctionWithQcodePar varptrfie_;
		std::array<SharedVarPtr, static_cast<uint32_t>(Qcode::QCODE_DUMMY_END)> varpool_;
	};
}
