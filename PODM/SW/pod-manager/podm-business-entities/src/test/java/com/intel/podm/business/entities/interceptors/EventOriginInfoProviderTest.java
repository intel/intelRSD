/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.entities.interceptors;

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Power;
import com.intel.podm.business.entities.redfish.PowerControl;
import com.intel.podm.business.entities.redfish.PowerSupply;
import com.intel.podm.business.entities.redfish.PowerVoltage;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.StorageController;
import com.intel.podm.business.entities.redfish.Thermal;
import com.intel.podm.business.entities.redfish.ThermalFan;
import com.intel.podm.business.entities.redfish.ThermalTemperature;
import com.intel.podm.business.entities.resolvers.ComputerSystemMultiSourceResolver;
import com.intel.podm.business.entities.resolvers.MultiSourceEntityResolver;
import com.intel.podm.business.entities.resolvers.MultiSourceEntityResolverProvider;
import com.intel.podm.business.entities.resolvers.StorageMultiSourceResolver;
import org.hamcrest.Matchers;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.business.entities.BusinessEntitiesTestsHelper.createEntity;
import static com.intel.podm.common.types.events.EventType.ALERT;
import static com.intel.podm.common.types.events.EventType.RESOURCE_ADDED;
import static com.intel.podm.common.types.events.EventType.RESOURCE_REMOVED;
import static com.intel.podm.common.types.events.EventType.RESOURCE_UPDATED;
import static com.intel.podm.common.types.events.EventType.STATUS_CHANGE;
import static java.util.Collections.singletonList;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.core.IsEqual.equalTo;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:ClassFanOutComplexity", "checkstyle:ClassDataAbstractionCoupling", "checkstyle:MethodCount"})
public class EventOriginInfoProviderTest {

    private EventOriginInfoProvider configProvider;

    private EventTypeConverter eventTypeConverter = new EventTypeConverter();

    @BeforeClass
    public void setUp() throws Exception {
        this.configProvider = createEventOriginInfoProviderWithMockedData(new ArrayList<>());
    }

    @Test
    public void testFindEventSourceEntityForPowerControl_PowerIsExpected() throws Exception {
        PowerControl powerControl = createEntity(PowerControl.class);
        Power power = createEntity(Power.class);
        power.addPowerControl(powerControl);
        assertThat(configProvider.findEventOrigin(powerControl), Matchers.equalTo(power));
    }

    @Test
    public void testFindEventSourceEntityForPowerSupply_PowerIsExpected() throws Exception {
        PowerSupply powerSupply = createEntity(PowerSupply.class);
        Power power = createEntity(Power.class);
        power.addPowerSupply(powerSupply);
        assertThat(configProvider.findEventOrigin(powerSupply), Matchers.equalTo(power));
    }

    @Test
    public void testFindEventSourceEntityForPowerVoltage_PowerIsExpected() throws Exception {
        PowerVoltage powerVoltage = createEntity(PowerVoltage.class);
        Power power = createEntity(Power.class);
        power.addVoltage(powerVoltage);
        assertThat(configProvider.findEventOrigin(powerVoltage), Matchers.equalTo(power));
    }

    @Test
    public void testFindEventSourceEntityForStorageController_StorageIsExpected() throws Exception {
        StorageController powerController = createEntity(StorageController.class);
        Storage storage = createEntity(Storage.class);
        storage.addStorageController(powerController);
        assertThat(configProvider.findEventOrigin(powerController), Matchers.equalTo(storage));
    }

    @Test
    public void testFindEventSourceEntityForThermalFan_ThermalIsExpected() throws Exception {
        Thermal thermal = createEntity(Thermal.class);
        ThermalFan thermalFan = createEntity(ThermalFan.class);
        thermal.addFan(thermalFan);
        assertThat(configProvider.findEventOrigin(thermalFan), Matchers.equalTo(thermal));
    }

    @Test
    public void testFindEventSourceEntityForThermalTemperature_ThermalIsExpected() throws Exception {
        Thermal thermal = createEntity(Thermal.class);
        ThermalTemperature thermalTemperature = createEntity(ThermalTemperature.class);
        thermal.addTemperature(thermalTemperature);
        assertThat(configProvider.findEventOrigin(thermalTemperature), Matchers.equalTo(thermal));
    }

    @Test
    public void testFindEventSourceEntityForComposedNode_ComposedNodeIsExpected() throws Exception {
        ComposedNode node = createEntity(ComposedNode.class);
        assertThat(configProvider.findEventOrigin(node), Matchers.equalTo(node));
    }

