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

import com.intel.podm.business.entities.EventOriginProvider;
import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.MultiSourceResource;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Protocol;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
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
@Table(name = "storage_controller", indexes = @Index(name = "idx_storage_controller_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
@Eventable
@NamedQueries({
    @NamedQuery(name = StorageController.GET_STORAGE_CONTROLLER_MULTI_SOURCE,
        query = "SELECT storageController "
            + "FROM StorageController storageController "
            + "WHERE storageController.multiSourceDiscriminator = :multiSourceDiscriminator "
            + "AND storageController.isComplementary = :isComplementary"
    )
})
public class StorageController extends DiscoverableEntity implements MultiSourceResource {
    public static final String GET_STORAGE_CONTROLLER_MULTI_SOURCE = "GET_STORAGE_CONTROLLER_MULTI_SOURCE";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "multi_source_discriminator")
    private String multiSourceDiscriminator;

    @Column(name = "member_id")
    private String memberId;

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

    @Column(name = "speed_gbps")
    private BigDecimal speedGbps;

    @Column(name = "firmware_version")
    private String firmwareVersion;

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "storage_controller_supported_controller_protocol", joinColumns = @JoinColumn(name = "storage_controller_id"))
    @Column(name = "supported_controller_protocol")
    @OrderColumn(name = "supported_controller_protocol_order")
    private List<Protocol> supportedControllerProtocols = new ArrayList<>();

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "storage_controller_supported_device_protocol", joinColumns = @JoinColumn(name = "storage_controller_id"))
    @Column(name = "supported_device_protocol")
    @OrderColumn(name = "supported_device_protocol_order")
    private List<Protocol> supportedDeviceProtocols = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "storage_controller_identifier", joinColumns = @JoinColumn(name = "storage_controller_id"))
    @OrderColumn(name = "identifier_order")
    private List<Identifier> identifiers = new ArrayList<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "storage_controller_pcie_device_function",
        joinColumns = {@JoinColumn(name = "storage_controller_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "pcie_device_function_id", referencedColumnName = "id")})
    private Set<PcieDeviceFunction> pcieDeviceFunctions = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_id")
    private Storage storage;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_adapter_id")
    private Storage storageAdapter;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        this.entityId = id;
    }

    public String getMemberId() {
        /*
            workaround for backward compatibility
            adapter does not show property called memberId
         */
        if (memberId == null) {
            return entityId.getValue();
        }

        return memberId;
    }

    public void setMemberId(String memberId) {
        this.memberId = memberId;
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

    public BigDecimal getSpeedGbps() {
        return speedGbps;
    }

    public void setSpeedGbps(BigDecimal speedGbps) {
        this.speedGbps = speedGbps;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.firmwareVersion = firmwareVersion;
    }

    public Collection<Protocol> getSupportedControllerProtocols() {
        return supportedControllerProtocols;
    }

    public void addSupportedControllerProtocol(Protocol supportedControllerProtocol) {
        this.supportedControllerProtocols.add(supportedControllerProtocol);
    }

    public Collection<Protocol> getSupportedDeviceProtocols() {
        return supportedDeviceProtocols;
    }

    public void addSupportedDeviceProtocol(Protocol supportedDeviceProtocol) {
        this.supportedDeviceProtocols.add(supportedDeviceProtocol);
    }

    public List<Identifier> getIdentifiers() {
        return identifiers;
    }

    public void addIdentifier(Identifier identifier) {
        identifiers.add(identifier);
    }

    public Set<PcieDeviceFunction> getPcieDeviceFunctions() {
        return pcieDeviceFunctions;
    }

    public void addPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        requiresNonNull(pcieDeviceFunction, "pcieDeviceFunction");

        pcieDeviceFunctions.add(pcieDeviceFunction);
        if (!pcieDeviceFunction.getStorageControllers().contains(this)) {
            pcieDeviceFunction.addStorageController(this);
        }
    }

    public void unlinkPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        if (pcieDeviceFunctions.contains(pcieDeviceFunction)) {
            pcieDeviceFunctions.remove(pcieDeviceFunction);
            if (pcieDeviceFunction != null) {
                pcieDeviceFunction.unlinkStorageController(this);
            }
        }
    }

    @EventOriginProvider
    public Storage getStorage() {
        return storage;
    }

    public void setStorage(Storage storage) {
        if (!Objects.equals(this.storage, storage)) {
            unlinkStorage(this.storage);
            this.storage = storage;
            if (storage != null && !storage.getStorageControllers().contains(this)) {
                storage.addStorageController(this);
            }
        }
    }

    public void unlinkStorage(Storage storage) {
        if (Objects.equals(this.storage, storage)) {
            this.storage = null;
            if (storage != null) {
                storage.unlinkStorageController(this);
            }
        }
    }

    public Storage getStorageAdapter() {
        return storageAdapter;
    }

    public void setStorageAdapter(Storage storageAdapter) {
        if (!Objects.equals(this.storageAdapter, storageAdapter)) {
            unlinkStorageAdapter(this.storageAdapter);
            this.storageAdapter = storageAdapter;
            if (storageAdapter != null && !storageAdapter.getAdapters().contains(this)) {
                storageAdapter.addAdapter(this);
            }
        }
    }

    public void unlinkStorageAdapter(Storage storage) {
        if (Objects.equals(this.storageAdapter, storage)) {
            this.storageAdapter = null;
            if (storage != null) {
                storage.unlinkAdapter(this);
            }
        }
    }

    @Override
    public String getMultiSourceDiscriminator() {
        return multiSourceDiscriminator;
    }

    @Override
    public void setMultiSourceDiscriminator(String multiSourceDiscriminator) {
        this.multiSourceDiscriminator = multiSourceDiscriminator;
    }

    @Override
    public void preRemove() {
        unlinkCollection(pcieDeviceFunctions, this::unlinkPcieDeviceFunction);
        unlinkStorage(storage);
        unlinkStorageAdapter(storageAdapter);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
