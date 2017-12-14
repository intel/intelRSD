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
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.BootObject;
import com.intel.podm.client.resources.redfish.ComputerSystemResource;
import com.intel.podm.client.resources.redfish.MemorySummaryObject;
import com.intel.podm.client.resources.redfish.ProcessorSummaryObject;
import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.InterfaceType;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;
import com.intel.podm.common.types.redfish.RedfishComputerSystem.TrustedModule;
import com.intel.podm.mappers.subresources.ComputerSystemDeviceMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;
import com.intel.podm.mappers.subresources.TrustedModuleMapper;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import java.math.BigDecimal;
import java.net.URI;
import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.UUID;

import static com.intel.podm.common.types.BootSourceMode.LEGACY;
import static com.intel.podm.common.types.BootSourceMode.UEFI;
import static com.intel.podm.common.types.BootSourceState.ONCE;
import static com.intel.podm.common.types.BootSourceType.BIOS_SETUP;
import static com.intel.podm.common.types.BootSourceType.DIAGS;
import static com.intel.podm.common.types.IndicatorLed.BLINKING;
import static com.intel.podm.common.types.PowerState.ON;
import static com.intel.podm.common.types.Status.statusFromString;
import static com.intel.podm.common.types.SystemType.PHYSICAL;
import static java.util.Collections.emptyList;
import static java.util.UUID.fromString;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNotNull;

@SuppressWarnings({
    "checkstyle:MethodName", "checkstyle:MethodLength", "checkstyle:MagicNumber", "checkstyle:ExecutableStatementCount", "checkstyle:MethodCount"
})
public class ComputerSystemMapperTest {
    @InjectMocks
    private ComputerSystemMapper mapper;


    @BeforeTest
    public void initMocks() {
        MockitoAnnotations.initMocks(this);
        mapper.computerSystemDeviceMapper = mock(ComputerSystemDeviceMapper.class);
        mapper.simpleTypeMapper = new SimpleTypeMapper();
        mapper.trustedModuleMapper = mock(TrustedModuleMapper.class);
    }

    @Test
    public void whenPropertyIsUnassigned_shouldBeIgnoredDuringMapping() throws Exception {
        final String modelValue = "SomeImportantModel";

        ComputerSystemResource source = mock(ComputerSystemTestResource.class);
        when(source.getModel()).thenReturn(Ref.unassigned());
        when(source.getPciDevices()).thenReturn(Ref.unassigned());
        when(source.getPcieConnectionId()).thenReturn(Ref.unassigned());
        when(source.getAllowableResetTypes()).thenReturn(Ref.unassigned());
        when(source.getAllowableInterfaceTypes()).thenReturn(Ref.unassigned());
        when(source.getTrustedModules()).thenReturn(Ref.unassigned());

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
        assertEquals(source.getProcessorSummary().getModel().get(), target.getProcessorsModel());
        assertEquals(source.getProcessorSummary().getCount().get(), target.getProcessorsCount());
        assertEquals(source.getProcessorSummary().getStatus().get(), target.getProcessorsStatus());
        assertEquals(source.getMemorySummary().getTotalSystemMemoryGiB().get(), target.getTotalSystemMemoryGiB());
        assertEquals(source.getMemorySummary().getStatus().get(), target.getMemoryStatus());
        assertEquals(source.getPcieConnectionId().get(), target.getPcieConnectionIds());
        assertEquals(source.getMemorySockets().get(), target.getMemorySockets());
        assertEquals(source.getProcessorSockets().get(), target.getProcessorSockets());

        assertEquals(source.getBootObject().getBootSourceOverrideEnabled().get(), target.getBoot().getBootSourceOverrideEnabled());
        assertEquals(source.getBootObject().getBootSourceOverrideMode().get(), target.getBoot().getBootSourceOverrideMode());
        assertEquals(source.getBootObject().getBootSourceOverrideModeAllowableValues(), target.getBoot().getBootSourceOverrideModeAllowableValues());
        assertEquals(source.getBootObject().getBootSourceOverrideTarget().get(), target.getBoot().getBootSourceOverrideTarget());
        assertEquals(source.getBootObject().getBootSourceOverrideTargetAllowableValues(), target.getBoot().getBootSourceOverrideTargetAllowableValues());
    }

    @SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
    public class ComputerSystemTestResource extends ComputerSystemResource {
        public Ref<SystemType> getSystemType() {
            return Ref.of(PHYSICAL);
        }

        public Ref<String> getAssetTag() {
            return Ref.of("tag");
        }

        public Ref<UUID> getUuid() {
            return Ref.of(fromString("7cce18bd-77a1-4e38-8cd1-f9251ec84b13"));
        }

        public Ref<String> getHostName() {
            return Ref.of("name");
        }

        public Ref<Status> getStatus() {
            return Ref.of(statusFromString("State=Enabled,Health=OK,HealthRollup=OK"));
        }

        public Ref<IndicatorLed> getIndicatorLed() {
            return Ref.of(BLINKING);
        }

        public Ref<PowerState> getPowerState() {
            return Ref.of(ON);
        }

