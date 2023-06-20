#include "StringUtils.h"
using namespace kaoisoft;

#include <algorithm>
#include <cctype>
#include <regex>

using namespace std;

/**
 * Returns true if the target string contains the specified string.
 */
bool StringUtils::contains(string target, string str) {
	if (target.find(str, 0) != string::npos) {
		return true;
	} else {
		return false;
	}
}

/**
 * Returns true if the target string ends with the specified string.
 */
bool StringUtils::endsWith(string target, string str) {

	// The target string must be at least as long as the specified string
	int targetLength = target.length();
	int strLength = str.length();
	if (targetLength < strLength) {
		return false;
	}

	// Compare the end of the target string to that other string
    return (0 == target.compare(targetLength - strLength, strLength, str));
}

/**
 * Removes leading whitespace from a string.  A modified copy of the string is returned.
 */
std::string StringUtils::ltrim(std::string &str) {
    
    // Find the first non-whitespace character
    size_t i, count;
    for (i = 0, count = str.length(); i < count; i++) {
        if (' ' == str[i] || '\t' == str[i] || '\r' == str[i] || '\n' == str[i]) {
            // Skip this character
            continue;
        } else {
            break;
        }
    }
    return str.substr(i);
}

/**
 * Replaces all occurrences of a character within a string with a different character.
 */
void StringUtils::replaceAll(string target, char findCh, char replaceCh) {
	std::replace(target.begin(), target.end(), findCh, replaceCh);
}

/**
 * Replaces all occurrences of a sub-string within a string with a different sub-string.
 */
void StringUtils::replaceAll(string target, string findStr, string replaceStr) {
    size_t start_pos = 0;
    while((start_pos = target.find(findStr, start_pos)) != std::string::npos) {
        target.replace(start_pos, findStr.length(), replaceStr);
        start_pos += replaceStr.length(); // Handles case where 'to' is a substring of 'from'
    }
}

/**
 * Removes trailing whitespace from a string. A modified copy of the string is returned.
 */
std::string StringUtils::rtrim(std::string &str) {
    
    // Find the last non-whitespace character
    size_t i = str.length() - 1;
    while (true)
    {
        if (' ' == str[i] || '\t' == str[i] || '\r' == str[i] || '\n' == str[i]) {
            // Skip this character
            continue;
        } else {
            break;
        }

        if (0 == i)     // We've reached the beginning of the string
        {
            break;
        }
        else
        {
            i--;
        }
    }
    return str.substr(0, i + 1);
}

/**
 * Splits a string into sub-strings.
 *
 * The delimiter string is not including in any of the sub-strings.
 *
 * @param input The string to be split
 * @param delim The delimiter string
 */
shared_ptr<vector<string>> StringUtils::split(string input, string delim) {

	// Allocate memory for the vector
	shared_ptr<vector<string>> parts = make_shared<vector<string>>();

	// Find all occurrences of the delimiter regular expression
    size_t start = 0;
    size_t index;
    while (string::npos != (index = input.find(delim, start)))
    {
        // Store the sub-string that is in front of the delimiter
        parts->push_back(input.substr(start, index - start));

        // Move past the delimiter
        start = index + delim.length();
    }

    // If there is a final sub-string after the last delimiter, save it
    if (start < input.length())
    {
        parts->push_back(input.substr(start));
    }

    return parts;
}

/**
 * Returns a copy of a string in which all of the characters are lower case.
 */
string StringUtils::toLowerCase(string str) {

	// Create a copy of the string
	string copy = str;

	// Convert the copy's characters
	std::transform(copy.begin(), copy.end(), copy.begin(),
      [](unsigned char c){ return std::tolower(c); });

	// Return the copy
	return copy;
}

/**
 * Returns a copy of a string in which all of the characters are upper case.
 */
string StringUtils::toUpperCase(string str) {

	// Create a copy of the string
	string copy = str;

	// Convert the copy's characters
	std::transform(copy.begin(), copy.end(), copy.begin(),
      [](unsigned char c){ return std::toupper(c); });

	// Return the copy
	return copy;
}

/**
 * Removes leading and trailing whitespace from a string. A modified copy of the string is returned.
 */
string StringUtils::trim(string str) {
	string newStr = ltrim(str);
	return rtrim(newStr);
}
