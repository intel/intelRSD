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

package com.intel.rsd.nodecomposer.composition.allocation.validation;

import com.intel.rsd.nodecomposer.business.ODataIdResolvingException;
import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.dto.redfish.ODataIdPossessor;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive.MasterDrive;
import com.intel.rsd.nodecomposer.composition.allocation.CompositionException;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.ResourceFinderException;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.stream.Stream;

import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.Optional.ofNullable;
import static org.apache.commons.collections.CollectionUtils.isEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class RequestedNodeResourceContextsValidator {
    private final EntityTreeTraverser traverser;

    @Autowired
    public RequestedNodeResourceContextsValidator(EntityTreeTraverser traverser) {
        this.traverser = traverser;
    }

    public void validateExistenceOfIncludedResources(RequestedNode requestedNode) throws CompositionException {
        Set<ODataId> oDataIds = getIncludedResourcesODataIds(requestedNode);
        Violations violations = validateResources(oDataIds);

        if (violations.hasViolations()) {
            throw new ResourceFinderException(violations);
        }
    }

    private Set<ODataId> getIncludedResourcesODataIds(RequestedNode requestedNode) {
        Set<ODataId> oDataIds = new HashSet<>();
        //TODO: pzak - validate if ODataId refers to existing Processor and/or Chassis (UriConversionException)
        oDataIds.addAll(getODataIds(requestedNode.getProcessors()));
        //TODO: pzak - validate if ODataId refers to existing Memory and/or Chassis (UriConversionException)
        oDataIds.addAll(getODataIds(requestedNode.getMemoryModules()));
        //TODO: pzak - validate if ODataId refers to existing LogicalDrive (UriConversionException)
        oDataIds.addAll(collectRemoteDriveODataIds(requestedNode.getRemoteDrives()));
        //TODO: pzak - validate if ODataId refers to existing Drive/SimpleStorage and/or Chassis (UriConversionException)
        oDataIds.addAll(getODataIds(requestedNode.getLocalDrives()));
        //TODO: pzak - validate if ODataId refers to existing EthernetInterface (under ComputerSystem) and/or Chassis (UriConversionException)
        oDataIds.addAll(getODataIds(requestedNode.getEthernetInterfaces()));

        return oDataIds;
    }

    private List<ODataId> getODataIds(List<? extends ODataIdPossessor> oDataIdPossessors) {
        if (isEmpty(oDataIdPossessors)) {
            return emptyList();
        }

        List<ODataId> oDataIds = new ArrayList<>();
        oDataIdPossessors.forEach(oDataIdPossessor -> {
            if (oDataIdPossessor.getResourceODataId() != null) {
                oDataIds.add(oDataIdPossessor.getResourceODataId());
            }
            if (oDataIdPossessor.getChassisODataId() != null) {
                oDataIds.add(oDataIdPossessor.getChassisODataId());
            }
        });
        return oDataIds;
    }

    private List<ODataId> collectRemoteDriveODataIds(List<RemoteDrive> remoteDrives) {
        if (isEmpty(remoteDrives)) {
            return emptyList();
        }

        List<ODataId> remoteDriveODataIds = new ArrayList<>();
        remoteDrives.forEach(remoteDrive -> {
            ofNullable(remoteDrive.getResourceODataId()).ifPresent(remoteDriveODataIds::add);

            MasterDrive masterDrive = remoteDrive.getMaster();
            if (masterDrive != null) {
                ofNullable(masterDrive.getResourceODataId()).ifPresent(remoteDriveODataIds::add);
            }
        });

        return remoteDriveODataIds;
    }

    private Violations validateResources(Set<ODataId> oDataIds) {
        Violations violations = new Violations();

        for (ODataId oDataId : oDataIds) {
            try {
                DiscoverableEntity entity = traverser.traverseDiscoverableEntity(oDataId);
                validateResourcesAchievability(entity, violations);
            } catch (ODataIdResolvingException e) {
                violations.addViolation("Specified resource (" + oDataId + ") does not exist.");
            }
        }
        return violations;
    }

    private void validateResourcesAchievability(DiscoverableEntity entity, Violations violations) {
        Stream.of(entity)
            .filter(ComposableAsset.class::isInstance)
            .map(ComposableAsset.class::cast)
            .filter(asset -> !asset.isAchievable())
            .forEach(resource -> violations.addViolation(format("Specified resource (%s) is not achievable.", entity.getUri())));
    }
}
