//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//
#include "stdafx.hpp"
#include <stdexcept>

namespace nwk {
  using namespace::std::literals;

  constinit bool ControlVar::syncPhase_{ true };

  inline constexpr auto gkThreadsStarveTime{ 350ms };

  namespace {
    constexpr uint16_t lkPSXport{ 10'747 };

    constinit std::atomic<bool> l_PSX_FD_WRITE{ false };
    constinit std::atomic<bool> l_SIOC_FD_WRITE{ false };
    constinit std::atomic<bool> l_user_closed{ false };
    constinit std::atomic<bool> l_looping{ false };
    constinit std::atomic<bool> l_reading_ssifile{ false };

    constinit bool l_mainloop_started{ false };

    constinit HWND l_main_window{ nullptr };

    const auto l_toSIOCPtr{ std::make_shared<ToSiocClient>() };
    const auto l_fromSIOCPtr{ std::make_shared<FromSiocClient>() };
    const auto l_toPSXPtr{ std::make_shared<ToPSXClient>() };
    const auto l_fromPSXPtr{ std::make_shared<FromPSXClient>() };

    auto l_varpoolPtr{ std::make_unique<VarPool>() };

    enum class LksExitPossibility : uint32_t {
      E_NEW_SITU, E_SIM_CLOSED, E_USER_CLOSE, E_MAIN_LOOP_EXIT, E_IOCP_CLOSE, E_PSX_CLOSE, E_PSX_EXIT, E_PSXBOOST_CLOSE, E_PSXBOOST_EXIT, E_IOCP_FIN
    };
    using StringArray10 = std::array<std::string, 10>;
    const StringArray10 lkExitPossibilityAsStr{ StringArray10{"E_NEW_SITU"s, "E_SIM_CLOSED"s, "E_USER_CLOSE"s, "E_MAIN_LOOP_EXIT"s, "E_IOCP_CLOSE"s,
            "E_PSX_CLOSE"s, "E_PSX_EXIT"s, "E_PSXBOOST_CLOSE"s, "E_PSXBOOST_EXIT"s, "E_IOCP_FIN"s } };

 
    SharedVarPtrVector l_infovars; // to be checked at 5Hz

    QcodeVector l_CduVar;

    QcodeValueData l_ifr_val{ Qcode::QCODE_DUMMY_END, 0 };
    DeltaColVal l_dc_val{ Qcode::QCODE_DUMMY_END,{} };

    auto lf_getVarptr(const Qcode q) -> SharedVarPtr {
      SharedVarPtr r{ nullptr };
      if (l_varpoolPtr) {
        r = (*l_varpoolPtr).getVarPtr(q);
      }
      return r;
    }

  } // namespace unnamed

  const RefToSharedVarPtrFunctionWithQcodePar ControlVar::varptrfie_{ &lf_getVarptr };
  const RefToSharedVarPtrFunctionWithQcodePar InfoVar::varptrfie_{ &lf_getVarptr };
  const RefToSharedVarPtrFunctionWithQcodePar FromSiocClient::varptrfie_{ &lf_getVarptr };
  const RefToSharedVarPtrFunctionWithQcodePar FromPSXClient::varptrfie_{ &lf_getVarptr };

  const std::shared_ptr<Client> InfoVar::tosiocptr_{ l_toSIOCPtr };
  const std::shared_ptr<Client> ControlVar::toPSXPtr_{ l_toPSXPtr };


  namespace {
    auto lf_systemError(const std::exception& e) -> void {
      g_log.line(std::format("System Error: {}\n", e.what()));
      l_looping.store(false, std::memory_order_relaxed);
    }

    auto lf_initFromClients() -> void {
      if (auto const& registeredvarsPSX{ (*l_varpoolPtr).getInicioPsx() }; !registeredvarsPSX.empty()) {
        (*l_fromPSXPtr).initInicio(registeredvarsPSX);
        g_log.line(std::format("{} variables initialised in FromPSXClient", registeredvarsPSX.size()));
      }
      if (auto const& registeredvars{ (*l_varpoolPtr).getInicio() }; !registeredvars.empty()) {
        (*l_fromSIOCPtr).initInicio(registeredvars);
        g_log.line(std::format("{} variables initialised in FromSIOCClient", registeredvars.size()));
      }
    }

