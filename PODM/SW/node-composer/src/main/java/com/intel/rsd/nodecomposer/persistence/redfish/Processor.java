/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.intel.rsd.nodecomposer.persistence.IgnoreUnlinkingRelationship;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.AttachableType;
import com.intel.rsd.nodecomposer.types.InstructionSet;
import com.intel.rsd.nodecomposer.types.ProcessorBrand;
import com.intel.rsd.nodecomposer.types.ProcessorType;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.types.AttachableType.PROCESSOR;
import static java.util.stream.Collectors.toSet;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.CascadeType.REMOVE;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "processor")
@SuppressWarnings({"checkstyle:MethodCount"})
public class Processor extends DiscoverableEntity implements AttachableAsset, ComposableAsset {
    @Getter
    @Setter
    @Column(name = "processor_type")
    @Enumerated(STRING)
    private ProcessorType processorType;

    @Getter
    @Setter
    @Column(name = "instruction_set")
    @Enumerated(STRING)
    private InstructionSet instructionSet;

    @Getter
    @Setter
    @Column(name = "model")
    private String model;

    @Getter
    @Setter
    @Column(name = "max_speed_mhz")
    private Integer maxSpeedMhz;

    @Getter
    @Setter
    @Column(name = "total_cores")
    private Integer totalCores;

    @Getter
    @Setter
    @Column(name = "brand")
    @Enumerated(STRING)
    private ProcessorBrand brand;

    @Column(name = "achievable")
    private Boolean achievable = true;

    @Getter
    @Setter
    @Column(name = "processor_erased")
    private Boolean processorErased;

    @ElementCollection
    @CollectionTable(name = "processor_capability", joinColumns = @JoinColumn(name = "processor_id"))
    @Column(name = "capability")
    @OrderColumn(name = "capability_order")
    private List<String> capabilities = new ArrayList<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "composed_node_id")
    private ComposedNode composedNode;

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST, REMOVE})
    @JoinColumn(name = "processor_metadata_id")
    private ProcessorMetadata metadata = new ProcessorMetadata();

    public List<String> getCapabilities() {
        return capabilities;
    }

    public void addCapability(String capability) {
        this.capabilities.add(capability);
    }

    public ComputerSystem getComputerSystem() {
        return computerSystem;
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        if (!Objects.equals(this.computerSystem, computerSystem)) {
            unlinkComputerSystem(this.computerSystem);
            this.computerSystem = computerSystem;
            if (computerSystem != null && computerSystem.getProcessors().contains(this)) {
                computerSystem.addProcessor(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkProcessor(this);
            }
        }
    }

    @Override
    public Set<Endpoint> getEndpoints() {
        return getEntityConnections().stream().map(ConnectedEntity::getEndpoint).collect(toSet());
    }

    public ComposedNode getComposedNode() {
        return composedNode;
    }

    public void setComposedNode(ComposedNode composedNode) {
        if (!Objects.equals(this.composedNode, composedNode)) {
            unlinkComposedNode(this.composedNode);
            this.composedNode = composedNode;
            if (composedNode != null && !composedNode.getProcessors().contains(this)) {
                composedNode.unlinkProcessor(this);
            }
        }
    }

    public void unlinkComposedNode(ComposedNode composedNode) {
        if (Objects.equals(this.composedNode, composedNode)) {
            this.composedNode = null;
            if (composedNode != null) {
                composedNode.unlinkProcessor(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkComputerSystem(computerSystem);
        unlinkComposedNode(composedNode);
    }

    @Override
    public AttachableType getAttachableType() {
        return PROCESSOR;
    }

    @Override
    public boolean isAllocated() {
        return metadata.isAllocated();
    }

    @Override
    public void setAllocated(boolean value) {
        metadata.setAllocated(value);
    }

    @Override
    public boolean isAchievable() {
        return achievable;
    }

    @Override
    public void setAchievable(boolean achievable) {
        this.achievable = achievable;
    }
}
