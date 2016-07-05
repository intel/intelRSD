/*
 * Copyright (c) 2015 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.business.dto.redfish;

import java.util.List;

/**
 * Interface providing requirements for a Composed Node. Contains all data necessary
 * to perform allocation process of a new Composed Node.
 */
public interface RequestedNode {
    String getName();
    String getDescription();
    List<RequestedProcessor> getProcessors();
    List<RequestedMemory> getMemoryModules();
    List<RequestedRemoteDrive> getRemoteDrives();
    List<RequestedLocalDrive> getLocalDrives();
    List<RequestedEthernetInterface> getEthernetInterfaces();
    Object getOem();
}
