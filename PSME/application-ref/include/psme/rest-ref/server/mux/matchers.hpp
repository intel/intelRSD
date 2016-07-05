/*
 * Copyright (C) 2014 MediaSift Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#ifndef PSME_REST_SERVER_MATCHERS_HPP
#define PSME_REST_SERVER_MATCHERS_HPP

#include "psme/rest-ref/server/mux/segment_matcher.hpp"
#include "psme/rest-ref/server/mux/empty_matcher.hpp"
#include "psme/rest-ref/server/mux/static_matcher.hpp"
#include "psme/rest-ref/server/mux/regex_matcher.hpp"
#include "psme/rest-ref/server/mux/variable_matcher.hpp"

#include <string>
#include <vector>

namespace psme {
namespace rest {
namespace server {
namespace mux {

/*!
 * @brief Splits path into vector of segments separated by '/'.
 * @param path Path to split
 * @return Vector of segments resulting from splitting path.
 */
std::vector<std::string> split_path(const std::string& path);

/*!
 * @brief Compiles a segment of path into a matchable segment object.
 * @param path_segment the segment of path to compile
 * @return a segment matcher compiled from the path segment
 */
SegmentMatcherPtr path_segment_to_matcher(const std::string& path_segment);

/*!
 * @brief Splits and compiles a path into segments.
 *
 * A request path is compiled into segments
 * in order to accomodate for various path matching
 * strategies, such as static, variable and regex.
 * The compiled path is returned as a list of
 * segments.
 *
 * @param path the endpoint path to be compiled
 * @return the compiled list of path segments for matching
 */
SegmentsVec path_to_segments(const std::string& path);

}
}
}
}

#endif // PSME_REST_SERVER_MATCHERS_HPP
