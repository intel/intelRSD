/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.Manageable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.business.entities.redfish.properties.RackChassisAttributes;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.podm.business.entities.base.DomainObjectLink.COMPUTER_SYSTEM;
import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectLink.MANAGED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.MANAGERS_IN_CHASSIS;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class Chassis extends DomainObject implements Discoverable, Manageable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<String> NAME = stringProperty("name");
    public static final DomainObjectProperty<ChassisType> CHASSIS_TYPE = enumProperty("chassisType", ChassisType.class);
    public static final DomainObjectProperty<String> DESCRIPTION = stringProperty("description");
    public static final DomainObjectProperty<String> MANUFACTURER = stringProperty("manufacturer");
    public static final DomainObjectProperty<String> MODEL = stringProperty("model");
    public static final DomainObjectProperty<String> SKU = stringProperty("sku");
    public static final DomainObjectProperty<String> SERIAL_NUMBER = stringProperty("serialNumber");
    public static final DomainObjectProperty<String> PART_NUMBER = stringProperty("partNumber");
    public static final DomainObjectProperty<String> ASSET_TAG = stringProperty("assetTag");
    public static final DomainObjectProperty<IndicatorLed> INDICATOR_LED = enumProperty("indicatorLed", IndicatorLed.class);
    public static final DomainObjectProperty<String> POWER_STATE = stringProperty("powerState");
    public static final DomainObjectProperty<String> LOCATION_ID = stringProperty("locationId");
    public static final DomainObjectProperty<String> LOCATION_PARENT_ID = stringProperty("locationParentId");

    @Override
    public String getName() {
        return getProperty(NAME);
    }

    @Override
    public void setName(String name) {
        setProperty(NAME, name);
    }

    @Override
    public String getDescription() {
        return getProperty(DESCRIPTION);
    }

    @Override
    public void setDescription(String description) {
        setProperty(DESCRIPTION, description);
    }

    public ChassisType getChassisType() {
        return getProperty(CHASSIS_TYPE);
    }

    public void setChassisType(ChassisType chassisType) {
        setProperty(CHASSIS_TYPE, chassisType);
    }

    public String getManufacturer() {
        return getProperty(MANUFACTURER);
    }

    public void setManufacturer(String manufacturer) {
        setProperty(MANUFACTURER, manufacturer);
    }

    public String getModel() {
        return getProperty(MODEL);
    }

    public void setModel(String model) {
        setProperty(MODEL, model);
    }

    public String getSku() {
        return getProperty(SKU);
    }

    public void setSku(String sku) {
        setProperty(SKU, sku);
    }

    public String getSerialNumber() {
        return getProperty(SERIAL_NUMBER);
    }

    public void setSerialNumber(String serialNumber) {
        setProperty(SERIAL_NUMBER, serialNumber);
    }

    public String getPartNumber() {
        return getProperty(PART_NUMBER);
    }

    public void setPartNumber(String partNumber) {
        setProperty(PART_NUMBER, partNumber);
    }

    public String getAssetTag() {
        return getProperty(ASSET_TAG);
    }

    public void setAssetTag(String assetTag) {
        setProperty(ASSET_TAG, assetTag);
    }

    public IndicatorLed getIndicatorLed() {
        return getProperty(INDICATOR_LED);
    }

    public void setIndicatorLed(IndicatorLed indicatorLed) {
        setProperty(INDICATOR_LED, indicatorLed);
    }

    public String getLocationId() {
        return getProperty(LOCATION_ID);
    }

    public void setLocationId(String locationId) {
        setProperty(LOCATION_ID, locationId);
    }

    public String getLocationParentId() {
        return getProperty(LOCATION_PARENT_ID);
    }

    public void setLocationParentId(String locationParentId) {
        setProperty(LOCATION_PARENT_ID, locationParentId);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public String getPowerState() {
        return getProperty(POWER_STATE);
    }

    public void setPowerState(String powerState) {
        setProperty(POWER_STATE, powerState);
    }

    public void contain(Chassis chassis) {
        // unlink all contained by links from target chassis
        //todo: this should be handled in a more generic way
        for (DomainObject domainObject : chassis.getLinked(CONTAINED_BY)) {
            chassis.unlink(CONTAINED_BY, domainObject);
        }

        link(CONTAINS, chassis);
    }

    public List<Chassis> getContainedChassis() {
        return getLinked(CONTAINS, Chassis.class);
    }

    public void linkEthernetSwitch(EthernetSwitch ethernetSwitch) {
        link(CONTAINS, ethernetSwitch);
    }

    public List<EthernetSwitch> getContainedSwitches() {
        return getLinked(CONTAINS, EthernetSwitch.class);
    }

    public Chassis getContainedBy() {
        return singleOrNull(getLinked(CONTAINED_BY));
    }

    public List<ComputerSystem> getComputerSystems() {
        return getLinked(COMPUTER_SYSTEM, ComputerSystem.class);
    }

    public RackChassisAttributes getRackChassisAttributes() {
        return singleOrNull(getLinked(CONTAINS, RackChassisAttributes.class));
    }

    public void setRackChassisAttributes(RackChassisAttributes rackChassisAttributes) {
        link(CONTAINS, rackChassisAttributes);
    }

    @Override
    public URI getSourceUri() {
        return getProperty(SOURCE_URI);
    }

    @Override
    public void setSourceUri(URI sourceUri) {
        setProperty(SOURCE_URI, sourceUri);
    }

    @Override
    public ExternalService getService() {
        return singleOrNull(getLinked(OWNED_BY, ExternalService.class));
    }

    public void link(ComputerSystem computerSystem) {
        link(COMPUTER_SYSTEM, computerSystem);
    }

    @Override
    public List<Manager> getManagers() {
        return getLinked(MANAGED_BY);
    }

    public List<Manager> getManagersInChassis() {
        return getLinked(MANAGERS_IN_CHASSIS, Manager.class);
    }

    @Override
    public void addManager(Manager manager) {
        link(MANAGED_BY, manager);
    }

    public void linkThermalZone(ThermalZone thermalZone) {
        link(CONTAINS, thermalZone);
    }

    public Collection<ThermalZone> getThermalZones() {
        return getLinked(CONTAINS, ThermalZone.class);
    }

    public List<PowerZone> getPowerZones() {
        return getLinked(CONTAINS, PowerZone.class);
    }

    public void linkPowerZone(PowerZone powerZone) {
        link(CONTAINS, powerZone);
    }

    public Set<ComputerSystem> getAllComputerSystemsUnderneath() {
        return getAllComputerSystemsUnderneathChassis(this, newHashSet());
    }

    private Set<ComputerSystem> getAllComputerSystemsUnderneathChassis(Chassis chassis, Set<Chassis> visitedChassis) {
        Set<ComputerSystem> computerSystems = newHashSet(chassis.getComputerSystems());
        visitedChassis.add(chassis);

        chassis.getContainedChassis().stream()
                .filter(containedChassis -> !visitedChassis.contains(containedChassis))
                .forEach(containedChassis -> computerSystems.addAll(getAllComputerSystemsUnderneathChassis(containedChassis, visitedChassis)));
        return computerSystems;
    }

    public Set<Chassis> getAllOnTopOfChassis() {
        Set<Chassis> chassisSet = newHashSet();

        Chassis containedByChassis = getContainedBy();
        while (containedByChassis != null && !chassisSet.contains(containedByChassis)) {
            chassisSet.add(containedByChassis);
            containedByChassis = containedByChassis.getContainedBy();
        }

        return chassisSet;
    }

    public boolean is(ChassisType chassisType) {
        return Objects.equals(getChassisType(), chassisType);
    }

    public void unlinkContainedChassis(Chassis chassis) {
        unlink(CONTAINS, chassis);
    }
}
