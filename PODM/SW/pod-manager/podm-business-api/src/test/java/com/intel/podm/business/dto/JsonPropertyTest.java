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

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.business.services.context.SingletonContext;
import org.testng.annotations.Test;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.dto.DtoClassHelper.getDtoClassesUsedInJsons;
import static com.intel.podm.common.utils.ClassGatherer.extractTypeOfField;
import static com.intel.podm.common.utils.FailManager.failWithMessageIfAnyError;
import static java.lang.Character.toUpperCase;
import static java.lang.reflect.Modifier.isAbstract;
import static java.lang.reflect.Modifier.isStatic;
import static java.util.Arrays.asList;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.collections4.CollectionUtils.subtract;

public class JsonPropertyTest {
    private List<String> defaultRedfishDtoFields = asList("@odata.context", "@odata.id");
    private List<Class<?>> typesToIgnoreForCheckingJsonPropertyOrderExistence = asList(
        Context.class, SingletonContext.class, ContextType.class, UnknownOemDto.class
    );
    private List<Class<?>> classesToIgnoreForCheckingJsonPropertyExistence = asList(
        RedfishDto.class, RedfishDto.RedfishLinksDto.class, RedfishDto.RedfishActionsDto.class
    );

    @Test
    public void allJsonPropertyOrderAnnotationsMustContainProperFields() {
        List<String> errors = new ArrayList<>();

        getDtoClassesUsedInJsons().stream()
            .filter(clazz -> !isAbstract(clazz.getModifiers()))
            .map(this::checkIfAllJsonPropertyOrderAnnotationsContainProperFields)
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "RedfishDto(s)");
    }

    @Test
    public void allJsonPropertyAnnotationsMustBeUsedOnlyWhenReallyNeeded() {
        List<String> errors = new ArrayList<>();

        getDtoClassesUsedInJsons().stream()
            .map(this::checkIfAllJsonPropertyAnnotationsAreUsedOnlyWhenReallyNeeded)
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "RedfishDto(s)");
    }

    private List<String> checkIfAllJsonPropertyOrderAnnotationsContainProperFields(Class<?> clazz) {
        List<String> errors = new ArrayList<>();

        Set<String> fieldNamesInClass = flattenFieldsInClassToNames(clazz, gatherNotIgnoredFieldsIncludingSuperClasses(clazz, new HashSet<>()));
        JsonPropertyOrder jsonPropertyOrderAnnotation = clazz.getAnnotation(JsonPropertyOrder.class);
        if (jsonPropertyOrderAnnotation == null) {
            if (fieldNamesInClass.size() > 1 && !typesToIgnoreForCheckingJsonPropertyOrderExistence.contains(clazz)) {
                errors.add(clazz.getName() + ": doesn't have mandatory @JsonPropertyOrder annotation.");
            }
        } else {
            Set<String> fieldNamesInJsonPropertyOrderAnnotation = gatherFieldNamesFromJsonPropertyOrderAnnotation(jsonPropertyOrderAnnotation);

            Collection<String> fieldNamesNotInJsonPropertyOrderAnnotation = subtract(fieldNamesInClass, fieldNamesInJsonPropertyOrderAnnotation);
            Collection<String> unnecessaryFieldNamesInJsonPropertyOrderAnnotation = subtract(fieldNamesInJsonPropertyOrderAnnotation, fieldNamesInClass);

            errors = fieldNamesNotInJsonPropertyOrderAnnotation.stream()
                .map(fieldName -> clazz.getName() + ": " + fieldName + " must be placed in @JsonPropertyOrder annotation.")
                .collect(toList());
            unnecessaryFieldNamesInJsonPropertyOrderAnnotation.stream()
                .map(fieldName -> clazz.getName() + ": " + fieldName + " mustn't be placed in @JsonPropertyOrder annotation.")
                .forEach(errors::add);
        }

        return errors;
    }

    private List<String> checkIfAllJsonPropertyAnnotationsAreUsedOnlyWhenReallyNeeded(Class<?> clazz) {
        List<String> errors;

        Set<Field> fieldsInClass = gatherNotIgnoredFieldsIncludingSuperClasses(clazz, new HashSet<>());
        errors = fieldsInClass.stream()
            .filter(field -> !classesToIgnoreForCheckingJsonPropertyExistence.contains(field.getDeclaringClass()))
            .filter(this::doesJsonPropertyAnnotationContainCapitalizedFieldName)
            .map(fieldInClass -> clazz.getName() + ": @JsonProperty for " + fieldInClass.getName() + " is not needed.").collect(toList());

        return errors;
    }

    private Set<Field> gatherNotIgnoredFieldsIncludingSuperClasses(Class<?> clazz, Set<Field> visitedFields) {
        visitedFields.addAll(Arrays.stream(clazz.getDeclaredFields())
            .filter(field -> !isStatic(field.getModifiers()) && !field.getName().startsWith("this$") && !field.isAnnotationPresent(JsonIgnore.class))
            .collect(toSet()));

        Class<?> superclass = clazz.getSuperclass();
        if (clazz != RedfishDto.class && superclass != null) {
            return gatherNotIgnoredFieldsIncludingSuperClasses(superclass, visitedFields);
        }

        return visitedFields;
    }

    private Set<String> gatherFieldNamesFromJsonPropertyOrderAnnotation(JsonPropertyOrder jsonPropertyOrderAnnotation) {
        return new HashSet<>(asList(jsonPropertyOrderAnnotation.value()));
    }

    private Set<String> flattenFieldsInClassToNames(Class<?> clazz, Set<Field> fieldsInClass) {
        unwrapFields(fieldsInClass);

        Set<String> fieldNamesInClass = fieldsInClass.stream()
            .map(fieldInClass -> {
                JsonProperty jsonProperty = fieldInClass.getAnnotation(JsonProperty.class);
                if (jsonProperty != null && jsonProperty.value().startsWith("@")) {
                    return jsonProperty.value();
                }
                return fieldInClass.getName();
            })
            .collect(toSet());

        if (RedfishDto.class.isAssignableFrom(clazz)) {
            fieldNamesInClass.addAll(defaultRedfishDtoFields);
        }

        return fieldNamesInClass;
    }

    private void unwrapFields(Set<Field> fieldsInClass) {
        Set<Field> fieldsToRemove = new HashSet<>();
        Set<Field> fieldsToAdd = new HashSet<>();
        for (Field fieldInClass : fieldsInClass) {
            JsonUnwrapped jsonUnwrapped = fieldInClass.getAnnotation(JsonUnwrapped.class);
            if (jsonUnwrapped != null) {
                Class<?> typeOfField = extractTypeOfField(fieldInClass);
                fieldsToRemove.add(fieldInClass);
                fieldsToAdd.addAll(gatherNotIgnoredFieldsIncludingSuperClasses(typeOfField, new HashSet<>()));
            }
        }
        fieldsInClass.removeAll(fieldsToRemove);
        fieldsInClass.addAll(fieldsToAdd);
    }

    private boolean doesJsonPropertyAnnotationContainCapitalizedFieldName(Field field) {
        JsonProperty jsonProperty = field.getAnnotation(JsonProperty.class);
        if (jsonProperty == null) {
            return false;
        }

        return Objects.equals(jsonProperty.value(), toUpperCase(field.getName().charAt(0)) + field.getName().substring(1));
    }
}
