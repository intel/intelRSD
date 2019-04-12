/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * @file enum_builder.hpp
 *
 * @brief Enum builder macros.
 *
 * ENUM macro is used to wrap enums into classes providing additional methods
 * to convert it to/from strings. This may be used like the normal enum keyword.
 * */

#pragma once

#include "agent-framework/exceptions/exception.hpp"

#include <string>
#include <cstring>
#include <stdexcept>
#include <ostream>

/*! Count defines macro arguments */
#define NUM_ARGS_LIST(_1,   _2,  _3,  _4,  _5,  _6,  _7,  _8, \
                  _9,  _10, _11, _12, _13, _14, _15, _16, \
                  _17, _18, _19, _20, _21, _22, _23, _24, \
                  _25, _26, _27, _28, _29, _30, _31, _32, \
                  _33, _34, _35, _36, _37, _38, _39, _40, \
                  _41, _42, _43, _44, _45, _46, _47, _48, \
                  _49, _50, _51, _52, _53, _54, _55, _56, \
                  _57, _58, _59, _60, _61, _62, _63, _64, N, ...) \
        N
#define NUM_ARGS(...) NUM_ARGS_LIST(__VA_ARGS__, \
                 64, 63, 62, 61, 60, 59, 58, 57,\
                 56, 55, 54, 53, 52, 51, 50, 49,\
                 48, 47, 46, 45, 44, 43, 42, 41,\
                 40, 39, 38, 37, 36, 35, 34, 33,\
                 32, 31, 30, 29, 28, 27, 26, 25,\
                 24, 23, 22, 21, 20, 19, 18, 17,\
                 16, 15, 14, 13, 12, 11, 10,  9,\
                  8,  7,  6,  5,  4,  3,  2,  1, 0)

#define EXPAND(X) X
#define APPLY(macro, ...) EXPAND(macro(__VA_ARGS__))

/*! Map macro argument to given expression */
#define MAP(macro, ...) \
    EXPAND( \
        APPLY(MAP_START, NUM_ARGS(__VA_ARGS__)) \
            (macro, __VA_ARGS__))

