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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.SuppressEvents;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.Resettable;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.actions.ResetType;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "switch", indexes = @Index(name = "idx_switch_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
@Eventable
public class Switch extends DiscoverableEntity implements Resettable {
    private static final String ENTITY_NAME = "Switch";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "model")
    private String model;

    @Column(name = "sku")
    private String sku;

    @Column(name = "serial_number")
    private String serialNumber;

    @Column(name = "part_number")
    private String partNumber;

    @Column(name = "asset_tag")
    private String assetTag;

    @Column(name = "domain_id")
    private Integer domainId;

    @Column(name = "switch_type")
    @Enumerated(STRING)
    private Protocol switchType;

    @Column(name = "is_managed")
    private Boolean isManaged;

    @Column(name = "total_switch_width")
    private Integer totalSwitchWidth;

    @Column(name = "indicator_led")
    @Enumerated(STRING)
    private IndicatorLed indicatorLed;

    @Column(name = "power_state")
    @Enumerated(STRING)
    private PowerState powerState;

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "switch_allowable_reset_type", joinColumns = @JoinColumn(name = "switch_id"))
    @Column(name = "allowable_reset_type")
    @OrderColumn(name = "allowable_reset_type_order")
    private List<ResetType> allowableResetTypes = new ArrayList<>();

    @SuppressEvents
    @OneToMany(mappedBy = "fabricSwitch", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Port> ports = new HashSet<>();

    @ManyToMany(mappedBy = "fabricSwitches", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Chassis> chassis = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "switch_zone",
        joinColumns = {@JoinColumn(name = "switch_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "zone_id", referencedColumnName = "id")})
    private Set<Zone> zones = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "switch_manager",
        joinColumns = {@JoinColumn(name = "switch_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "manager_id", referencedColumnName = "id")})
    private Set<Manager> managers = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "fabric_id")
    private Fabric fabric;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
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

    public String getSku() {
        return sku;
    }

    public void setSku(String sku) {
        this.sku = sku;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public void setPartNumber(String partNumber) {
        this.partNumber = partNumber;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
    }

    public Integer getDomainId() {
        return domainId;
    }

    public void setDomainId(Integer domainId) {
        this.domainId = domainId;
    }

    public Protocol getSwitchType() {
        return switchType;
    }

    public void setSwitchType(Protocol switchType) {
        this.switchType = switchType;
    }

    public Boolean getManaged() {
        return isManaged;
    }

    public void setManaged(Boolean managed) {
        isManaged = managed;
    }

    public Integer getTotalSwitchWidth() {
        return totalSwitchWidth;
    }

    public void setTotalSwitchWidth(Integer totalSwitchWidth) {
        this.totalSwitchWidth = totalSwitchWidth;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public void setIndicatorLed(IndicatorLed indicatorLed) {
        this.indicatorLed = indicatorLed;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public void setPowerState(PowerState powerState) {
        this.powerState = powerState;
    }

    @Override
    public List<ResetType> getAllowableResetTypes() {
        return allowableResetTypes;
    }

    public void addAllowableResetTypes(ResetType allowableResetType) {
        this.allowableResetTypes.add(allowableResetType);
    }

    public Set<Port> getPorts() {
        return ports;
    }

    public void addPort(Port port) {
        requiresNonNull(port, "port");
        ports.add(port);
        if (!this.equals(port.getSwitch())) {
            port.setSwitch(this);
        }
    }

    public void unlinkPort(Port port) {
        if (ports.contains(port)) {
            ports.remove(port);
            if (port != null) {
                port.unlinkSwitch(this);
            }
        }
    }

    public Set<Chassis> getChassis() {
        return chassis;
    }

    public void addChassis(Chassis newChassis) {
        requiresNonNull(newChassis, "newChassis");

        chassis.add(newChassis);
        if (!newChassis.getSwitch().contains(this)) {
            newChassis.addSwitch(this);
        }
    }

    public void unlinkChassis(Chassis chassisToUnlink) {
        if (chassis.contains(chassisToUnlink)) {
            chassis.remove(chassisToUnlink);
            if (chassisToUnlink != null) {
                chassisToUnlink.unlinkSwitch(this);
            }
        }
    }

    public Set<Zone> getZones() {
        return zones;
    }

    public void addZone(Zone zone) {
        requiresNonNull(zone, "zone");

        zones.add(zone);
        if (!zone.getSwitches().contains(this)) {
            zone.addSwitch(this);
        }
    }

    public void unlinkZone(Zone zone) {
        if (zones.contains(zone)) {
            zones.remove(zone);
            if (zone != null) {
                zone.unlinkSwitch(this);
            }
        }
    }

    public Set<Manager> getManagers() {
        return managers;
    }

    public void addManager(Manager manager) {
        requiresNonNull(manager, "manager");

        managers.add(manager);
        if (!manager.getSwitches().contains(this)) {
            manager.addSwitch(this);
        }
    }

    public void unlinkManager(Manager manager) {
        if (managers.contains(manager)) {
            managers.remove(manager);
            if (manager != null) {
                manager.unlinkSwitch(this);
            }
        }
    }

    public Fabric getFabric() {
        return fabric;
    }

    public void setFabric(Fabric fabric) {
        if (fabric == null) {
            unlinkFabric(this.fabric);
        } else {
            this.fabric = fabric;
            if (!fabric.getSwitches().contains(this)) {
                fabric.addSwitch(this);
            }
        }
    }

    public void unlinkFabric(Fabric fabric) {
        if (Objects.equals(this.fabric, fabric)) {
            this.fabric = null;
            if (fabric != null) {
                fabric.unlinkSwitch(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(ports, this::unlinkPort);
        unlinkCollection(chassis, this::unlinkChassis);
        unlinkCollection(zones, this::unlinkZone);
        unlinkCollection(managers, this::unlinkManager);
        unlinkFabric(fabric);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, fabric);
    }

    @Override
    public String getResetName() {
        return ENTITY_NAME;
    }
}
