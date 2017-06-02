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

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;
import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;
import com.intel.podm.mappers.subresources.ComputerSystemDeviceMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import java.math.BigDecimal;
import java.net.URI;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNotNull;

@SuppressWarnings({
    "checkstyle:MethodName", "checkstyle:MethodLength", "checkstyle:MagicNumber",
    "checkstyle:ExecutableStatementCount", "checkstyle:MethodCount"
})
public class ComputerSystemMapperTest {
    @InjectMocks
    ComputerSystemMapper mapper;

    @BeforeTest
    public void initMocks() {
        MockitoAnnotations.initMocks(this);
        mapper.computerSystemDeviceMapper = mock(ComputerSystemDeviceMapper.class);
        mapper.simpleTypeMapper = new SimpleTypeMapper();
    }

    @Test
    public void whenPropertyIsUnassigned_shouldBeIgnoredDuringMapping() throws Exception {
        final String modelValue = "SomeImportantModel";

        ComputerSystemResource source = mock(ComputerSystemTestResource.class);
        when(source.getModel()).thenReturn(Ref.unassigned());
        when(source.getPciDevices()).thenReturn(Ref.unassigned());
        when(source.getPcieConnectionId()).thenReturn(Ref.unassigned());
        when(source.getAllowableResetTypes()).thenReturn(Ref.unassigned());

        ComputerSystem target = new ComputerSystem();
        target.setModel(modelValue);

        mapper.map(source, target);

        assertNotNull(target.getModel());
        assertEquals(target.getModel(), modelValue);
    }

    @Test(invocationCount = 100, threadPoolSize = 10)
    public void whenMappingFromSourceToTarget_shouldMapAllProperties() throws Exception {
        ComputerSystemTestResource source = new ComputerSystemTestResource();
        ComputerSystem target = new ComputerSystem();

        mapper.map(source, target);

        assertEquals(source.getName(), target.getName());
        assertEquals(source.getDescription(), target.getDescription());
        assertEquals(source.getSystemType().get(), target.getSystemType());
        assertEquals(source.getAssetTag().get(), target.getAssetTag());
        assertEquals(source.getUuid().get(), target.getUuid());
        assertEquals(source.getHostName().get(), target.getHostName());
        assertEquals(source.getStatus().get(), target.getStatus());
        assertEquals(source.getIndicatorLed().get(), target.getIndicatorLed());
        assertEquals(source.getPowerState().get(), target.getPowerState());
        assertEquals(source.getBiosVersion().get(), target.getBiosVersion());
        assertEquals(source.getManufacturer().get(), target.getManufacturer());
        assertEquals(source.getModel().get(), target.getModel());
        assertEquals(source.getSku().get(), target.getSku());
        assertEquals(source.getSerialNumber().get(), target.getSerialNumber());
        assertEquals(source.getPartNumber().get(), target.getPartNumber());
        assertEquals(source.getProcessorModel().get(), target.getProcessorModel());
        assertEquals(source.getProcessorsCount().get(), target.getProcessorsCount());
        assertEquals(source.getProcessorStatus().get(), target.getProcessorStatus());
        assertEquals(source.getTotalSystemMemoryGiB().get(), target.getTotalSystemMemoryGiB());
        assertEquals(source.getMemoryStatus().get(), target.getMemoryStatus());
        assertEquals(source.getPcieConnectionId().get(), target.getPcieConnectionId());
        assertEquals(source.getMemorySockets().get(), target.getMemorySockets());
        assertEquals(source.getProcessorSockets().get(), target.getProcessorSockets());

        assertEquals(source.getBootObject().getBootSourceOverrideEnabled().get(),
            target.getBoot().getBootSourceOverrideEnabled());
        assertEquals(source.getBootObject().getBootSourceOverrideMode().get(), target.getBoot().getBootSourceOverrideMode());
        assertEquals(source.getBootObject().getBootSourceOverrideModeAllowableValues(),
            target.getBoot().getBootSourceOverrideModeAllowableValues());
        assertEquals(source.getBootObject().getBootSourceOverrideTarget().get(),
            target.getBoot().getBootSourceOverrideTarget());
        assertEquals(source.getBootObject().getBootSourceOverrideTargetAllowableValues(),
            target.getBoot().getBootSourceOverrideTargetAllowableValues());

    }

    static class ComputerSystemTestResource implements ComputerSystemResource {

        @Override
        public Ref<SystemType> getSystemType() {
            return Ref.of(SystemType.PHYSICAL);
        }

        @Override
        public Ref<String> getAssetTag() {
            return Ref.of("tag");
        }

        @Override
        public Ref<UUID> getUuid() {
            return Ref.of(UUID.fromString("7cce18bd-77a1-4e38-8cd1-f9251ec84b13"));
        }

        @Override
        public Ref<String> getHostName() {
            return Ref.of("name");
        }

        @Override
        public Ref<Status> getStatus() {
            return Ref.of(Status.fromString("State=Enabled,Health=OK,HealthRollup=OK"));
        }

