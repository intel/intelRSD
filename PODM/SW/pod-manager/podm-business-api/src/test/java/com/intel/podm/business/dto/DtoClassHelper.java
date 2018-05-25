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

package com.intel.podm.business.dto;

import java.util.Set;

import static com.intel.podm.common.utils.ClassGatherer.getAllClassesByPackageAndSuperTypeWithTheirInnerClasses;
import static com.intel.podm.common.utils.ClassGatherer.getAllFieldsTypesDeeplyFromClassesDeclaredInPackage;
import static java.util.Collections.unmodifiableSet;

public final class DtoClassHelper {
    private static final String POD_MANAGER_DTO_PACKAGE = "com.intel.podm.business.*";
    private static final Set<Class<?>> DTO_CLASSES_USED_IN_JSONS;

    static {
        Set<Class<?>> classes = getAllClassesByPackageAndSuperTypeWithTheirInnerClasses(POD_MANAGER_DTO_PACKAGE, RedfishDto.class);
        classes.addAll(getAllFieldsTypesDeeplyFromClassesDeclaredInPackage(classes, POD_MANAGER_DTO_PACKAGE));

        DTO_CLASSES_USED_IN_JSONS = unmodifiableSet(classes);
    }

    private DtoClassHelper() {
    }

    public static Set<Class<?>> getDtoClassesUsedInJsons() {
        return DTO_CLASSES_USED_IN_JSONS;
    }
}
