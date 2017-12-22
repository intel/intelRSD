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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.embeddables.Fpga;
import com.intel.podm.business.entities.redfish.embeddables.OnPackageMemory;
import com.intel.podm.business.entities.redfish.embeddables.ProcessorId;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.EndpointResource;
import com.intel.podm.client.resources.redfish.FpgaObject;
import com.intel.podm.client.resources.redfish.ProcessorIdObject;
import com.intel.podm.client.resources.redfish.ProcessorMemoryObject;
import com.intel.podm.client.resources.redfish.ProcessorResource;
import com.intel.podm.common.types.FpgaType;
import com.intel.podm.common.types.HssiConfig;
import com.intel.podm.common.types.HssiSideband;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorMemoryType;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.mappers.subresources.FpgaMapper;
import com.intel.podm.mappers.subresources.OnPackageMemoryMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import java.math.BigDecimal;
import java.net.URI;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static com.intel.podm.common.types.FpgaType.INTEGRATED;
import static com.intel.podm.common.types.HssiConfig.HSSI_4x10G;
import static com.intel.podm.common.types.HssiSideband.I2C;
import static com.intel.podm.common.types.InstructionSet.MIPS64;
import static com.intel.podm.common.types.ProcessorArchitecture.MIPS;
import static com.intel.podm.common.types.ProcessorBrand.E7;
import static com.intel.podm.common.types.ProcessorMemoryType.HBM2;
import static com.intel.podm.common.types.ProcessorType.CPU;
import static com.intel.podm.common.types.ProcessorType.FPGA;
import static com.intel.podm.common.types.Status.statusFromString;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNotNull;

@SuppressWarnings({
    "checkstyle:MethodName", "checkstyle:MethodLength", "checkstyle:MagicNumber",
    "checkstyle:ExecutableStatementCount", "checkstyle:MethodCount", "checkstyle:AnonInnerLength"
})
public class ProcessorMapperTest {
    @InjectMocks
    ProcessorMapper mapper;

    @InjectMocks
    EmbeddableCleanAndCreateStrategy<Fpga> embeddableCleanAndCreateStrategyFpga;

    @InjectMocks
    EmbeddableCleanAndCreateStrategy<OnPackageMemory> embeddableCleanAndCreateStrategyOnPackageMemory;

    @BeforeTest
    public void initMocks() {
        MockitoAnnotations.initMocks(this);
        mapper.fpgaMapper = new FpgaMapper(embeddableCleanAndCreateStrategyFpga);
        mapper.onPackageMemoryMapper = new OnPackageMemoryMapper(embeddableCleanAndCreateStrategyOnPackageMemory);
        mapper.simpleTypeMapper = new SimpleTypeMapper();
    }

    @Test
    public void whenPropertyIsUnassigned_shouldBeIgnoredDuringMapping() throws Exception {
        String vendorId = "test_id";
        ProcessorType type = CPU;

        ProcessorId targetProcessorId = new ProcessorId();
        targetProcessorId.setVendorId(vendorId);

        Processor target = new Processor();
        target.setProcessorId(targetProcessorId);
        target.setProcessorType(type);

        ProcessorResourceTest source = mock(ProcessorResourceTest.class);
        ProcessorIdObject sourceProcessorId = mock(ProcessorIdObject.class);
        when(source.getProcessorId()).thenReturn(sourceProcessorId);
        when(sourceProcessorId.getVendorId()).thenReturn(Ref.unassigned());
        when(source.getProcessorType()).thenReturn(Ref.unassigned());

        assertNotNull(target.getProcessorId().getVendorId());
        assertEquals(target.getProcessorId().getVendorId(), vendorId);

        assertNotNull(target.getProcessorType());
        assertEquals(target.getProcessorType(), type);
    }

    @Test
    public void whenMappingFromSourceToTarget_shouldMapAllProperties() throws Exception {
        ProcessorResourceTest source = new ProcessorResourceTest();
        Processor target = new Processor();

        mapper.map(source, target);

        assertEquals(source.getName(), target.getName());
        assertEquals(source.getSocket().get(), target.getSocket());
        assertEquals(source.getProcessorType().get(), target.getProcessorType());
        assertEquals(source.getProcessorArchitecture().get(), target.getProcessorArchitecture());
        assertEquals(source.getInstructionSet().get(), target.getInstructionSet());
        assertEquals(source.getManufacturer().get(), target.getManufacturer());
        assertEquals(source.getBrand().get(), target.getBrand());
        assertEquals(source.getCapabilities().get().size(), target.getCapabilities().size());
        assertEquals(source.getTotalCores().get(), target.getTotalCores());
        assertEquals(source.getTotalThreads().get(), target.getTotalThreads());
        assertEquals(source.getModel().get(), target.getModel());

        assertEquals(source.getMaxSpeedMhz().get(), target.getMaxSpeedMhz());
        assertEquals(source.getStatus().get(), target.getStatus());

        assertEquals(source.getProcessorId().getVendorId().get(), target.getProcessorId().getVendorId());
        assertEquals(source.getProcessorId().getIdentificationRegisters().get(),
            target.getProcessorId().getIdentificationRegisters());
        assertEquals(source.getProcessorId().getEffectiveFamily().get(), target.getProcessorId().getEffectiveFamily());
        assertEquals(source.getProcessorId().getEffectiveModel().get(), target.getProcessorId().getEffectiveModel());
        assertEquals(source.getProcessorId().getStep().get(), target.getProcessorId().getStep());
        assertEquals(source.getProcessorId().getMicrocodeInfo().get(), target.getProcessorId().getMicrocodeInfo());
    }