/*! Macro for each map argument */
#define MAP_START(count) MAP ## count
#define MAP1(m, x)      m(x)
#define MAP2(m, x, ...) m(x) EXPAND(MAP1(m, __VA_ARGS__))
#define MAP3(m, x, ...) m(x) EXPAND(MAP2(m, __VA_ARGS__))
#define MAP4(m, x, ...) m(x) EXPAND(MAP3(m, __VA_ARGS__))
#define MAP5(m, x, ...) m(x) EXPAND(MAP4(m, __VA_ARGS__))
#define MAP6(m, x, ...) m(x) EXPAND(MAP5(m, __VA_ARGS__))
#define MAP7(m, x, ...) m(x) EXPAND(MAP6(m, __VA_ARGS__))
#define MAP8(m, x, ...) m(x) EXPAND(MAP7(m, __VA_ARGS__))
#define MAP9(m, x, ...) m(x) EXPAND(MAP8(m, __VA_ARGS__))
#define MAP10(m, x, ...) m(x) EXPAND(MAP9(m, __VA_ARGS__))
#define MAP11(m, x, ...) m(x) EXPAND(MAP10(m, __VA_ARGS__))
#define MAP12(m, x, ...) m(x) EXPAND(MAP11(m, __VA_ARGS__))
#define MAP13(m, x, ...) m(x) EXPAND(MAP12(m, __VA_ARGS__))
#define MAP14(m, x, ...) m(x) EXPAND(MAP13(m, __VA_ARGS__))
#define MAP15(m, x, ...) m(x) EXPAND(MAP14(m, __VA_ARGS__))
#define MAP16(m, x, ...) m(x) EXPAND(MAP15(m, __VA_ARGS__))
#define MAP17(m, x, ...) m(x) EXPAND(MAP16(m, __VA_ARGS__))
#define MAP18(m, x, ...) m(x) EXPAND(MAP17(m, __VA_ARGS__))
#define MAP19(m, x, ...) m(x) EXPAND(MAP18(m, __VA_ARGS__))
#define MAP20(m, x, ...) m(x) EXPAND(MAP19(m, __VA_ARGS__))
#define MAP21(m, x, ...) m(x) EXPAND(MAP20(m, __VA_ARGS__))
#define MAP22(m, x, ...) m(x) EXPAND(MAP21(m, __VA_ARGS__))
#define MAP23(m, x, ...) m(x) EXPAND(MAP22(m, __VA_ARGS__))
#define MAP24(m, x, ...) m(x) EXPAND(MAP23(m, __VA_ARGS__))
#define MAP25(m, x, ...) m(x) EXPAND(MAP24(m, __VA_ARGS__))
#define MAP26(m, x, ...) m(x) EXPAND(MAP25(m, __VA_ARGS__))
#define MAP27(m, x, ...) m(x) EXPAND(MAP26(m, __VA_ARGS__))
#define MAP28(m, x, ...) m(x) EXPAND(MAP27(m, __VA_ARGS__))
#define MAP29(m, x, ...) m(x) EXPAND(MAP28(m, __VA_ARGS__))
#define MAP30(m, x, ...) m(x) EXPAND(MAP29(m, __VA_ARGS__))
#define MAP31(m, x, ...) m(x) EXPAND(MAP30(m, __VA_ARGS__))
#define MAP32(m, x, ...) m(x) EXPAND(MAP31(m, __VA_ARGS__))
#define MAP33(m, x, ...) m(x) EXPAND(MAP32(m, __VA_ARGS__))
#define MAP34(m, x, ...) m(x) EXPAND(MAP33(m, __VA_ARGS__))
#define MAP35(m, x, ...) m(x) EXPAND(MAP34(m, __VA_ARGS__))
#define MAP36(m, x, ...) m(x) EXPAND(MAP35(m, __VA_ARGS__))
#define MAP37(m, x, ...) m(x) EXPAND(MAP36(m, __VA_ARGS__))
#define MAP38(m, x, ...) m(x) EXPAND(MAP37(m, __VA_ARGS__))
#define MAP39(m, x, ...) m(x) EXPAND(MAP38(m, __VA_ARGS__))
#define MAP40(m, x, ...) m(x) EXPAND(MAP39(m, __VA_ARGS__))
#define MAP41(m, x, ...) m(x) EXPAND(MAP40(m, __VA_ARGS__))
#define MAP42(m, x, ...) m(x) EXPAND(MAP41(m, __VA_ARGS__))
#define MAP43(m, x, ...) m(x) EXPAND(MAP42(m, __VA_ARGS__))
#define MAP44(m, x, ...) m(x) EXPAND(MAP43(m, __VA_ARGS__))
#define MAP45(m, x, ...) m(x) EXPAND(MAP44(m, __VA_ARGS__))
#define MAP46(m, x, ...) m(x) EXPAND(MAP45(m, __VA_ARGS__))
#define MAP47(m, x, ...) m(x) EXPAND(MAP46(m, __VA_ARGS__))
#define MAP48(m, x, ...) m(x) EXPAND(MAP47(m, __VA_ARGS__))
#define MAP49(m, x, ...) m(x) EXPAND(MAP48(m, __VA_ARGS__))
#define MAP50(m, x, ...) m(x) EXPAND(MAP49(m, __VA_ARGS__))
#define MAP51(m, x, ...) m(x) EXPAND(MAP50(m, __VA_ARGS__))
#define MAP52(m, x, ...) m(x) EXPAND(MAP51(m, __VA_ARGS__))
#define MAP53(m, x, ...) m(x) EXPAND(MAP52(m, __VA_ARGS__))
#define MAP54(m, x, ...) m(x) EXPAND(MAP53(m, __VA_ARGS__))
#define MAP55(m, x, ...) m(x) EXPAND(MAP54(m, __VA_ARGS__))
#define MAP56(m, x, ...) m(x) EXPAND(MAP55(m, __VA_ARGS__))
#define MAP57(m, x, ...) m(x) EXPAND(MAP56(m, __VA_ARGS__))
#define MAP58(m, x, ...) m(x) EXPAND(MAP57(m, __VA_ARGS__))
#define MAP59(m, x, ...) m(x) EXPAND(MAP58(m, __VA_ARGS__))
#define MAP60(m, x, ...) m(x) EXPAND(MAP59(m, __VA_ARGS__))
#define MAP61(m, x, ...) m(x) EXPAND(MAP60(m, __VA_ARGS__))
#define MAP62(m, x, ...) m(x) EXPAND(MAP61(m, __VA_ARGS__))
#define MAP63(m, x, ...) m(x) EXPAND(MAP62(m, __VA_ARGS__))
#define MAP64(m, x, ...) m(x) EXPAND(MAP63(m, __VA_ARGS__))

