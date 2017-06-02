/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.common.base.Objects;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;
import com.intel.podm.common.types.redfish.OemType;
import org.testng.annotations.Test;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.utils.ClassGatherer.getAllClassesByPackageAndSuperTypeWithTheirInnerClasses;
import static com.intel.podm.common.utils.ClassGatherer.getAllClassesBySuperType;
import static com.intel.podm.common.utils.ClassGatherer.getAllFieldsTypesDeeplyFromClassesDeclaredInPackage;
import static java.util.Collections.sort;
import static org.testng.Assert.fail;

public class OemInResourcesTest {
    private static final String PODM_CLIENT_PACKAGE = "com.intel.podm.client.*";

    @Test
    public void allOemFieldsMustHaveProperSuperClass() {
        List<String> errors = new ArrayList<>();

        Set<Class> classes = getAllClassesByPackageAndSuperTypeWithTheirInnerClasses(PODM_CLIENT_PACKAGE, ExternalServiceResourceImpl.class);
        classes.addAll(getAllFieldsTypesDeeplyFromClassesDeclaredInPackage(classes, PODM_CLIENT_PACKAGE));

        for (Class clazz : classes) {
            errors.addAll(checkIfAllOemFieldsHaveProperSuperClass(clazz));
        }

        if (!errors.isEmpty()) {
            sort(errors);
            String error = "\n" + errors.size() + " problem(s) found in ExternalResources:\n" + String.join("\n", errors);
            fail(error);
        }
    }

    @Test
    public void allRedfishOemSubclassesMustHaveOemTypeAnnotation() {
        List<String> errors = new ArrayList<>();

        Set<Class> classes = getAllClassesBySuperType(ExternalServiceResourceImpl.RedfishOem.class);
        for (Class clazz : classes) {
            errors.addAll(checkIfRedfishOemSubclassHasOemTypeAnnotation(clazz));
        }

        if (!errors.isEmpty()) {
            sort(errors);
            String error = "\n" + errors.size() + " problem(s) found in ExternalResources:\n" + String.join("\n", errors);
            fail(error);
        }
    }

    private List<String> checkIfAllOemFieldsHaveProperSuperClass(Class clazz) {
        List<String> errors = new ArrayList<>();

        for (Field field : clazz.getDeclaredFields()) {
            JsonProperty jsonProperty = field.getAnnotation(JsonProperty.class);
            for (OemInResourceFieldDetail oemInResourceFieldDetail : OemInResourceFieldDetail.values()) {
                if (isFieldToCheck(field, jsonProperty, oemInResourceFieldDetail)) {
                    if (!oemInResourceFieldDetail.getSuperClass().isAssignableFrom(field.getType())) {
                        errors.add(clazz.getName() + ": " + field.getName() + " field must extend "
                            + oemInResourceFieldDetail.getSuperClass().getName() + " class.");
                    }
                }
            }
        }

        return errors;
    }

    private boolean isFieldToCheck(Field field, JsonProperty jsonProperty, OemInResourceFieldDetail oemInResourceFieldDetail) {
        return !field.isAnnotationPresent(IgnoreAutomaticOem.class)
            && (field.getName().equals(oemInResourceFieldDetail.getFieldName())
            || (jsonProperty != null && Objects.equal(oemInResourceFieldDetail.getJsonPropertyValue(), jsonProperty.value())));
    }

    private List<String> checkIfRedfishOemSubclassHasOemTypeAnnotation(Class clazz) {
        List<String> errors = new ArrayList<>();

        if (!clazz.isAnnotationPresent(OemType.class)) {
            errors.add(clazz.getName() + ": class must have OemType annotation.");
        }

        return errors;
    }

    private enum OemInResourceFieldDetail {
        OEM("oem", "Oem", ExternalServiceResourceImpl.RedfishOem.class),
        LINKS("links", "Links", ExternalServiceResourceImpl.RedfishLinks.class),
        ACTIONS("actions", "Actions", ExternalServiceResourceImpl.RedfishActions.class);

        private final String fieldName;
        private final String jsonPropertyValue;
        private final Class<?> superClass;

        OemInResourceFieldDetail(String fieldName, String jsonPropertyValue, Class<?> superClass) {
            this.fieldName = fieldName;
            this.jsonPropertyValue = jsonPropertyValue;
            this.superClass = superClass;
        }

        public String getFieldName() {
            return fieldName;
        }

        public String getJsonPropertyValue() {
            return jsonPropertyValue;
        }

        public Class<?> getSuperClass() {
            return superClass;
        }
    }
}
