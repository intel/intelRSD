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
 * */

#include "psme/rest/server/mux/matchers.hpp"

#include <algorithm>

namespace psme {
namespace rest {
namespace server {
namespace mux {

std::vector<std::string> split_path(const std::string& path) {
    static constexpr const char PATH_SEPARATOR = '/';
    std::vector<std::string> segments{};

    if (path.empty()) {
        return segments;
    }

    auto path_cbegin = path.cbegin();
    auto path_cend = path.cend();
    if (PATH_SEPARATOR == *path_cbegin) {
        ++path_cbegin;
    }

    std::string segment{};
    segment.reserve(path.length());

    std::for_each(path_cbegin, path_cend,
        [&segment, &segments] (const char& ch) {
            if (PATH_SEPARATOR == ch) {
                segments.emplace_back(segment);
                segment.clear();
            }
            else {
                segment.push_back(ch);
            }
        }
    );

    /* Bound end check */
    if (segment.size() > 0) {
       segments.emplace_back(segment);
    }

    return segments;
}

SegmentMatcherPtr path_segment_to_matcher(const std::string& path_segment) {
    if (path_segment.empty()) {
        return std::make_shared<EmptyMatcher>();
    }
    if (path_segment[0] == '{' &&
            path_segment[path_segment.length() - 1] == '}') {

        std::string trimmed_segment(path_segment.substr(1, path_segment.length() - 2));
        auto colon_index = trimmed_segment.find(':');
        if (colon_index == std::string::npos) {
            return std::make_shared<VariableMatcher>(trimmed_segment);
        }
        return std::make_shared<RegexMatcher>(
                trimmed_segment.substr(0, colon_index),
                trimmed_segment.substr(colon_index + 1, std::string::npos));
    }
    return std::make_shared<StaticMatcher>(path_segment);
}

SegmentsVec path_to_segments(const std::string& path) {
    SegmentsVec segments{};

    for (const auto& chunk : split_path(path)) {
        segments.emplace_back(mux::path_segment_to_matcher(chunk));
    }

    return segments;
}

bool segments_match(const mux::SegmentsVec& segments, const std::vector<std::string>& request_segments) {
    const auto s_size = segments.size();
    if (s_size != request_segments.size()) {
        return false;
    }
    for (auto i = mux::SegmentsVec::size_type(0); i < s_size; ++i) {
        if (!segments[i]->check_match(request_segments[i])) {
            return false;
        }
    }
    return true;
}

}
}
}
}