/*! Stringizes each argument */
#define STRINGIZE_SINGLE(e) #e,
#define STRINGIZE(...) EXPAND(MAP(STRINGIZE_SINGLE, __VA_ARGS__))

/*! Definition macro ENUM, smart enum class */
#define ENUM(EnumName, T, ...)                                                \
namespace EnumName ## _data_ns {                                              \
    enum EnumName ## _enum : T { __VA_ARGS__ };                               \
                                                                              \
    constexpr const std::size_t g_size = EXPAND(NUM_ARGS(__VA_ARGS__));       \
                                                                              \
    constexpr const T g_values[] = { __VA_ARGS__ };                           \
    constexpr const char* const g_names[] =                                   \
                            { EXPAND(STRINGIZE(__VA_ARGS__)) };               \
}                                                                             \
                                                                              \
class EnumName {                                                              \
public:                                                                       \
    using underlying_type = T;                                                \
                                                                              \
    enum EnumName ## _enum : T { __VA_ARGS__ };                               \
    using base_enum = EnumName ## _enum;                                      \
                                                                              \
    const char* to_string() const {                                           \
        for (std::size_t index = 0;                                           \
                         index < EnumName ## _data_ns::g_size;                \
                                                     ++index) {               \
            if (EnumName ## _data_ns::g_values[index] == m_value) {           \
                return EnumName ## _data_ns::g_names[index];                  \
            }                                                                 \
        }                                                                     \
        throw std::runtime_error("Invalid enum value: '" +                    \
                    std::to_string(m_value) + "'.");                          \
    }                                                                         \
                                                                              \
    static EnumName from_string(const std::string& str) {                     \
        for (std::size_t index = 0;                                           \
                         index < EnumName ## _data_ns::g_size;                \
                                                     ++index) {               \
            if (!strcmp(EnumName ## _data_ns::g_names[index],                 \
                        str.c_str())) {                                       \
                return                                                        \
                    {static_cast<EnumName ## _enum>(                          \
                        EnumName ## _data_ns::g_values[index])};              \
            }                                                                 \
        }                                                                     \
        THROW(agent_framework::exceptions::InvalidValue, "agent-framework",   \
            std::string(#EnumName" enum value not found: '") + str + "'.");   \
    }                                                                         \
                                                                              \
    /*! Gets all available enum values as a vector of strings */              \
    static std::vector<std::string> get_values() {                            \
        return std::vector<std::string>(EnumName ## _data_ns::g_names,        \
            EnumName ## _data_ns::g_names + EnumName ## _data_ns::g_size);    \
    }                                                                         \
                                                                              \
    static bool is_allowable_value(const std::string& str) {                  \
        for (std::size_t index = 0;                                           \
                         index < EnumName ## _data_ns::g_size;                \
                                                     ++index) {               \
            if (!strcmp(EnumName ## _data_ns::g_names[index],                 \
                        str.c_str())) {                                       \
                return true;                                                  \
            }                                                                 \
        }                                                                     \
        return false;                                                         \
    }                                                                         \
                                                                              \
    EnumName() = delete;                                                      \
                                                                              \
    constexpr EnumName(EnumName ## _enum value) : m_value(value) {}           \
                                                                              \
    constexpr operator EnumName ## _enum() const {                            \
        return static_cast<EnumName ## _enum>(m_value);                       \
    }                                                                         \
                                                                              \
    friend std::ostream&                                                      \
    operator<<(std::ostream& out_stream, const EnumName& in_enum) {           \
        out_stream << in_enum.to_string();                                    \
        return out_stream;                                                    \
    }                                                                         \
                                                                              \
private:                                                                      \
    T m_value{};                                                              \
}
