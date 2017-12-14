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

import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.embeddables.MemoryLocation;
import com.intel.podm.business.entities.redfish.embeddables.Region;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.MemoryLocationObject;
import com.intel.podm.client.resources.redfish.MemoryRegionObject;
import com.intel.podm.client.resources.redfish.MemoryResource;
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.MemoryClassification;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.MemoryType;
import com.intel.podm.common.types.OperatingMemoryMode;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.mappers.subresources.RegionMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import java.math.BigDecimal;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.BaseModuleType.DIMM_SO_32B;
import static com.intel.podm.common.types.ErrorCorrection.SINGLE_BIT_ECC;
import static com.intel.podm.common.types.MemoryClassification.VOLATILE;
import static com.intel.podm.common.types.MemoryDeviceType.DDR4;
import static com.intel.podm.common.types.MemoryMedia.DRAM;
import static com.intel.podm.common.types.MemoryMedia.NAND;
import static com.intel.podm.common.types.MemoryType.NVDIMM_P;
import static com.intel.podm.common.types.OperatingMemoryMode.BLOCK;
import static com.intel.podm.common.types.Status.statusFromString;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNotNull;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({
    "checkstyle:MethodName", "checkstyle:MethodLength", "checkstyle:MagicNumber", "checkstyle:ExecutableStatementCount",
    "checkstyle:MethodCount", "checkstyle:AnonInnerLength", "checkstyle:ClassFanOutComplexity"
})
public class MemoryMapperTest {
    @InjectMocks
    MemoryMapper mapper;

    @InjectMocks
    EmbeddableCleanAndCreateStrategy<Region> embeddableCleanAndCreateStrategy;

    @BeforeTest
    public void initMocks() {
        MockitoAnnotations.initMocks(this);
        mapper.regionMapper = new RegionMapper(embeddableCleanAndCreateStrategy);
        mapper.simpleTypeMapper = new SimpleTypeMapper();
    }

    @Test
    public void whenMappingFromSourceToTarget_shouldMapAllProperties() throws Exception {
        MemoryTestResource source = new MemoryTestResource();
        Memory target = new Memory();

        mapper.map(source, target);

        assertEquals(source.getName(), target.getName());
        assertEquals(source.getDescription(), target.getDescription());
        assertEquals(source.getMemoryDeviceType().get(), target.getMemoryDeviceType());
        assertEquals(source.getBaseModuleType().get(), target.getBaseModuleType());
        assertEquals(source.getDataWidthBits().get(), target.getDataWidthBits());
        assertEquals(source.getBusWidthBits().get(), target.getBusWidthBits());
        assertEquals(source.getManufacturer().get(), target.getManufacturer());
        assertEquals(source.getSerialNumber().get(), target.getSerialNumber());
        assertEquals(source.getPartNumber().get(), target.getPartNumber());
        assertEquals(source.getAllowedSpeedsMhz().get(), target.getAllowedSpeedsMhz());
        assertEquals(source.getDeviceLocator().get(), target.getDeviceLocator());
        assertEquals(source.getRankCount().get(), target.getRankCount());
        assertEquals(source.getErrorCorrection().get(), target.getErrorCorrection());
        assertEquals(source.getVoltageVolt().get(), target.getVoltageVolt());

        assertEquals(source.getMemoryType().get(), target.getMemoryType());
        assertEquals(source.getMemoryMedia().get(), target.getMemoryMedia());
        assertEquals(source.getCapacityMib().get(), target.getCapacityMib());
        assertEquals(source.getStatus().get(), target.getStatus());
        assertEquals(source.getOperatingSpeedMhz().get(), target.getOperatingSpeedMhz());
        assertTrue(source.getRegions().isAssigned());
        assertEquals(source.getRegions().get().size(), target.getRegions().size());
        assertEquals(source.getFirmwareRevision().get(), target.getFirmwareRevision());
        assertEquals(source.getFirmwareApiVersion().get(), target.getFirmwareApiVersion());
        assertEquals(source.getFunctionClasses().get(), target.getFunctionClasses());
        assertEquals(source.getVendorId().get(), target.getVendorId());
        assertEquals(source.getDeviceId().get(), target.getDeviceId());
        assertEquals(source.getOperatingMemoryModes().get(), target.getOperatingMemoryModes());

        assertEquals(source.getMemoryLocation().getSocket().get(), target.getMemoryLocation().getSocket());
        assertEquals(source.getMemoryLocation().getMemoryController().get(), target.getMemoryLocation().getMemoryController());
        assertEquals(source.getMemoryLocation().getChannel().get(), target.getMemoryLocation().getChannel());
        assertEquals(source.getMemoryLocation().getSlot().get(), target.getMemoryLocation().getSlot());
    }

