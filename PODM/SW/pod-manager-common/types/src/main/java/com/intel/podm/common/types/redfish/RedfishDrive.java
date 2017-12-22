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

package com.intel.podm.common.types.redfish;

import com.intel.podm.common.types.EncryptionAbility;
import com.intel.podm.common.types.EncryptionStatus;
import com.intel.podm.common.types.HotspareType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StatusIndicator;

import java.math.BigDecimal;
import java.util.List;

import static java.util.Collections.emptyList;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface RedfishDrive extends RedfishResource {

    default StatusIndicator getStatusIndicator() {
        return null;
    }

    default IndicatorLed getIndicatorLed() {
        return null;
    }

    default String getModel() {
        return null;
    }

    default String getRevision() {
        return null;
    }

    default Long getCapacityBytes() {
        return null;
    }

    default Boolean getFailurePredicted() {
        return null;
    }

    default Protocol getProtocol() {
        return null;
    }

    default MediaType getMediaType() {
        return null;
    }

    default String getManufacturer() {
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

    default String getAssetTag() {
        return null;
    }

    default List<DurableIdentifier> getIdentifiers() {
        return emptyList();
    }

    default List<Location> getLocation() {
        return emptyList();
    }

    default HotspareType getHotspareType() {
        return null;
    }

    default EncryptionAbility getEncryptionAbility() {
        return null;
    }

    default EncryptionStatus getEncryptionStatus() {
        return null;
    }

    default BigDecimal getRotationSpeedRpm() {
        return null;
    }

    default Integer getBlockSizeBytes() {
        return null;
    }

    default BigDecimal getCapableSpeedGbs() {
        return null;
    }

    default BigDecimal getNegotiatedSpeedGbs() {
        return null;
    }

    default BigDecimal getPredictedMediaLifeLeftPercent() {
        return null;
    }

    default Boolean getEraseOnDetach() {
        return null;
    }

    default String getFirmwareVersion() {
        return null;
    }

    default Boolean getDriveErased() {
        return null;
    }

    default Status getStatus() {
        return null;
    }

    default NavigationProperty getPcieFunction() {
        return null;
    }

    interface Location {
        default String getInfo() {
            return null;
        }
        default String getInfoFormat() {
            return null;
        }
    }
}
