#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <memory>
#include <vector>

namespace kaoisoft {

	/**
	 * Utility functions for std::strings.
	 */
	class StringUtils {
	public:
		static bool contains(std::string target, std::string str);
		static bool endsWith(std::string target, std::string str);
		static std::string ltrim(std::string &str);
		static void replaceAll(std::string target, char findCh, char replaceCh);
		static void replaceAll(std::string target, std::string findStr, std::string replaceStr);
		static std::string rtrim(std::string &str);
		static std::shared_ptr<std::vector<std::string>> split(std::string input, std::string delim);
		static std::string toLowerCase(std::string str);
		static std::string toUpperCase(std::string str);
		static std::string trim(std::string str);
    };
};

#endif /* STRINGUTILS_H */

