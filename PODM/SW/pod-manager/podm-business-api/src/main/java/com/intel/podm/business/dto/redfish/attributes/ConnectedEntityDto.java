/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.business.dto.redfish.attributes;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.EntityRole;
import com.intel.podm.common.types.EntityType;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.Set;

public final class ConnectedEntityDto {
    private final EntityType entityType;
    private final EntityRole entityRole;
    private final Integer pciFunctionNumber;
    private final String pciClassCode;
    private final PciIdDto entityPciId;
    private final Set<IdentifierDto> identifiers;
    private final Context entityLink;

    private ConnectedEntityDto(Builder builder) {
        this.entityType = builder.entityType;
        this.entityRole = builder.entityRole;
        this.pciFunctionNumber = builder.pciFunctionNumber;
        this.pciClassCode = builder.pciClassCode;
        this.entityPciId = builder.entityPciId;
        this.identifiers = builder.identifiers;
        this.entityLink = builder.entityLink;
    }

    public EntityType getEntityType() {
        return entityType;
    }

    public EntityRole getEntityRole() {
        return entityRole;
    }

    public PciIdDto getEntityPciId() {
        return entityPciId;
    }

    public String getPciClassCode() {
        return pciClassCode;
    }

    public Integer getPciFunctionNumber() {
        return pciFunctionNumber;
    }

    public Set<IdentifierDto> getIdentifiers() {
        return identifiers;
    }

    public Context getEntityLink() {
        return entityLink;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Links implements RedfishResource.Links {
    }

    public static final class Builder {
        private EntityType entityType;
        private EntityRole entityRole;
        private Integer pciFunctionNumber;
        private String pciClassCode;
        private PciIdDto entityPciId;
        private Set<IdentifierDto> identifiers;
        private Context entityLink;

        private Builder() {
        }

        public ConnectedEntityDto build() {
            return new ConnectedEntityDto(this);
        }

        public Builder entityType(EntityType entityType) {
            this.entityType = entityType;
            return this;
        }

        public Builder entityRole(EntityRole entityRole) {
            this.entityRole = entityRole;
            return this;
        }

        public Builder entityLink(Context entityLink) {
            this.entityLink = entityLink;
            return this;
        }

        public Builder pciFunctionNumber(Integer pciFunctionNumber) {
            this.pciFunctionNumber = pciFunctionNumber;
            return this;
        }

        public Builder pciClassCode(String pciClassCode) {
            this.pciClassCode = pciClassCode;
            return this;
        }

        public Builder entityPciId(PciIdDto entityPciId) {
            this.entityPciId = entityPciId;
            return this;
        }

        public Builder identifiers(Set<IdentifierDto> identifiers) {
            this.identifiers = identifiers;
            return this;
        }
    }
}
