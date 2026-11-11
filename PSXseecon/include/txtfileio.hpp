#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
	class TxtFileReader {
	public:
		TxtFileReader() = default;
		~TxtFileReader() noexcept;  // implicitly closes the file
		TxtFileReader(TxtFileReader const& v) = delete;
		auto operator=(TxtFileReader const& v) -> TxtFileReader& = delete;
		TxtFileReader(TxtFileReader && v) = delete;
		auto operator=(TxtFileReader && v) -> TxtFileReader& = delete;

		auto openFile(std::filesystem::path const& path_name) -> bool;  // true: it will search in 'MyDocuments' folder
		auto closeFile() -> void;
		auto getLine(std::string& s) -> bool;
		auto toStringList(std::list<std::string>& li) -> void;    // converts the complete file to a list of strings
		auto toStringList(std::forward_list<std::string>& li) -> void;    // converts the complete file to a list of strings
		auto toStringVector(std::vector<std::string>& sv) -> void;  // idem to vector

		auto findArgString(const std::string& argument) -> std::string;
		// start from begin of file and find find a line -starting from the beginning of the line- 
		// with std::string "s" followed by equal sign; if so it returns the std::string at the rest of the line

		auto findArgNumber(const std::string& argument) -> std::string;
		// start from begin of file and find a line beginning with std::string "s" followed by equal sign; 
		// if so it returns the number at the rest of the line

		auto findArgIntAsBool(std::string const& argument) -> std::string;

		auto findArgIPaddress(const std::string& argument) -> std::string;
		// start from begin of file and find a line beginning with std::string "s" followed by equal sign; 
		// if so it returns the IP address at the rest of the line 

	private:
		auto gotoBeginOfFile() -> void;
		auto findArg(const std::regex& r) -> std::string;

		std::ifstream file_;
	};

	class TxtFileWriter {
	public:
		TxtFileWriter() = default;
		virtual ~TxtFileWriter() noexcept;   // implicitly closes the file
		TxtFileWriter(const TxtFileWriter& v) = delete;
		auto operator=(const TxtFileWriter& v) -> TxtFileWriter& = delete;
		TxtFileWriter(TxtFileWriter&& v) = delete;
		auto operator=(TxtFileWriter&& v) -> TxtFileWriter& = delete;

		auto openFile(std::filesystem::path const& path_name) -> bool;
		auto closeFile() -> void;

		auto writeString(std::string_view s) -> void;       // writes (just) the std::string to the file (note and NOT adding end of line)
		auto writeStringList(const std::list<std::string>& li) -> void;   // writes a list of strings, adding end of line to each std::string

		auto line(std::string_view s) -> void;  // thread safe
		auto newLine() -> void;

	protected:
		std::mutex mutex_;
		std::ofstream file_;
	};
}
