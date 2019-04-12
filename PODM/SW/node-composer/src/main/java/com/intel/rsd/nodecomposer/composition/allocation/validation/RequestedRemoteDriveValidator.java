/*
 * Copyright (c) 2017-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.function.Predicate;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class RequestedRemoteDriveValidator {
    private final Predicate<RemoteDrive> parametersPredicate;

    public RequestedRemoteDriveValidator() {
        this.parametersPredicate = createParametersPredicate();
    }

    public Violations validate(RemoteDrive requestedRemoteDrive) {
        Violations violations = new Violations();

        if (!parametersPredicate.test(requestedRemoteDrive)) {
            violations.addViolation("This set of parameters are not supported for allocation");
        }

        return violations;
    }

    private Predicate<RemoteDrive> createParametersPredicate() {
        return allocationWithExistingResource()
            .or(allocationWithMasterResource())
            .or(allocationWithProtocolAndExistingResource())
            .or(allocationWithNewDrive())
            .or(allocationWithProtocolAndMaster())
            .or(allocationWithProtocolAndCapacityAndMaster())
            .or(allocationWithCapacityAndMaster());
    }

    private Predicate<RemoteDrive> allocationWithExistingResource() {
        return RemoteDriveValidator.newRemoteDriveValidator()
            .resourceRequired()
            .build()
            .getRequestedParameters();
    }

    private Predicate<RemoteDrive> allocationWithMasterResource() {
        return RemoteDriveValidator.newRemoteDriveValidator()
            .masterRequired()
            .build()
            .getRequestedParameters();
    }

    private Predicate<RemoteDrive> allocationWithProtocolAndExistingResource() {
        return RemoteDriveValidator.newRemoteDriveValidator()
            .resourceRequired()
            .protocolRequired()
            .build()
            .getRequestedParameters();
    }

    private Predicate<RemoteDrive> allocationWithNewDrive() {
        return RemoteDriveValidator.newRemoteDriveValidator()
            .capacityGibRequired()
            .protocolRequired()
            .build()
            .getRequestedParameters();
    }

    private Predicate<RemoteDrive> allocationWithProtocolAndMaster() {
        return RemoteDriveValidator.newRemoteDriveValidator()
            .masterRequired()
            .protocolRequired()
            .build()
            .getRequestedParameters();
    }

    private Predicate<RemoteDrive> allocationWithProtocolAndCapacityAndMaster() {
        return RemoteDriveValidator.newRemoteDriveValidator()
            .masterRequired()
            .protocolRequired()
            .capacityGibRequired()
            .build()
            .getRequestedParameters();
    }

    private Predicate<RemoteDrive> allocationWithCapacityAndMaster() {
        return RemoteDriveValidator.newRemoteDriveValidator()
            .masterRequired()
            .capacityGibRequired()
            .build()
            .getRequestedParameters();
    }

    public static final class RemoteDriveValidator {
        private Predicate<RemoteDrive> requestedParameters;

        private RemoteDriveValidator(Builder builder) {
            this.requestedParameters = builder.capacityGib
                .and(builder.protocol)
                .and(builder.resource)
                .and(builder.master);
        }

        public Predicate<RemoteDrive> getRequestedParameters() {
            return requestedParameters;
        }

        public static Builder newRemoteDriveValidator() {
            return new Builder();
        }

        public static final class Builder {
            private Predicate<RemoteDrive> capacityGib = json -> json.getCapacityGib() == null;
            private Predicate<RemoteDrive> protocol = json -> json.getProtocol() == null;
            private Predicate<RemoteDrive> resource = json -> json.getResourceODataId() == null;
            private Predicate<RemoteDrive> master = json -> json.getMaster() == null;

            private Builder() {
            }

            public RemoteDriveValidator build() {
                return new RemoteDriveValidator(this);
            }

            public Builder capacityGibRequired() {
                this.capacityGib = json -> json.getCapacityGib() != null;
                return this;
            }

            public Builder protocolRequired() {
                this.protocol = json -> json.getProtocol() != null;
                return this;
            }

            public Builder resourceRequired() {
                this.resource = json -> json.getResourceODataId() != null;
                return this;
            }

            public Builder masterRequired() {
                this.master = json -> json.getMaster() != null;
                return this;
            }
        }
    }
}

