/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.LinkName;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;
import com.intel.rsd.nodecomposer.types.MediaType;
import com.intel.rsd.nodecomposer.types.Protocol;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.Status;
import com.intel.rsd.nodecomposer.types.annotations.AsUnassigned;
import lombok.Getter;

import java.math.BigDecimal;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.rsd.nodecomposer.types.Ref.unassigned;
import static com.intel.rsd.nodecomposer.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Drive" + VERSION_PATTERN + "Drive"
})
@SuppressWarnings({"checkstyle:MethodCount"})
@Getter
public class DriveResource extends ExternalServiceResourceImpl {
    @AsUnassigned(WHEN_NULL)
    @JsonSetter(value = "CapacityBytes", nulls = AS_EMPTY)
    private Ref<Long> capacityBytes = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonSetter(value = "Protocol", nulls = AS_EMPTY)
    private Ref<Protocol> protocol = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonSetter(value = "MediaType", nulls = AS_EMPTY)
    private Ref<MediaType> mediaType = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonSetter(value = "SerialNumber", nulls = AS_EMPTY)
    private Ref<String> serialNumber = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonSetter(value = "RotationSpeedRPM", nulls = AS_EMPTY)
    private Ref<BigDecimal> rotationSpeedRpm = unassigned();

    @AsUnassigned(WHEN_NULL)
    @JsonSetter(value = "Status", nulls = AS_EMPTY)
    private Ref<Status> status = unassigned();

    @JsonProperty("Oem")
    private Oem oem = new Oem();

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

    public Ref<Boolean> getDriveErased() {
        return oem.rackScaleOem.driveErased;
    }

    public class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("PCIeFunction")
            private ODataId pcieFunction;

            @JsonProperty("Storage")
            private ODataId storage;

            @JsonSetter(value = "DriveErased", nulls = AS_EMPTY)
            @AsUnassigned(WHEN_NULL)
            private Ref<Boolean> driveErased = unassigned();
        }
    }
}