    @Test
    public void whenPropertyIsUnassigned_shouldBeIgnoredDuringMapping() throws Exception {
        Integer channel = 1;

        MemoryLocation targetMemoryLocation = new MemoryLocation();
        targetMemoryLocation.setChannel(channel);

        Memory target = new Memory();
        target.setMemoryLocation(targetMemoryLocation);

        MemoryTestResource source = prepareMockedSourceWithMemoryLocation();

        mapper.map(source, target);

        assertNotNull(target.getMemoryLocation().getChannel());
        assertEquals(target.getMemoryLocation().getChannel(), channel);
    }

    @Test
    public void whenListPropertyIsUnassigned_shouldBeIgnoredDuringMapping() throws Exception {
        String regionId = "test_id";

        Region targetRegion = new Region();
        targetRegion.setRegionId(regionId);

        Memory target = new Memory();
        target.addRegion(targetRegion);

        MemoryTestResource source = prepareMockResourceWithRegions();

        mapper.map(source, target);

        assertNotNull(target.getRegions());
        assertEquals(target.getRegions().get(0).getRegionId(), regionId);
    }

    private MemoryTestResource prepareMockedSourceWithMemoryLocation() {
        MemoryTestResource source = mock(MemoryTestResource.class);
        MemoryLocationObject sourceMemoryLocation = mock(MemoryLocationObject.class);
        when(source.getMemoryLocation()).thenReturn(sourceMemoryLocation);
        when(source.getRegions()).thenReturn(Ref.unassigned());
        when(sourceMemoryLocation.getChannel()).thenReturn(Ref.unassigned());
        return prepareCollections(source);
    }

    private MemoryTestResource prepareMockResourceWithRegions() {
        MemoryTestResource source = mock(MemoryTestResource.class);
        MemoryRegionObject sourceRegionObject = mock(MemoryRegionObject.class);
        when(sourceRegionObject.getRegionId()).thenReturn(Ref.unassigned());
        when(sourceRegionObject.getMemoryClassification()).thenReturn(Ref.unassigned());
        when(sourceRegionObject.getOffsetMib()).thenReturn(Ref.unassigned());
        when(sourceRegionObject.getSizeMib()).thenReturn(Ref.unassigned());

        when(source.getRegions()).thenReturn(Ref.of(new ArrayList<MemoryRegionObject>() {{
            add(sourceRegionObject);
        }}));
        return prepareCollections(source);
    }

    private MemoryTestResource prepareCollections(MemoryTestResource source) {
        when(source.getMemoryMedia()).thenReturn(Ref.of(new ArrayList<MemoryMedia>() {{
            add(DRAM);
            add(NAND);
        }}));
        when(source.getAllowedSpeedsMhz()).thenReturn(Ref.of(new ArrayList<Integer>() {{
            add(1);
        }}));
        when(source.getFunctionClasses()).thenReturn(Ref.of(new ArrayList<String>() {{
            add("test_fc1");
        }}));
        when(source.getOperatingMemoryModes()).thenReturn(Ref.of(new ArrayList<OperatingMemoryMode>() {{
            add(BLOCK);
        }}));

        return source;
    }

