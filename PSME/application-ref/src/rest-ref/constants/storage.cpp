
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

#include "psme/rest-ref/constants/storage.hpp"

namespace app {
namespace rest {
namespace constants {

namespace LogicalDrive {
     const char TYPE[] = "Type";
     const char MODE[] = "Mode";
     const char PROTECTED[] = "Protected";
     const char SNAPSHOT[] = "Snapshot";
     const char CAPACITY[] = "CapacityGiB";
     const char IMAGE[] = "Image";
     const char BOOTABLE[] = "Bootable";
     const char LOGICAL_DRIVES[] = "LogicalDrives";
     const char PHYSICAL_DRIVES[] = "PhysicalDrives";
     const char MASTER_DRIVE[] = "MasterDrive";
     const char TARGETS[] = "Targets";
     const char CLONE[] = "Clone";
     const char TARGET[] = "target";
}

namespace PhysicalDrive {
}

namespace StorageService {
     const char TARGETS[] = "Targets";
     const char REMOTE_TARGETS[] = "RemoteTargets";
     const char LOGICAL_DRIVES[] = "LogicalDrives";
     const char DRIVES[] = "Drives";
}

namespace RemoteTarget {
     const char TYPE[] = "Type";
     const char ADDRESSES[] = "Addresses";
     const char ISCSI[] = "iSCSI";
     const char TARGET_LUN[] = "TargetLUN";
     const char LUN[] = "LUN";
     const char LOGICAL_DRIVE[] = "LogicalDrive";
     const char TARGET_IQN[] = "TargetIQN";
     const char TARGET_PORTAL_IP[] = "TargetPortalIP";
     const char TARGET_PORTAL_PORT[] = "TargetPortalPort";
     const char INITIATOR[] = "Initiator";
     const char INITIATOR_IQN[] = "InitiatorIQN";
     const char LOGICAL_DRIVES[] = "LogicalDrives";
}

}
}
}

