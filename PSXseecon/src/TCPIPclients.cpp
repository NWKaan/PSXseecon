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

	QcodeValueData::QcodeValueData(const Qcode q, const int32_t v) noexcept
		:
		qcode{ q },
		value{ v } {
		static_assert(sizeof(QcodeValueData) == 8);
	}

	auto Client::writeToBuffer(const Qcode q) -> void {
		const std::lock_guard lck(mutex_);
		queue_.emplace(q, 0);
	}

	auto Client::writeToBuffer(QcodeValueData const& e) -> void {
		const std::lock_guard lck(mutex_);
		queue_.push(e);
	}

	auto Client::writeToBuffer(QcodeValueData&& e) -> void {
		const std::lock_guard lck(mutex_);
		queue_.push(std::move(e));
	}

	auto Client::flushBuffer() -> void {
		const std::lock_guard lck(mutex_);
		if (!queue_.empty()) {
			while (!queue_.empty()) {
				flushElement(queue_.front());
				queue_.pop();
			}
			endOfFlush(); 
		}
	}

	auto Client::endOfFlush() -> void {
	}

	Client::Client(std::string clientName)
		:
		clientName_{ std::move(clientName) } {
	}






	// ToTCPIPClient

	// using a Non-blocking and Asynchronous Socket
	// windows message MSG_IOCP_WINSOCK will show up in main window if there is activity at the socket.

	ToTCPIPClient::ToTCPIPClient(std::string const& clientName)
		:
		Client(clientName),
		wsa_data_(),
		WSAStart_error_num_{ static_cast<int32_t>(WSAStartup(gkSocketVersion22, &wsa_data_)) },
		socket_{ socket(AF_INET, SOCK_STREAM, IPPROTO_IP) } {
		// do not test here for wsa errors, cannot throw exceptions from ctor; perform the test in connectToServer
	}

	ToTCPIPClient::~ToTCPIPClient() {
		if (socket_ != INVALID_SOCKET) {
			closesocket(socket_);
			socket_ = INVALID_SOCKET;
		}
		WSACleanup();
	};


	auto ToTCPIPClient::connectToServer(HWND mainwindow, std::string const& hostaddress, const uint16_t port, const unsigned int windows_msg, const std::shared_ptr<FromTCPIPClient> fromTCPIPClientPtr) -> void {
		FromTCPIPClientPtr_ = fromTCPIPClientPtr;
		// test what happened in the constructor first:
		if (0 != WSAStart_error_num_) {
			throw std::runtime_error(__func__ + gkSpaceStr + clientName_ + std::format(" WSA startup error: {}", WSAStart_error_num_));
		}
		if (socket_ == INVALID_SOCKET) {
			throw std::runtime_error(__func__ + gkSpaceStr + clientName_ + " failed to create the socket"s);
		}
		// ready with testing

		// Connect Client to the IOCP server:
		// Connects to a given address, on a given port, both of which must be in network byte order.

		// The WSAAsyncSelect function requests Windows message-based notification of network events for a socket.
		if (WSAAsyncSelect(socket_, mainwindow, windows_msg, static_cast<long>(FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE)) == SOCKET_ERROR) {  //lint !e960 NOLINT
																																																																							 // Mark our socket non-blocking, and ask for async messages.
			throw std::runtime_error(__func__ + gkSpaceStr + clientName_ + " WSAAyncSelect failed"s);
		}
		sockaddr_in sinRemote{};
		sinRemote.sin_family = static_cast<short>(AF_INET);
		sinRemote.sin_addr.s_addr = inet_addr(hostaddress.c_str());
		sinRemote.sin_port = htons(port);
		// The connect function establishes a connection to a specified socket.
		if (connect(socket_, reinterpret_cast<sockaddr*>(&sinRemote), static_cast<int>(sizeof(sockaddr_in))) == SOCKET_ERROR) {  // is normal for nonblocking
			if (const auto error_num = static_cast<int32_t>(WSAGetLastError()); error_num == static_cast<int32_t>(WSAEWOULDBLOCK)) {
				// --> This is the normal exit; in main it will continue in handle_IOCPWinsock with FD_CONNECT <--
				sending_ = true;
			} else {
				throw UserErrorException(17, "connect to TCP/IP failure: "s + hostaddress + std::format(", {}, ", port) + gf_errornum2String(error_num));
			}
		}
	}

	auto ToTCPIPClient::sendData(std::string const& str) const -> void {
		if (sending_) {
			const std::lock_guard lck(send_mutex_);
			pending_send_ += str;
			while (!pending_send_.empty()) {
				const auto num_sent{ send(socket_, pending_send_.c_str(), static_cast<int>(pending_send_.length()), 0) };
				if (num_sent == SOCKET_ERROR) {
					if (const auto error_num = static_cast<int32_t>(WSAGetLastError()); error_num == static_cast<int32_t>(WSAEWOULDBLOCK)) {
						g_log.line("Trace: " + clientName_ + " sendData, Operation would block"s);
						return;
					}
					else {
						throw std::runtime_error(clientName_ + gkSpaceStr + __func__ + gkSpaceStr + gf_errornum2String(error_num));
					}
				}
				if (num_sent == 0) {
					return;
				}
				pending_send_.erase(0, static_cast<size_t>(num_sent));
			}
		}
	}

	auto ToTCPIPClient::retrySendPending() const -> void {
		sendData(gkEmptyStr);
	}

	auto ToTCPIPClient::stopSending() noexcept -> void {
		sending_ = false;
	}

	auto ToTCPIPClient::receive() -> void {
		const auto newbytes{ static_cast<int32_t>(recv(socket_, recv_buffer_, gk64K - 1, 0)) };
		if (newbytes == static_cast<int32_t>(SOCKET_ERROR)) {
			throw std::runtime_error(clientName_ + " receive" + gkColonStr + gkSpaceStr + gf_errornum2String(WSAGetLastError()));
		}
		if (0 != newbytes) {
			if (newbytes < gk64K) {
				pending_receive_.append(recv_buffer_, static_cast<size_t>(newbytes));
				auto pos{ pending_receive_.find(gkLfChar) };
				while (pos != std::string::npos) {
					auto message{ pending_receive_.substr(0, pos + 1U) };
					pending_receive_.erase(0, pos + 1U);
					(*FromTCPIPClientPtr_).decodeMessage(message);
					pos = pending_receive_.find(gkLfChar);
				}
			} else {
				g_log.line(std::format("buffer overrun: {}", newbytes));
			}
		}
	}

	auto ToTCPIPClient::closeConnectionWithServer() -> void {
		closesocket(socket_);
		socket_ = INVALID_SOCKET;
	}

	auto ToTCPIPClient::asyncError(const int32_t async_error) const -> void {
		if (async_error == static_cast<int32_t>(WSAETIMEDOUT)) {
			throw UserErrorException(18, "Connection attempt to "s + clientName_ + " timed out"s);
		}
		if (async_error == static_cast<int32_t>(WSAECONNREFUSED)) {
			throw UserErrorException(19, clientName_ + " not detected (connection refused)"s);
		}
		if (async_error == static_cast<int32_t>(WSAEADDRNOTAVAIL)) {
			throw UserIPAddressErrorException("Can't assign requested address");
		}
		if (async_error == static_cast<int32_t>(WSAEADDRINUSE)) {
			throw UserIPAddressErrorException("Address already in use");
		}
		if (async_error == static_cast<int32_t>(WSAEAFNOSUPPORT)) {
			throw UserIPAddressErrorException("Address family not supported");
		}
		if (async_error == static_cast<int32_t>(WSAECONNRESET) || async_error == static_cast<int32_t>(WSAECONNABORTED)) {
			// other party might have disappeared, let the main loop handle this error
		} else {
			throw std::runtime_error(__func__ + gkSpaceStr + clientName_ + " connection failure - "s + gf_errornum2String(async_error));
		}
	}



	// FromTCPIPClient

	FromTCPIPClient::FromTCPIPClient(std::string const& clientName)
		:
		Client(clientName) {
	}


	auto FromTCPIPClient::connectToTCPIPClient(std::shared_ptr<ToTCPIPClient> const toTCPIPClientPtr) noexcept -> void {
		toTCPIPClientPtr_ = toTCPIPClientPtr;
	}
} // namespace nwk
