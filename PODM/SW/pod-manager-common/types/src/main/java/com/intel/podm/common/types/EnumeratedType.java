/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.common.types;

import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.common.types.events.EventType;
import com.intel.podm.common.types.rmm.RackUnitType;
import com.intel.podm.common.types.rmm.ReadingUnit;

import java.util.Set;

import static com.google.common.base.Preconditions.checkArgument;
import static com.google.common.collect.Sets.newHashSet;
import static java.util.Collections.unmodifiableSet;

/**
 * All enums which implement this interface will be automatically deserialized to String value got from method getValue();
 */
public interface EnumeratedType {
    Set<Class<? extends EnumeratedType>> SUB_TYPES = unmodifiableSet(newHashSet(
        AddressRangeType.class,
        AddressState.class,
        AdministrativeState.class,
        BaseModuleType.class,
        BootSourceState.class,
        BootSourceType.class,
        ChassisType.class,
        ComposedNodeState.class,
        MemoryDeviceType.class,
        MemoryMedia.class,
        MemoryType.class,
        DiscoveryState.class,
        DriveType.class,
        EnumStatus.class,
        ErrorCorrection.class,
        EventType.class,
        FormFactor.class,
        GeneralConnectType.class,
        GraphicalConnectType.class,
        IndicatorLed.class,
        InstructionSet.class,
        InstructionSetExtension.class,
        IpV4AddressOrigin.class,
        IpV6AddressOrigin.class,
        LinkTechnology.class,
        LinkType.class,
        LogicalDriveType.class,
        ManagerType.class,
        MemoryClassification.class,
        NodeSystemType.class,
        NotifyIpV6Scope.class,
        OperatingMemoryMode.class,
        OperationalState.class,
        PhysicalContext.class,
        PortClass.class,
        PortMode.class,
        PortType.class,
        Power.class,
        PowerState.class,
        ProcessorArchitecture.class,
        ProcessorBrand.class,
        ProcessorType.class,
        RackUnitType.class,
        ReadingUnit.class,
        RemoteStorageBackend.class,
        RemoteStorageType.class,
        ReplicationMethod.class,
        ResetType.class,
        State.class,
        StorageControllerInterface.class,
        SystemType.class,
        VolumeMode.class,
        VolumeType.class));

    String getValue();

    static <T extends EnumeratedType> T stringToEnum(Class<T> enumType, String text) {
        checkArgument(enumType != null, "Type of enum (class) should not be null");
        checkArgument(text != null, "Constant with null test for enum [" + enumType.getSimpleName() + "] is not allowed");

        for (T extension : enumType.getEnumConstants()) {
            if (text.equalsIgnoreCase(extension.getValue())) {
                return extension;
            }
        }

        throw new IllegalArgumentException("No constant with text [" + text + "] found for enum [" + enumType.getSimpleName() + "]");
    }
}