    auto lf_sync() -> void {
      g_log.line("Synchronising PSX with SIOC variables ..."s);
      if (auto const& registeredvars{ (*l_varpoolPtr).getInicio() }; !registeredvars.empty()) {
        (*l_toSIOCPtr).sendInicio(registeredvars);
        g_log.line(std::format("{} variables registered at SIOC server", registeredvars.size()));
      }
      std::this_thread::sleep_for(1s);      // give some time for receival of initial values from SIOC and PSX ...
      // IMPORTANT
      // flush SIOC first, because while flushing from PSX we need to re-send QH values immediately, 
      // otherwise we would miss the initial hardware value from SIOC, 
      // causing sending Qhxyz=0 followed by Qhxyz=1 in the same std::string....
      //
      (*l_fromSIOCPtr).flushBuffer();
      (*l_fromPSXPtr).flushBuffer();

      l_varpoolPtr.reset(); //  deletes managed object, is no longer needed, we are up and running ...
      l_varpoolPtr = nullptr;
      ControlVar::syncPhase_ = false;
    }

    auto lf_decideUponExitStrategy(const LksExitPossibility e) -> void {
      static auto first{ true };
      if (first) {
        g_log.line("Exiting: "s + lkExitPossibilityAsStr.at(static_cast<uint32_t>(e)));
        first = false;  // prevent more then one call
        if (l_looping.load(std::memory_order_relaxed)) {
          l_looping.store(false, std::memory_order_relaxed);      // main loop will notice and stop
          std::this_thread::sleep_for(gkThreadsStarveTime);  // give the main loop thread time to notice l_looping = false and stop
        }
        if (l_PSX_FD_WRITE.load(std::memory_order_relaxed) && e != LksExitPossibility::E_PSX_EXIT && e != LksExitPossibility::E_PSX_CLOSE) {
          g_log.line("Force disconnect from PSX ..."s);
          try {
            (*l_toPSXPtr).exit();
            std::this_thread::sleep_for(250ms);  // give PSX time to stop sending
          } catch (const std::exception& ew) {
            g_log.line("Exception while trying to exit PSX: "s + ew.what() + gkNewLineStr);
          }
        }
        if (l_SIOC_FD_WRITE.load(std::memory_order_relaxed) && e != LksExitPossibility::E_IOCP_FIN && e != LksExitPossibility::E_IOCP_CLOSE) {
          g_log.line("Force disconnect from SIOC ..."s);
          try {
            (*l_toSIOCPtr).exit();
            std::this_thread::sleep_for(100ms);  // give SIOC time to stop sending
          } catch (const std::exception& ew) {
            g_log.line("Exception while trying to exit SIOC: "s + ew.what() + gkNewLineStr);
          }
        }
        if (!l_PSX_FD_WRITE.load(std::memory_order_relaxed) || !l_SIOC_FD_WRITE.load(std::memory_order_relaxed)) {
          std::this_thread::sleep_for(250ms);  // give connecting proces(-es) time to stop, upon l_user_closed
        }
        gf_postWindowsMessage(MSG_EXIT);
      }
    }