        @Override
        public Ref<IndicatorLed> getIndicatorLed() {
            return Ref.of(IndicatorLed.BLINKING);
        }

        @Override
        public Ref<PowerState> getPowerState() {
            return Ref.of(PowerState.ON);
        }

        @Override
        public Ref<String> getBiosVersion() {
            return Ref.of("1");
        }

        @Override
        public Ref<String> getManufacturer() {
            return Ref.of("2");
        }

        @Override
        public Ref<String> getModel() {
            return Ref.of("3");
        }

        @Override
        public Ref<String> getSku() {
            return Ref.of("4");
        }

        @Override
        public Ref<String> getSerialNumber() {
            return Ref.of("5");
        }

        @Override
        public Ref<String> getPartNumber() {
            return Ref.of("6");
        }

        @Override
        @SuppressWarnings("checkstyle:AnonInnerLength")
        public ComputerSystemResource.Boot getBootObject() {
            return new ComputerSystemResource.Boot() {
                @Override
                public Ref<BootSourceState> getBootSourceOverrideEnabled() {
                    return Ref.of(BootSourceState.ONCE);
                }

                @Override
                public Ref<BootSourceType> getBootSourceOverrideTarget() {
                    return Ref.of(BootSourceType.DIAGS);
                }

                @Override
                public Set<BootSourceType> getBootSourceOverrideTargetAllowableValues() {
                    return new HashSet<BootSourceType>() {{
                        add(BootSourceType.BIOS_SETUP);
                    }};
                }

                @Override
                public Ref<BootSourceMode> getBootSourceOverrideMode() {
                    return Ref.of(BootSourceMode.LEGACY);
                }

                @Override
                public Set<BootSourceMode> getBootSourceOverrideModeAllowableValues() {
                    return new HashSet<BootSourceMode>() {{
                        add(BootSourceMode.LEGACY);
                        add(BootSourceMode.UEFI);
                    }};
                }
            };
        }

        @Override
        public Ref<String> getProcessorModel() {
            return Ref.of("7");
        }

        @Override
        public Ref<LinkedHashSet<RedfishComputerSystem.Device>> getPciDevices() {
            return Ref.of(new LinkedHashSet<RedfishComputerSystem.Device>());
        }

        @Override
        public Ref<Integer> getProcessorsCount() {
            return Ref.of(1);
        }

        @Override
        public Ref<Status> getProcessorStatus() {
            return Ref.of(Status.fromString("State=Enabled,Health=OK,HealthRollup=OK"));
        }

        @Override
        public Ref<BigDecimal> getTotalSystemMemoryGiB() {
            return Ref.of(new BigDecimal(4));
        }

        @Override
        public Ref<Status> getMemoryStatus() {
            return Ref.of(Status.fromString("State=Enabled,Health=OK,HealthRollup=OK"));
        }

        @Override
        public Ref<Integer> getMemorySockets() {
            return Ref.of(7);
        }

        @Override
        public Ref<List<String>> getPcieConnectionId() {
            return Ref.of(new ArrayList<String>() {{
                add("XYU.5.KLO");
            }});
        }

        @Override
        public Ref<Integer> getProcessorSockets() {
            return Ref.of(8);
        }

        @Override
        public boolean isBasic() {
            return false;
        }

        @Override
        public Ref<LinkedHashSet<ResetType>> getAllowableResetTypes() {
            return Ref.of(new LinkedHashSet<ResetType>());
        }

        @Override
        public Iterable<ResourceSupplier> getPcieDevices() throws ExternalServiceApiReaderException {
            return null;
        }

        @Override
        public Iterable<ResourceSupplier> getPcieFunctions() throws ExternalServiceApiReaderException {
            return null;
        }

        @Override
        public Iterable<ResourceSupplier> getNetworkInterfaces() throws ExternalServiceApiReaderException {
            return null;
        }

        @Override
        public Iterable<ResourceSupplier> getProcessors() throws ExternalServiceApiReaderException {
            return null;
        }

        @Override
        public Iterable<ResourceSupplier> getEthernetInterfaces() throws ExternalServiceApiReaderException {
            return null;
        }

        @Override
        public Iterable<ResourceSupplier> getMemoryModules() throws ExternalServiceApiReaderException {
            return null;
        }

        @Override
        public Iterable<ResourceSupplier> getSimpleStorages() throws ExternalServiceApiReaderException {
            return null;
        }

        @Override
        public Iterable<ResourceSupplier> getStorages() throws ExternalServiceApiReaderException {
            return null;
        }

        @Override
        public Iterable<ResourceSupplier> getEndpoints() throws ExternalServiceApiReaderException {
            return null;
        }

        @Override
        public Iterable<ResourceSupplier> getAdapters() throws ExternalServiceApiReaderException {
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
            return "1";
        }

        @Override
        public String getName() {
            return "name";
        }

        @Override
        public String getDescription() {
            return "desc";
        }

        @Override
        public Links getLinks() {
            return null;
        }
    }
}
