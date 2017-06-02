/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.business.entities.redfish.Fan;
import com.intel.podm.business.entities.redfish.Power;
import com.intel.podm.business.entities.redfish.PowerControl;
import com.intel.podm.business.entities.redfish.PowerSupply;
import com.intel.podm.business.entities.redfish.PowerVoltage;
import com.intel.podm.business.entities.redfish.PowerZone;
import com.intel.podm.business.entities.redfish.PowerZonePowerSupply;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.RemoteTargetIscsiAddress;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.StorageController;
import com.intel.podm.business.entities.redfish.Temperature;
import com.intel.podm.business.entities.redfish.Thermal;
import com.intel.podm.business.entities.redfish.ThermalFan;
import com.intel.podm.business.entities.redfish.ThermalTemperature;
import com.intel.podm.business.entities.redfish.ThermalZone;
import com.intel.podm.business.entities.redfish.base.Entity;
import org.hamcrest.Matchers;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.util.Random;

import static com.intel.podm.common.types.events.EventType.ALERT;
import static com.intel.podm.common.types.events.EventType.RESOURCE_ADDED;
import static com.intel.podm.common.types.events.EventType.RESOURCE_REMOVED;
import static com.intel.podm.common.types.events.EventType.RESOURCE_UPDATED;
import static com.intel.podm.common.types.events.EventType.STATUS_CHANGE;
import static java.lang.System.currentTimeMillis;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.core.IsEqual.equalTo;
import static org.mockito.internal.util.reflection.Whitebox.setInternalState;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:ClassFanOutComplexity", "checkstyle:ClassDataAbstractionCoupling", "checkstyle:MethodCount"})
public class EventOriginInfoProviderTest {

    private static final Random RANDOM = new Random(currentTimeMillis());

    private EventOriginInfoProvider configProvider;

    private EventTypeConverter eventTypeConverter = new EventTypeConverter();

    @BeforeClass
    public void setUp() throws Exception {
        this.configProvider = createEventOriginInfoProviderWithMockedData();
    }

    @Test
    public void testFindEventSourceEntityForPowerControl_PowerIsExpected() throws Exception {
        PowerControl powerControl = createEntity(PowerControl.class);
        Power power = createEntity(Power.class);
        power.addPowerControl(powerControl);
        assertThat(configProvider.findEventOrigin(powerControl), Matchers.equalTo(power));
    }

    @Test
    public void testFindEventSourceEntityForPowerZonePowerSupply_PowerZoneIsExpected() throws Exception {
        PowerZonePowerSupply powerSupply = createEntity(PowerZonePowerSupply.class);
        PowerZone powerZone = createEntity(PowerZone.class);
        powerZone.addPowerSupply(powerSupply);
        assertThat(configProvider.findEventOrigin(powerSupply), Matchers.equalTo(powerZone));
    }

    @Test
    public void testFindEventSourceEntityForFan_ThermalZoneIsExpected() throws Exception {
        Fan fan = createEntity(Fan.class);
        ThermalZone thermalZone = createEntity(ThermalZone.class);
        thermalZone.addFan(fan);
        assertThat(configProvider.findEventOrigin(fan), Matchers.equalTo(thermalZone));
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
        StorageController storageController = createEntity(StorageController.class);
        Storage storage = createEntity(Storage.class);
        storage.addStorageController(storageController);
        assertThat(configProvider.findEventOrigin(storageController), Matchers.equalTo(storage));
    }

    @Test
    public void testFindEventSourceEntityForTemperature_ThermalZoneIsExpected() throws Exception {
        ThermalZone thermalZone = createEntity(ThermalZone.class);
        Temperature temperature = createEntity(Temperature.class);
        thermalZone.addTemperature(temperature);
        assertThat(configProvider.findEventOrigin(temperature), Matchers.equalTo(thermalZone));
    }

    @Test
    public void testFindEventSourceEntityForRemoteTargetIscsiAddress_RemoteTargetIsExpected() throws Exception {
        RemoteTarget remoteTarget = createEntity(RemoteTarget.class);
        RemoteTargetIscsiAddress remoteTargetIscsiAddress = createEntity(RemoteTargetIscsiAddress.class);
        remoteTarget.addRemoteTargetIscsiAddress(remoteTargetIscsiAddress);
        assertThat(configProvider.findEventOrigin(remoteTargetIscsiAddress), Matchers.equalTo(remoteTarget));
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
    public void whenFindConfiguredEventTypeForMultiSourceEntityWithNoResolver_shouldReturnTheSameEntity() throws Exception {
        ComputerSystem computerSystem = createEntity(ComputerSystem.class);
        EventOriginInfoProvider eventOriginInfoProvider = createEventOriginInfoProviderWithMockedData();

        assertThat(eventOriginInfoProvider.findEventOrigin(computerSystem), equalTo(computerSystem));
    }

    private static <A extends Entity> A createEntity(Class<A> aClass) throws IllegalAccessException, InstantiationException {
        A instance = aClass.newInstance();
        setInternalState(instance, "id", RANDOM.nextLong());
        return instance;
    }

    private EventOriginInfoProvider createEventOriginInfoProviderWithMockedData() {
        return new EventOriginInfoProvider();
    }
}
