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

package com.intel.podm.templates.requestednode;

import com.intel.podm.business.dto.redfish.RequestedEthernetInterface;
import com.intel.podm.business.dto.redfish.RequestedLocalDrive;
import com.intel.podm.business.dto.redfish.RequestedMemory;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.dto.redfish.RequestedProcessor;
import com.intel.podm.business.dto.redfish.RequestedRemoteDrive;

import java.util.List;

import static java.util.Collections.emptyList;
import static org.apache.commons.lang.StringUtils.EMPTY;

public class TestRequestedNode implements RequestedNode {
    private static final String DEFAULT_NAME = "Composed Node";
    private static final String DEFAULT_DESCRIPTION = EMPTY;

    @Override
    public String getName() {
        return DEFAULT_NAME;
    }

    @Override
    public String getDescription() {
        return DEFAULT_DESCRIPTION;
    }

    @Override
    public List<RequestedProcessor> getProcessors() {
        return emptyList();
    }

    @Override
    public List<RequestedMemory> getMemoryModules() {
        return emptyList();
    }

    @Override
    public List<RequestedRemoteDrive> getRemoteDrives() {
        return emptyList();
    }

    @Override
    public List<RequestedLocalDrive> getLocalDrives() {
        return emptyList();
    }

    @Override
    public List<RequestedEthernetInterface> getEthernetInterfaces() {
        return emptyList();
    }

    @Override
    public Object getOem() {
        return null;
    }
}
