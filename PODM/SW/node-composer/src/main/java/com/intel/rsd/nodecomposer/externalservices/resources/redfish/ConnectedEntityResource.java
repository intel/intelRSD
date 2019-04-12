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

package com.intel.rsd.nodecomposer.externalservices.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.LinkName;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceLinks;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import com.intel.rsd.nodecomposer.types.EntityRole;
import lombok.Getter;
import lombok.Setter;

import java.net.URI;
import java.util.Set;

@OdataTypes({
    "Endpoint\\.ConnectedEntity"
})
public class ConnectedEntityResource implements ExternalServiceResource {
    @Setter
    @JsonIgnore
    private WebClient webClient;

    @Getter
    @Setter
    @JsonIgnore
    private URI uri;

    @Getter
    @JsonProperty("EntityRole")
    private EntityRole entityRole;

    @Getter
    @JsonProperty("Identifiers")
    private Set<IdentifierObject> identifiers;

    @JsonProperty("EntityLink")
    private ODataId entityLink;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    @LinkName("resourceInConnectedEntity")
    public ResourceSupplier getEntityLink() {
        if (entityLink == null) {
            return null;
        }
        return toSupplier(webClient, entityLink);
    }

    public Integer getLunFromOem() {
        return oem.rackScaleOem.lun;
    }

    @Override
    public ResourceLinks getLinks() {
        return new ResourceLinks(this);
    }

    public class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("LUN")
            private Integer lun;
        }
    }
}
