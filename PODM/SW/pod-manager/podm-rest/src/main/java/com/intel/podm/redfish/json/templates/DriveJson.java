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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.EncryptionAbility;
import com.intel.podm.common.types.EncryptionStatus;
import com.intel.podm.common.types.HotspareType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StatusIndicator;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.redfish.json.templates.attributes.IdentifierJson;
import com.intel.podm.redfish.json.templates.attributes.RedfishLocationJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.math.BigDecimal;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "indicatorLed", "model", "status", "capacityBytes", "protocol",
    "mediaType", "manufacturer", "serialNumber", "partNumber", "sku", "statusIndicator", "revision",
    "failurePredicted", "assetTag", "capableSpeedGbs", "negotiatedSpeedGbs", "location", "identifiers",
    "hotspareType", "encryptionAbility", "encryptionStatus", "rotationSpeedRpm", "blockSizeBytes",
    "predictedMediaLifeLeftPercent", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier", "checkstyle:ClassFanOutComplexity"})
public class DriveJson extends BaseResourceJson {
    public StatusIndicator statusIndicator;
    @JsonProperty("IndicatorLED")
    public IndicatorLed indicatorLed;
    public String model;
    public Status status;
    public String revision;
    public Long capacityBytes;
    public Boolean failurePredicted;
    public Protocol protocol;
    public MediaType mediaType;
    public String manufacturer;
    @JsonProperty("SKU")
    public String sku;
    public String serialNumber;
    public String partNumber;
    public String assetTag;
    public List<IdentifierJson> identifiers;
    public List<RedfishLocationJson> location;
    public HotspareType hotspareType;
    public EncryptionAbility encryptionAbility;
    public EncryptionStatus encryptionStatus;
    @JsonProperty("RotationSpeedRPM")
    public BigDecimal rotationSpeedRpm;
    public Integer blockSizeBytes;
    public BigDecimal capableSpeedGbs;
    public BigDecimal negotiatedSpeedGbs;
    public BigDecimal predictedMediaLifeLeftPercent;
    public Links links = new Links();
    public Actions actions = new Actions();
    public Oem oem = new Oem();

    public DriveJson() {
        super("#Drive.v1_1_1.Drive");
    }

    @JsonPropertyOrder({"oDataType", "volumes", "endpoints", "oem"})
    public class Links extends RedfishLinksJson {
        @JsonProperty("@odata.type")
        public String oDataType = "#Drive.v1_1_0.Links";
        @JsonProperty("Volumes")
        public Set<ODataId> volumes = new LinkedHashSet<>();
        @JsonProperty("Endpoints")
        public Set<ODataId> endpoints = new LinkedHashSet<>();
        @JsonProperty("Oem")
        public Oem oem = new Oem();

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOemJson {
            public Oem() {
            }
        }
    }

    @JsonPropertyOrder({"secureEraseAction", "oem"})
    public class Actions extends RedfishActionsJson {
        @JsonProperty("#Drive.SecureErase")
        public SecureEraseAction secureEraseAction = new SecureEraseAction();

        public class SecureEraseAction {
            public String target;
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemJson {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonPropertyOrder({"oDataType", "eraseOnDetach", "firmwareVersion", "driveErased", "pcieFunction", "storage"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            public String oDataType = "#Intel.Oem.Drive";
            public Boolean eraseOnDetach;
            public String firmwareVersion;
            public Boolean driveErased;
            @JsonProperty("PCIeFunction")
            public ODataId pcieFunction;
            @JsonProperty("Storage")
            public ODataId storage;
        }
    }
}
