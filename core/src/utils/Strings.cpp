#include "utils/Strings.h"

#include <algorithm>
#include <cctype>

std::string ConvertStringToLowerCase(const std::string& string)
{
    std::string lowerCaseString = string;
    std::transform(
        lowerCaseString.begin(),
        lowerCaseString.end(),
        lowerCaseString.begin(),
        [](unsigned char character) {
            return std::tolower(character);
        }
    );

    return lowerCaseString;
}
