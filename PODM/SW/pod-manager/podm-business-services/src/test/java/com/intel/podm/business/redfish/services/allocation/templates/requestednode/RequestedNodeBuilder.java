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

package com.intel.podm.business.redfish.services.allocation.templates.requestednode;

import com.intel.podm.business.services.redfish.requests.RequestedNode;

import java.util.ArrayList;
import java.util.List;

import static org.apache.commons.lang3.StringUtils.EMPTY;

public class RequestedNodeBuilder implements RequestedNode {
    private static final String DEFAULT_NAME = "Composed Node";
    private static final String DEFAULT_DESCRIPTION = EMPTY;

    private String name;
    private String description;
    private Integer totalSystemMemoryMib;
    private Integer totalSystemCoreCount;
    private List<Processor> processors;
    private List<Memory> memoryModules;
    private List<RemoteDrive> remoteDrives;
    private List<LocalDrive> localDrives;
    private List<EthernetInterface> ethernetInterfaces;
    private Object getOem;
    private Security security;

    public RequestedNodeBuilder() {
        this.name = DEFAULT_NAME;
        this.description = DEFAULT_DESCRIPTION;
    }

    private RequestedNodeBuilder(Builder builder) {
        this.name = builder.name;
        this.description = builder.description;
        this.totalSystemMemoryMib = builder.totalSystemMemoryMib;
        this.totalSystemCoreCount = builder.totalSystemCoreCount;
        this.processors = builder.processors;
        this.memoryModules = builder.memoryModules;
        this.remoteDrives = builder.remoteDrives;
        this.localDrives = builder.localDrives;
        this.ethernetInterfaces = builder.ethernetInterfaces;
        this.getOem = builder.getOem;
        this.security = builder.security;
    }

    public static Builder newRequestedNode() {
        return new Builder();
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public Integer getTotalSystemMemoryMib() {
        return totalSystemMemoryMib;
    }

    @Override
    public Integer getTotalSystemCoreCount() {
        return totalSystemCoreCount;
    }

    @Override
    public List<Processor> getProcessors() {
        return processors;
    }

    @Override
    public List<Memory> getMemoryModules() {
        return memoryModules;
    }

    @Override
    public List<RemoteDrive> getRemoteDrives() {
        return remoteDrives;
    }

    @Override
    public List<LocalDrive> getLocalDrives() {
        return localDrives;
    }

    @Override
    public List<EthernetInterface> getEthernetInterfaces() {
        return ethernetInterfaces;
    }

    public Security getSecurity() {
        return security;
    }

    @Override
    public Object getOem() {
        return null;
    }

    public static final class Builder {
        private String name = DEFAULT_NAME;
        private String description = DEFAULT_DESCRIPTION;
        private Integer totalSystemMemoryMib;
        private Integer totalSystemCoreCount;
        private List<Processor> processors;
        private List<Memory> memoryModules;
        private List<RemoteDrive> remoteDrives;
        private List<LocalDrive> localDrives;
        private List<EthernetInterface> ethernetInterfaces;
        private Object getOem;
        private Security security;

        private Builder() {
        }

        public RequestedNodeBuilder build() {
            return new RequestedNodeBuilder(this);
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder totalSystemMemoryMib(Integer totalSystemMemoryMib) {
            this.totalSystemMemoryMib = totalSystemMemoryMib;
            return this;
        }

        public Builder totalSystemCoreCount(Integer totalSystemCoreCount) {
            this.totalSystemCoreCount = totalSystemCoreCount;
            return this;
        }

        public Builder addProcessor(Processor processors) {
            if (this.processors == null) {
                this.processors = new ArrayList<>();
            }
            this.processors.add(processors);
            return this;
        }

        public Builder addMemoryModule(Memory memoryModule) {
            if (this.memoryModules == null) {
                this.memoryModules = new ArrayList<>();
            }
            this.memoryModules.add(memoryModule);
            return this;
        }

        public Builder addRemoteDrive(RemoteDrive remoteDrive) {
            if (this.remoteDrives == null) {
                this.remoteDrives = new ArrayList<>();
            }
            this.remoteDrives.add(remoteDrive);
            return this;
        }

        public Builder addLocalDrive(LocalDrive localDrive) {
            if (this.localDrives == null) {
                this.localDrives = new ArrayList<>();
            }
            this.localDrives.add(localDrive);
            return this;
        }

        public Builder addethernetInterface(EthernetInterface ethernetInterface) {
            if (this.ethernetInterfaces == null) {
                this.ethernetInterfaces = new ArrayList<>();
            }
            this.ethernetInterfaces .add(ethernetInterface);
            return this;
        }

        public Builder getOem(Object getOem) {
            this.getOem = getOem;
            return this;
        }

        public Builder security(Security security) {
            this.security = security;
            return this;
        }
    }
}
