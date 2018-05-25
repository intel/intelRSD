/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.business.services.redfish.requests;

import com.intel.podm.business.dto.redfish.ContextPossessor;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.InterfaceType;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.ReplicaType;

import java.math.BigDecimal;
import java.util.List;
import java.util.Optional;

/**
 * Interface providing requirements for a Composed Node. Contains all data necessary
 * to perform allocation process of a new Composed Node.
 */
public interface RequestedNode {
    String getName();
    String getDescription();
    Integer getTotalSystemMemoryMib();
    Integer getTotalSystemCoreCount();
    List<Processor> getProcessors();
    List<Memory> getMemoryModules();
    List<RemoteDrive> getRemoteDrives();
    List<LocalDrive> getLocalDrives();
    List<EthernetInterface> getEthernetInterfaces();
    Security getSecurity();
    Object getOem();

    interface Processor extends ContextPossessor {
        String getModel();
        ProcessorBrand getBrand();
        Integer getTotalCores();
        Integer getAchievableSpeedMhz();
        InstructionSet getInstructionSet();
        ProcessorType getProcessorType();
        List<String> getCapabilities();
    }

    interface Memory extends ContextPossessor {
        BigDecimal getCapacityMib();
        MemoryDeviceType getMemoryDeviceType();
        Integer getSpeedMhz();
        String getManufacturer();
        Integer getDataWidthBits();
    }

    interface EthernetInterface extends ContextPossessor {
        Integer getSpeedMbps();
        Integer getPrimaryVlan();
        Optional<List<Vlan>> getVlans();

        interface Vlan {
            boolean isTagged();
            Integer getVlanId();
            Boolean isEnabled();
        }
    }

    interface LocalDrive extends ContextPossessor {
        BigDecimal getCapacityGib();
        MediaType getType();
        BigDecimal getMinRpm();
        String getSerialNumber();
        Protocol getInterface();
        Boolean isFromFabricSwitch();
    }

    interface RemoteDrive {
        BigDecimal getCapacityGib();
        MasterDrive getMaster();
        Context getResourceContext();
        Protocol getProtocol();

        interface MasterDrive {
            ReplicaType getType();
            Context getResourceContext();
        }
    }

    interface Security {
        Boolean getTpmPresent();
        InterfaceType getTpmInterfaceType();
        Boolean getTxtEnabled();
        Boolean getClearTpmOnDelete();
    }
}
