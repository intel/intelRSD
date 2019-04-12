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

#pragma once
#include "psme/rest/server/parameters.hpp"

#include <string>
#include <memory>
#include <vector>

namespace psme {
namespace rest {
namespace server {
namespace mux {

/*!
 * @brief A representation of a segment of a handler path for matching requests.
 */
class SegmentMatcher {
public:
    virtual ~SegmentMatcher();

	/*!
	 * @brief Checks whether the path segment matches this segment matcher.
	 * @param path_segment the path segment to check matching against.
	 * @return true is the path segment matches, false otherwise.
	 */
	virtual bool check_match(const std::string& path_segment) = 0;

	/*!
	 * @brief Appends any parameters extracted from the path segment to a list of params.
	 *
	 * This is used to propagate any REST parameters.
	 *
	 * @param params the list of parameters to append to
	 * @param path_segment the segment of path the variable should be extracted from
	 */
	virtual void get_param(Parameters& params, const std::string& path_segment) = 0;
};

using SegmentMatcherPtr = std::shared_ptr<SegmentMatcher>;
using SegmentsVec = std::vector<SegmentMatcherPtr>;

}
}
}
}