    auto lf_loop() -> void {
      l_looping.store(true, std::memory_order_relaxed);
      g_log.line("processing input from PSX and SIOC started"s);

      // BEGIN LOOP ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      while (l_looping.load(std::memory_order_relaxed)) {
        static uint32_t l_hartbeat = 0;
        ++l_hartbeat;
        std::this_thread::sleep_for(50ms);  // 20Hz
        const auto fiveHz{ (0 == (l_hartbeat % 4)) };
        (*l_fromPSXPtr).flushBuffer();

        // ugly trick:
        // a CDU code 39 (CLR) produced in (*l_fromSIOCPtr).flushBuffer() in the previous tick, has to
        // be followed by a -1 ( 1 tick later)
        // all CDU's might have typed CLR in the same tick ...
        while (!l_CduVar.empty()) {
          const auto q{ l_CduVar.back() };
          l_CduVar.pop_back();
          (*l_toPSXPtr).writeToBuffer(QcodeValueData(q, -1));
        }
        // end of trick

        (*l_fromSIOCPtr).flushBuffer();
        if (fiveHz) {
          std::ranges::for_each(std::as_const(l_infovars), [](auto const p) -> auto {(*p).check(); });
        }
        if (gf_getIFRData(l_ifr_val)) { // only one rotary value per tick
          (*l_toPSXPtr).writeToBuffer(l_ifr_val);
        }
        if (gf_getDCVData(l_dc_val)) {  // only one collection of rotary values per tick
          g_str[static_cast<uint32_t>(l_dc_val.qcode)] = l_dc_val.val;
          (*l_toPSXPtr).writeToBuffer(l_dc_val.qcode);
        }
        (*l_toPSXPtr).flushBuffer();
        (*l_toSIOCPtr).flushBuffer();
        if (g_CfgCduScreen && fiveHz) {  //fiveHz added Sep 28, 2021
          g_gui.areThereCduChanges();
        }
      }
      // END LOOP ---------------------------------------------------------------------
      g_log.line("processing input from PSX and SIOC stopped"s);
      lf_decideUponExitStrategy(LksExitPossibility::E_MAIN_LOOP_EXIT);
    }

    auto lf_mainLoop_as_Thread() noexcept -> void {
      static constexpr auto iam{ "lf_mainLoop_as_Thread"sv };
      gf_threadStart(iam);
      try {
        lf_sync();
        if (g_DemandForQi211) {
          (*l_toPSXPtr).demandForQi(211U);
        }
        if (g_DemandForQi214) {
          (*l_toPSXPtr).demandForQi(214U);
        }
        if (g_DemandForQi271) {
          (*l_toPSXPtr).demandForQi(271U);
        }
        if (g_DemandForQi273) {
          (*l_toPSXPtr).demandForQi(273U);
        }
        if (g_DemandForQi274) {
          (*l_toPSXPtr).demandForQi(274U);
        }
        if (g_DemandForQs480) {
          (*l_toPSXPtr).demandForQs(480U);
        }
        if (g_DemandForQs483) {
          (*l_toPSXPtr).demandForQs(483U);
        }
        g_gui.clearMessage();
        if (g_CfgCduScreen) {
          g_gui.setCdu(true);
        }
        lf_loop();
      } catch (const std::exception& e) {
        lf_systemError(e);
      }
      gf_threadFinished(iam);
    }

    const QcodeSet l_internalQhvars{
      Qcode::Qh13,  Qcode::Qh14,  Qcode::Qh32,  Qcode::Qh33, Qcode::Qh34,  Qcode::Qh35,  Qcode::Qh53,  Qcode::Qh54,
      Qcode::Qh76,  Qcode::Qh116, Qcode::Qh117, Qcode::Qh118, Qcode::Qh119, Qcode::Qh120, Qcode::Qh121, Qcode::Qh122,
      Qcode::Qh123, Qcode::Qh124, Qcode::Qh125, Qcode::Qh126, Qcode::Qh127, Qcode::Qh129, Qcode::Qh130, Qcode::Qh131,
      Qcode::Qh133, Qcode::Qh137, Qcode::Qh170, Qcode::Qh178, Qcode::Qh183, Qcode::Qh184, Qcode::Qh185, Qcode::Qh186,
      Qcode::Qh187, Qcode::Qh192, Qcode::Qh193, Qcode::Qh386, Qcode::Qh213,
      Qcode::Qh214, Qcode::Qh215, Qcode::Qh224, Qcode::Qh229, Qcode::Qh230, Qcode::Qh231, Qcode::Qh232, Qcode::Qh234,
      Qcode::Qh235, Qcode::Qh236, Qcode::Qh263, Qcode::Qh265, Qcode::Qh275, Qcode::Qh276, Qcode::Qh277, Qcode::Qh278,
      Qcode::Qh279, Qcode::Qh286, Qcode::Qh288, Qcode::Qh293, Qcode::Qh300, Qcode::Qh301, Qcode::Qh319, Qcode::Qh320,
      Qcode::Qh321, Qcode::Qh322, Qcode::Qh358, Qcode::Qh387, Qcode::Qh390, Qcode::Qh391, Qcode::Qh398, Qcode::Qh399,
      Qcode::Qh420, Qcode::Qh421, Qcode::Qh422, Qcode::Qh423, Qcode::Qh424
    };

