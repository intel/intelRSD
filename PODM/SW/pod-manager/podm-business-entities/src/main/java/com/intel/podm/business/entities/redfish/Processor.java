/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperties;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.business.entities.redfish.properties.ProcessorId;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class Processor extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<String> SOCKET = stringProperty("socket");
    public static final DomainObjectProperty<ProcessorType> PROCESSOR_TYPE = enumProperty("processorType", ProcessorType.class);
    public static final DomainObjectProperty<ProcessorArchitecture> PROCESSOR_ARCHITECTURE
            = enumProperty("processorArchitecture", ProcessorArchitecture.class);
    public static final DomainObjectProperty<InstructionSet> INSTRUCTION_SET = enumProperty("instructionSet", InstructionSet.class);
    public static final DomainObjectProperty<String> MANUFACTURER = stringProperty("manufacturer");
    public static final DomainObjectProperty<String> MODEL = stringProperty("model");
    public static final DomainObjectProperty<Integer> MAX_SPEED_MHZ = integerProperty("maxSpeedMhz");
    public static final DomainObjectProperty<Integer> TOTAL_CORES = integerProperty("totalCores");
    public static final DomainObjectProperty<Integer> TOTAL_THREADS = integerProperty("totalThreads");
    public static final DomainObjectProperty<ProcessorBrand> BRAND = enumProperty("brand", ProcessorBrand.class);

    public static final DomainObjectProperty<List<String>> PROCESSOR_CAPABILITIES = DomainObjectProperties.listProperty("processorCapabilities");

    @Override
    public String getName() {
        return getProperty(NAME);
    }

    @Override
    public void setName(String name) {
        setProperty(NAME, name);
    }

    @Override
    public String getDescription() {
        return getProperty(DESCRIPTION);
    }

    @Override
    public void setDescription(String description) {
        setProperty(DESCRIPTION, description);
    }

    public String getSocket() {
        return getProperty(SOCKET);
    }

    public void setSocket(String socket) {
        setProperty(SOCKET, socket);
    }

    public ProcessorType getProcessorType() {
        return getProperty(PROCESSOR_TYPE);
    }

    public void setProcessorType(ProcessorType processorType) {
        setProperty(PROCESSOR_TYPE, processorType);
    }

    public ProcessorArchitecture getProcessorArchitecture() {
        return getProperty(PROCESSOR_ARCHITECTURE);
    }

    public void setProcessorArchitecture(ProcessorArchitecture processorArchitecture) {
        setProperty(PROCESSOR_ARCHITECTURE, processorArchitecture);
    }

    public InstructionSet getInstructionSet() {
        return getProperty(INSTRUCTION_SET);
    }

    public void setInstructionSet(InstructionSet instructionSet) {
        setProperty(INSTRUCTION_SET, instructionSet);
    }

    public String getManufacturer() {
        return getProperty(MANUFACTURER);
    }

    public void setManufacturer(String manufacturer) {
        setProperty(MANUFACTURER, manufacturer);
    }

    public String getModel() {
        return getProperty(MODEL);
    }

    public void setModel(String model) {
        setProperty(MODEL, model);
    }

    public Integer getMaxSpeedMhz() {
        return getProperty(MAX_SPEED_MHZ);
    }

    public void setMaxSpeedMhz(Integer maxSpeedMhz) {
        setProperty(MAX_SPEED_MHZ, maxSpeedMhz);
    }

    public Integer getTotalCores() {
        return getProperty(TOTAL_CORES);
    }

    public void setTotalCores(Integer totalCores) {
        setProperty(TOTAL_CORES, totalCores);
    }

    public Integer getTotalThreads() {
        return getProperty(TOTAL_THREADS);
    }

    public void setTotalThreads(Integer totalThreads) {
        setProperty(TOTAL_THREADS, totalThreads);
    }

    public ProcessorBrand getBrand() {
        return getProperty(BRAND);
    }

    public void setBrand(ProcessorBrand brand) {
        setProperty(BRAND, brand);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public List<String> getCapabilities() {
        return getProperty(PROCESSOR_CAPABILITIES);
    }

    public void setCapabilities(List<String> processorCapabilities) {
        setProperty(PROCESSOR_CAPABILITIES, processorCapabilities);
    }

    public ComputerSystem getComputerSystem() {
        return single(getLinked(CONTAINED_BY, ComputerSystem.class));
    }

    @Override
    public URI getSourceUri() {
        return getProperty(SOURCE_URI);
    }

    @Override
    public void setSourceUri(URI sourceUri) {
        setProperty(SOURCE_URI, sourceUri);
    }

    @Override
    public ExternalService getService() {
        return singleOrNull(getLinked(OWNED_BY, ExternalService.class));
    }

    public void setProcessorId(ProcessorId processorId) {
        link(CONTAINS, processorId);
    }

    public ProcessorId getProcessorId() {
        return singleOrNull(getLinked(CONTAINS, ProcessorId.class));
    }
}
