/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.redfish.json.templates.assembly;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.types.ReplicationMethod;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import static com.intel.podm.business.services.context.ContextType.LOGICAL_DRIVE;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUri;

public final class RequestedMasterDriveImpl implements RequestedNode.RemoteDrive.MasterDrive {
    @JsonProperty
    private ReplicationMethod type;

    private Context logicalDriveContext;

    @Override
    public ReplicationMethod getType() {
        return type;
    }

    @JsonProperty("Resource")
    public void setResourceContext(ODataId resource) {
        if (resource == null) {
            return;
        }

        logicalDriveContext = getContextFromUri(resource.toUri(), LOGICAL_DRIVE);
    }

    @Override
    public Context getResourceContext() {
        return logicalDriveContext;
    }
}