    auto lf_remove_internalvars_other_then_Qs(QcodeSet& st) -> void {
      for (auto q : l_internalQhvars) {
        const auto it{ st.find(q) };
        if (it != st.end()) {
          st.erase(it);
        }
      };
    }

    auto lf_var(const Qcode q) -> std::string {
      return "Var "s + gf_Qcode2Str(q);
    }

    auto lf_update_to_whitelisted_vars(QcodeSet& st) -> void {
      QcodeSet res;
      for (auto q : g_whitelist_vars) {
        if (const auto it{ st.find(q) }; it != st.end()) {
          res.insert(q);
          g_log.line(lf_var(q) + " whitelisted"s);
        } else {
          g_log.line(lf_var(q) + " of your white list does not occur in CONFIG_FILE ..."s);
        }
      };
      st = res;
    }


    auto lf_remove_blacklisted_vars(QcodeSet& st) -> void {
      for (auto q : g_blacklist_vars) {
        if (const auto it{ st.find(q) }; it != st.end()) {
          st.erase(it);
          g_log.line(lf_var(q) + " blacklisted"s);
        } else {
          g_log.line(lf_var(q) + " of your black list does not occur in CONFIG_FILE ..."s);
        }
      }
    }

    auto lf_CollectConfigureAndDispatchVars(std::string const& ssifile) -> void {
      SsiFileReader ssi(Qcode::Qh0, static_cast<Qcode>(static_cast<uint32_t>(Qcode::QCODE_DUMMY_END) - 1));
      g_log.line("Reading file "s + ssifile);
      l_reading_ssifile.store(true, std::memory_order_relaxed);
      if (!ssi.openFile(ssifile)) {
        l_reading_ssifile.store(false, std::memory_order_relaxed);
        throw UserErrorException(16, ssifile + " not found"s);
      }
      g_gui.clearMessage();
      ssi.collectStaticVarNumbers();
      l_reading_ssifile.store(false, std::memory_order_relaxed);
      QcodeSet vars_found_in_configfile = ssi.static_vars_;
      lf_remove_internalvars_other_then_Qs(vars_found_in_configfile);
      if (!g_whitelist_vars.empty()) {
        lf_update_to_whitelisted_vars(vars_found_in_configfile);
        if (vars_found_in_configfile.empty()) {
          g_log.line("The WHITELIST does not contain valid PSXseecon variable numbers!"s);
        } else if (!g_blacklist_vars.empty()) {
          g_log.line("You cannot have a WHITELIST and a BLACKLIST; the latter is ignored!"s);
        }
      } else if (!g_blacklist_vars.empty()) {
        lf_remove_blacklisted_vars(vars_found_in_configfile);
      }
      if (vars_found_in_configfile.empty()) {
        throw UserErrorException(1, "No variables found in "s + ssifile);
      }
      (*l_varpoolPtr).configurePool(vars_found_in_configfile);

      for (auto q : vars_found_in_configfile) {
        if (!(*l_varpoolPtr).getVarPtr(q)) {
          throw UserErrorException(2, lf_var(q) + " in your "s + ssifile + " is not a PSXseecon variable"s);
        }
      };

      (*l_varpoolPtr).preInitAllVars();

      // dispatch infovars that needs regular checking
      for (const auto q : vars_found_in_configfile) {
        if (const auto p{ (*l_varpoolPtr).getVarPtr(q) }; p) {
          if ((*p).isInfoVar() && (*p).needsCheck()) {
            l_infovars.push_back(p);
          }
        }
      }
    }

    auto lf_initVars() -> void {
      lf_CollectConfigureAndDispatchVars((*g_siocparameters).configfile.value_);
      const auto numvars{ (*l_varpoolPtr).getNumberOfVars() };
      g_gui.setNumberOfPSXseeconVars(numvars);
      g_log.line(std::format("\n{} PSXseecon variable", numvars) + gf_Plural(numvars) + " detected:"s);
      g_log.writeStringList((*l_varpoolPtr).getVarNumbersForLog());
      g_log.line(std::format("\nNumber of infovars to be checked: {}", l_infovars.size()));
    }

