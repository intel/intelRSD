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

package com.intel.rsd.resourcemanager.layers.unifier.unification;

import java.util.Optional;

public interface OdataIdMapper {

    Optional<String> mapOdataId(String sourceOdataId);
    Optional<String> mapEntityId(String sourceOdataId);

    static OdataIdMapper createUnificationMapper(UnifiedOdataIdResolver resolver) {
        return new OdataIdMapper() {
            @Override
            public Optional<String> mapOdataId(String sourceOdataId) {
                return resolver.toUnified(sourceOdataId);
            }

            @Override
            public Optional<String> mapEntityId(String sourceOdataId) {
                return mapOdataId(sourceOdataId)
                    .map(unifiedOdataId -> unifiedOdataId.substring(unifiedOdataId.lastIndexOf("/") + 1).replace("+", " "));
            }
        };
    }

    static OdataIdMapper createDeunificationMapper(UnifiedOdataIdResolver resolver) {
        return new OdataIdMapper() {
            @Override
            public Optional<String> mapOdataId(String sourceOdataId) {
                return resolver.toServiceSpecific(sourceOdataId);
            }

            @Override
            public Optional<String> mapEntityId(String sourceOdataId) {
                throw new UnsupportedOperationException("De-unification of entity id is not implemented.");
            }
        };
    }
}
