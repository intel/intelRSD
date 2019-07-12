/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.converters.IdToLongConverter;
import com.intel.rsd.nodecomposer.persistence.converters.TaggedValuesMapConverter;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import com.intel.rsd.nodecomposer.types.AttachableType;
import com.intel.rsd.nodecomposer.types.ComposedNodeState;
import com.intel.rsd.nodecomposer.types.Id;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.Getter;
import lombok.Setter;
import org.hibernate.annotations.Generated;
import org.springframework.lang.Nullable;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.Convert;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToMany;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.PostPersist;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.net.URI;
import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import java.util.TreeMap;
import java.util.UUID;

import static com.intel.rsd.json.JsonUtils.stringToJsonNode;
import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromString;
import static com.intel.rsd.nodecomposer.types.Health.CRITICAL;
import static com.intel.rsd.nodecomposer.types.State.UNAVAILABLE_OFFLINE;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.Boolean.TRUE;
import static java.util.Arrays.stream;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toSet;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;
import static org.hibernate.annotations.GenerationTime.INSERT;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = ComposedNode.GET_ALL_NODES_URIS,
        query = "SELECT composedNode.uri FROM ComposedNode composedNode ORDER BY composedNode.composedNodeId"),
    @NamedQuery(name = ComposedNode.GET_NODE_MATCHING_RELATED_SYSTEM_UUID,
        query = "SELECT composedNode FROM ComposedNode composedNode WHERE composedNode.associatedComputerSystemUuid = :uuid"),
    @NamedQuery(name = ComposedNode.GET_NODES_ELIGIBLE_FOR_RECOVERY,
        query = "SELECT composedNode FROM ComposedNode composedNode WHERE composedNode.eligibleForRecovery = true")
})
@Table(name = "composed_node", indexes = {
    @Index(name = "idx_composed_node_composed_node_id", columnList = "composed_node_id", unique = true),
    @Index(name = "idx_composed_node_uri", columnList = "uri", unique = true),
})
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class ComposedNode extends Entity {
    public static final String GET_ALL_NODES_URIS = "GET_ALL_NODES_URIS";
    public static final String GET_NODE_MATCHING_RELATED_SYSTEM_UUID = "GET_NODE_MATCHING_RELATED_SYSTEM_UUID";
    public static final String GET_NODES_ELIGIBLE_FOR_RECOVERY = "GET_NODES_ELIGIBLE_FOR_RECOVERY";
    public static final Status OFFLINE_CRITICAL_STATUS = new Status(UNAVAILABLE_OFFLINE, CRITICAL, null);

    @Generated(INSERT)
    @Convert(converter = IdToLongConverter.class)
    @Column(name = "composed_node_id", columnDefinition = ENTITY_ID_NUMERIC_COLUMN_DEFINITION, insertable = false, nullable = false)
    private Id composedNodeId;

    @Getter
    @Setter
    @Column(name = "uri")
    private ODataId uri;

    @Getter
    @Setter
    @Column(name = "name")
    private String name;

    @Getter
    @Setter
    @Column(name = "description")
    private String description;

    @Getter
    @Setter
    @Column(name = "status")
    private Status status;

    @Getter
    @Setter
    @Column(name = "composed_node_state")
    @Enumerated(STRING)
    private ComposedNodeState composedNodeState;

    @Getter
    @Setter
    @Column(name = "remote_drive_capacity_gib")
    private BigDecimal remoteDriveCapacityGib;

    @Getter
    @Setter
    @Column(name = "associated_computer_system_uuid")
    private UUID associatedComputerSystemUuid;

    @Getter
    @Setter
    @Column(name = "associated_remote_target_iqn")
    private String associatedRemoteTargetIqn;

    @Getter
    @Setter
    @Column(name = "eligible_for_recovery")
    private boolean eligibleForRecovery;

    @Getter
    @Setter
    @Column(name = "prior_untagged_vlan_id")
    private Integer priorUntaggedVlanId;

    @Getter
    @Setter
    @Column(name = "clear_tpm_on_delete")
    private boolean clearTpmOnDelete;

    @Getter
    @Setter
    @Column(name = "clear_optane_memory_on_delete")
    private boolean clearOptanePersistentMemoryOnDelete;

    @ElementCollection
    @CollectionTable(name = "associated_drive_uri", joinColumns = @JoinColumn(name = "associated_drive_uri_id"))
    @OrderColumn(name = "associated_drive_uri_order")
    @Column(name = "associated_drives_uris")
    private Set<URI> associatedDriveUris = new HashSet<>();

    @ElementCollection
    @CollectionTable(name = "associated_endpoint_uri", joinColumns = @JoinColumn(name = "associated_endpoint_uri_id"))
    @OrderColumn(name = "associated_endpoint_uri_order")
    @Column(name = "associated_endpoints_uris")
    private Set<URI> associatedEndpointsUris = new HashSet<>();

    @ElementCollection
    @CollectionTable(name = "associated_volume_uri", joinColumns = @JoinColumn(name = "associated_volume_uri_id"))
    @OrderColumn(name = "associated_volume_uri")
    @Column(name = "associated_volumes_uris")
    private Set<URI> associatedVolumesUris = new HashSet<>();

    @ElementCollection
    @CollectionTable(name = "associated_processor_uri", joinColumns = @JoinColumn(name = "associated_processor_uri_id"))
    @OrderColumn(name = "associated_processor_uri")
    @Column(name = "associated_processor_uris")
    private Set<URI> associatedProcessorUris = new HashSet<>();

    @OneToMany(mappedBy = "composedNode", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @OneToMany(mappedBy = "composedNode", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Volume> volumes = new HashSet<>();

    @OneToMany(mappedBy = "composedNode", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @OneToMany(mappedBy = "composedNode", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Processor> processors = new HashSet<>();

    @ManyToMany(mappedBy = "composedNodes", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StoragePool> storagePools = new HashSet<>();

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @Convert(converter = TaggedValuesMapConverter.class)
    @Column(name = "tagged_values", columnDefinition = "text")
    private TreeMap<String, String> taggedValues = new TreeMap<>();

    public Id getComposedNodeId() {
        return composedNodeId;
    }

    public Set<URI> getAssociatedDrivesUris() {
        return associatedDriveUris;
    }

    private void addAssociatedDriveUri(URI associatedDriveUri) {
        associatedDriveUris.add(associatedDriveUri);
    }

    public Set<URI> getAssociatedEndpointsUris() {
        return associatedEndpointsUris;
    }

    private void addAssociatedEndpointUri(URI associatedEndpointUri) {
        associatedEndpointsUris.add(associatedEndpointUri);
    }

    public Set<URI> getAssociatedVolumesUris() {
        return associatedVolumesUris;
    }

    private void addAssociatedVolumeUri(URI associatedVolumeUri) {
        associatedVolumesUris.add(associatedVolumeUri);
    }

    private void addAssociatedProcessorUri(URI associatedProcessorUri) {
        associatedProcessorUris.add(associatedProcessorUri);
    }

    public Set<URI> getAssociatedProcessorsUris() {
        return associatedProcessorUris;
    }

    public Set<Endpoint> getEndpoints() {
        return endpoints;
    }

    public void addEndpoint(Endpoint endpoint) {
        requiresNonNull(endpoint, "endpoint");

        endpoints.add(endpoint);
        if (!this.equals(endpoint.getComposedNode())) {
            endpoint.setComposedNode(this);
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (endpoints.contains(endpoint)) {
            endpoints.remove(endpoint);
            if (endpoint != null) {
                endpoint.unlinkComposedNode(this);
            }
        }
    }

    public Set<Volume> getVolumes() {
        return volumes;
    }

    public void addVolume(Volume volume) {
        requiresNonNull(volume, "volume");

        volumes.add(volume);
        if (!this.equals(volume.getComposedNode())) {
            volume.setComposedNode(this);
        }
    }

    public void unlinkVolume(Volume volume) {
        if (volumes.contains(volume)) {
            volumes.remove(volume);
            if (volume != null) {
                volume.unlinkComposedNode(this);
            }
        }
    }

    public Set<Processor> getProcessors() {
        return processors;
    }

    public void addProcessor(Processor processor) {
        requiresNonNull(processor, "processor");

        processors.add(processor);
        if (!this.equals(processor.getComposedNode())) {
            processor.setComposedNode(this);
        }
    }

    public void unlinkProcessor(Processor processor) {
        if (processors.contains(processor)) {
            processors.remove(processor);
            if (processor != null) {
                processor.unlinkComposedNode(this);
            }
        }
    }

    public Set<Drive> getDrives() {
        return drives;
    }

    public void addDrive(Drive drive) {
        requiresNonNull(drive, "drive");

        drives.add(drive);
        if (!this.equals(drive.getComposedNode())) {
            drive.setComposedNode(this);
        }
    }

    public void unlinkDrive(Drive drive) {
        if (drives.contains(drive)) {
            drives.remove(drive);
            if (drive != null) {
                drive.unlinkComposedNode(this);
            }
        }
    }

    public Set<StoragePool> getStoragePools() {
        return storagePools;
    }

    public void addStoragePool(StoragePool storagePool) {
        requiresNonNull(storagePool, "storagePool");

        storagePools.add(storagePool);
        if (!storagePool.getComposedNodes().contains(this)) {
            storagePool.addComposedNode(this);
        }
    }

    public void unlinkStoragePool(StoragePool storagePool) {
        if (storagePools.contains(storagePool)) {
            storagePools.remove(storagePool);
            if (storagePool != null) {
                storagePool.unlinkComposedNode(this);
            }
        }
    }

    @Nullable
    public ComputerSystem getComputerSystem() {
        return computerSystem;
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        if (!Objects.equals(this.computerSystem, computerSystem)) {
            unlinkComputerSystem(this.computerSystem);
            this.computerSystem = computerSystem;
            if (computerSystem != null && !this.equals(computerSystem.getComposedNode())) {
                computerSystem.setComposedNode(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                setStatus(OFFLINE_CRITICAL_STATUS);
                computerSystem.unlinkComposedNode(this);
            }
        }
    }

    public TreeMap<String, JsonNode> getTaggedValues() {
        TreeMap<String, JsonNode> treeMap = new TreeMap<>();
        for (Map.Entry<String, String> entry : taggedValues.entrySet()) {
            treeMap.put(entry.getKey(), stringToJsonNode(entry.getValue()));
        }
        return treeMap;
    }

    public void setTaggedValues(Map<String, JsonNode> taggedValues) {
        for (Map.Entry<String, JsonNode> entry : taggedValues.entrySet()) {
            String key = entry.getKey();
            JsonNode value = entry.getValue();
            if (value.isNull()) {
                this.taggedValues.remove(key);
            } else {
                this.taggedValues.put(key, value.toString());
            }
        }
    }

    public boolean isInAnyOfStates(ComposedNodeState... states) {
        ComposedNodeState actualComposedNodeState = getComposedNodeState();
        return actualComposedNodeState != null
            && stream(states).anyMatch(expectedComposedNodeState -> actualComposedNodeState == expectedComposedNodeState);
    }

    public Set<EthernetInterface> getEthernetInterfaces() {
        return computerSystem != null ? computerSystem.getEthernetInterfaces() : emptySet();
    }

    public Set<Drive> getLocalDrives() {
        if (computerSystem == null) {
            return emptySet();
        }

        return computerSystem.getStorages().stream()
            .map(Storage::getDrives)
            .flatMap(Collection::stream)
            .collect(toSet());
    }

    public Set<SimpleStorage> getSimpleStorages() {
        return computerSystem != null ? computerSystem.getSimpleStorages() : emptySet();
    }

    public Optional<ConnectedEntity> findAnyConnectedEntityWithBootableVolume() {
        return getEndpoints().stream()
            .flatMap(endpoint -> endpoint.getConnectedEntities().stream())
            .filter(this::hasLinkToBootableVolume)
            .findFirst();
    }

    private boolean hasLinkToBootableVolume(ConnectedEntity connectedEntity) {
        DiscoverableEntity discoverableEntity = connectedEntity.getEntityLink();

        if (discoverableEntity instanceof Volume) {
            Volume volume = (Volume) discoverableEntity;

            return Objects.equals(volume.getBootable(), TRUE);
        }

        return false;
    }

    @SuppressWarnings({"checkstyle:MethodLength"})
    public void deallocate(AttachableAsset asset) {
        AttachableType type = asset.getAttachableType();
        asset.setAllocated(false);
        switch (type) {
            case DRIVE:
                Drive drive = (Drive) asset;
                this.unlinkDrive(drive);
                this.getAssociatedDrivesUris().removeIf(uri -> Objects.equals(drive.getUri().toUri(), uri));
                break;
            case PROCESSOR:
                Processor processor = (Processor) asset;
                this.unlinkProcessor(processor);
                this.getAssociatedProcessorsUris().removeIf(uri -> Objects.equals(processor.getUri().toUri(), uri));
                break;
            case VOLUME:
                Volume volume = (Volume) asset;
                this.unlinkVolume(volume);
                this.getAssociatedVolumesUris().removeIf(uri -> Objects.equals(volume.getUri().toUri(), uri));
                break;
            case ENDPOINT:
                Endpoint endpoint = (Endpoint) asset;
                this.unlinkEndpoint(endpoint);
                this.getAssociatedEndpointsUris().removeIf(uri -> Objects.equals(endpoint.getUri().toUri(), uri));
                break;
            default:
                break;
        }
    }

    @SuppressWarnings({"checkstyle:MethodLength"})
    public void attachAsset(AttachableAsset asset) {
        AttachableType type = asset.getAttachableType();
        asset.setAllocated(true);
        switch (type) {
            case DRIVE:
                Drive drive = (Drive) asset;
                this.addDrive(drive);
                addAssociatedDriveUri(drive.getUri().toUri());
                break;
            case PROCESSOR:
                Processor processor = (Processor) asset;
                this.addProcessor(processor);
                addAssociatedProcessorUri(processor.getUri().toUri());
                break;
            case VOLUME:
                Volume volume = (Volume) asset;
                this.addVolume(volume);
                addAssociatedVolumeUri(volume.getUri().toUri());
                break;
            case ENDPOINT:
                Endpoint endpoint = (Endpoint) asset;
                this.addEndpoint(endpoint);
                addAssociatedEndpointUri(endpoint.getUri().toUri());
            default:
                break;
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkCollection(volumes, this::unlinkVolume);
        unlinkCollection(drives, this::unlinkDrive);
        unlinkCollection(storagePools, this::unlinkStoragePool);
        unlinkCollection(processors, this::unlinkProcessor);
        unlinkComputerSystem(computerSystem);
    }

    @PostPersist
    public void ensureUriIsSet() {
        if (uri == null) {
            uri = oDataIdFromString(String.format("/redfish/v1/Nodes/%s", getComposedNodeId()));
        }
    }
}
