/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.types.actions;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;

public final class EthernetSwitchPortRedefinition {
    private final ODataId primaryVlan;

    private EthernetSwitchPortRedefinition(Builder builder) {
        this.primaryVlan = builder.primaryVlan;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public ODataId getPrimaryVlan() {
        return primaryVlan;
    }

    public static final class Builder {
        private ODataId primaryVlan;

        private Builder() {
        }

        public Builder primaryVlan(ODataId primaryVlanUri) {
            this.primaryVlan = primaryVlanUri;
            return this;
        }

        public EthernetSwitchPortRedefinition build() {
            return new EthernetSwitchPortRedefinition(this);
        }
    }
}