        public Ref<String> getBiosVersion() {
            return Ref.of("1");
        }

        public Ref<String> getManufacturer() {
            return Ref.of("2");
        }

        public Ref<String> getModel() {
            return Ref.of("3");
        }

        public Ref<String> getSku() {
            return Ref.of("4");
        }

        public Ref<String> getSerialNumber() {
            return Ref.of("5");
        }

        public Ref<String> getPartNumber() {
            return Ref.of("6");
        }

        @SuppressWarnings("checkstyle:AnonInnerLength")
        public BootObject getBootObject() {
            return new BootObjectTest();
        }

        public Ref<LinkedHashSet<RedfishComputerSystem.Device>> getPciDevices() {
            return Ref.of(new LinkedHashSet<RedfishComputerSystem.Device>());
        }

        public ProcessorSummaryObject getProcessorSummary() {
            return new ProcessorSummaryTest();
        }

        public MemorySummaryObject getMemorySummary() {
            return new MemorySummaryObjectTest();
        }

        public Ref<Integer> getMemorySockets() {
            return Ref.of(7);
        }

        public Ref<List<String>> getPcieConnectionId() {
            return Ref.of(new ArrayList<String>() {
                private static final long serialVersionUID = -6715565183879841907L;

                {
                    add("XYU.5.KLO");
                }
            });
        }

        public Ref<Integer> getProcessorSockets() {
            return Ref.of(8);
        }

        public Ref<List<TrustedModule>> getTrustedModules() {
            return Ref.of(emptyList());
        }

        public Ref<Boolean> getUserModeEnabled() {
            return Ref.of(true);
        }

        public Ref<Boolean> getTrustedExecutionTechnologyEnabled() {
            return Ref.of(true);
        }

        public boolean isBasic() {
            return false;
        }

        public Ref<LinkedHashSet<ResetType>> getAllowableResetTypes() {
            return Ref.of(new LinkedHashSet<ResetType>());
        }

        public Ref<LinkedHashSet<InterfaceType>> getAllowableInterfaceTypes() {
            return Ref.of(new LinkedHashSet<InterfaceType>());
        }

        public Iterable<ResourceSupplier> getPcieDevices() throws WebClientRequestException {
            return null;
        }

        public Iterable<ResourceSupplier> getPcieFunctions() throws WebClientRequestException {
            return null;
        }

        public Iterable<ResourceSupplier> getNetworkInterfaces() throws WebClientRequestException {
            return null;
        }

        public ResourceSupplier getComputerSystemMetrics() throws WebClientRequestException {
            return null;
        }

        public Iterable<ResourceSupplier> getProcessors() throws WebClientRequestException {
            return null;
        }

        public Iterable<ResourceSupplier> getEthernetInterfaces() throws WebClientRequestException {
            return null;
        }

        public Iterable<ResourceSupplier> getMemoryModules() throws WebClientRequestException {
            return null;
        }

        public Iterable<ResourceSupplier> getSimpleStorages() throws WebClientRequestException {
            return null;
        }

        public Iterable<ResourceSupplier> getStorages() throws WebClientRequestException {
            return null;
        }

        public Iterable<ResourceSupplier> getEndpoints() throws WebClientRequestException {
            return null;
        }

        public Iterable<ResourceSupplier> getAdapters() throws WebClientRequestException {
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
    }

    public class BootObjectTest extends BootObject {
        public Ref<BootSourceState> getBootSourceOverrideEnabled() {
            return Ref.of(ONCE);
        }

        public Ref<BootSourceType> getBootSourceOverrideTarget() {
            return Ref.of(DIAGS);
        }

        public LinkedHashSet<BootSourceType> getBootSourceOverrideTargetAllowableValues() {
            return new LinkedHashSet<BootSourceType>() {
                private static final long serialVersionUID = 2217996478740345307L;

                {
                    add(BIOS_SETUP);
                }
            };
        }

        public Ref<BootSourceMode> getBootSourceOverrideMode() {
            return Ref.of(LEGACY);
        }

        public LinkedHashSet<BootSourceMode> getBootSourceOverrideModeAllowableValues() {
            return new LinkedHashSet<BootSourceMode>() {
                private static final long serialVersionUID = -7380914118594642708L;

                {
                    add(LEGACY);
                    add(UEFI);
                }
            };
        }
    }

    public class MemorySummaryObjectTest extends MemorySummaryObject {
        public Ref<BigDecimal> getTotalSystemMemoryGiB() {
            return Ref.of(BigDecimal.valueOf(4));
        }

        public Ref<Status> getStatus() {
            return Ref.of(statusFromString("State=Enabled,Health=OK,HealthRollup=OK"));
        }
    }

    public class ProcessorSummaryTest extends ProcessorSummaryObject {
        public Ref<String> getModel() {
            return Ref.of("7");
        }

        public Ref<Integer> getCount() {
            return Ref.of(1);
        }

        public Ref<Status> getStatus() {
            return Ref.of(statusFromString("State=Enabled,Health=OK,HealthRollup=OK"));
        }
    }
}
