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

package com.intel.podm.common.types.actions;

import com.intel.podm.common.types.PortMode;

import java.net.URI;
import java.util.Set;

public final class EthernetSwitchPortDefinition {
    private final String name;
    private final String portId;
    private final PortMode portMode;
    private final Set<URI> uris;

    private EthernetSwitchPortDefinition(Builder builder) {
        name = builder.name;
        portId = builder.portId;
        portMode = builder.portMode;
        uris = builder.uris;
    }

    public String getName() {
        return name;
    }

    public String getPortId() {
        return portId;
    }

    public PortMode getPortMode() {
        return portMode;
    }

    public Set<URI> getUris() {
        return uris;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private String name;
        private String portId;
        private PortMode portMode;
        private Set<URI> uris;

        private Builder() {
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder portId(String portId) {
            this.portId = portId;
            return this;
        }

        public Builder portMode(PortMode portMode) {
            this.portMode = portMode;
            return this;
        }

        public Builder uris(Set<URI> uris) {
            this.uris = uris;
            return this;
        }

        public EthernetSwitchPortDefinition build() {
            return new EthernetSwitchPortDefinition(this);
        }
    }
}
