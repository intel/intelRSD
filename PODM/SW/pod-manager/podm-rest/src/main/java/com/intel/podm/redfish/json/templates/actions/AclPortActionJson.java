/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.business.services.redfish.requests.BindPortActionRequest;
import com.intel.podm.business.services.redfish.requests.UnbindPortActionRequest;

import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUri;

public class AclPortActionJson implements BindPortActionRequest, UnbindPortActionRequest {
    private Context portContext;

    @Override
    public Context getPortContext() {
        return portContext;
    }

    @JsonProperty("Port")
    public void setPortContext(ODataId port) {
        if (port == null) {
            return;
        }

        this.portContext = getContextFromUri(port.toUri(), ETHERNET_SWITCH_PORT);
    }
}
