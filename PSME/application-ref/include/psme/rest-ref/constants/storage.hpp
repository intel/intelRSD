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

#pragma once
namespace app {
namespace rest {
namespace constants {

namespace LogicalDrive {
    extern const char TYPE[];
    extern const char MODE[];
    extern const char PROTECTED[];
    extern const char SNAPSHOT[];
    extern const char CAPACITY[];
    extern const char IMAGE[];
    extern const char BOOTABLE[];
    extern const char LOGICAL_DRIVES[];
    extern const char PHYSICAL_DRIVES[];
    extern const char MASTER_DRIVE[];
    extern const char TARGETS[];
    extern const char ACTIONS[];
    extern const char CLONE[];
    extern const char TARGET[];
}

namespace PhysicalDrive {
}

namespace StorageService {
    extern const char TARGETS[];
    extern const char REMOTE_TARGETS[];
    extern const char LOGICAL_DRIVES[];
    extern const char DRIVES[];
}

namespace RemoteTarget {
    extern const char TYPE[];
    extern const char ADDRESSES[];
    extern const char ISCSI[];
    extern const char TARGET_LUN[];
    extern const char LUN[];
    extern const char LOGICAL_DRIVE[];
    extern const char TARGET_IQN[];
    extern const char TARGET_PORTAL_IP[];
    extern const char TARGET_PORTAL_PORT[];
    extern const char INITIATOR[];
    extern const char INITIATOR_IQN[];
    extern const char LOGICAL_DRIVES[];
}

}
}
}

