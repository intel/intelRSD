/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 * */

#ifndef AGENT_CHASSIS_TEST_LOADER_HPP
#define AGENT_CHASSIS_TEST_LOADER_HPP

static constexpr const char CHASSIS_FULL_CONFIGURATION[] = R"({
    "agent": {
        "vendor": "Intel Corporation",
        "capabilities" : ["Chassis"]
    },
    "server": {
        "port": 7780
    },
    "registration": {
        "ipv4": "localhost",
        "port": 8383,
        "interval": 3
    },
    "managers": [
         {
         "slot" : 1,
         "chassis" : {
             "size": 4,
             "locationOffset": 1,
             "networkInterface": "enp0s20f0.4094",
             "parentId" : 1,
             "platform" : "BDCR",
             "type": "Drawer"
         },
         "serialConsoleEnabled": true,
         "managers" : [
             {
                 "ipv4" : "1.1.2.1",
                 "username" : "put_username_hash_here",
                 "password" : "put_password_hash_here",
                 "port" : 623,
                 "slot" : 1,
                 "serialConsoleEnabled": true
             },
             {
                 "ipv4" : "1.1.2.2",
                 "username" : "put_username_hash_here",
                 "password" : "put_password_hash_here",
                 "port" : 623,
                 "slot" : 2,
                 "serialConsoleEnabled": true
             },
             {
                 "ipv4" : "1.1.2.3",
                 "username" : "put_username_hash_here",
                 "password" : "put_password_hash_here",
                 "port" : 623,
                 "slot" : 3,
                 "serialConsoleEnabled": true
             },
             {
                 "ipv4" : "1.1.2.4",
                 "username" : "put_username_hash_here",
                 "password" : "put_password_hash_here",
                 "port" : 623,
                 "slot" : 4,
                 "serialConsoleEnabled": true
             }
         ]
       }
    ],
    "logger" : {
        "agent" : {
            "level" : "INFO",
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



#endif //AGENT_COMPUTE_TEST_LOADER_HPP
