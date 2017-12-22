/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @section DESCRIPTION
 * */

#pragma once
static constexpr const char NETWORK_FULL_CONFIGURATION[] = R"({
    "agent": {
        "vendor" : "Intel Corporation",
        "capabilities" : [ "Network" ]
    },
    "server": {
        "port": 7779
    },
    "registration": {
        "ipv4": "localhost",
        "port": 8383,
        "interval": 3
    },
    "managers": [
        {
            "ipv4": "127.0.0.1",
            "serialConsoleEnabled": true,
            "switches": [
                {
                    "mgmt_port": "p1p1",
                    "sysfs_driver": "virtio_net",
                    "ports" : [
                        { "id": "sw0p1", "portType": "Downstream" },
                        { "id": "sw0p2", "portType": "Downstream" },
                        { "id": "sw0p3", "portType": "Downstream" },
                        { "id": "sw0p4", "portType": "Downstream" },
                        { "id": "sw0p37", "linkTechnology": "PCIe", "portType": "Downstream" },
                        { "id": "sw0p38", "linkTechnology": "PCIe", "portType": "Downstream" }
                    ]
                }
            ]
        }
    ],
    "service-uuid-file" : "/var/opt/psme/network-service-uuid.json",
    "logger" : {
        "agent" : {
            "level" : "WARNING",
            "timeformat" : "DATE_NS",
            "color" : true,
            "output" : true,
            "tagging" : true,
            "moredebug" : false,
            "streams" : [
                {
                    "type" : "STDOUT"
                }
            ]
        }
    }
})";



