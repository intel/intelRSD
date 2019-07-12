/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.helpers;

import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Manager;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.List;

import static com.intel.rsd.nodecomposer.types.ProcessorType.FPGA;
import static com.intel.rsd.nodecomposer.types.Protocol.FPGA_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static com.intel.rsd.nodecomposer.types.SystemType.VIRTUAL;
import static com.intel.rsd.nodecomposer.utils.Collector.toSingle;
import static java.util.Arrays.asList;

@Component
public class ProcessorHelper {
    public static final List<Protocol> FPGA_PROCESSOR_PROTOCOLS = asList(PCIE, FPGA_OVER_FABRICS);

    public static boolean isRemoteFpga(Processor processor) {
        return FPGA.equals(processor.getProcessorType()) && VIRTUAL.equals(processor.getComputerSystem().getSystemType());
    }

    public boolean processorProtocolEquals(Protocol expectedProtocol, Processor processor) {
        return expectedProtocol.equals(retrieveProtocolFromProcessor(processor));
    }

    public Protocol retrieveProtocolFromProcessor(Processor processor) {
        return retrieveFabricFromProcessor(processor).getFabricType();
    }

    public Fabric retrieveFabricFromProcessor(Processor processor) {
        return getFabricFromProcessor(processor);
    }

    public static Protocol getProtocolFromProcessor(Processor processor) {
        return getFabricFromProcessor(processor)
            .getFabricType();
    }

    private static Fabric getFabricFromProcessor(Processor processor) {
        return processor.getComputerSystem().getManagers().stream()
            .map(Manager::getFabrics)
            .flatMap(Collection::stream)
            .filter(fabric -> FPGA_PROCESSOR_PROTOCOLS.contains(fabric.getFabricType()))
            .collect(toSingle());
    }
}