    @SuppressWarnings({"checkstyle:MethodCount", "checkstyle:MagicNumber", "checkstyle:AnonInnerLength"})
    static class MemoryTestResource extends MemoryResource {
        public Ref<MemoryDeviceType> getMemoryDeviceType() {
            return Ref.of(DDR4);
        }

        public Ref<BaseModuleType> getBaseModuleType() {
            return Ref.of(DIMM_SO_32B);
        }

        public Ref<Integer> getDataWidthBits() {
            return Ref.of(1);
        }

        public Ref<Integer> getBusWidthBits() {
            return Ref.of(1);
        }

        public Ref<String> getManufacturer() {
            return Ref.of("test_man");
        }

        public Ref<String> getSerialNumber() {
            return Ref.of("test_sn");
        }

        public Ref<String> getPartNumber() {
            return Ref.of("test_pn");
        }

        public Ref<List<Integer>> getAllowedSpeedsMhz() {
            return Ref.of(new ArrayList<Integer>() {
                {
                    add(1);
                }
            });
        }

        public Ref<String> getDeviceLocator() {
            return Ref.of("test_dl");
        }

        public Ref<Integer> getRankCount() {
            return Ref.of(2);
        }

        public Ref<ErrorCorrection> getErrorCorrection() {
            return Ref.of(SINGLE_BIT_ECC);
        }

        public Ref<BigDecimal> getVoltageVolt() {
            return Ref.of(new BigDecimal(3));
        }

        public MemoryLocationObject getMemoryLocation() {
            return new MemoryLocationObject() {
                public Ref<Integer> getSocket() {
                    return Ref.of(11);
                }

                public Ref<Integer> getMemoryController() {
                    return Ref.of(21);
                }

                public Ref<Integer> getChannel() {
                    return Ref.of(13);
                }

                public Ref<Integer> getSlot() {
                    return Ref.of(14);
                }
            };
        }

        public Ref<MemoryType> getMemoryType() {
            return Ref.of(NVDIMM_P);
        }

        public Ref<List<MemoryMedia>> getMemoryMedia() {
            return Ref.of(new ArrayList<MemoryMedia>() {{
                add(DRAM);
                add(NAND);
            }});
        }

        public Ref<Integer> getCapacityMib() {
            return Ref.of(4);
        }

        public Ref<Status> getStatus() {
            return Ref.of(statusFromString("State=Enabled,Health=OK,HealthRollup=OK"));
        }

        public Ref<Integer> getOperatingSpeedMhz() {
            return Ref.of(5);
        }

        public Ref<List<MemoryRegionObject>> getRegions() {
            return Ref.of(new ArrayList<MemoryRegionObject>() {{
                add(new MemoryRegionObject() {
                    public Ref<String> getRegionId() {
                        return Ref.of("test_reg");
                    }

                    public Ref<MemoryClassification> getMemoryClassification() {
                        return Ref.of(VOLATILE);
                    }

                    public Ref<Integer> getOffsetMib() {
                        return Ref.of(21);
                    }

                    public Ref<Integer> getSizeMib() {
                        return Ref.of(22);
                    }
                });
            }});
        }

        public Ref<String> getFirmwareRevision() {
            return Ref.of("test_fr");
        }

        public Ref<String> getFirmwareApiVersion() {
            return Ref.of("test_fav");
        }

        public Ref<List<String>> getFunctionClasses() {
            return Ref.of(new ArrayList<String>() {{
                add("test_fc1");
            }});
        }

        public Ref<String> getVendorId() {
            return Ref.of("test_vi");
        }

        public Ref<String> getDeviceId() {
            return Ref.of("test_di");
        }

        public Ref<List<OperatingMemoryMode>> getOperatingMemoryModes() {
            return Ref.of(new ArrayList<OperatingMemoryMode>() {{
                add(BLOCK);
            }});
        }

        public ResourceSupplier getComputerSystem() {
            return null;
        }

        public ResourceSupplier getMemoryMetrics() {
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
            return "test_1";
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
