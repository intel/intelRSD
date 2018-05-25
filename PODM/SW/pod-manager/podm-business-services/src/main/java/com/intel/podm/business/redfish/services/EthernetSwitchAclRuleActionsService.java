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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.entities.redfish.EthernetSwitchAcl;
import com.intel.podm.business.entities.redfish.EthernetSwitchAclRule;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.EthernetSwitchAclRuleActionsInvoker;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.actions.AclRuleModificationRequest;
import com.intel.podm.common.types.redfish.NavigationProperty;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Optional;
import java.util.Set;

import static com.intel.podm.business.Violations.createWithViolations;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.common.types.Ref.of;
import static com.intel.podm.common.types.Ref.unassigned;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchAclRuleActionsService {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EthernetSwitchAclRuleActionsInvoker invoker;

    @Transactional(REQUIRES_NEW)
    public Context create(Context creationalContext, RedfishEthernetSwitchAclRule representation) throws BusinessApiException {
        EthernetSwitchAcl ethernetSwitchAcl = (EthernetSwitchAcl) traverser.traverse(creationalContext);
        AclRuleModificationRequest aclRuleModificationRequest = buildRequest(representation);

        EthernetSwitchAclRule createdAclRule = invoker.create(ethernetSwitchAcl, aclRuleModificationRequest);
        ethernetSwitchAcl.addRule(createdAclRule);
        return toContext(createdAclRule);
    }

    @Transactional(REQUIRES_NEW)
    public void update(Context target, RedfishEthernetSwitchAclRule representation)
        throws ContextResolvingException, EntityOperationException, RequestValidationException {
        EthernetSwitchAclRule aclRule = (EthernetSwitchAclRule) traverser.traverse(target);
        AclRuleModificationRequest aclRuleModificationRequest = buildRequest(representation);
        invoker.update(aclRule, aclRuleModificationRequest);
    }

    private AclRuleModificationRequest buildRequest(RedfishEthernetSwitchAclRule representation)
        throws ContextResolvingException, RequestValidationException {
        Ref<URI> forwardPortUri = getForwardPortUri(representation);
        Ref<Set<URI>> mirrorPortRegionUris = getMirrorPortRegionUris(representation);

        return new AclRuleModificationRequest.Builder()
            .setRuleId(representation.getRuleId())
            .setActionType(representation.getActionType())
            .setForwardMirrorInterface(forwardPortUri)
            .setMirrorPortRegion(mirrorPortRegionUris)
            .setMirrorType(representation.getMirrorType())
            .setCondition(representation.getCondition())
            .build();
    }

    private Ref<Set<URI>> getMirrorPortRegionUris(RedfishEthernetSwitchAclRule representation) throws RequestValidationException {
        if (!representation.getMirrorPortRegions().isAssigned()) {
            return unassigned();
        }

        Set<? extends NavigationProperty> navigationProperties = representation.getMirrorPortRegions().get();
        if (navigationProperties == null) {
            return of(null);
        }

        return of(retrieveMirrorPortUris(navigationProperties));
    }

    private Set<URI> retrieveMirrorPortUris(Set<? extends NavigationProperty> navigationProperties) throws RequestValidationException {
        Set<Context> mirrorPortRegion = navigationProperties.stream()
                .map(navigationProperty -> (Context) navigationProperty)
                .collect(toSet());

        return getMirrorPortUris(mirrorPortRegion);
    }

    private Ref<URI> getForwardPortUri(RedfishEthernetSwitchAclRule representation) throws RequestValidationException {
        if (!representation.getForwardMirrorInterface().isAssigned()) {
            return unassigned();
        }

        Context forwardMirrorInterface = (Context) representation.getForwardMirrorInterface().get();
        if (forwardMirrorInterface == null) {
            return of(null);
        }

        return of(getPortSourceUriForForwardMirrorInterface(forwardMirrorInterface));
    }

    private URI getPortSourceUriForForwardMirrorInterface(Context forwardMirrorInterface) throws RequestValidationException {
        EthernetSwitchPort port = (EthernetSwitchPort) traverser.tryTraverse(forwardMirrorInterface)
            .orElseThrow(() -> new RequestValidationException(createWithViolations("Selected Forward Mirror Interface does not exist")));
        return port.getSourceUri();
    }

    private Set<URI> getMirrorPortUris(Set<Context> mirrorPortRegion) throws RequestValidationException {
        Set<Optional<EthernetSwitchPort>> ports = traverser.tryTraverse(mirrorPortRegion);
        for (Optional<EthernetSwitchPort> port : ports) {
            port.orElseThrow(() -> new RequestValidationException(createWithViolations("Selected Mirror Port Region cannot be found")));
        }
        return ports.stream().map(Optional::get).map(EthernetSwitchPort::getSourceUri).collect(toSet());
    }

    @Transactional(REQUIRES_NEW)
    public void deleteAclRule(Context aclRuleContext) throws ContextResolvingException, EntityOperationException {
        EthernetSwitchAclRule ethernetSwitchAclRule = (EthernetSwitchAclRule) traverser.traverse(aclRuleContext);
        invoker.deleteAclRule(ethernetSwitchAclRule);
    }
}
