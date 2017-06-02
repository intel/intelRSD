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
import com.intel.podm.business.entities.redfish.embeddables.ProcessorId;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.ProcessorIdObject;
import com.intel.podm.client.api.resources.redfish.ProcessorResource;
import com.intel.podm.common.types.InstructionSet;
import com.intel.podm.common.types.ProcessorArchitecture;
import com.intel.podm.common.types.ProcessorBrand;
import com.intel.podm.common.types.ProcessorType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.InstructionSet.MIPS64;
import static com.intel.podm.common.types.ProcessorArchitecture.MIPS;
import static com.intel.podm.common.types.ProcessorBrand.E7;
import static com.intel.podm.common.types.ProcessorType.CPU;
import static com.intel.podm.common.types.ProcessorType.FPGA;
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

    @BeforeTest
    public void initMocks() {
        MockitoAnnotations.initMocks(this);
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
    static class ProcessorResourceTest implements ProcessorResource {
        @Override
        public Ref<String> getSocket() {
            return Ref.of("test_s");
        }

        @Override
        public Ref<ProcessorType> getProcessorType() {
            return Ref.of(FPGA);
        }

        @Override
        public Ref<ProcessorArchitecture> getProcessorArchitecture() {
            return Ref.of(MIPS);
        }

        @Override
        public Ref<InstructionSet> getInstructionSet() {
            return Ref.of(MIPS64);
        }

        @Override
        public Ref<String> getManufacturer() {
            return Ref.of("test_man");
        }

        @Override
        public Ref<ProcessorBrand> getBrand() {
            return Ref.of(E7);
        }

        @Override
        public Ref<List<String>> getCapabilities() {
            return Ref.of(new ArrayList<String>() {{
                add("test_cap_1");
                add("test_cap_2");
            }});
        }

        @Override
        public Ref<Integer> getTotalCores() {
            return Ref.of(0);
        }

        @Override
        public Ref<Integer> getTotalThreads() {
            return Ref.of(2);
        }

        @Override
        public Ref<String> getModel() {
            return Ref.of("test_mod");
        }

        @Override
        public ProcessorIdObject getProcessorId() {
            return new ProcessorIdObject() {
                @Override
                public Ref<String> getVendorId() {
                    return Ref.of("test_vi");
                }

                @Override
                public Ref<String> getIdentificationRegisters() {
                    return Ref.of("test_ir");
                }

                @Override
                public Ref<String> getEffectiveFamily() {
                    return Ref.of("test_ef");
                }

                @Override
                public Ref<String> getEffectiveModel() {
                    return Ref.of("test_em");
                }

                @Override
                public Ref<String> getStep() {
                    return Ref.of("test_step");
                }

                @Override
                public Ref<String> getMicrocodeInfo() {
                    return Ref.of("test_mi");
                }
            };
        }

        @Override
        public Ref<Integer> getMaxSpeedMhz() {
            return Ref.of(1);
        }

        @Override
        public Ref<Status> getStatus() {
            return Ref.of(Status.fromString("State=Enabled,Health=OK,HealthRollup=OK"));
        }

        @Override
        public ResourceSupplier getComputerSystem() {
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

        @Override
        public Links getLinks() {
            return null;
        }
    }
}
