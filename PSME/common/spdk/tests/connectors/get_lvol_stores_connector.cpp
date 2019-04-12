/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_lvol_stores_connector.cpp
 */

#include "get_lvol_stores_connector.hpp"



namespace spdk {

const json::Json GetLvolStoresConnector::LVOL_STORES =
    {
        {
            {"uuid", "a9959197-b5e2-4f2d-8095-251ffb6985a5"},
            {"base_bdev", "NVMe1n1"},
            {"free_clusters", 31},
            {"cluster_size", 4194304},
            {"total_data_clusters", 31},
            {"block_size", 4096},
            {"name", "LVS0"}
        },
        {
            {"uuid", "a9959197-b5e2-4f2d-8095-251ffb6985a6"},
            {"base_bdev", "Malloc"},
            {"free_clusters", 15},
            {"cluster_size", 41941},
            {"total_data_clusters", 39},
            {"block_size", 4000},
            {"name", "LVS1"}
        }

    };
}
