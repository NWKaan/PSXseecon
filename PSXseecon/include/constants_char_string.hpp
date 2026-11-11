#pragma once
//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

namespace nwk {
	inline constexpr char gkQuestionmarkChar{ '?' };
	inline constexpr char gkStarChar{ '*' };
	inline constexpr char gkEqualChar{ '=' };
	inline constexpr char gkHTabChar{ '\t' };
	inline constexpr char gkSpaceChar{ ' ' };
	inline constexpr char gkDotChar{ '.' };
	inline constexpr char gkSemiColonChar{ ';' };
	inline constexpr char gkDashChar{ '-' };
	inline constexpr char gkUnderscoreChar{ '_' };
	inline constexpr char gkSqBrackOpenChar{ '[' };
	inline constexpr char gkSqBrackCloseChar{ ']' };
	inline constexpr char gkAccoladeOpenChar{ '{' };
	inline constexpr char gkAccoladeCloseChar{ '}' };
	inline constexpr char gkUpArrowChar{ '^' };
	inline constexpr char gkDownArrowChar{ 'v' };
	inline constexpr char gkSlashChar{ '/' };
	inline constexpr char gkBackSlashChar{ '\\' };
	inline constexpr char gkPlusChar{ '+' };
	inline constexpr char gQuoteChar{ '\'' };
	inline constexpr char gkCommaChar{ ',' };
	inline constexpr char gkColonChar{ ':' };
	inline constexpr char gkDQuoteChar{ '\"' };
	inline constexpr char gkAmpersandChar{ '&' };
	inline constexpr char gkLtChar{ '<' };
	inline constexpr char gkGtChar{ '>' };
	inline constexpr char gkLfChar = '\n';
	inline constexpr char gkBracketOpenChar = '(';
	inline constexpr char gkBracketCloseChar = ')';
	inline constexpr char gkPercentageChar{ '%' };
	inline constexpr char gkNumSignChar{ '#' };
	inline constexpr char gkVerticalBarChar{ '|' };
	inline constexpr char gkSingleQuoteChar{ '\'' };
	inline constexpr char gkDoubleQuoteChar{ '"' };
	inline constexpr char gkAtSignChar{ '@' };
	inline constexpr char gkNullTerminationChar{ '\0' };

	inline constexpr char gkAChar{ 'A' }, gkBChar{ 'B' }, gkCChar{ 'C' }, gkDChar{ 'D' }, gkEChar{ 'E' }, gkFChar{ 'F' }, gkGChar{ 'G' }, gkHChar{ 'H' }, gkIChar{ 'I' }, gkJChar{ 'J' },
		gkKChar{ 'K' }, gkLChar{ 'L' }, gkMChar{ 'M' }, gkNChar{ 'N' }, gkOChar{ 'O' }, gkPChar{ 'P' }, gkQChar{ 'Q' }, gkRChar{ 'R' }, gkSChar{ 'S' }, gkTChar{ 'T' }, gkUChar{ 'U' }, gkVChar{ 'V' },
		gkWChar{ 'W' }, gkXChar{ 'X' }, gkYChar{ 'Y' }, gkZChar{ 'Z' },
		gkaChar{ 'a' }, gkbChar{ 'b' }, gkcChar{ 'c' }, gkdChar{ 'd' }, gkeChar{ 'e' }, gkgChar{ 'g' }, gkhChar{ 'h' }, gklChar{ 'l' }, gkmChar{ 'm' }, gknChar{ 'n' }, gkoChar{ 'o' }, gkiChar{ 'i' },
		gkrChar{ 'r' }, gksChar{ 's' }, gktChar{ 't' }, gkuChar{ 'u' }, gkvChar{ 'v' }, gkwChar{ 'w' }, gkxChar{ 'x' }, gkyChar{ 'y' }, gkzChar{ 'z' },
		gk0Char{ '0' }, gk1Char{ '1' }, gk2Char{ '2' }, gk3Char{ '3' }, gk4Char{ '4' }, gk5Char{ '5' }, gk6Char{ '6' }, gk7Char{ '7' }, gk8Char{ '8' }, gk9Char{ '9' };

#pragma warning(push)
#pragma warning(disable:26426)
	const std::string gkEqualStr{ gkEqualChar };
	const std::string gkSpaceStr{ gkSpaceChar };
	const std::string gkColonStr{ gkColonChar };
	const std::string gkCommaStr{ gkCommaChar };
	const std::string gkNewLineStr{ "\n" };
	const std::string gkQuestionmarkStr{ gkQuestionmarkChar };
	const std::string gkEmptyStr{ "" };
	const std::string gkDoubleQuoteStr{ gkDoubleQuoteChar };

	const std::string gkAStr{ gkAChar };
	const std::string gkEStr{ gkeChar };
	const std::string gkHStr{ gkHChar };
	const std::string gkKStr{ gkKChar };
	const std::string gkMStr{ gkMChar };
	const std::string gkOStr{ gkOChar };
	const std::string gkRStr{ gkRChar };

	const std::string gksStr{ gksChar };

	const std::string gkDigit0Str{ gk0Char };
	const std::string gkDigit1Str{ gk1Char };
	const std::string gkAtSignStr{ gkAtSignChar };

	inline constexpr auto gkCrLfStr = "\r\n";

	inline constexpr auto gkThreeDotsStr{ " ..." };
#pragma warning(pop)

}


