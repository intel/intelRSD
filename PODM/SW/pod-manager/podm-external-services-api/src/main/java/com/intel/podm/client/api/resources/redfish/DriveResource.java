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

package com.intel.podm.client.api.resources.redfish;

import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.types.EncryptionAbility;
import com.intel.podm.common.types.EncryptionStatus;
import com.intel.podm.common.types.HotspareType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StatusIndicator;

import java.math.BigDecimal;
import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface DriveResource extends ExternalServiceResource {
    Ref<StatusIndicator> getStatusIndicator();
    Ref<IndicatorLed> getIndicatorLed();
    Ref<String> getModel();
    Ref<String> getRevision();
    Ref<Long> getCapacityBytes();
    Ref<Boolean> getFailurePredicted();
    Ref<Protocol> getProtocol();
    Ref<MediaType> getMediaType();
    Ref<String> getManufacturer();
    Ref<String> getSku();
    Ref<String> getSerialNumber();
    Ref<String> getPartNumber();
    Ref<Status> getStatus();
    Ref<String> getAssetTag();
    Ref<List<IdentifierObject>> getIdentifiers();
    Ref<List<RedfishLocationObject>> getLocation();
    Ref<HotspareType> getHotspareType();
    Ref<EncryptionAbility> getEncryptionAbility();
    Ref<EncryptionStatus> getEncryptionStatus();
    Ref<BigDecimal> getRotationSpeedRpm();
    Ref<Integer> getBlockSizeBytes();
    Ref<BigDecimal> getCapableSpeedGbs();
    Ref<BigDecimal> getNegotiatedSpeedGbs();
    Ref<BigDecimal> getPredictedMediaLifeLeftPercent();
    Ref<String> getFirmwareVersion();
    Ref<Boolean> getDriveErased();

    ResourceSupplier getParentChassis();
    ResourceSupplier getStorage();
    ResourceSupplier getPcieDeviceFunction();
}