    auto lf_connectToPSX() -> void {
      g_gui.line("connecting to PSX ..."s);
      if (!l_user_closed.load(std::memory_order_relaxed)) {
        (*l_toPSXPtr).connectToServer(g_gui.getMainWindow(), *g_CfgPSXHostAddress, lkPSXport,
                                      static_cast<unsigned int>(MSG_PSX_WINSOCK), l_fromPSXPtr);
      }
    }

    auto lf_connectToSIOC() -> void {
      g_gui.line("connecting to SIOC ..."s);
      if (!l_user_closed.load(std::memory_order_relaxed)) {
        (*l_toSIOCPtr).connectToServer(g_gui.getMainWindow(), (*g_siocparameters).hostaddress.value_,
                                       (*g_siocparameters).port.value_, static_cast<unsigned int>(MSG_IOCP_WINSOCK), l_fromSIOCPtr);
      }
    }

    auto lf_startUp_as_Thread() noexcept -> void {
      static constexpr auto iam{ "lf_startUp_as_Thread"sv };
      gf_threadStart(iam);
      try {
        gf_getCfgData();
        if (*g_CfgVersionCheck && gf_isWin10OrLater()) {
          gf_versionCheck();
        }
#ifdef LIST_SIOC_VARS
        (*l_varpoolPtr).generateSIOCVarsList();
        lf_decideUponExitStrategy(LksExitPossibility::E_IOCP_CLOSE);
#else
        if (g_siocparameters = gf_determineSiocParameters(); !g_siocparameters) {
          throw std::runtime_error("g_siocparameters == null"s);
        }
        g_log.line("\nSIOC parameters\n"s + (*g_siocparameters).toString());
        (*l_fromPSXPtr).connectToTCPIPClient(l_toPSXPtr);
        g_log.line("\nPSX parameters"s);
        g_log.line(" PSX_HOSTADDRESS="s + *g_CfgPSXHostAddress);
        g_log.line(std::format(" PSX_port={}", lkPSXport));
        (*l_fromSIOCPtr).connectToTCPIPClient(l_toSIOCPtr);
        lf_initVars();
        lf_initFromClients();
        lf_connectToSIOC();
        lf_connectToPSX();
#endif
      } catch (const UserErrorException& e) {
        g_log.line(__func__);
        g_gui.error(e.what() + gkNewLineStr);
      } catch (const std::exception& e) {
        g_log.line(__func__);
        lf_systemError(e);
      }
      gf_threadFinished(iam);
    }

    auto lf_handle_WINSOCK_IOCP_param(const int32_t winsock_param) -> void {
      switch (winsock_param) {
      case FD_CONNECT:
        g_log.line("SIOC connected"s);
        g_gui.setSIOCStatus(true);
        break;

      case FD_WRITE:
        l_SIOC_FD_WRITE.store(true, std::memory_order_relaxed);
        (*l_toSIOCPtr).retrySendPending();
        if (!l_mainloop_started) {
          if (l_PSX_FD_WRITE.load(std::memory_order_relaxed)) {
            l_mainloop_started = true;
            // depending on who (PSX-SIOC) is last, will start the main loop, and only if all are ok...
            std::thread t(lf_mainLoop_as_Thread);
            t.detach();
          }
        }
        break;

      case FD_READ:    // Winsock ready for read
        (*l_toSIOCPtr).receive();
        break;

      case FD_CLOSE:
        (*l_toSIOCPtr).stopSending();
        l_SIOC_FD_WRITE.store(false, std::memory_order_relaxed);
        g_gui.setSIOCStatus(false);
        throw SIOC_FD_CLOSE_Exception();
        break;

      default:
        throw std::runtime_error(std::string(__func__));
        break;
      }
    }