    @SuppressWarnings({"checkstyle:MethodCount", "checkstyle:MagicNumber", "checkstyle:AnonInnerLength"})
    static class ProcessorResourceTest extends ProcessorResource {
        public Ref<String> getSocket() {
            return Ref.of("test_s");
        }

        public Ref<ProcessorType> getProcessorType() {
            return Ref.of(FPGA);
        }

        public Ref<ProcessorArchitecture> getProcessorArchitecture() {
            return Ref.of(MIPS);
        }

        public Ref<InstructionSet> getInstructionSet() {
            return Ref.of(MIPS64);
        }

        public Ref<String> getManufacturer() {
            return Ref.of("test_man");
        }

        public Ref<ProcessorBrand> getBrand() {
            return Ref.of(E7);
        }

        public Ref<List<String>> getCapabilities() {
            return Ref.of(new ArrayList<String>() {{
                add("test_cap_1");
                add("test_cap_2");
            }});
        }

        public Ref<Map<String, String>> getExtendedIdentificationRegisters() {
            return Ref.of(new HashMap<String, String>() {{
                put("EAX_00h", "0x0429943FFFFFFFFF");
                put("EAX_01h", "0x0429943FFFFFFFFF");
                put("EAX_02h", "0x0429943FFFFFFFFF");
                put("EAX_03h", "0x0429943FFFFFFFFF");
                put("EAX_04h", "0x0429943FFFFFFFFF");
                put("EAX_05h", "0x0429943FFFFFFFFF");
                put("EAX_07h", "0x0429943FFFFFFFFF");
                put("EAX_80000000h", "0x0429943FFFFFFFFF");
                put("EAX_80000001h", "0x0429943FFFFFFFFF");
                put("EAX_80000002h", "0x0429943FFFFFFFFF");
                put("EAX_80000003h", "0x0429943FFFFFFFFF");
                put("EAX_80000004h", "0x0429943FFFFFFFFF");
                put("EAX_80000005h", "0x0429943FFFFFFFFF");
                put("EAX_80000006h", "0x0429943FFFFFFFFF");
                put("EAX_80000007h", "0x0429943FFFFFFFFF");
                put("EAX_80000008h", "0x0429943FFFFFFFFF");
            }});
        }

        public Ref<Set<EndpointResource>> getEndpoints() {
            return null;
        }

        public Ref<List<ProcessorMemoryObject>> getProcessorMemory() {
            ProcessorMemoryObject processorMemoryObject = new ProcessorMemoryObject() {
                public Ref<ProcessorMemoryType> getType() {
                    return Ref.of(HBM2);
                }

                public Ref<Integer> getCapacityMb() {
                    return Ref.of(512);
                }

                public Ref<Integer> getSpeedMhz() {
                    return Ref.of(1066);
                }
            };

            return Ref.of(new ArrayList<ProcessorMemoryObject>() {{
                add(processorMemoryObject);
            }});
        }

        public Ref<BigDecimal> getThermalDesignPowerWatt() {
            return Ref.of(BigDecimal.valueOf(4));
        }

        public Ref<FpgaObject> getFpgaObject() {
            return Ref.of(new FpgaObject() {
                public Ref<FpgaType> getType() {
                    return Ref.of(INTEGRATED);
                }

                public Ref<String> getBitStreamVersion() {
                    return Ref.of("test_version");
                }

                public Ref<HssiConfig> getHssiConfiguration() {
                    return Ref.of(HSSI_4x10G);
                }

                public Ref<HssiSideband> getHssiSideband() {
                    return Ref.of(I2C);
                }

                public Ref<Integer> getReconfigurationSlots() {
                    return Ref.of(1);
                }

            });
        }

        public Ref<Integer> getTotalCores() {
            return Ref.of(0);
        }

        public Ref<Integer> getTotalThreads() {
            return Ref.of(2);
        }

        public Ref<String> getModel() {
            return Ref.of("test_mod");
        }

        public ProcessorIdObject getProcessorId() {
            return new ProcessorIdObject() {
                public Ref<String> getVendorId() {
                    return Ref.of("test_vi");
                }

                public Ref<String> getIdentificationRegisters() {
                    return Ref.of("test_ir");
                }

                public Ref<String> getEffectiveFamily() {
                    return Ref.of("test_ef");
                }

                public Ref<String> getEffectiveModel() {
                    return Ref.of("test_em");
                }

                public Ref<String> getStep() {
                    return Ref.of("test_step");
                }

                public Ref<String> getMicrocodeInfo() {
                    return Ref.of("test_mi");
                }
            };
        }

        public Ref<Integer> getMaxSpeedMhz() {
            return Ref.of(1);
        }

        public Ref<Status> getStatus() {
            return Ref.of(statusFromString("State=Enabled,Health=OK,HealthRollup=OK"));
        }

        public ResourceSupplier getComputerSystem() {
            return null;
        }

        public ResourceSupplier getProcessorMetrics() {
            return null;
        }

        @Override
        public URI getUri() {
            return null;
        }

        @Override
        public void setUri(URI uri) {

        }

        @Override
        public void setWebClient(WebClient webClient) {

        }

        @Override
        public String getId() {
            return "test_id";
        }

        @Override
        public String getName() {
            return "test_name";
        }

        @Override
        public String getDescription() {
            return "test_desc";
        }
    }
}
