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
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.DriveResource;
import com.intel.podm.client.api.resources.redfish.IdentifierObject;
import com.intel.podm.client.api.resources.redfish.RedfishLocationObject;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.client.resources.redfish.properties.IdentifierObjectImpl;
import com.intel.podm.client.resources.redfish.properties.RedfishLocationObjectImpl;
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

import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;
import static com.intel.podm.common.utils.Contracts.checkState;
import static java.util.Arrays.stream;
import static java.util.stream.Collectors.joining;

@OdataTypes({
    "#Drive" + OdataTypes.VERSION_PATTERN + "Drive"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class DriveResourceImpl extends ExternalServiceResourceImpl implements DriveResource {
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
    private Ref<List<IdentifierObjectImpl>> identifiers = unassigned();

    @JsonProperty("Location")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<RedfishLocationObjectImpl>> location = unassigned();

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

    @Override
    public ResourceSupplier getParentChassis() {
        final String splittingChar = "/";
        String[] splitUri = getUri().toString().split(splittingChar);
        checkState(splitUri.length - 2 > 0, "Unexpected URI. Cannot proceed to Chassis which owns this drive.");
        String chassisUri = stream(splitUri).limit(splitUri.length - 2).collect(joining(splittingChar));

        return toSupplier(new ODataId(chassisUri));
    }

    @Override
    @LinkName("storageInDrive")
    public ResourceSupplier getStorage() {
        if (oem.rackScaleOem.storage == null) {
            return null;
        }
        return toSupplier(oem.rackScaleOem.storage);
    }

    @Override
    @LinkName("pcieDeviceFunctionInDrive")
    public ResourceSupplier getPcieDeviceFunction() {
        if (oem.rackScaleOem.pcieFunction == null) {
            return null;
        }
        return toSupplier(oem.rackScaleOem.pcieFunction);
    }

    @Override
    public Ref<StatusIndicator> getStatusIndicator() {
        return statusIndicator;
    }

    @Override
    public Ref<List<IdentifierObject>> getIdentifiers() {
        return (Ref) identifiers;
    }

    @Override
    public Ref<List<RedfishLocationObject>> getLocation() {
        return (Ref) location;
    }

    @Override
    public Ref<IndicatorLed> getIndicatorLed() {
        return indicatorLed;
    }

    @Override
    public Ref<String> getModel() {
        return model;
    }

    @Override
    public Ref<String> getRevision() {
        return revision;
    }

    @Override
    public Ref<Long> getCapacityBytes() {
        return capacityBytes;
    }

    @Override
    public Ref<Boolean> getFailurePredicted() {
        return failurePredicted;
    }

    @Override
    public Ref<Protocol> getProtocol() {
        return protocol;
    }

    @Override
    public Ref<MediaType> getMediaType() {
        return mediaType;
    }

    @Override
    public Ref<String> getManufacturer() {
        return manufacturer;
    }

    @Override
    public Ref<String> getSku() {
        return sku;
    }

    @Override
    public Ref<String> getSerialNumber() {
        return serialNumber;
    }

    @Override
    public Ref<String> getPartNumber() {
        return partNumber;
    }

    @Override
    public Ref<String> getAssetTag() {
        return assetTag;
    }

    @Override
    public Ref<HotspareType> getHotspareType() {
        return hotspareType;
    }

    @Override
    public Ref<EncryptionAbility> getEncryptionAbility() {
        return encryptionAbility;
    }

    @Override
    public Ref<EncryptionStatus> getEncryptionStatus() {
        return encryptionStatus;
    }

    @Override
    public Ref<BigDecimal> getRotationSpeedRpm() {
        return rotationSpeedRpm;
    }

    @Override
    public Ref<Integer> getBlockSizeBytes() {
        return blockSizeBytes;
    }

    @Override
    public Ref<BigDecimal> getCapableSpeedGbs() {
        return capableSpeedGbs;
    }

    @Override
    public Ref<BigDecimal> getNegotiatedSpeedGbs() {
        return negotiatedSpeedGbs;
    }

    @Override
    public Ref<BigDecimal> getPredictedMediaLifeLeftPercent() {
        return predictedMediaLifeLeftPercent;
    }

    @Override
    public Ref<String> getFirmwareVersion() {
        return oem.rackScaleOem.firmwareVersion;
    }

    @Override
    public Ref<Boolean> getDriveErased() {
        return oem.rackScaleOem.driveErased;
    }

    @Override
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
