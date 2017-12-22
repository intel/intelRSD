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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.IgnoreUnlinkingRelationship;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;

import javax.persistence.Column;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import java.net.URI;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.EAGER;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = ExternalLink.GET_EXTERNAL_LINK_BY_SERVICE_AND_ENTITY,
        query = "SELECT el FROM ExternalLink el WHERE el.externalService = :externalService AND el.discoverableEntity = :discoverableEntity"
    )
})
@Table(name = "external_link")
public class ExternalLink extends Entity {

    public static final String GET_EXTERNAL_LINK_BY_SERVICE_AND_ENTITY = "GET_EXTERNAL_LINK_BY_SERVICE_AND_ENTITY";

    @Column(name = "uri")
    private URI sourceUri;

    @IgnoreUnlinkingRelationship
    @ManyToOne(fetch = EAGER, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "external_service_id")
    private ExternalService externalService;

    @ManyToOne(fetch = EAGER, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "discoverable_entity_id")
    private DiscoverableEntity discoverableEntity;

    public ExternalLink() {
    }

    public ExternalService getExternalService() {
        return externalService;
    }

    public void setExternalService(ExternalService externalService) {
        if (!Objects.equals(this.externalService, externalService)) {
            unlinkExternalService(this.externalService);
            this.externalService = externalService;
            if (externalService != null && !externalService.getOwnedLinks().contains(this)) {
                externalService.addOwnedLink(this);
            }
        }
    }

    public void unlinkExternalService(ExternalService externalService) {
        if (Objects.equals(this.externalService, externalService)) {
            this.externalService = null;
            if (externalService != null) {
                externalService.unlinkOwnedLink(this);
            }
        }
    }

    public URI getSourceUri() {
        return sourceUri;
    }

    public void setSourceUri(URI sourceUri) {
        this.sourceUri = sourceUri;
    }

    public DiscoverableEntity getDiscoverableEntity() {
        return discoverableEntity;
    }

    public void setDiscoverableEntity(DiscoverableEntity discoverableEntity) {
        if (!Objects.equals(this.discoverableEntity, discoverableEntity)) {
            unlinkDiscoverableEntity(this.discoverableEntity);
            this.discoverableEntity = discoverableEntity;
            if (discoverableEntity != null && !discoverableEntity.getExternalLinks().contains(this)) {
                discoverableEntity.addExternalLink(this);
            }
        }
    }

    public void unlinkDiscoverableEntity(DiscoverableEntity discoverableEntity) {
        if (Objects.equals(this.discoverableEntity, discoverableEntity)) {
            this.discoverableEntity = null;
            if (discoverableEntity != null) {
                discoverableEntity.unlinkExternalLink(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkExternalService(externalService);
        unlinkDiscoverableEntity(discoverableEntity);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