    auto lf_handle_WINSOCK_PSX_param(const int32_t winsock_param) -> void {
      switch (winsock_param) {
      case FD_CONNECT:
        g_log.line("PSX connected"s);
        g_gui.setPSXStatus(true);
        break;

      case FD_WRITE:
        l_PSX_FD_WRITE.store(true, std::memory_order_relaxed);
        (*l_toPSXPtr).retrySendPending();
        if (!l_mainloop_started) {
          if (l_SIOC_FD_WRITE.load(std::memory_order_relaxed)) {
            l_mainloop_started = true;
            // depending on who (PSX-SIOC) is last, will start the main loop, and only if all are ok...
            std::thread t(lf_mainLoop_as_Thread);
            t.detach();
          }
        }
        break;

      case FD_READ:    // Winsock ready for read
        (*l_toPSXPtr).receive();
        break;

      case FD_CLOSE:
        (*l_toPSXPtr).stopSending();
        l_PSX_FD_WRITE.store(false, std::memory_order_relaxed);
        g_gui.setPSXStatus(false);
        throw PSX_FD_CLOSE_Exception();
        break;

      default:
        throw std::runtime_error(std::string(__func__));
        break;
      }
    }

    auto lf_MSG_IOCP_WINSOCK(const LPARAM lParam) -> void {
      try {
        if (const auto async_error = static_cast<int32_t>(WSAGETASYNCERROR(lParam)); 0 != async_error) {
          (*l_toSIOCPtr).asyncError(async_error);
        } else {
          lf_handle_WINSOCK_IOCP_param(static_cast<int32_t>(WSAGETSELECTEVENT(lParam)));
        }
      } catch (const UserErrorException& e) {
        g_log.line(e.what() + gkNewLineStr);
        if (!l_user_closed.load(std::memory_order_relaxed)) {
          lf_connectToSIOC();  // retry
        }
      } catch (const SIOC_FD_CLOSE_Exception& e) {
        UNREFERENCED_PARAMETER(e);
        lf_decideUponExitStrategy(LksExitPossibility::E_IOCP_CLOSE);
      } catch (const FinReceivedException& e) {
        UNREFERENCED_PARAMETER(e);
        lf_decideUponExitStrategy(LksExitPossibility::E_IOCP_FIN);
      } catch (const std::exception& e) {
        lf_systemError(e);
      }
    }

    auto lf_MSG_PSX_WINSOCK(const LPARAM lParam) -> void {
      try {
        const auto async_error = static_cast<int32_t>(WSAGETASYNCERROR(lParam));
        if (0 != async_error) {
          (*l_toPSXPtr).asyncError(async_error);
        } else {
          lf_handle_WINSOCK_PSX_param(static_cast<int32_t>(WSAGETSELECTEVENT(lParam)));
        }
      } catch (const UserErrorException& e) {
        g_log.line(e.what() + gkNewLineStr);
        if (!l_user_closed.load(std::memory_order_relaxed)) {
          lf_connectToPSX();  // retry 
        }
      } catch (const PSX_FD_CLOSE_Exception& e) {
        UNREFERENCED_PARAMETER(e);
        lf_decideUponExitStrategy(LksExitPossibility::E_PSX_CLOSE);
      } catch (const ExitPSXReceivedException& e) {
        UNREFERENCED_PARAMETER(e);
        lf_decideUponExitStrategy(LksExitPossibility::E_PSX_EXIT);
      } catch (const std::exception& e) {
        lf_systemError(e);
      }
    }

    auto lf_MSG_EXIT() -> void {
      if (g_CfgCduScreen) {
        g_gui.setCdu(false);
        std::this_thread::sleep_for(2s);
      } else {
        std::this_thread::sleep_for(500ms);
      }
      std::this_thread::sleep_for(500ms);
    }

    auto CALLBACK lf_messageHandler(HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam) -> LRESULT {
      l_main_window = hWnd;
      switch (message) {
      case MSG_START:
      {
        std::thread t(lf_startUp_as_Thread);
        t.detach();
      }
      break;

      case MSG_IOCP_WINSOCK:
        lf_MSG_IOCP_WINSOCK(lParam);
        break;

      case MSG_PSX_WINSOCK:
        lf_MSG_PSX_WINSOCK(lParam);
        break;

      case WM_CLOSE:
        if (l_reading_ssifile.load(std::memory_order_relaxed)) {
          g_gui.line("Cannot close now, wait till result of reading .ssi file ..."s);
        } else {
          l_user_closed.store(true, std::memory_order_relaxed);
          g_log.line(gkEmptyStr);
          GuiBase::userClosedApp();
          lf_decideUponExitStrategy(LksExitPossibility::E_USER_CLOSE);
        }
        break;

      case WM_DESTROY:
        PostQuitMessage(0);
        break;

      case MSG_EXIT:
        lf_MSG_EXIT();
        gf_MSG_EXIT();
        PostQuitMessage(0);
        break;

      case WM_PAINT:
        PAINTSTRUCT ps;
        g_gui.paintWindow(BeginPaint(hWnd, &ps));
        EndPaint(hWnd, &ps);
        break;

      default:
        return DefWindowProc(hWnd, message, wParam, lParam);
      }
      return static_cast<LRESULT>(0);
    }
  }  // namespace unnamed


