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
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = EthernetSwitch.GET_ALL_ETHERNET_SWITCH_IDS,
        query = "SELECT ethernetSwitch.entityId FROM EthernetSwitch ethernetSwitch")
})
@Table(name = "ethernet_switch", indexes = @Index(name = "idx_ethernet_switch_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class EthernetSwitch extends DiscoverableEntity {
    public static final String GET_ALL_ETHERNET_SWITCH_IDS = "GET_ALL_ETHERNET_SWITCH_IDS";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "switch_id")
    private String switchId;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "manufacturing_date")
    private String manufacturingDate;

    @Column(name = "model")
    private String model;

    @Column(name = "serial_number")
    private String serialNumber;

    @Column(name = "part_number")
    private String partNumber;

    @Column(name = "firmware_name")
    private String firmwareName;

    @Column(name = "firmware_version")
    private String firmwareVersion;

    @Column(name = "role")
    private String role;

    @SuppressEvents
    @OneToMany(mappedBy = "ethernetSwitch", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchPort> ports = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "ethernetSwitch", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchAcl> acls = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_switch_metrics_id")
    private EthernetSwitchMetrics ethernetSwitchMetrics;

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "ethernet_switch_manager",
        joinColumns = {@JoinColumn(name = "ethernet_switch_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "manager_id", referencedColumnName = "id")})
    private Set<Manager> managers = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "chassis_id")
    private Chassis chassis;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public String getSwitchId() {
        return switchId;
    }

    public void setSwitchId(String switchId) {
        this.switchId = switchId;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getManufacturingDate() {
        return manufacturingDate;
    }

    public void setManufacturingDate(String manufacturingDate) {
        this.manufacturingDate = manufacturingDate;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
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

    public String getFirmwareName() {
        return firmwareName;
    }

    public void setFirmwareName(String firmwareName) {
        this.firmwareName = firmwareName;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String version) {
        this.firmwareVersion = version;
    }

    public String getRole() {
        return role;
    }

    public void setRole(String role) {
        this.role = role;
    }

    public Set<EthernetSwitchPort> getPorts() {
        return ports;
    }

    public void addPort(EthernetSwitchPort ethernetSwitchPort) {
        requiresNonNull(ethernetSwitchPort, "ethernetSwitchPort");

        ports.add(ethernetSwitchPort);
        if (!this.equals(ethernetSwitchPort.getEthernetSwitch())) {
            ethernetSwitchPort.setEthernetSwitch(this);
        }
    }

    public void unlinkPort(EthernetSwitchPort ethernetSwitchPort) {
        if (ports.contains(ethernetSwitchPort)) {
            ports.remove(ethernetSwitchPort);
            if (ethernetSwitchPort != null) {
                ethernetSwitchPort.unlinkEthernetSwitch(this);
            }
        }
    }

    public EthernetSwitchMetrics getEthernetSwitchMetrics() {
        return ethernetSwitchMetrics;
    }

    public void setSwitchMetrics(EthernetSwitchMetrics ethernetSwitchMetrics) {
        if (!Objects.equals(this.ethernetSwitchMetrics, ethernetSwitchMetrics)) {
            unlinkEthernetSwitchMetrics(this.ethernetSwitchMetrics);
            this.ethernetSwitchMetrics = ethernetSwitchMetrics;
            if (ethernetSwitchMetrics != null && !this.equals(ethernetSwitchMetrics.getEthernetSwitch())) {
                ethernetSwitchMetrics.setEthernetSwitch(this);
            }
        }
    }

    public void unlinkEthernetSwitchMetrics(EthernetSwitchMetrics ethernetSwitchMetrics) {
        if (Objects.equals(this.ethernetSwitchMetrics, ethernetSwitchMetrics)) {
            this.ethernetSwitchMetrics = null;
            if (ethernetSwitchMetrics != null) {
                ethernetSwitchMetrics.unlinkEthernetSwitch(this);
            }
        }
    }

    public Set<EthernetSwitchAcl> getAcls() {
        return acls;
    }

    public void addAcl(EthernetSwitchAcl ethernetSwitchAcl) {
        requiresNonNull(ethernetSwitchAcl, "ethernetSwitchAcl");

        acls.add(ethernetSwitchAcl);
        if (!this.equals(ethernetSwitchAcl.getEthernetSwitch())) {
            ethernetSwitchAcl.setEthernetSwitch(this);
        }
    }

    public void unlinkAcl(EthernetSwitchAcl ethernetSwitchAcl) {
        if (acls.contains(ethernetSwitchAcl)) {
            acls.remove(ethernetSwitchAcl);
            if (ethernetSwitchAcl != null) {
                ethernetSwitchAcl.unlinkEthernetSwitch(this);
            }
        }
    }

    public Set<Manager> getManagers() {
        return managers;
    }

    public void addManager(Manager manager) {
        requiresNonNull(manager, "manager");

        managers.add(manager);
        if (!manager.getEthernetSwitches().contains(this)) {
            manager.addEthernetSwitch(this);
        }
    }

    public void unlinkManager(Manager manager) {
        if (managers.contains(manager)) {
            managers.remove(manager);
            if (manager != null) {
                manager.unlinkEthernetSwitch(this);
            }
        }
    }

    public Chassis getChassis() {
        return chassis;
    }

    public void setChassis(Chassis chassis) {
        if (!Objects.equals(this.chassis, chassis)) {
            unlinkChassis(this.chassis);
            this.chassis = chassis;
            if (chassis != null && !chassis.getEthernetSwitches().contains(this)) {
                chassis.addEthernetSwitch(this);
            }
        }
    }

    public void unlinkChassis(Chassis chassis) {
        if (Objects.equals(this.chassis, chassis)) {
            this.chassis = null;
            if (chassis != null) {
                chassis.unlinkEthernetSwitch(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(ports, this::unlinkPort);
        unlinkCollection(acls, this::unlinkAcl);
        unlinkCollection(managers, this::unlinkManager);
        unlinkChassis(chassis);
        unlinkEthernetSwitchMetrics(ethernetSwitchMetrics);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, chassis);
    }
}
