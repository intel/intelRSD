/*
 * Copyright (c) 2018 Intel Corporation
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

import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.ApplicationProtocolType;
import com.intel.podm.business.entities.redfish.embeddables.BandwidthMapping;
import com.intel.podm.business.entities.redfish.embeddables.PriorityClassMapping;
import com.intel.podm.common.types.Id;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "dcbx_shared_configuration", indexes = @Index(name = "idx_dcbx_shared_configuration_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
public class DcbxConfig extends Entity {

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @ElementCollection
    @CollectionTable(name = "dcbx_shared_configuration_application_protocol", joinColumns = @JoinColumn(name = "application_protocol_id"))
    @OrderColumn(name = "application_protocol_order")
    private List<ApplicationProtocolType> applicationProtocol = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "dcbx_shared_configuration_priority_to_priority_group_mapping",
        joinColumns = @JoinColumn(name = "priority_to_priority_group_mapping_id"))
    @OrderColumn(name = "priority_to_priority_group_mapping_order")
    private List<PriorityClassMapping> priorityToPriorityGroupMapping = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "dcbx_shared_configuration_bandwidth_allocation", joinColumns = @JoinColumn(name = "bandwidth_allocation_id"))
    @OrderColumn(name = "bandwidth_allocation_order")
    private List<BandwidthMapping> bandwidthAllocation = new ArrayList<>();

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_switch_id")
    private EthernetSwitch ethernetSwitch;

    public Id getEntityId() {
        return entityId;
    }

    public void setEntityId(Id entityId) {
        this.entityId = entityId;
    }

    public List<ApplicationProtocolType> getApplicationProtocol() {
        return applicationProtocol;
    }

    public void addApplicationProtocol(ApplicationProtocolType applicationProtocolType) {
        this.applicationProtocol.add(applicationProtocolType);
    }

    public List<PriorityClassMapping> getPriorityToPriorityGroupMapping() {
        return priorityToPriorityGroupMapping;
    }

    public void addPriorityGroupToPriorityMapping(PriorityClassMapping priorityClassMapping) {
        this.priorityToPriorityGroupMapping.add(priorityClassMapping);
    }

    public List<BandwidthMapping> getBandwidthAllocation() {
        return bandwidthAllocation;
    }

    public void addBandwidthAllocation(BandwidthMapping bandwidthMapping) {
        this.bandwidthAllocation.add(bandwidthMapping);
    }

    public EthernetSwitch getEthernetSwitch() {
        return ethernetSwitch;
    }

    public void setEthernetSwitch(EthernetSwitch ethernetSwitch) {
        if (!Objects.equals(this.ethernetSwitch, ethernetSwitch)) {
            unlinkEthernetSwitch(this.ethernetSwitch);
            this.ethernetSwitch = ethernetSwitch;
            if (ethernetSwitch != null && !this.equals(ethernetSwitch.getDcbxSharedConfiguration())) {
                ethernetSwitch.setDcbxSharedConfiguration(this);
            }
        }
    }

    public void unlinkEthernetSwitch(EthernetSwitch ethernetSwitch) {
        if (Objects.equals(this.ethernetSwitch, ethernetSwitch)) {
            this.ethernetSwitch = null;
            if (ethernetSwitch != null) {
                ethernetSwitch.unlinkDcbxSharedConfiguration(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkEthernetSwitch(ethernetSwitch);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, ethernetSwitch);
    }
}
