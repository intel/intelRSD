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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.MultiSourceResource;
import com.intel.podm.business.entities.redfish.embeddables.Fpga;
import com.intel.podm.business.entities.redfish.embeddables.OnPackageMemory;
import com.intel.podm.business.entities.redfish.embeddables.ProcessorId;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embedded;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "processor", indexes = @Index(name = "idx_processor_entity_id", columnList = "entity_id", unique = true))
@NamedQueries({
    @NamedQuery(name = Processor.GET_PROCESSOR_MULTI_SOURCE,
        query = "SELECT processor "
            + "FROM Processor processor "
            + "WHERE processor.multiSourceDiscriminator = :multiSourceDiscriminator "
            + "AND processor.isComplementary = :isComplementary"
    )
})
@Eventable
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class Processor extends DiscoverableEntity implements MultiSourceResource {
    public static final String GET_PROCESSOR_MULTI_SOURCE = "GET_PROCESSOR_MULTI_SOURCE";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "socket")
    private String socket;

    @Column(name = "processor_type")
    @Enumerated(STRING)
    private ProcessorType processorType;

    @Column(name = "processor_architecture")
    @Enumerated(STRING)
    private ProcessorArchitecture processorArchitecture;

    @Column(name = "instruction_set")
    @Enumerated(STRING)
    private InstructionSet instructionSet;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "model")
    private String model;

    @Column(name = "max_speed_mhz")
    private Integer maxSpeedMhz;

    @Column(name = "total_cores")
    private Integer totalCores;

    @Column(name = "total_threads")
    private Integer totalThreads;

    @Column(name = "brand")
    @Enumerated(STRING)
    private ProcessorBrand brand;

    @Column(name = "multi_source_discriminator")
    private String multiSourceDiscriminator;

    @Column(name = "thermal_design_power_watt")
    private BigDecimal thermalDesignPowerWatt;

    @Column(name = "extended_identification_registers")
    private String extendedIdentificationRegisters;

    @Embedded
    private ProcessorId processorId;

    @Embedded
    private Fpga fpga;

    @ElementCollection
    @CollectionTable(name = "processor_capability", joinColumns = @JoinColumn(name = "processor_id"))
    @Column(name = "capability")
    @OrderColumn(name = "capability_order")
    private List<String> capabilities = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "processor_on_package_memory", joinColumns = @JoinColumn(name = "processor_id"))
    @OrderColumn(name = "on_package_memory_order")
    private List<OnPackageMemory> onPackageMemory = new ArrayList<>();

    @OneToMany(mappedBy = "processor", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @OneToOne(mappedBy = "processor", fetch = LAZY, cascade = {MERGE, PERSIST})
    private ProcessorMetrics processorMetrics;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public String getSocket() {
        return socket;
    }

    public void setSocket(String socket) {
        this.socket = socket;
    }

    public ProcessorType getProcessorType() {
        return processorType;
    }

    public void setProcessorType(ProcessorType processorType) {
        this.processorType = processorType;
    }

    public ProcessorArchitecture getProcessorArchitecture() {
        return processorArchitecture;
    }

    public void setProcessorArchitecture(ProcessorArchitecture processorArchitecture) {
        this.processorArchitecture = processorArchitecture;
    }

    public InstructionSet getInstructionSet() {
        return instructionSet;
    }

    public void setInstructionSet(InstructionSet instructionSet) {
        this.instructionSet = instructionSet;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public Integer getMaxSpeedMhz() {
        return maxSpeedMhz;
    }

    public void setMaxSpeedMhz(Integer maxSpeedMhz) {
        this.maxSpeedMhz = maxSpeedMhz;
    }

    public Integer getTotalCores() {
        return totalCores;
    }

    public void setTotalCores(Integer totalCores) {
        this.totalCores = totalCores;
    }

    public Integer getTotalThreads() {
        return totalThreads;
    }

    public void setTotalThreads(Integer totalThreads) {
        this.totalThreads = totalThreads;
    }

    public ProcessorBrand getBrand() {
        return brand;
    }

    public void setBrand(ProcessorBrand brand) {
        this.brand = brand;
    }

    @Override
    public String getMultiSourceDiscriminator() {
        return multiSourceDiscriminator;
    }

    @Override
    public void setMultiSourceDiscriminator(String multiSourceDiscriminator) {
        this.multiSourceDiscriminator = multiSourceDiscriminator;
    }

    public BigDecimal getThermalDesignPowerWatt() {
        return thermalDesignPowerWatt;
    }

    public void setThermalDesignPowerWatt(BigDecimal thermalDesignPowerWatt) {
        this.thermalDesignPowerWatt = thermalDesignPowerWatt;
    }

    public String getExtendedIdentificationRegisters() {
        return extendedIdentificationRegisters;
    }

    public void setExtendedIdentificationRegisters(String extendedIdentificationRegisters) {
        this.extendedIdentificationRegisters = extendedIdentificationRegisters;
    }

    public ProcessorId getProcessorId() {
        return processorId;
    }

    public void setProcessorId(ProcessorId processorId) {
        this.processorId = processorId;
    }

    public Fpga getFpga() {
        return fpga;
    }

    public void setFpga(Fpga fpga) {
        this.fpga = fpga;
    }

    public List<String> getCapabilities() {
        return capabilities;
    }

    public void addCapability(String capability) {
        this.capabilities.add(capability);
    }

    public List<OnPackageMemory> getOnPackageMemory() {
        return onPackageMemory;
    }

    public void addOnPackageMemory(OnPackageMemory memory) {
        this.onPackageMemory.add(memory);
    }

    public Set<Endpoint> getEndpoints() {
        return endpoints;
    }

    public void addEndpoint(Endpoint endpoint) {
        requiresNonNull(endpoint, "endpoint");

        endpoints.add(endpoint);
        if (!this.equals(endpoint.getProcessor())) {
            endpoint.setProcessor(this);
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (this.endpoints.contains(endpoint)) {
            this.endpoints.remove(endpoint);
            if (endpoint != null) {
                endpoint.unlinkProcessor(this);
            }
        }
    }

    public ProcessorMetrics getProcessorMetrics() {
        return processorMetrics;
    }

    public void setProcessorMetrics(ProcessorMetrics processorMetrics) {
        if (!Objects.equals(this.processorMetrics, processorMetrics)) {
            unlinkProcessorMetrics(this.processorMetrics);
            this.processorMetrics = processorMetrics;
            if (processorMetrics != null && !this.equals(processorMetrics.getProcessor())) {
                processorMetrics.setProcessor(this);
            }
        }
    }

    public void unlinkProcessorMetrics(ProcessorMetrics processorMetrics) {
        if (Objects.equals(this.processorMetrics, processorMetrics)) {
            this.processorMetrics = null;
            if (processorMetrics != null) {
                processorMetrics.unlinkProcessor(this);
            }
        }
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
    public void preRemove() {
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkProcessorMetrics(processorMetrics);
        unlinkComputerSystem(computerSystem);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, computerSystem);
    }
}