  auto gf_setPSXTfcExternal(const bool on) -> void {
    (*l_toPSXPtr).writeToBuffer(QcodeValueData(Qcode::Qi201, on ? 1 : 0));  // set TfcMode external or not
  }

  auto gf_sendCDUClearDelayed(const Qcode var) -> void {
    l_CduVar.push_back(var);
  }

  auto gf_isInternalQhvar(const Qcode var) -> bool {
    return (l_internalQhvars.find(var) != l_internalQhvars.end());
  }

  namespace {
    [[nodiscard]] auto lf_cduConfigured() -> bool {
      TxtFileReader cfg;
      if (!cfg.openFile(gkProductNameCfgPath)) {
        return false;
      }
      return cfg.findArgIntAsBool("CDU_L") == gkDigit1Str ||
             cfg.findArgIntAsBool("CDU_C") == gkDigit1Str ||
             cfg.findArgIntAsBool("CDU_R") == gkDigit1Str;
    }

    [[nodiscard]] auto lf_executableDirectory() -> std::filesystem::path {
      std::wstring filename(32768, L'\0');
      const DWORD length = GetModuleFileNameW(nullptr, filename.data(), static_cast<DWORD>(filename.size()));
      if (length == 0 || static_cast<size_t>(length) >= filename.size()) {
        throw std::runtime_error("Cannot determine PSXseecon.exe directory");
      }
      filename.resize(length);
      return std::filesystem::path(filename).parent_path();
    }

    class CduFonts {
    public:
      explicit CduFonts(const std::filesystem::path& directory)
        : small_(directory / L"awnxmcduS_101.TTF"),
          large_(directory / L"awnxmcduL_101.TTF") {
        if (AddFontResourceExW(small_.c_str(), FR_PRIVATE, nullptr) == 0) {
          throw std::runtime_error("Cannot load awnxmcduS_101.TTF next to PSXseecon.exe");
        }
        g_log.line("CDU font loaded: awnxmcduS_101.TTF");

        if (AddFontResourceExW(large_.c_str(), FR_PRIVATE, nullptr) == 0) {
          removeFont(small_, "awnxmcduS_101.TTF");
          throw std::runtime_error("Cannot load awnxmcduL_101.TTF next to PSXseecon.exe");
        }
        g_log.line("CDU font loaded: awnxmcduL_101.TTF");
      }

      CduFonts(const CduFonts&) = delete;
      auto operator=(const CduFonts&) -> CduFonts& = delete;

      ~CduFonts() {
        removeFont(large_, "awnxmcduL_101.TTF");
        removeFont(small_, "awnxmcduS_101.TTF");
      }

    private:
      static auto removeFont(const std::filesystem::path& file, const char* name) -> void {
        if (RemoveFontResourceExW(file.c_str(), FR_PRIVATE, nullptr) != 0) {
          g_log.line(std::format("CDU font removed: {}", name));
        } else {
          g_log.line(std::format("Cannot remove CDU font: {}", name));
        }
      }

      std::filesystem::path small_;
      std::filesystem::path large_;
    };
  }

  extern "C" int32_t APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int32_t nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    gf_openLog();

    std::optional<CduFonts> fonts;
    try {
      if (lf_cduConfigured()) {
        fonts.emplace(lf_executableDirectory());
      }
    } catch (const std::exception& e) {
      g_log.line(e.what());
      MessageBoxA(nullptr, e.what(), "PSXseecon", MB_OK | MB_ICONERROR);
      return 1;
    }

    gf_initReadCfg();
    const auto result = gf_winMain(hInstance, lf_messageHandler);
    fonts.reset();
    g_log.line(gf_UTCDateTime());
    g_log.closeFile();
    return result;
  }
}

