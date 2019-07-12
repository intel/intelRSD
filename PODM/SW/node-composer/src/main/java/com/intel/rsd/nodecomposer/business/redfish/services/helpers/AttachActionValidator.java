/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.helpers;

import com.intel.rsd.nodecomposer.business.RequestValidationException;
import com.intel.rsd.nodecomposer.business.ResourceStateMismatchException;
import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.stereotype.Component;

import java.util.List;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static com.intel.rsd.nodecomposer.types.Protocol.FPGA_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static java.lang.String.format;
import static java.util.Arrays.asList;

@Component
public class AttachActionValidator<T extends DiscoverableEntity & AttachableAsset & ComposableAsset> {
    private static final List<Protocol> PROTOCOLS_ALLOWABLE_TO_ATTACH = asList(NVME, NVME_OVER_FABRICS, PCIE, FPGA_OVER_FABRICS);

    public void validateProtocolMatches(Protocol requestedProtocol, Protocol assetProtocol) throws RequestValidationException {
        if (assetProtocol != null && !PROTOCOLS_ALLOWABLE_TO_ATTACH.contains(assetProtocol)) {
            throw new RequestValidationException(
                createWithViolations(format("Only Assets of %s protocol are allowed to attach, while asset protocol %s",
                    PROTOCOLS_ALLOWABLE_TO_ATTACH, assetProtocol)));
        }

        if (requestedProtocol != null && !requestedProtocol.equals(assetProtocol)) {
            throw new RequestValidationException(createWithViolations(format("Asset protocol does not match chosen protocol: %s", requestedProtocol)));
        }
    }

    public void validateAssetAttachAbility(T asset) throws ResourceStateMismatchException {
        if (asset.isAllocated()) {
            throw new ResourceStateMismatchException("Selected Asset is currently in use!");
        }
    }

    public void validateAssetExists(T asset, ODataId resourceContext) throws ResourceStateMismatchException {
        if (asset == null) {
            throw new ResourceStateMismatchException(format("Asset with id %s does not exist", resourceContext));
        }
    }

    public void validateAssetPresence(T asset, ODataId resourceContext) throws ResourceStateMismatchException {
        if (!asset.isAchievable()) {
            throw new ResourceStateMismatchException(format("Asset with id %s should be enabled and healthy in order to invoke actions on it",
                resourceContext));
        }
    }

    public void checkAssetSpecificViolations(Violations assetSpecificViolations) throws RequestValidationException {
        if (assetSpecificViolations.hasViolations()) {
            throw new RequestValidationException(assetSpecificViolations);
        }
    }
}
