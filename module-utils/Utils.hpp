#pragma once
#include "i18/i18.hpp"
#include <algorithm> // std::find_if_not
#include <log/log.hpp>
#include <sstream>

namespace utils
{
    static const std::string WHITESPACE = " \n\r\t\f\v";

    template <typename Out> void split(const std::string &s, char delim, Out result)
    {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim))
        {
            *(result++) = item;
        }
    }

    static inline std::vector<std::string> split(const std::string &s, char delim)
    {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }

    static inline bool is_number(const std::string &s)
    {
        return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
    }

    static inline std::string ltrim(const std::string &s)
    {
        size_t start = s.find_first_not_of(WHITESPACE);
        return (start == std::string::npos) ? "" : s.substr(start);
    }

    static inline std::string rtrim(const std::string &s)
    {
        size_t end = s.find_last_not_of(WHITESPACE);
        return (end == std::string::npos) ? "" : s.substr(0, end + 1);
    }

    static inline std::string trim(const std::string &s)
    {
        return rtrim(ltrim(s));
    }

    template <typename T> std::string to_string(T Number)
    {
        std::ostringstream ss;
        ss << Number;
        return ss.str();
    }
} // namespace utils
