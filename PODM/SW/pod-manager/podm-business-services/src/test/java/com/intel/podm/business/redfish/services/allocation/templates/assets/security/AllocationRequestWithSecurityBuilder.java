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

package com.intel.podm.business.redfish.services.allocation.templates.assets.security;

import com.intel.podm.business.services.redfish.requests.RequestedNode.Security;
import com.intel.podm.common.types.InterfaceType;

public final class AllocationRequestWithSecurityBuilder {
    private Boolean tpmPresent;
    private Boolean txtEnabled;
    private InterfaceType interfaceType;

    private AllocationRequestWithSecurityBuilder() {
    }

    public static AllocationRequestWithSecurityBuilder requestedNodeSecurityBuilder() {
        return new AllocationRequestWithSecurityBuilder();
    }

    public AllocationRequestWithSecurityBuilder tpmPresent(Boolean tpmPresent) {
        this.tpmPresent = tpmPresent;
        return this;
    }

    public AllocationRequestWithSecurityBuilder txtEnabled(Boolean txtEnabled) {
        this.txtEnabled = txtEnabled;
        return this;
    }

    public AllocationRequestWithSecurityBuilder interfaceType(InterfaceType interfaceType) {
        this.interfaceType = interfaceType;
        return this;
    }

    @SuppressWarnings({"checkstyle:AnonInnerLength"})
    public Security build() {
        return new Security() {
            @Override
            public Boolean getTpmPresent() {
                return tpmPresent;
            }

            @Override
            public InterfaceType getTpmInterfaceType() {
                return interfaceType;
            }

            @Override
            public Boolean getTxtEnabled() {
                return txtEnabled;
            }
        };
    }
}
