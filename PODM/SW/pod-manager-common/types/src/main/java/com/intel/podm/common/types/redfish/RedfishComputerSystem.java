/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.common.types.redfish;

import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.InterfaceType;
import com.intel.podm.common.types.InterfaceTypeSelection;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;

import java.math.BigDecimal;
import java.util.Set;
import java.util.UUID;

import static java.util.Collections.emptySet;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface RedfishComputerSystem extends RedfishResource {
    default String getAssetTag() {
        return null;
    }

    default String getManufacturer() {
        return null;
    }

    default String getModel() {
        return null;
    }

    default String getSku() {
        return null;
    }

    default String getSerialNumber() {
        return null;
    }

    default String getPartNumber() {
        return null;
    }

    default UUID getUuid() {
        return null;
    }

    default String getHostName() {
        return null;
    }

    default SystemType getSystemType() {
        return null;
    }

    default Status getStatus() {
        return null;
    }

    default IndicatorLed getIndicatorLed() {
        return null;
    }

    default PowerState getPowerState() {
        return null;
    }

    default String getBiosVersion() {
        return null;
    }

    default RedfishComputerSystem.Boot getBoot() {
        return null;
    }

    default BigDecimal getTotalSystemMemoryGiB() {
        return null;
    }

    default Status getMemoryStatus() {
        return null;
    }

    default Integer getProcessorsCount() {
        return null;
    }

    default String getProcessorsModel() {
        return null;
    }

    default Status getProcessorsStatus() {
        return null;
    }

    default Set<RedfishComputerSystem.Device> getPciDevices() {
        return emptySet();
    }

    default Set<ResetType> getAllowableResetTypes() {
        return emptySet();
    }

    default DiscoveryState getDiscoveryState() {
        return null;
    }

    default Integer getProcessorSockets() {
        return null;
    }

    default Integer getMemorySockets() {
        return null;
    }

    default Set<TrustedModule> getTrustedModules() {
        return emptySet();
    }

    default Boolean getUserModeEnabled() {
        return null;
    }

    default Boolean getClearTpmOnDelete() {
        return null;
    }

    interface Boot {
        BootSourceState getBootSourceOverrideEnabled();

        BootSourceType getBootSourceOverrideTarget();

        default Set<BootSourceType> getBootSourceOverrideTargetAllowableValues() {
            return emptySet();
        }

        default BootSourceMode getBootSourceOverrideMode() {
            return null;
        }

        default Set<BootSourceMode> getBootSourceOverrideModeAllowableValues() {
            return emptySet();
        }
    }

    interface Device {
        String getVendorId();
        String getDeviceId();
    }

    interface TrustedModule {
        Ref<String> getFirmwareVersion();
        Ref<String> getFirmwareVersion2();
        Ref<InterfaceType> getInterfaceType();
        Ref<InterfaceTypeSelection> getInterfaceTypeSelection();
        Ref<Status> getStatus();
    }
}
