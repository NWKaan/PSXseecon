#pragma once

// utf8win.hpp - header-only helpers to "work fully in UTF-8" on Windows.
// - Keep all app strings as UTF-8 (std::string).
// - Convert to wide ONLY at the OS boundary (paths, Win32 APIs).
// - Provide tiny file helpers that accept UTF-8 paths.
// MSVC: compile with /utf-8 so UTF-8 literals in source files are read correctly.

#ifndef NOMINMAX
#define NOMINMAX
#endif

namespace nwk {

// ===== UTF-8 <-> UTF-16 =====
inline auto utf8_to_wide(std::string_view s) -> std::wstring {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.data(), (int)s.size(), nullptr, 0);
    if (n <= 0) throw std::runtime_error("utf8_to_wide: invalid UTF-8");
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.data(), (int)s.size(), w.data(), n);
    return w;
}

inline auto wide_to_utf8(std::wstring_view w) -> std::string {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    if (n <= 0) throw std::runtime_error("wide_to_utf8: invalid UTF-16");
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, w.data(), (int)w.size(), s.data(), n, nullptr, nullptr);
    return s;
}


// ===== Paths (boundary helpers) =====
inline auto path_from_utf8(std::string_view utf8) -> std::filesystem::path {
    return std::filesystem::path{ utf8_to_wide(utf8) };
}
inline auto path_to_utf8(std::filesystem::path const& p) -> std::string 
{
    return wide_to_utf8(p.wstring());
}

// ===== Files (UTF-8 paths, raw bytes content) =====
inline auto write_file(std::string_view utf8_path, std::string_view bytes, bool truncate = true) -> bool {
    auto p = path_from_utf8(utf8_path);
    std::ios::openmode mode = std::ios::binary | std::ios::out;
    if (truncate) mode |= std::ios::trunc; else mode |= std::ios::app;
    std::ofstream out(p, mode);
    if (!out) return false;
    out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    return static_cast<bool>(out);
}

inline auto read_file(std::string_view utf8_path) -> std::string {
    auto p = path_from_utf8(utf8_path);
    std::ifstream in(p, std::ios::binary);
    if (!in) return {};
    in.seekg(0, std::ios::end);
    std::string data;
    data.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0, std::ios::beg);
    if (!data.empty())
        in.read(data.data(), static_cast<std::streamsize>(data.size()));
    return data;
}

inline auto exists(std::string_view utf8_path) -> bool {
    return std::filesystem::exists(path_from_utf8(utf8_path));
}

// ===== Directory listing in UTF-8 =====
inline auto list_dir(std::string_view utf8_root) -> std::vector<std::string> {
    std::vector<std::string> out;
    auto root = path_from_utf8(utf8_root);
    for (auto const& e : std::filesystem::directory_iterator(root)) {
        out.emplace_back(path_to_utf8(e.path()));
    }
    return out;
}

// ===== Console UTF-8 (RAII) =====

class ConsoleUtf8Guard final {  // plus wide streams locale etc..
public:
  ConsoleUtf8Guard() {
    old_out_cp_ = GetConsoleOutputCP();
    old_in_cp_ = GetConsoleCP();

    // Zet console I/O op UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

#pragma warning(push)
#pragma warning(disable:6031)    // reinterpret_cast
    // Zet de CRT-tekstmodi op UTF-8 voor wide I/O (wcout/wcin/wcerr)
    _setmode(_fileno(stdout), _O_U8TEXT);
    _setmode(_fileno(stderr), _O_U8TEXT);
    _setmode(_fileno(stdin), _O_U8TEXT);
#pragma warning(pop)

    // Locale van wide streams (optioneel, maar helpt)
    std::wcout.imbue(std::locale(".UTF-8"));
    std::wcin.imbue(std::locale(".UTF-8"));
    std::wcerr.imbue(std::locale(".UTF-8"));
  }

  ~ConsoleUtf8Guard() noexcept {
    if (old_out_cp_) SetConsoleOutputCP(old_out_cp_);
    if (old_in_cp_)  SetConsoleCP(old_in_cp_);
  }

  ConsoleUtf8Guard(const ConsoleUtf8Guard&) = delete;
  auto operator=(const ConsoleUtf8Guard&) -> ConsoleUtf8Guard& = delete;
  ConsoleUtf8Guard(ConsoleUtf8Guard&&) = delete;
  auto operator=(ConsoleUtf8Guard&&) -> ConsoleUtf8Guard& = delete;

private:
  UINT old_out_cp_{ 0 };
  UINT old_in_cp_{ 0 };
};


} // namespace utf8win
