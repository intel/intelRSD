/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file psme/rest/server/connector/connector_options_loader.hpp
 */

#pragma once



#include "connector_options.hpp"



namespace psme {
namespace rest {
namespace server {

/*!
 * Connector options loader
 * @param config JSON configuration
 * @return Collection of connector options read from configuration
 */
ConnectorOptionsVec load_connectors_options(const json::Json& config);

}
}
}
