/*!
 * @brief Stub Client Connector.
 *
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
 * @file get_bdevs_connector.cpp
 */

#include "get_bdevs_connector.hpp"



namespace {

const json::Json NVME_DISK_BDEV = {
    {"name",                 "NVMe1n1"},
    {"product_name",         "NVMe disk"},
    {"block_size",           51205},
    {"num_blocks",           781422768},
    {"aliases",              std::vector<std::string>{}},
    {"claimed",              true},
    {"driver_specific",      {
                                 {"nvme",           {
                                                        {"trid", {
                                                                     {"trtype", "PCIe"},
                                                                     {"traddr", "0000:43:00.0"}
                                                                 }
                                                        },
                                                        {"ns_data", {
                                                                        {"id", 1}
                                                                    }
                                                        },
                                                        {"pci_address", "0000:43:00.0"},
                                                        {"vs", {
                                                                   {"nvme_version", "1.0"}
                                                               }
                                                        },
                                                        {"ctrlr_data", {
                                                                           {"firmware_revision", "version1509"},
                                                                           {"serial_number", "BTSPJ1310"},
                                                                           {"oacs", {
                                                                                        {"ns_manage", 0},
                                                                                        {"security", 0},
                                                                                        {"firmware", 1},
                                                                                        {"format", 1}
                                                                                    }
                                                                           },
                                                                           {"vendor_id", "1234"},
                                                                           {"model_number", "MN 0987"}
                                                                       }
                                                        },
                                                        {"csts", {
                                                                     {"rdy", 1},
                                                                     {"cfs", 0}
                                                                 }
                                                        }
                                                    }
                                 }
                             }
    },
    {
     "assigned_rate_limits", {
                                 {"rw_ios_per_sec", 20000},
                                 {"rw_mbytes_per_sec", 100},
                                 {"r_mbytes_per_sec", 50},
                                 {"w_mbytes_per_sec", 50}
                             }

    },
    {
     "supported_io_types",   {
                                 {"read", true},
                                 {"write", true},
                                 {"unmap", true},
                                 {"write_zeroes", true},
                                 {"flush", true},
                                 {"reset", true},
                                 {"nvme_admin", true},
                                 {"nvme_io", true}
                             }
    },
    {"qos_ios_per_sec",      10}
};

const json::Json MALLOC_BDEV = {
    {"name",               "Malloc"},
    {"product_name",       "Malloc Disk"},
    {"block_size",         512},
    {"num_blocks",         20480},
    {"aliases",            std::vector<std::string>{}},
    {"claimed",            false},
    {"driver_specific",    json::Json::object()},
    {"supported_io_types", {
                                 {"read", true},
                                 {"write", true},
                                 {"unmap", true},
                                 {"write_zeroes", true},
                                 {"flush", true},
                                 {"reset", true},
                                 {"nvme_admin", false},
                                 {"nvme_io", false}
                             }
    },
    {
     "assigned_rate_limits", {
                                 {"rw_ios_per_sec", 0},
                                 {"rw_mbytes_per_sec", 0},
                                 {"r_mbytes_per_sec", 0},
                                 {"w_mbytes_per_sec", 0}
                             }

    },
    {"qos_ios_per_sec",      5}
};

const json::Json LVOL_BDEV = {
    {"name",               "76f0d58f-4c03-4d97-9176-cb33057b6f57"},
    {"product_name",       "Logical Volume"},
    {"block_size",         500},
    {"num_blocks",         2501},
    {"uuid",               "76f0d58f-4c03-4d97-9176-cb33057b6f57"},
    {"aliases",            {"LVS1/76f0d58f-4c03-4d97-9176-cb33057b6f57"}},
    {"claimed",            false},
    {"driver_specific",    {
                            {
                                "lvol",  {
                                             {"base_bdev", "NVMe1n1"},
                                             {"clone", false},
                                             {"snapshot", false},
                                             {"lvol_store_uuid", "413f21dc-0246-4d63-94e2-25e2161d69dc"},
                                             {"thin_provision", false}
                                         }
                            }
                           }},
    {"supported_io_types", {
                            {   "reset", true},
                               {"nvme_admin", false},
                               {"unmap", true},
                               {"read", true},
                               {"write_zeroes", true},
                               {"write", true},
                               {"flush", false},
                               {"nvme_io", false}
                           }
    },
    {"qos_ios_per_sec",    1}
};

const json::Json SNAPSHOT_LVOL_BDEV = json::Json::parse(R"({
    "num_blocks": 3014656,
    "uuid": "4f04cb62-8280-404c-a0ef-2383be8d81db",
    "aliases": [ "LVS1/MySnapshot" ],
    "claimed": false,
    "driver_specific": {
      "lvol": {
        "thin_provision": false,
        "clone": false,
        "lvol_store_uuid": "0a86d76c-1f49-4616-b3e0-c36b1761ed28",
        "base_bdev": "NVMe1n1",
        "snapshot": true,
        "clones": [
          "Clone-1"
        ]
      }
    },
    "supported_io_types": {
      "reset": true,
      "nvme_admin": false,
      "unmap": false,
      "read": true,
      "write_zeroes": false,
      "write": false,
      "flush": false,
      "nvme_io": false
    },
    "assigned_rate_limits": {
        "rw_ios_per_sec": 20000,
        "rw_mbytes_per_sec": 100,
        "r_mbytes_per_sec": 50,
        "w_mbytes_per_sec": 50
    },
    "qos_ios_per_sec": 0,
    "block_size": 4096,
    "product_name": "Logical Volume",
    "name": "4f04cb62-8280-404c-a0ef-2383be8d81db"
})");

const json::Json CLONED_SNAPSHOT_BDEV = json::Json::parse(R"({
    "num_blocks": 3014656,
    "uuid": "f8110801-9df4-4ea7-84cb-82a30e4ed56a",
    "aliases": [ "LVS1/Clone-1" ],
    "claimed": false,
    "driver_specific": {
        "lvol": {
            "thin_provision": true,
            "clone": true,
            "base_snapshot": "MySnapshot",
            "lvol_store_uuid": "0a86d76c-1f49-4616-b3e0-c36b1761ed28",
            "base_bdev": "NVMe1n1",
            "snapshot": false
        }
    },
    "supported_io_types": {
        "reset": true,
        "nvme_admin": false,
        "unmap": true,
        "read": true,
        "write_zeroes": true,
        "write": true,
        "flush": false,
        "nvme_io": false
    },
    "assigned_rate_limits": {
        "rw_ios_per_sec": 20000,
        "rw_mbytes_per_sec": 100,
        "r_mbytes_per_sec": 50,
        "w_mbytes_per_sec": 50
    },
    "qos_ios_per_sec": 0,
    "block_size": 4096,
    "product_name": "Logical Volume",
    "name": "f8110801-9df4-4ea7-84cb-82a30e4ed56a"
})");

}

namespace spdk {

const json::Json GetBdevsConnector::BDEVS = {
    ::NVME_DISK_BDEV,
    ::MALLOC_BDEV,
    ::LVOL_BDEV,
    ::CLONED_SNAPSHOT_BDEV,
    ::SNAPSHOT_LVOL_BDEV
};

}
