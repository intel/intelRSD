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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.FpgaType;
import com.intel.podm.common.types.HssiConfig;
import com.intel.podm.common.types.HssiSideband;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorMemoryType;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "name", "description", "id", "socket", "processorType", "processorArchitecture", "instructionSet",
    "manufacturer", "model", "processorId", "maxSpeedMhz", "totalCores", "totalThreads", "status", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class ProcessorDto extends RedfishDto {
    private final Oem oem = new Oem();
    private String socket;
    private ProcessorType processorType;
    private ProcessorArchitecture processorArchitecture;
    private InstructionSet instructionSet;
    private String manufacturer;
    private String model;
    @JsonProperty("MaxSpeedMHz")
    private Integer maxSpeedMhz;
    private Integer totalCores;
    private Integer totalThreads;
    private ProcessorIdDto processorId = new ProcessorIdDto();
    private Status status;

    public ProcessorDto() {
        super("#Processor.v1_0_0.Processor");
    }

    public String getSocket() {
        return socket;
    }

    public void setSocket(String socket) {
        this.socket = socket;
    }

    public ProcessorType getProcessorType() {
        return processorType;
    }

    public void setProcessorType(ProcessorType processorType) {
        this.processorType = processorType;
    }

    public ProcessorArchitecture getProcessorArchitecture() {
        return processorArchitecture;
    }

    public void setProcessorArchitecture(ProcessorArchitecture processorArchitecture) {
        this.processorArchitecture = processorArchitecture;
    }

    public InstructionSet getInstructionSet() {
        return instructionSet;
    }

    public void setInstructionSet(InstructionSet instructionSet) {
        this.instructionSet = instructionSet;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public Integer getMaxSpeedMhz() {
        return maxSpeedMhz;
    }

    public void setMaxSpeedMhz(Integer maxSpeedMhz) {
        this.maxSpeedMhz = maxSpeedMhz;
    }

    public Integer getTotalCores() {
        return totalCores;
    }

    public void setTotalCores(Integer totalCores) {
        this.totalCores = totalCores;
    }

    public Integer getTotalThreads() {
        return totalThreads;
    }

    public void setTotalThreads(Integer totalThreads) {
        this.totalThreads = totalThreads;
    }

    public ProcessorIdDto getProcessorId() {
        return processorId;
    }

    public void setProcessorId(ProcessorIdDto processorId) {
        this.processorId = processorId;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Oem getOem() {
        return oem;
    }

    @JsonPropertyOrder({"vendorId", "identificationRegisters", "effectiveFamily", "effectiveModel", "step", "microcodeInfo"})
    public static final class ProcessorIdDto {
        private String vendorId;
        private String identificationRegisters;
        private String effectiveFamily;
        private String effectiveModel;
        private String step;
        private String microcodeInfo;

        public String getVendorId() {
            return vendorId;
        }

        public void setVendorId(String vendorId) {
            this.vendorId = vendorId;
        }

        public String getIdentificationRegisters() {
            return identificationRegisters;
        }

        public void setIdentificationRegisters(String identificationRegisters) {
            this.identificationRegisters = identificationRegisters;
        }

        public String getEffectiveFamily() {
            return effectiveFamily;
        }

        public void setEffectiveFamily(String effectiveFamily) {
            this.effectiveFamily = effectiveFamily;
        }

        public String getEffectiveModel() {
            return effectiveModel;
        }

        public void setEffectiveModel(String effectiveModel) {
            this.effectiveModel = effectiveModel;
        }

        public String getStep() {
            return step;
        }

        public void setStep(String step) {
            this.step = step;
        }

        public String getMicrocodeInfo() {
            return microcodeInfo;
        }

        public void setMicrocodeInfo(String microcodeInfo) {
            this.microcodeInfo = microcodeInfo;
        }
    }

    @JsonPropertyOrder({"type", "capacityMb", "speedMhz"})
    public static final class OemProcessorMemory {
        private ProcessorMemoryType type;
        @JsonProperty("CapacityMB")
        private int capacityMb;
        @JsonProperty("SpeedMHz")
        private int speedMhz;

        public ProcessorMemoryType getType() {
            return type;
        }

        public void setType(ProcessorMemoryType type) {
            this.type = type;
        }

        public int getCapacityMb() {
            return capacityMb;
        }

        public void setCapacityMb(int capacityMb) {
            this.capacityMb = capacityMb;
        }

        public int getSpeedMhz() {
            return speedMhz;
        }

        public void setSpeedMhz(int speedMhz) {
            this.speedMhz = speedMhz;
        }
    }

    @JsonPropertyOrder({"type", "bitStreamVersion", "hssiConfiguration", "hssiSideband", "reconfigurationSlots"})
    public static final class OemFpga {
        private FpgaType type;
        private String bitStreamVersion;
        @JsonProperty("HSSIConfiguration")
        private HssiConfig hssiConfiguration;
        @JsonProperty("HSSISideband")
        private HssiSideband hssiSideband;
        private int reconfigurationSlots;

        public FpgaType getType() {
            return type;
        }

        public void setType(FpgaType type) {
            this.type = type;
        }

        public String getBitStreamVersion() {
            return bitStreamVersion;
        }

        public void setBitStreamVersion(String bitStreamVersion) {
            this.bitStreamVersion = bitStreamVersion;
        }

        public HssiConfig getHssiConfiguration() {
            return hssiConfiguration;
        }

        public void setHssiConfiguration(HssiConfig hssiConfiguration) {
            this.hssiConfiguration = hssiConfiguration;
        }

        public HssiSideband getHssiSideband() {
            return hssiSideband;
        }

        public void setHssiSideband(HssiSideband hssiSideband) {
            this.hssiSideband = hssiSideband;
        }

        public int getReconfigurationSlots() {
            return reconfigurationSlots;
        }

        public void setReconfigurationSlots(int reconfigurationSlots) {
            this.reconfigurationSlots = reconfigurationSlots;
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({
            "@odata.type", "brand", "capabilities", "onPackageMemory", "thermalDesignPowerWatt", "fpga", "processorMetrics", "extendedIdentificationRegisters"
        })
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String odataType = "#Intel.Oem.Processor";
            private ProcessorBrand brand;
            private List<String> capabilities = new ArrayList<>();
            private List<OemProcessorMemory> onPackageMemory = new ArrayList<>();
            private BigDecimal thermalDesignPowerWatt;
            @JsonProperty("FPGA")
            private OemFpga fpga;
            @JsonInclude(NON_NULL)
            @JsonProperty("Metrics")
            private SingletonContext processorMetrics;
            private Map<String, String> extendedIdentificationRegisters;

            public ProcessorBrand getBrand() {
                return brand;
            }

            public void setBrand(ProcessorBrand brand) {
                this.brand = brand;
            }

            public List<String> getCapabilities() {
                return capabilities;
            }

            public void setCapabilities(List<String> capabilities) {
                this.capabilities = capabilities;
            }

            public List<OemProcessorMemory> getOnPackageMemory() {
                return onPackageMemory;
            }

            public void setOnPackageMemory(List<OemProcessorMemory> onPackageMemory) {
                this.onPackageMemory = onPackageMemory;
            }

            public BigDecimal getThermalDesignPowerWatt() {
                return thermalDesignPowerWatt;
            }

            public void setThermalDesignPowerWatt(BigDecimal thermalDesignPowerWatt) {
                this.thermalDesignPowerWatt = thermalDesignPowerWatt;
            }

            public OemFpga getFpga() {
                return fpga;
            }

            public void setFpga(OemFpga fpga) {
                this.fpga = fpga;
            }

            public SingletonContext getProcessorMetrics() {
                return processorMetrics;
            }

            public void setProcessorMetrics(SingletonContext processorMetrics) {
                this.processorMetrics = processorMetrics;
            }

            public Map<String, String> getExtendedIdentificationRegisters() {
                return extendedIdentificationRegisters;
            }

            public void setExtendedIdentificationRegisters(Map<String, String> extendedIdentificationRegisters) {
                this.extendedIdentificationRegisters = extendedIdentificationRegisters;
            }
        }
    }
}
