/*
 * Copyright (c) 2015 Intel Corporation
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
import com.intel.podm.business.entities.base.DomainObjectLink;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;
import java.util.Objects;
import java.util.UUID;

import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.macAddressProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.uriProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.uuidProperty;
import static java.lang.Boolean.TRUE;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class ExternalService extends DomainObject {
    public static final DomainObjectProperty<UUID> UUID = uuidProperty("uuid");
    public static final DomainObjectProperty<URI> BASE_URI = uriProperty("baseUri");
    public static final DomainObjectProperty<ServiceType> SERVICE_TYPE = enumProperty("type", ServiceType.class);
    public static final DomainObjectProperty<MacAddress> MAC_ADDRESS = macAddressProperty("macAddress");
    public static final DomainObjectProperty<Boolean> IS_DIRTY = booleanProperty("isDirty");

    public boolean getIsDirty() {
        return TRUE.equals(getProperty(IS_DIRTY));
    }

    public void setIsDirty(Boolean isDirty) {
        setProperty(IS_DIRTY, isDirty);
    }

    public UUID getUuid() {
        return getProperty(UUID);
    }

    public void setUuid(UUID uuid) {
        setProperty(UUID, uuid);
    }

    public URI getBaseUri() {
        return getProperty(BASE_URI);
    }

    public void setBaseUri(URI baseUri) {
        setProperty(BASE_URI, baseUri);
    }

    public ServiceType getServiceType() {
        return getProperty(SERVICE_TYPE);
    }

    public void setServiceType(ServiceType serviceType) {
        setProperty(SERVICE_TYPE, serviceType);
    }

    public MacAddress getMacAddress() {
        return getProperty(MAC_ADDRESS);
    }

    public void setMacAddress(MacAddress macAddress) {
        setProperty(MAC_ADDRESS, macAddress);
    }

    public <T extends DomainObject & Discoverable> List<T> getOwned(Class<T> clazz) {
        return getLinked(DomainObjectLink.OWNS, clazz);
    }

    public <T extends DomainObject> List<T> getAllOwnedDomainObjects() {
        return getLinked(DomainObjectLink.OWNS);
    }

    public <T extends DomainObject & Discoverable> T findOrCreate(URI uri, Class<T> clazz) {
        for (T domainObject : getOwned(clazz)) {
            URI sourceUri = domainObject.getSourceUri();

            if (Objects.equals(uri, sourceUri)) {
                return domainObject;
            }
        }

        T domainObject = addDomainObject(DomainObjectLink.OWNS, clazz);
        domainObject.setSourceUri(uri);

        return domainObject;
    }

    public List<Discoverable> getDiscoverables() {
        return getLinked(DomainObjectLink.OWNS).stream().
                filter(domainObject -> domainObject instanceof Discoverable).
                map(domainObject -> (Discoverable) domainObject).
                collect(toList());
    }

    @Override
    public String toString() {
        return format("ExternalService {UUID=%s, baseUri=%s, MAC=%s, type=%s}", getUuid(), getBaseUri(), getMacAddress(), getServiceType());
    }

}
