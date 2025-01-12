//
// Created by mecha on 29.09.2021.
//

#pragma once

#include <core/vector.h>
#include <core/primitives.h>
#include <core/core.h>
#include <string>

#define MAX_LENGTH 10
#define SWITCH(str) switch(engine::string::str_hash_for_switch(str))
#define CASE(str) static_assert(engine::string::str_is_correct(str) && (engine::string::str_len(str) <= MAX_LENGTH), "CASE : string is incorrect or length is higher than 10"); \
case engine::string::str_hash(str, engine::string::str_len(str))
// number conversions
#define TO_FLOAT(v) std::stof(v)
#define TO_INT(v) std::stoi(v)
#define TO_DOUBLE(v) std::stod(v)
#define TO_UINT32(v) static_cast<uint32_t>(std::stoul(v))
#define TO_UINT16(v) static_cast<uint16_t>(TO_INT(v)) // undefined behaviour if v is out of uint16_t range!
// string conversions
#define STR_FROM_WCHAR(v) engine::string::from(v)
#define SAME(v1, v2) engine::string::equals(v1, v2)

namespace engine::string {

    typedef unsigned char uchar;
    typedef unsigned long long ullong;

    const ullong N_HASH = static_cast<ullong>(-1);  // analog of std::string::npos

    static std::vector<char*> c_split(const char* str, const char* sep) {
        char* cstr = const_cast<char*>(str);
        char* current;
        std::vector<char*> arr;
        current = strtok(cstr, sep);

        while (current != nullptr){
            arr.emplace_back(current);
            current = strtok(nullptr, sep);
        }

        return arr;
    }

    static std::vector<char*> c_split(const std::string &str, const std::string &sep) {
        return c_split(str.c_str(), sep.c_str());
    }

    static std::vector<std::string> split(const char* str, const char* sep) {
        char* cstr = const_cast<char*>(str);
        char* current;
        std::vector<std::string> arr;
        current = strtok(cstr, sep);

        while (current != nullptr){
            arr.emplace_back(current);
            current = strtok(nullptr, sep);
        }

        return arr;
    }

    static std::vector<std::string> split(const std::string &str, const std::string &sep) {
        return split(str.c_str(), sep.c_str());
    }

    static size_t find(const char* str, const char target) {
        const auto end = strlen(str);

        for (auto i = 0 ; i < end ; i++) {
            if (str[i] == target) {
                return i;
            }
        }

        return 0;
    }

    // works very strange. don't use for now!
    static const char* c_sub_str(const char* str, const char target) {
        const auto str_end = strlen(str);
        size_t target_pos;

        for (auto i = 0 ; i < str_end ; i++) {
            if (str[i] == target) {
                target_pos = i;
                break;
            }
        }

        char* substr = new char[target_pos];

        for (auto i = 0 ; i < target_pos ; i++) {
            substr[i] = str[i];
        }

        return substr;
    }

    // works very strange. don't use for now!
    static const char* c_sub_str(const std::string &str, const char target) {
        return c_sub_str(str.c_str(), target);
    }

    static constexpr unsigned char str_len(const char* const str) {
        return *str ? (1 + str_len(str + 1)) : 0;
    }

    static constexpr ullong raise_128_to(const uchar power) {
        return 0x1ULL << 7*power;
    }

    static constexpr ullong str_hash(const char* str, uchar current_len) {
        return *str ? (raise_128_to(current_len - 1) * static_cast<uchar>(*str)
        + str_hash(str + 1, current_len - 1)) : 0;
    }

    static constexpr bool str_is_correct(const char *const str) {
        return (static_cast<signed char>(*str) > 0) ? str_is_correct(str + 1) : (*str ? false : true);
    }

    static constexpr ullong str_hash_for_switch(const char* const str) {
        return (str_is_correct(str) && (str_len(str) <= MAX_LENGTH)) ? str_hash(str, str_len(str)) : N_HASH;
    }

    static const bool contains(const char* str, const char &ch) {
        return strchr(str, ch) != nullptr;
    }

    static const bool contains(const std::string &str, const char &ch) {
        return str.find(ch) != std::string::npos;
    }

    static const bool equals(const char* str1, const char* str2) {
        return strcmp(str1, str2) == 0;
    }

    static std::string from(const wchar_t* w_chars) {
        std::wstring ws(w_chars);
        return { ws.begin(), ws.end() };
    }

    static bool removePrefix(std::string &target, const std::string &prefix) {
        auto targetPrefix = target.substr(prefix.length());
        if (targetPrefix == prefix) {
            target = targetPrefix;
            return true;
        }
        return false;
    }

    static bool removePrefix(std::string &target, const std::vector<std::string>& prefixes) {
        for (const auto& prefix : prefixes) {
            bool prefixRemoved = removePrefix(target, prefix);
            if (prefixRemoved) {
                return true;
            }
        }
        return false;
    }
}