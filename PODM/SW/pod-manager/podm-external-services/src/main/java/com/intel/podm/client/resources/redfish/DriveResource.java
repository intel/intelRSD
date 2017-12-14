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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.EncryptionAbility;
import com.intel.podm.common.types.EncryptionStatus;
import com.intel.podm.common.types.HotspareType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StatusIndicator;
import com.intel.podm.common.types.annotations.AsUnassigned;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;
import static com.intel.podm.common.utils.Contracts.checkState;
import static java.util.Arrays.stream;
import static java.util.stream.Collectors.joining;

@OdataTypes({
    "#Drive" + VERSION_PATTERN + "Drive"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class DriveResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @AsUnassigned(WHEN_NULL)
    @JsonProperty("StatusIndicator")
    private Ref<StatusIndicator> statusIndicator = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("IndicatorLED")
    private Ref<IndicatorLed> indicatorLed = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("Model")
    private Ref<String> model = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("Revision")
    private Ref<String> revision = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("CapacityBytes")
    private Ref<Long> capacityBytes = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("FailurePredicted")
    private Ref<Boolean> failurePredicted = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("Protocol")
    private Ref<Protocol> protocol = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("MediaType")
    private Ref<MediaType> mediaType = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("Manufacturer")
    private Ref<String> manufacturer = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("SKU")
    private Ref<String> sku = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("SerialNumber")
    private Ref<String> serialNumber = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("PartNumber")
    private Ref<String> partNumber = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("AssetTag")
    private Ref<String> assetTag = unassigned();

    @JsonProperty("Identifiers")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<IdentifierObject>> identifiers = unassigned();

    @JsonProperty("Location")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<RedfishLocationObject>> location = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("HotspareType")
    private Ref<HotspareType> hotspareType = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("EncryptionAbility")
    private Ref<EncryptionAbility> encryptionAbility = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("EncryptionStatus")
    private Ref<EncryptionStatus> encryptionStatus = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("RotationSpeedRPM")
    private Ref<BigDecimal> rotationSpeedRpm = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("BlockSizeBytes")
    private Ref<Integer> blockSizeBytes = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("CapableSpeedGbs")
    private Ref<BigDecimal> capableSpeedGbs = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("NegotiatedSpeedGbs")
    private Ref<BigDecimal> negotiatedSpeedGbs = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("PredictedMediaLifeLeftPercent")
    private Ref<BigDecimal> predictedMediaLifeLeftPercent = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonProperty("Status")
    private Ref<Status> status = unassigned();

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public ResourceSupplier getParentChassis() {
        final String splittingChar = "/";
        String[] splitUri = getUri().toString().split(splittingChar);
        checkState(splitUri.length - 2 > 0, "Unexpected URI. Cannot proceed to Chassis which owns this drive.");
        String chassisUri = stream(splitUri).limit(splitUri.length - 2).collect(joining(splittingChar));

        return toSupplier(new ODataId(chassisUri));
    }

    @LinkName("storageInDrive")
    public ResourceSupplier getStorage() {
        if (oem.rackScaleOem.storage == null) {
            return null;
        }
        return toSupplier(oem.rackScaleOem.storage);
    }

    @LinkName("pcieDeviceFunctionInDrive")
    public ResourceSupplier getPcieDeviceFunction() {
        if (oem.rackScaleOem.pcieFunction == null) {
            return null;
        }
        return toSupplier(oem.rackScaleOem.pcieFunction);
    }

    public Ref<StatusIndicator> getStatusIndicator() {
        return statusIndicator;
    }

    public Ref<List<IdentifierObject>> getIdentifiers() {
        return identifiers;
    }

    public Ref<List<RedfishLocationObject>> getLocation() {
        return location;
    }

    public Ref<IndicatorLed> getIndicatorLed() {
        return indicatorLed;
    }

    public Ref<String> getModel() {
        return model;
    }

    public Ref<String> getRevision() {
        return revision;
    }

    public Ref<Long> getCapacityBytes() {
        return capacityBytes;
    }

    public Ref<Boolean> getFailurePredicted() {
        return failurePredicted;
    }

    public Ref<Protocol> getProtocol() {
        return protocol;
    }

    public Ref<MediaType> getMediaType() {
        return mediaType;
    }

    public Ref<String> getManufacturer() {
        return manufacturer;
    }

    public Ref<String> getSku() {
        return sku;
    }

    public Ref<String> getSerialNumber() {
        return serialNumber;
    }

    public Ref<String> getPartNumber() {
        return partNumber;
    }

    public Ref<String> getAssetTag() {
        return assetTag;
    }

    public Ref<HotspareType> getHotspareType() {
        return hotspareType;
    }

    public Ref<EncryptionAbility> getEncryptionAbility() {
        return encryptionAbility;
    }

    public Ref<EncryptionStatus> getEncryptionStatus() {
        return encryptionStatus;
    }

    public Ref<BigDecimal> getRotationSpeedRpm() {
        return rotationSpeedRpm;
    }

    public Ref<Integer> getBlockSizeBytes() {
        return blockSizeBytes;
    }

    public Ref<BigDecimal> getCapableSpeedGbs() {
        return capableSpeedGbs;
    }

    public Ref<BigDecimal> getNegotiatedSpeedGbs() {
        return negotiatedSpeedGbs;
    }

    public Ref<BigDecimal> getPredictedMediaLifeLeftPercent() {
        return predictedMediaLifeLeftPercent;
    }

    public Ref<String> getFirmwareVersion() {
        return oem.rackScaleOem.firmwareVersion;
    }

    public Ref<Boolean> getDriveErased() {
        return oem.rackScaleOem.driveErased;
    }

    public Ref<Status> getStatus() {
        return status;
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("FirmwareVersion")
            @AsUnassigned(WHEN_NULL)
            private Ref<String> firmwareVersion = unassigned();

            @JsonProperty("EraseOnDetach")
            @AsUnassigned(WHEN_NULL)
            private Ref<String> eraseOnDetach = unassigned();

            @JsonProperty("PCIeFunction")
            private ODataId pcieFunction;

            @JsonProperty("Storage")
            private ODataId storage;

            @JsonProperty("DriveErased")
            @AsUnassigned(WHEN_NULL)
            private Ref<Boolean> driveErased = unassigned();
        }
    }
}
