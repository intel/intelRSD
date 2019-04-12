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

package com.intel.rsd.nodecomposer.business.services.redfish.requests;

import com.intel.rsd.nodecomposer.business.dto.redfish.ODataIdPossessor;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.types.Connectivity;
import com.intel.rsd.nodecomposer.types.InstructionSet;
import com.intel.rsd.nodecomposer.types.InterfaceType;
import com.intel.rsd.nodecomposer.types.MediaType;
import com.intel.rsd.nodecomposer.types.MemoryDeviceType;
import com.intel.rsd.nodecomposer.types.MemoryType;
import com.intel.rsd.nodecomposer.types.PerformanceConfigurationType;
import com.intel.rsd.nodecomposer.types.ProcessorBrand;
import com.intel.rsd.nodecomposer.types.ProcessorType;
import com.intel.rsd.nodecomposer.types.Protocol;
import com.intel.rsd.nodecomposer.types.ReplicaType;
import com.intel.rsd.nodecomposer.types.net.MacAddress;

import java.io.Serializable;
import java.math.BigDecimal;
import java.util.List;
import java.util.Optional;

/**
 * Interface providing requirements for a Composed Node. Contains all data necessary
 * to perform allocation process of a new Composed Node.
 */
public interface RequestedNode extends Serializable {
    String getName();
    String getDescription();
    Integer getTotalSystemMemoryMib();
    Integer getTotalSystemCoreCount();
    List<PerformanceConfigurationType> getSupportedPerformanceConfigurations();
    List<Processor> getProcessors();
    List<Memory> getMemoryModules();
    List<RemoteDrive> getRemoteDrives();
    List<LocalDrive> getLocalDrives();
    List<EthernetInterface> getEthernetInterfaces();
    Security getSecurity();
    Object getOem();

    interface Processor extends ODataIdPossessor, Serializable {
        String getModel();
        ProcessorBrand getBrand();
        Integer getTotalCores();
        Integer getAchievableSpeedMhz();
        InstructionSet getInstructionSet();
        ProcessorType getProcessorType();
        List<String> getCapabilities();
        List<Connectivity> getConnectivity();
    }

    interface Memory extends ODataIdPossessor, Serializable {
        BigDecimal getCapacityMib();
        MemoryType getMemoryType();
        MemoryDeviceType getMemoryDeviceType();
        Integer getSpeedMhz();
        String getManufacturer();
        Integer getDataWidthBits();
    }

    interface EthernetInterface extends ODataIdPossessor, Serializable {
        Integer getSpeedMbps();
        Integer getPrimaryVlan();
        MacAddress getMacAddress();
        Optional<List<Vlan>> getVlans();

        interface Vlan extends Serializable {
            boolean isTagged();
            Integer getVlanId();
            Boolean isEnabled();
        }
    }

    interface LocalDrive extends ODataIdPossessor, Serializable {
        BigDecimal getCapacityGib();
        MediaType getType();
        BigDecimal getMinRpm();
        String getSerialNumber();
        Protocol getInterface();
        Boolean isFromFabricSwitch();
    }

    interface RemoteDrive extends Serializable {
        BigDecimal getCapacityGib();
        MasterDrive getMaster();
        ODataId getResourceODataId();
        Protocol getProtocol();

        interface MasterDrive extends Serializable {
            ReplicaType getType();
            ODataId getResourceODataId();
        }
    }

    interface Security extends Serializable {
        Boolean getTpmPresent();
        InterfaceType getTpmInterfaceType();
        Boolean getTxtEnabled();
        Boolean getClearTpmOnDelete();
        Boolean getClearOptaneMemoryOnDelete();
    }
}
