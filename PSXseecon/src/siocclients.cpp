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
#pragma warning(push)
		constexpr auto kVivo{ "Vivo"s };
		constexpr auto kArn{ "Arn"s };
		constexpr auto kResp{ "Resp"s };
		constexpr auto kFin{ "Fin"s };
		constexpr auto kArnkVivo{ "Arn.Vivo:"s };
		constexpr auto kArnInicio{ "Arn.Inicio:"s };
		constexpr auto kArnResp{ "Arn.Resp:"s };
		constexpr auto kArnFin{ "Arn.Fin:"s };
		constexpr size_t kMaxBytesToSIOC = 4'000U;
#pragma warning(pop)
	} // namespace unnamed

#pragma warning(push)
#pragma warning(disable:26455)
	ToSiocClient::ToSiocClient()
		:
		ToTCPIPClient("ToSIOC"s) {
	}
#pragma warning(pop)

	auto ToSiocClient::flushElement(QcodeValueData const& e) -> void {
		flushbuf_ += std::format("{}={}:", static_cast<int32_t>(e.qcode), e.value);
		if (flushbuf_.size() > kMaxBytesToSIOC) {
			ToSiocClient::endOfFlush();
		}
	}

	auto ToSiocClient::sendInicio(QcodeSet const& registeredvars) -> void {
		std::string inicio;
		for (auto qcode : registeredvars) {
			inicio += gf_Qcode2Str(qcode) + gkColonStr;
			if (inicio.size() > kMaxBytesToSIOC) {
				sendData(kArnInicio + inicio + gkCrLfStr);
				if (*g_CfgMONITOR_SIOC_OUT) {
					g_log.line(kSendS + kArnInicio + inicio);
				}
				inicio.clear();
			}
		}
		if (!inicio.empty()) {
			sendData(kArnInicio + inicio + gkCrLfStr);
			if (*g_CfgMONITOR_SIOC_OUT) {
				g_log.line(kSendS + kArnInicio + inicio);
			}
			inicio.clear();
		}
	}

	auto ToSiocClient::endOfFlush() -> void {
		if (!flushbuf_.empty()) {
			sendData(kArnResp + flushbuf_ + gkCrLfStr);
			if (*g_CfgMONITOR_SIOC_OUT) {
				g_log.line(kSendS + kArnResp + flushbuf_);
			}
			flushbuf_.clear();
		}
	}

	auto ToSiocClient::exit() -> void {
		sendData(kArnFin + gkCrLfStr);
		if (*g_CfgMONITOR_SIOC_OUT) {
			g_log.line("Fin sent to SIOC"s);
		}
	}





	// FromSiocClient
#pragma warning(push)
#pragma warning(disable:26455)
	FromSiocClient::FromSiocClient()
		:
		FromTCPIPClient("FromSIOC"s) {
	}
#pragma warning(pop)

	auto FromSiocClient::initInicio(QcodeSet const& registeredvars) -> void {
		// Get the pointers to the Vars for which we may receive Value changes
		for (auto q : registeredvars) {
			auto ptr{ varptrfie_(q) };
			if (!ptr) {  // not necessary but defensive programming
				throw std::runtime_error(std::string(__func__));
			}
			varpool_[static_cast<uint32_t>(q)] = ptr;
		};
	}

	auto FromSiocClient::flushElement(QcodeValueData const& e) -> void {
		const auto ptr{ varpool_[static_cast<uint32_t>(e.qcode)] };
		if (!ptr) {  // not necessary but defensive programming
			throw std::runtime_error(std::string(__func__));
		}
		(*ptr).setValue(e.value);
	}

	auto FromSiocClient::decodeMessage(std::string const& line) -> void {
		if (*g_CfgMONITOR_SIOC_IN) {
			g_log.writeString(kRecvS + line);
		}
		try {
#ifdef PMDG_NGX
			QcodeValueData el{ Qcode::N1, 0 };
#else
			QcodeValueData el{ Qcode::Qs0, 0 };
#endif
			auto varnumber_received{ false };
			const std::regex regexpr{ R"(-*\w+)" };
			const std::sregex_token_iterator end;
			for (std::sregex_token_iterator i(line.begin(), line.end(), regexpr); i != end; ++i) {
				auto const& token{ *i };
				if (token == kFin) {
					throw FinReceivedException();
				}
				if (token == kVivo) {
					(*toTCPIPClientPtr_).sendData(kArnkVivo + gkCrLfStr);
					// continue with this line, because Vivo and Resp can be in one message
				} else if (token == kResp || token == kArn) {
					// just eat it up, continue  ...
				} else if (!varnumber_received) {
					el.qcode = static_cast<Qcode>(str2int32(token));  // var received, continue reading value
					varnumber_received = true;
				} else {
					el.value = str2int32(token);  // complete "var=value" received
					varnumber_received = false;
					writeToBuffer(el);
				}
			}
		} catch (const FinReceivedException& e) {
			UNREFERENCED_PARAMETER(e);
			throw;  // re-throw,  main will handle this
		} catch (const std::exception& e) {
			throw std::runtime_error("From SIOC: "s + line + gkCommaStr + gkSpaceStr + e.what() + gkNewLineStr);  // re-throw, main will handle this
		}
	}
} // namespace nwk
