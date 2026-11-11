#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {

  struct QcodeValueData final { // March 2016, Effective C++, Scott Meyers, item 17.
    Qcode qcode;
    int32_t value;
    QcodeValueData(const Qcode q, const int32_t v) noexcept;
    // automatic generation of copy ctor, copy assign operator, move ctor, move assign operator, and destructor
  };

  //
  // Client 
  // provides thread safe buffering of QcodeValues for all its subclasses
  //
  class Client {
  public:
    virtual ~Client() = default;

    Client(Client const& v) = delete;
    auto operator=(Client const& v) -> Client& = delete;
    Client(Client && v) = delete;
    auto operator=(Client && v) -> Client& = delete;

    auto writeToBuffer(const Qcode q) -> void;
    auto writeToBuffer(QcodeValueData const& e) -> void;
    auto writeToBuffer(QcodeValueData&& e) -> void;
    auto flushBuffer() -> void;  // empty the buffer by calling flushElement (of the subclass) for each Value.

  protected:
    explicit Client(std::string clientName);  // Client cannot be instantiated, only subclasses...
    virtual auto flushElement(QcodeValueData const& e) -> void = 0;
    virtual auto endOfFlush() -> void;
    const std::string clientName_;
    const std::string kexitStr = "exit";
    const std::string kSendS = "            > ";
    const std::string kRecvS = "               < ";
    const std::string kSend = "> ";
    const std::string kRecv = "   < ";

  private:
    std::queue<QcodeValueData> queue_;
    std::mutex mutex_;
  };




  class FromTCPIPClient;

  //
  // ToTCPIPClient 
  // provides functionality for connecting to an IOCP server in SIOC and to send data to 
  // the IOCP server. It also 'manages/initiates' the receive of data via the IOCP protocol, 
  // but this is finally handled by the FromTCPIPClient.
  //
  //    Note: this separation of concern is to let ToTCPIPClient buffer the data to be send, while the data received is
  //    buffered by the FromTCPIPClient (both clients inherit this buffering capability from the Client class). 
  //
  // ToTCPIPClient uses windows socket communication (WINSOCK2), for more info http://tangentsoft.net/wskfaq/
  // The connection is non blocking with asynchronous messages.
  //
  //   Events (messages) by the network, such as FD_CONNECT ('SIOC connected'), FD_WRITE ('socket ready for write'), 
  //   FD_READ ('data received at socket') and FD_CLOSE ('socket closed by server'), will be received in the 
  //   main window of the application (using ToTCPIPClient); see my IOCPClient for an example how that can be done.
  //
  class ToTCPIPClient : public Client {
  public:
    ~ToTCPIPClient();

    ToTCPIPClient(ToTCPIPClient const& v) = delete;
    auto operator=(ToTCPIPClient const& v) -> ToTCPIPClient& = delete;
    ToTCPIPClient(ToTCPIPClient && v) = delete;
    auto operator=(ToTCPIPClient && v) -> ToTCPIPClient& = delete;


    auto connectToServer(const HWND mainwindow,            // HANDLE to main window of the application using ToTCPIPClient.
                         std::string const& hostaddress,   // IOCP_HOSTADDRESS (IP address of PC running SIOC)
                         const uint16_t port,               // IOCP_Port number
                         const unsigned int windows_msg,   // WINSOCK windows message code
                         const std::shared_ptr<FromTCPIPClient> fromTCPIPClientPtr) -> void; // pointer to a fromSIOCClient for decoding (and buffering) received values
    auto closeConnectionWithServer() -> void;                      // disconnects if connected, safe to call
    auto sendData(std::string const& str) const -> void;
    auto retrySendPending() const -> void;
    auto receive() -> void;      // should be called when there is new data received at the socket
    auto stopSending() noexcept -> void;
    virtual auto exit() -> void = 0;
    auto asyncError(const int32_t async_error) const -> void;

  protected:
    explicit ToTCPIPClient(std::string const& clientName);
    using RecvBuffer = char[gk64KU];
    RecvBuffer recv_buffer_;

  private:
    mutable std::mutex send_mutex_;
    mutable std::string pending_send_;
    std::string pending_receive_;
    std::shared_ptr<FromTCPIPClient> FromTCPIPClientPtr_{ nullptr };
    bool sending_{ false };
    WSAData wsa_data_;
    int32_t WSAStart_error_num_{ 0 };
    SOCKET socket_;
  };


  

  //
  // FromTCPIPClient 
  // provides functionality for decoding the data received by the ToTCPIPClient from the IOCP server.
  //

  class FromTCPIPClient : public Client {
  public:
    ~FromTCPIPClient() = default;

    FromTCPIPClient(FromTCPIPClient const& v) = delete;
    auto operator=(FromTCPIPClient const& v) -> FromTCPIPClient& = delete;
    FromTCPIPClient(FromTCPIPClient && v) = delete;
    auto operator=(FromTCPIPClient && v) -> FromTCPIPClient& = delete;

    auto connectToTCPIPClient(std::shared_ptr<ToTCPIPClient> const toTCPIPClientPtr) noexcept -> void;  // connect to ToTCPIPClient, so it is able to sendData as well 
    virtual auto decodeMessage(std::string const& line) -> void = 0;

  protected:
    explicit FromTCPIPClient(std::string const& clientName);
    std::shared_ptr<ToTCPIPClient> toTCPIPClientPtr_ = nullptr;
  };

} // namespace nwk
