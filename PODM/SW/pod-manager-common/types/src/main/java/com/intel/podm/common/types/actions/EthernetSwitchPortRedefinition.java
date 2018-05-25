/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.intel.podm.common.types.AdministrativeState;
import com.intel.podm.common.types.DcbxState;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort.PriorityFlowControl;

import java.net.URI;
import java.util.Set;

public final class EthernetSwitchPortRedefinition {
    private final AdministrativeState administrativeState;
    private final Integer linkSpeed;
    private final Integer frameSize;
    private final Boolean autosense;
    private final URI primaryVlan;
    private final Set<URI> uris;
    private final DcbxState dcbxState;
    private final Boolean lldpEnabled;
    private final PriorityFlowControl priorityFlowControl;

    private EthernetSwitchPortRedefinition(Builder builder) {
        this.administrativeState = builder.administrativeState;
        this.linkSpeed = builder.linkSpeed;
        this.frameSize = builder.frameSize;
        this.autosense = builder.autosense;
        this.primaryVlan = builder.primaryVlan;
        this.uris = builder.uris;
        this.dcbxState = builder.dcbxState;
        this.lldpEnabled = builder.lldpEnabled;
        this.priorityFlowControl = builder.priorityFlowControl;
    }

    public AdministrativeState getAdministrativeState() {
        return administrativeState;
    }

    public Integer getLinkSpeed() {
        return linkSpeed;
    }

    public Integer getFrameSize() {
        return frameSize;
    }

    public Boolean getAutosense() {
        return autosense;
    }

    public URI getPrimaryVlan() {
        return primaryVlan;
    }

    public Set<URI> getUris() {
        return uris;
    }

    public DcbxState getDcbxState() {
        return dcbxState;
    }

    public Boolean getLldpEnabled() {
        return lldpEnabled;
    }

    public PriorityFlowControl getPriorityFlowControl() {
        return priorityFlowControl;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private AdministrativeState administrativeState;
        private Integer linkSpeed;
        private Integer frameSize;
        private Boolean autosense;
        private URI primaryVlan;
        private Set<URI> uris;
        private DcbxState dcbxState;
        private Boolean lldpEnabled;
        private PriorityFlowControl priorityFlowControl;

        private Builder() {
        }

        public Builder administrativeState(AdministrativeState administrativeState) {
            this.administrativeState = administrativeState;
            return this;
        }

        public Builder linkSpeedMbps(Integer linkSpeed) {
            this.linkSpeed = linkSpeed;
            return this;
        }

        public Builder frameSize(Integer frameSize) {
            this.frameSize = frameSize;
            return this;
        }

        public Builder autosense(Boolean autosense) {
            this.autosense = autosense;
            return this;
        }

        public Builder primaryVlan(URI primaryVlanUri) {
            this.primaryVlan = primaryVlanUri;
            return this;
        }

        public Builder uris(Set<URI> uris) {
            this.uris = uris;
            return this;
        }

        public Builder dcbxState(DcbxState dcbxState) {
            this.dcbxState = dcbxState;
            return this;
        }

        public Builder lldpEnabled(Boolean lldpEnabled) {
            this.lldpEnabled = lldpEnabled;
            return this;
        }

        public Builder priorityFlowControl(PriorityFlowControl priorityFlowControl) {
            this.priorityFlowControl = priorityFlowControl;
            return this;
        }

        public EthernetSwitchPortRedefinition build() {
            return new EthernetSwitchPortRedefinition(this);
        }
    }

}
