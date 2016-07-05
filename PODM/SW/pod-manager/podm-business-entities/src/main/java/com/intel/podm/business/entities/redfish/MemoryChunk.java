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
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.types.AddressRangeType;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class MemoryChunk extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<String> MEMORY_CHUNK_NAME = stringProperty("memoryChunkName");
    public static final DomainObjectProperty<Integer> MEMORY_CHUNK_UID = integerProperty("memoryChunkUid");
    public static final DomainObjectProperty<Integer> MEMORY_CHUNK_SIZE_MIB = integerProperty("memoryChunkSizeMib");
    public static final DomainObjectProperty<AddressRangeType> ADDRESS_RANGE_TYPE = enumProperty("addressRangeType", AddressRangeType.class);
    public static final DomainObjectProperty<Boolean> IS_MIRROR_ENABLED = booleanProperty("isMirrorEnabled");
    public static final DomainObjectProperty<Boolean> IS_SPARE = booleanProperty("isSpare");

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

    public String getMemoryChunkName() {
        return getProperty(MEMORY_CHUNK_NAME);
    }

    public void setMemoryChunkName(String memoryChunkName) {
        setProperty(MEMORY_CHUNK_NAME, memoryChunkName);
    }

    public Integer getMemoryChunkUid() {
        return getProperty(MEMORY_CHUNK_UID);
    }

    public void setMemoryChunkUid(Integer memoryChunkUid) {
        setProperty(MEMORY_CHUNK_UID, memoryChunkUid);
    }

    public Integer getMemoryChunkSizeMib() {
        return getProperty(MEMORY_CHUNK_SIZE_MIB);
    }

    public void setMemoryChunkSizeMib(Integer memoryChunkSizeMib) {
        setProperty(MEMORY_CHUNK_SIZE_MIB, memoryChunkSizeMib);
    }

    public AddressRangeType getAddressRangeType() {
        return getProperty(ADDRESS_RANGE_TYPE);
    }

    public void setAddressRangeType(AddressRangeType addressRangeType) {
        setProperty(ADDRESS_RANGE_TYPE, addressRangeType);
    }

    public Boolean getIsMirrorEnabled() {
        return getProperty(IS_MIRROR_ENABLED);
    }

    public void setIsMirrorEnabled(Boolean isMirrorEnabled) {
        setProperty(IS_MIRROR_ENABLED, isMirrorEnabled);
    }

    public Boolean getIsSpare() {
        return getProperty(IS_SPARE);
    }

    public void setIsSpare(Boolean isSpare) {
        setProperty(IS_SPARE, isSpare);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public List<Memory> getInterleaveSets() {
        return single(getLinked(CONTAINED_BY, ComputerSystem.class)).getMemoryModules();
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
}
