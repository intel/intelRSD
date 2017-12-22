/*!
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file jsonptr/src/jsonptr.cpp
 */

#include "jsonptr/jsonptr.hpp"
#include <json/value.hpp>
#include <algorithm>

using namespace jsonptr;


json::Value& jsonptr::find(const json::Value& json, const std::string& jsonptr) {
    JsonPtr ptr(jsonptr);
    return ptr.find(json);
}


void jsonptr::set_at(json::Value& json, const std::string& jsonptr, const json::Value& value) {
    JsonPtr ptr(jsonptr);
    ptr.set_at(json, value);
}


const char JsonPtr::PTR_SEPARATOR = '/';
const char JsonPtr::ESCAPED_SEPARATOR[] = "~1";
const char JsonPtr::TILDE = '~';
const char JsonPtr::ESCAPED_TILDE[] = "~0";



JsonPtr::JsonPtr(const std::string& jsonptr) : m_segments{} {

    // verify if JsonPtr is properly escaped:
    for (size_t pos = jsonptr.find_first_of(TILDE); pos != std::string::npos;
                pos = jsonptr.find_first_of(TILDE, pos + 1)) {
        if (pos  == jsonptr.size()-1 || (jsonptr[pos+1] != '0' && jsonptr[pos+1] != '1')) {
            throw InvalidPointerError(std::string("The escape character '") + TILDE + "' can only be used in sequences '"
                                      + ESCAPED_TILDE + "' and '" + ESCAPED_SEPARATOR + "'.");
        }
    }

    if (jsonptr.empty()) {
        return;
    }

    if (jsonptr.front() != PTR_SEPARATOR) {
        throw InvalidPointerError("JSON pointer should contain no text before the first '/'.");
    }

    std::string segment{};
    auto ptr_cbegin = jsonptr.cbegin() + 1; // begin after separator '/'
    auto ptr_cend = jsonptr.cend();

    std::for_each(ptr_cbegin, ptr_cend,
                  [&segment, this] (const char& ch) {
                      if (PTR_SEPARATOR == ch) {
                          unescape(segment);
                          this->m_segments.emplace_back(segment);
                          segment.clear();
                      }
                      else {
                          segment.push_back(ch);
                      }
                  }
    );
    // insert segment with final chars:
    unescape(segment);
    m_segments.emplace_back(segment);
}


void JsonPtr::replace_all_occurrences(std::string& source, const std::string& old, const std::string& replacement) {
    for(size_t pos = source.find(old); pos != std::string::npos;) {
        source.replace(pos, old.size(), replacement);
        pos += replacement.size();
        pos = source.find(old, pos+1);
    }
}


void JsonPtr::escape(std::string& segment) {
    replace_all_occurrences(segment, std::string{TILDE}, ESCAPED_TILDE);
    replace_all_occurrences(segment, std::string{PTR_SEPARATOR}, ESCAPED_SEPARATOR);
}


void JsonPtr::unescape(std::string& escaped_segment) {
    replace_all_occurrences(escaped_segment, ESCAPED_TILDE, std::string{TILDE});
    replace_all_occurrences(escaped_segment, ESCAPED_SEPARATOR, std::string{PTR_SEPARATOR});
}


unsigned long JsonPtr::get_index(const std::string& segment) {
    if (segment == "-") {
        throw InvalidPointerError("Pointing at the end of an array is not supported.");
    }
    if (!std::all_of(segment.begin(), segment.end(), ::isdigit)) {
        throw InvalidPointerError("Index in JSON array can only contain digits.");
    }

    unsigned long index;
    size_t idx;
    try {
        index = std::stoul(segment, &idx);
        if (idx != segment.size()) {
            throw InvalidPointerError("invalid index in JSON array.");
        }
    }
    catch (std::invalid_argument&) {
        throw InvalidPointerError("invalid index in JSON array.");
    }
    catch (std::out_of_range&) {
        throw InvalidPointerError("JSON array index out of range.");
    }

    if (segment.size() > 1 and segment.front() == '0') {
        throw InvalidPointerError("No leading zeros are allowed in an index array.");
    }

    if (static_cast<size_t>(index) != index) {
        throw InvalidPointerError("JSON array index out of range.");
    }

    return index;
}


json::Value& JsonPtr::find(const json::Value& json) {
    if (m_segments.empty()) {
        // return whole json document
        return const_cast<json::Value&>(json);
    }

    const json::Value* result = &json;
    for (const auto& segment : m_segments) {
        if (result->is_array()) {
            auto index = get_index(segment);
            if (index < result->size()) {
                result = &(*result)[static_cast<size_t>(index)];
            }
            else {
                throw NonexistentValueError("JSON array does not contain element at index " + segment + ".");
            }
        }
        else if (result->is_object()) {
            if (result->is_member(segment)) {
                result = &(*result)[segment];
            }
            else {
                throw NonexistentValueError("Did not find key " + segment + " in JSON object.");
            }
        }
        else { // other JSON types
            throw NonexistentValueError("Could not access key " + segment + " because JSON value is not an object.");
        }
    }
    return const_cast<json::Value&>(*result);
}


void JsonPtr::set_at(json::Value& json, const json::Value& value) {
    find(json) = value;
}


InvalidPointerError::InvalidPointerError(const std::string& what_arg)
    : std::runtime_error("Invalid pointer: " + what_arg) {}


InvalidPointerError::~InvalidPointerError() {}


NonexistentValueError::NonexistentValueError(const std::string& what_arg)
    : std::runtime_error("Nonexistent value in JSON: " + what_arg) {}


NonexistentValueError::~NonexistentValueError() {}