    @Test
    public void testDetermineConfiguredEventTypeForEventableExtension_anyEventShouldBeConvertedToResourceUpdated() {
        PowerVoltage expandableEntity = new PowerVoltage();
        Power power = new Power();
        power.addVoltage(expandableEntity);

        assertThat(eventTypeConverter.apply(expandableEntity, RESOURCE_ADDED), equalTo(RESOURCE_UPDATED));
        assertThat(eventTypeConverter.apply(expandableEntity, RESOURCE_UPDATED), equalTo(RESOURCE_UPDATED));
        assertThat(eventTypeConverter.apply(expandableEntity, RESOURCE_REMOVED), equalTo(RESOURCE_UPDATED));
        assertThat(eventTypeConverter.apply(expandableEntity, STATUS_CHANGE), equalTo(STATUS_CHANGE));
        assertThat(eventTypeConverter.apply(expandableEntity, ALERT), equalTo(ALERT));

    }

    @Test
    public void testDetermineConfiguredEventTypeForEventable_anyEventTypeShouldNotBeConverted() {
        ComposedNode node = new ComposedNode();

        assertThat(eventTypeConverter.apply(node, RESOURCE_ADDED), equalTo(RESOURCE_ADDED));
        assertThat(eventTypeConverter.apply(node, RESOURCE_UPDATED), equalTo(RESOURCE_UPDATED));
        assertThat(eventTypeConverter.apply(node, RESOURCE_REMOVED), equalTo(RESOURCE_REMOVED));
        assertThat(eventTypeConverter.apply(node, STATUS_CHANGE), equalTo(STATUS_CHANGE));
        assertThat(eventTypeConverter.apply(node, ALERT), equalTo(ALERT));
    }

    @Test
    public void whenFindConfiguredEventTypeForMultiSourceEntity_shouldReturnPrimaryEntity() throws Exception {
        ComputerSystem primaryComputerSystem = createEntity(ComputerSystem.class);
        ComputerSystem complementaryComputerSystem = createEntity(ComputerSystem.class);

        ComputerSystemMultiSourceResolver multiSourceResolver = createComputerSystemMultiSourceResolverMock(primaryComputerSystem,
            complementaryComputerSystem);
        List<MultiSourceEntityResolver> multiSourceEntityResolvers = singletonList(multiSourceResolver);

        EventOriginInfoProvider eventOriginInfoProvider = createEventOriginInfoProviderWithMockedData(multiSourceEntityResolvers);

        assertThat(eventOriginInfoProvider.findEventOrigin(complementaryComputerSystem), equalTo(primaryComputerSystem));
    }

    @Test
    public void whenFindConfiguredEventTypeForMultiSourceAndEmbeddedEntity_shouldReturnPrimaryEntity() throws Exception {
        StorageController complementaryStorageController = createEntity(StorageController.class);
        Storage complementaryStorage = createEntity(Storage.class);
        complementaryStorage.addStorageController(complementaryStorageController);
        Storage primaryStorage = createEntity(Storage.class);

        StorageMultiSourceResolver multiSourceResolver = createStorageMultiSourceResolverMock(complementaryStorage, primaryStorage);
        List<MultiSourceEntityResolver> multiSourceEntityResolvers = singletonList(multiSourceResolver);

        EventOriginInfoProvider eventOriginInfoProvider = createEventOriginInfoProviderWithMockedData(multiSourceEntityResolvers);

        assertThat(eventOriginInfoProvider.findEventOrigin(complementaryStorageController), equalTo(primaryStorage));

    }

    @Test
    public void whenFindConfiguredEventTypeForMultiSourceEntityWithNoResolver_shouldReturnTheSameEntity() throws Exception {
        ComputerSystem computerSystem = createEntity(ComputerSystem.class);
        EventOriginInfoProvider eventOriginInfoProvider = createEventOriginInfoProviderWithMockedData(new ArrayList<>());

        assertThat(eventOriginInfoProvider.findEventOrigin(computerSystem), equalTo(computerSystem));
    }

    private ComputerSystemMultiSourceResolver createComputerSystemMultiSourceResolverMock(ComputerSystem primaryComputerSystem,
                                                                                          ComputerSystem complementaryComputerSystem) {
        ComputerSystemMultiSourceResolver resolver = mock(ComputerSystemMultiSourceResolver.class);
        when(resolver.apply(complementaryComputerSystem)).thenReturn(primaryComputerSystem);
        when(resolver.getEntityClass()).thenReturn(ComputerSystem.class);

        return resolver;
    }

    private StorageMultiSourceResolver createStorageMultiSourceResolverMock(Storage complementaryStorage, Storage primaryStorage) {
        StorageMultiSourceResolver resolver = mock(StorageMultiSourceResolver.class);
        when(resolver.apply(complementaryStorage)).thenReturn(primaryStorage);
        when(resolver.getEntityClass()).thenReturn(Storage.class);

        return resolver;
    }

    private EventOriginInfoProvider createEventOriginInfoProviderWithMockedData(List<MultiSourceEntityResolver> multiSourceEntityResolvers) {
        MultiSourceEntityResolverProvider multiSourceEntityResolverProvider = mock(MultiSourceEntityResolverProvider.class);
        when(multiSourceEntityResolverProvider.getCachedMultiSourceEntityResolvers()).thenReturn(multiSourceEntityResolvers);
        return new EventOriginInfoProvider(multiSourceEntityResolverProvider);
    }
}
