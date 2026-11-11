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

    auto lf_knownFolderPath(REFKNOWNFOLDERID id) -> std::optional<std::filesystem::path> {
      PWSTR raw = nullptr;
      if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &raw))) {
        std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> mem(raw, CoTaskMemFree);
        return std::filesystem::path(mem.get()); // wide -> path (geen ACP)
      }
      return std::nullopt;
    }

    auto lf_RegisterClass(HINSTANCE hInstance, WNDPROC message_handler, WCHAR const* szTitle) noexcept -> std::expected<ATOM, DWORD> {
     WNDCLASSEX w{
     .cbSize = sizeof(WNDCLASSEX),
     .style = gsl::narrow_cast<UINT>(CS_HREDRAW | CS_VREDRAW),
     .lpfnWndProc = message_handler,
     .cbClsExtra = 0,
     .cbWndExtra = 0,
     .hInstance = hInstance,
     .hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROGRAM)),
     .hCursor = LoadCursor(nullptr, IDC_ARROW),
     //     .hbrBackground = (HBRUSH)(COLOR_GRAYTEXT + 1),  
     .hbrBackground = CreateSolidBrush(RGB(0, 0, 0)),
     .lpszMenuName = nullptr,
     .lpszClassName = szTitle,  // take app title as window class name (easy)
     .hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROGRAM)) };

     if (ATOM result{ RegisterClassEx(&w) }; result != 0) {
       return result;
     } 
     return std::unexpected{ GetLastError() };
   }
  } // namespace unnamed

  auto gf_myProgramFilesPath() -> std::filesystem::path {
    if (auto p = lf_knownFolderPath(FOLDERID_ProgramFiles)) return *p;
    return {};
  }

  auto gf_myProgramFilesX86Path() -> std::filesystem::path {
    if (auto p = lf_knownFolderPath(FOLDERID_ProgramFilesX86)) return *p;
    return {};
  }

  auto gf_postWindowsMessage(const UINT m) noexcept -> void {
    if (g_gui.getMainWindow()) {
      constexpr WPARAM wp{ 0 };
      constexpr LPARAM lp{ 0 };
      PostMessage(g_gui.getMainWindow(), m, wp, lp);
    }
  }

  auto gf_MSG_EXIT() -> void {
    g_gui.saveWindowPos();
  }

  auto gf_winMain(HINSTANCE hInstance, WNDPROC message_handler) noexcept -> int32_t {
   constexpr size_t MAX_LOADSTRING = 100;
   std::array<WCHAR, MAX_LOADSTRING> szTitle{};

   // De titel laden in de array
   if (LoadStringW(hInstance, IDS_APP_TITLE, szTitle.data(), gsl::narrow_cast<int>(MAX_LOADSTRING)) == 0) {
     g_log.line("Failed to load application title"s);
     return gsl::narrow_cast<int32_t>(FALSE);
   }

   // Registratie van het venster
   if (const auto regResult{ lf_RegisterClass(hInstance, message_handler, szTitle.data()) }; !regResult) {
     g_log.line("Failed to register window class"s);
     return gsl::narrow_cast<int32_t>(FALSE);
   }

   // Initialisatie van de GUI-instantie
   if (!g_gui.initInstance(hInstance, szTitle.data())) {
     g_log.line("initInstance failed"s);
     return gsl::narrow_cast<int32_t>(FALSE);
   }

   // Controleer en maak de directory als deze niet bestaat
   try {
     if (!std::filesystem::exists(g_ProductName)) {
       std::filesystem::create_directories(g_ProductName);
     }
   } catch (const std::filesystem::filesystem_error& e) {
     g_log.line(std::format("Failed to create directory: {}", e.what()));
     return gsl::narrow_cast<int32_t>(FALSE);
   }

   gf_postWindowsMessage(MSG_START);

   MSG msg;
   while (GetMessage(&msg, nullptr, 0, 0)) {
     TranslateMessage(&msg);
     DispatchMessage(&msg);
   }

   return gsl::narrow_cast<int32_t>(msg.wParam);
 }
}
