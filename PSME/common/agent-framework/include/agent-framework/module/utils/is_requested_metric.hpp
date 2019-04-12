/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file is_requested_metric.hpp
 */

#pragma once



namespace agent_framework {
namespace model {

/* forward declaration */
class Metric;

/* forward declaration */
namespace requests {
class GetMetrics;
}

namespace utils {

std::function<bool(const agent_framework::model::Metric&)>
is_requested_metric_filter(const requests::GetMetrics& request);

}
}
}
