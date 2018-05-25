/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.client;

import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.Ref;
import org.testng.annotations.Test;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.types.RefUtil.verifyRefField;
import static com.intel.podm.common.utils.ClassGatherer.getAllClassesByPackageAndSuperTypeWithTheirInnerClasses;
import static com.intel.podm.common.utils.ClassGatherer.getAllFieldsTypesDeeplyFromClassesDeclaredInPackage;
import static com.intel.podm.common.utils.FailManager.failWithMessageIfAnyError;
import static java.util.Arrays.stream;

public class RefJsonSetterTest {
    private static final String PODM_CLIENT_PACKAGE = "com.intel.podm.client.*";

    @Test
    public void refPropertiesShouldBeAnnotatedWithJsonSetter() {
        List<String> errors = new ArrayList<>();

        Set<Class<?>> classes = getAllClassesByPackageAndSuperTypeWithTheirInnerClasses(PODM_CLIENT_PACKAGE, ExternalServiceResourceImpl.class);
        classes.addAll(getAllFieldsTypesDeeplyFromClassesDeclaredInPackage(classes, PODM_CLIENT_PACKAGE));

        classes.forEach(clazz -> stream(clazz.getDeclaredFields())
            .filter(field -> Ref.class.isAssignableFrom(field.getType()))
            .forEach(field -> errors.addAll(verifyRefField(clazz, field)))
        );

        failWithMessageIfAnyError(errors, "ExternalResources");
    }
}
