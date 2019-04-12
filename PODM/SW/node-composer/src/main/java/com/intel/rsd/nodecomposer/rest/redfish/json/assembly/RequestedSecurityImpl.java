/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.redfish.json.assembly;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.types.InterfaceType;

import java.io.Serializable;

public class RequestedSecurityImpl implements RequestedNode.Security, Serializable {
    private static final long serialVersionUID = -7175995162179875163L;

    @JsonProperty
    private Boolean tpmPresent;

    @JsonProperty
    private InterfaceType tpmInterfaceType;

    @JsonProperty
    private Boolean txtEnabled;

    @JsonProperty("ClearTPMOnDelete")
    private Boolean clearTpmOnDelete;

    @JsonProperty("ClearOptaneDCPersistentMemoryOnDelete")
    private Boolean clearOptaneMemoryOnDelete;

    @Override
    public Boolean getTpmPresent() {
        if (tpmInterfaceType != null && tpmPresent == null) {
            return true;
        }
        return tpmPresent;
    }

    @Override
    public InterfaceType getTpmInterfaceType() {
        return tpmInterfaceType;
    }

    @Override
    public Boolean getTxtEnabled() {
        return txtEnabled;
    }

    @Override
    public Boolean getClearTpmOnDelete() {
        return clearTpmOnDelete;
    }

    @Override
    public Boolean getClearOptaneMemoryOnDelete() {
        return clearOptaneMemoryOnDelete;
    }
}
