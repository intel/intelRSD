/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.google.common.base.Objects;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;
import com.intel.podm.common.types.redfish.OemType;
import org.testng.annotations.Test;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import static com.intel.podm.business.dto.DtoClassHelper.getDtoClassesUsedInJsons;
import static com.intel.podm.common.utils.ClassGatherer.getAllClassesBySuperType;
import static com.intel.podm.common.utils.FailManager.failWithMessageIfAnyError;
import static java.util.Arrays.stream;

public class OemInDtoTest {
    private static final String POD_MANAGER_DTO_PACKAGE = "com.intel.podm.business.*";

    @Test
    public void allOemFieldsMustHaveProperSuperClass() {
        List<String> errors = new ArrayList<>();

        getDtoClassesUsedInJsons().stream()
            .map(this::checkIfAllOemFieldsHaveProperSuperClass)
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "RedfishDto(s)");
    }

    @Test
    public void allRedfishOemSubclassesMustHaveOemTypeAnnotation() {
        List<String> errors = new ArrayList<>();

        Set<Class<?>> classes = getAllClassesBySuperType(RedfishDto.RedfishOemDto.class);
        classes.stream()
            .map(this::checkIfRedfishOemSubclassHasOemTypeAnnotation)
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "RedfishDto(s)");
    }

    private List<String> checkIfAllOemFieldsHaveProperSuperClass(Class<?> clazz) {
        List<String> errors = new ArrayList<>();

        for (Field field : clazz.getDeclaredFields()) {
            JsonProperty jsonProperty = field.getAnnotation(JsonProperty.class);
            stream(OemInJsonFieldDetail.values())
                .filter(oemInJsonFieldDetail -> isFieldToCheck(field, jsonProperty, oemInJsonFieldDetail))
                .filter(oemInJsonFieldDetail -> !oemInJsonFieldDetail.getSuperClass().isAssignableFrom(field.getType()))
                .map(oemInJsonFieldDetail -> clazz.getName() + ": " + field.getName() + " field must extend "
                    + oemInJsonFieldDetail.getSuperClass().getName() + " class.").forEach(errors::add);
        }

        return errors;
    }

    private boolean isFieldToCheck(Field field, JsonProperty jsonProperty, OemInJsonFieldDetail oemInJsonFieldDetail) {
        return !field.isAnnotationPresent(IgnoreAutomaticOem.class)
            && field.getName().equals(oemInJsonFieldDetail.getFieldName())
            || (jsonProperty != null && Objects.equal(oemInJsonFieldDetail.getJsonPropertyValue(), jsonProperty.value()));
    }

    private List<String> checkIfRedfishOemSubclassHasOemTypeAnnotation(Class<?> clazz) {
        List<String> errors = new ArrayList<>();

        if (!clazz.isAnnotationPresent(OemType.class)) {
            errors.add(clazz.getName() + ": class must have OemType annotation.");
        }

        return errors;
    }

    private enum OemInJsonFieldDetail {
        OEM("oem", "Oem", RedfishDto.RedfishOemDto.class),
        LINKS("links", "Links", RedfishDto.RedfishLinksDto.class),
        ACTIONS("actions", "Actions", RedfishDto.RedfishActionsDto.class);

        private final String fieldName;
        private final String jsonPropertyValue;
        private final Class<?> superClass;

        OemInJsonFieldDetail(String fieldName, String jsonPropertyValue, Class<?> superClass) {
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
