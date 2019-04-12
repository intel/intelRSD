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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.intel.rsd.nodecomposer.persistence.redfish.EntityDetail.EntityRelationshipDetail;
import com.intel.rsd.nodecomposer.persistence.redfish.EntityDetail.SpiedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import org.mockito.invocation.InvocationOnMock;
import org.testng.annotations.Test;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.EntityDetailsGatherer.getEmbeddables;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityDetailsGatherer.getEntityDetails;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.SETTER_PREFIX;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.getEntityDetailByEntityType;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.getEnumTypeFromEnumFieldOrCollectionOfEnumsField;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.invokeGetMethodAndReturnEntityContained;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.isEnumeratedAnnotationWithStringValuePresent;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.unproxy;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.validateIfGetMethodReturnsNullOrEmptyCollection;
import static com.intel.rsd.nodecomposer.utils.FailManager.failWithMessageIfAnyError;
import static java.util.Arrays.stream;
import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.toList;
import static org.apache.commons.lang3.exception.ExceptionUtils.getStackTrace;
import static org.mockito.Mockito.mockingDetails;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodCount"})
public class EntityRelationshipsTest {
    private static final Set<String> ENTITIES_PACKAGES = new HashSet<String>() {
        private static final long serialVersionUID = -7771584079645233761L;

        {
            add("com.intel.rsd.nodecomposer.persistence.redfish");
            add("com.intel.rsd.nodecomposer.persistence.redfish.base");
        }
    };
    private static final List<String> EMBEDDABLES_PACKAGE = new ArrayList<>(
        singletonList("com.intel.rsd.nodecomposer.persistence.redfish.embeddables"));

    @Test
    public void allEntityRelationshipsShouldHaveProperMethods() {
        List<String> errors = new ArrayList<>();

        Set<EntityDetail> entityDetails = getEntityDetails();
        assertTrue(entityDetails.size() > 0, "No entities were found in current classpath");

        entityDetails.stream()
            .map(EntityDetail::validate)
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "Entity relationships");
    }

    @Test
    public void allEmbeddablesShouldHaveEqualsAndHashCodeImplemented() {
        List<String> errors = new ArrayList<>();

        Set<Object> embeddables = getEmbeddables();
        assertTrue(embeddables.size() > 0, "No embeddables were found in current classpath");

        embeddables.stream()
            .map(this::checkIfEqualsAndHashCodeAreDeclared)
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "Embeddables");
    }

    @Test
    public void allEntitiesShouldBeInProperPackage() {
        List<String> errors = getEntityDetails().stream()
            .filter(entityDetail -> !ENTITIES_PACKAGES.contains(entityDetail.getEntityClass().getPackage().getName()))
            .map(entityDetail -> entityDetail.getEntityClass().getName() + ": Class must be located directly in one of the following packages: "
                + ENTITIES_PACKAGES + ".").collect(toList());

        failWithMessageIfAnyError(errors, "Entity package location");
    }

    @Test
    public void allEmbeddablesShouldBeInProperPackage() {

        List<String> errors = new ArrayList<>();

        getEmbeddables().stream()
            .map(embeddable -> EMBEDDABLES_PACKAGE.stream()
                .filter(embeddableFromPackage -> embeddableFromPackage.getClass().getPackage().getName().equals(embeddable))
                .map(embeddableFromPackage -> embeddable.getClass().getName()
                    + ": Class must be located directly in following package: " + EMBEDDABLES_PACKAGE + ".")
                .collect(toList()))
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "Embeddable package location");
    }

    @Test(dependsOnMethods = {"allEntityRelationshipsShouldHaveProperMethods"})
    public void allUnlinkersShouldBeCalledInPreRemoveMethod() {
        List<String> errors = new ArrayList<>();

        getEntityDetails().stream()
            .map(this::checkIfAllUnlinkersWereCalled)
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "Entity relationships");
    }

    @Test(dependsOnMethods = {"allEntityRelationshipsShouldHaveProperMethods"})
    public void allEntityRelationshipsShouldHaveMethodsInProperOrder() {
        List<String> errors = new ArrayList<>();

        getEntityDetails().stream()
            .map(this::checkIfAllRelationshipsHaveMethodsInProperOrder)
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "Entity methods order");
    }

    @Test
    public void collectionsMustNotBeenPassedToSetters() {
        List<String> errors = new ArrayList<>();

        getEntityDetails().stream()
            .map(entityDetail -> checkIfAllMethodsFromGivenClassDoNotHaveCollectionsPassedToSetters(entityDetail.getEntityClass()))
            .forEach(errors::addAll);

        getEmbeddables().stream()
            .map(embeddable -> checkIfAllMethodsFromGivenClassDoNotHaveCollectionsPassedToSetters(embeddable.getClass()))
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "Collections passed to setters");
    }

    @Test
    public void enumsMustBePersistedAsStrings() {
        List<String> errors = new ArrayList<>();

        getEntityDetails().stream()
            .map(entityDetail -> checkIfAllEnumsFromGivenClassArePersistedAsStrings(entityDetail.getEntityClass()))
            .forEach(errors::addAll);

        getEmbeddables().stream()
            .map(embeddable -> checkIfAllEnumsFromGivenClassArePersistedAsStrings(embeddable.getClass()))
            .forEach(errors::addAll);

        failWithMessageIfAnyError(errors, "Persisted enums");
    }

    private List<String> checkIfEqualsAndHashCodeAreDeclared(Object embeddable) {
        List<String> errors = new ArrayList<>();

        try {
            embeddable.getClass().getDeclaredMethod("hashCode");
        } catch (NoSuchMethodException e) {
            errors.add(embeddable.getClass().getName() + ": Method hashCode() was not declared.");
        }

        try {
            embeddable.getClass().getDeclaredMethod("equals", Object.class);
        } catch (NoSuchMethodException e) {
            errors.add(embeddable.getClass().getName() + ": Method equals(Object object) was not declared.");
        }

        return errors;
    }

    private List<String> checkIfAllUnlinkersWereCalled(EntityDetail entityDetail) {
        List<String> errors = new ArrayList<>();

        try {
            SpiedEntity spiedEntity = entityDetail.spyWithAllRelationships();
            errors.addAll(checkIfGettersReturnProperEntities(entityDetail, spiedEntity));

            entityDetail.invokePreRemoveMethods(spiedEntity.getSpiedEntity());
            errors.addAll(checkIfRelationshipsUnlinkersWereCalled(spiedEntity));
            errors.addAll(checkIfGettersReturnNullsOrEmptyCollections(entityDetail, spiedEntity));
        } catch (EntityRelationshipException e) {
            errors.add(entityDetail.getEntityClass().getName() + ": " + getStackTrace(e));
        }

        return errors;
    }

    private List<String> checkIfGettersReturnProperEntities(EntityDetail entityDetail, SpiedEntity spiedEntity) throws EntityRelationshipException {
        List<String> errors = new ArrayList<>();

        for (EntityRelationshipDetail entityRelationshipDetail : entityDetail.getEntityRelationshipDetails()) {
            Entity receivedEntity = invokeGetMethodAndReturnEntityContained(spiedEntity.getSpiedEntity(), entityRelationshipDetail);
            if (!receivedEntity.equals(spiedEntity.getSpiedRelationships().get(entityRelationshipDetail))) {
                errors.add(entityDetail.getEntityClass().getName() + ": Entity received in "
                    + entityRelationshipDetail.getGetterMethod().get().getName() + " is not the same Entity which was previously provided in "
                    + entityRelationshipDetail.getSetterOrAdder().get().getName() + ".");
            }
        }

        return errors;
    }

    private List<String> checkIfGettersReturnNullsOrEmptyCollections(EntityDetail entityDetail, SpiedEntity spiedEntity) throws EntityRelationshipException {
        List<String> errors = new ArrayList<>();

        try {
            for (EntityRelationshipDetail entityRelationshipDetail : entityDetail.getEntityRelationshipDetails()) {
                validateIfGetMethodReturnsNullOrEmptyCollection(spiedEntity.getSpiedEntity(), entityRelationshipDetail);
            }
        } catch (EntityRelationshipException e) {
            errors.add(entityDetail.getEntityClass().getName() + ": " + getStackTrace(e));
        }

        return errors;
    }


    private List<String> checkIfRelationshipsUnlinkersWereCalled(SpiedEntity spiedEntity) throws EntityRelationshipException {
        Class<? extends Entity> typeOfSpiedEntity = unproxy(spiedEntity.getSpiedEntity().getClass());
        Set<EntityRelationshipDetail> entityRelationshipDetails = spiedEntity.getSpiedRelationships().keySet();

        Set<Entity> spiedRelatedEntities = new HashSet<>();
        for (EntityRelationshipDetail entityRelationshipDetail : entityRelationshipDetails) {
            Entity entityToSpy = spiedEntity.getSpiedRelationships().get(entityRelationshipDetail);
            spiedRelatedEntities.add(entityToSpy);

            List<Method> listOfTestedMethods = mockingDetails(entityToSpy).getInvocations().stream()
                .map(InvocationOnMock::getMethod)
                .collect(toList());

            Set<Method> unlinkerMethods = getEntityDetailByEntityType(unproxy(entityToSpy.getClass())).getUnlinkerMethods(typeOfSpiedEntity);
            unlinkerMethods.stream()
                .filter(listOfTestedMethods::contains)
                .forEach(unlinkerMethod -> spiedRelatedEntities.remove(entityToSpy));
        }

        return gatherErrorsForNotCalledUnlinkers(spiedRelatedEntities, typeOfSpiedEntity);
    }

    private List<String> gatherErrorsForNotCalledUnlinkers(Set<Entity> spiedRelatedEntities, Class<? extends Entity> typeOfSpiedEntity) {
        return spiedRelatedEntities.stream()
            .map(spiedRelatedEntity -> typeOfSpiedEntity.getName() + ": Mandatory proper unlinking with "
                + unproxy(spiedRelatedEntity.getClass()).getSimpleName() + " was not done in @PreRemove method(s).")
            .collect(toList());
    }

    private List<String> checkIfAllRelationshipsHaveMethodsInProperOrder(EntityDetail entityDetail) {
        List<String> errors = new ArrayList<>();
        List<Method> allMethods = entityDetail.getAllMethods();

        for (EntityRelationshipDetail entityRelationshipDetail : entityDetail.getEntityRelationshipDetails()) {
            int indexOfGetter = allMethods.indexOf(entityRelationshipDetail.getGetterMethod().get());
            int indexOfSetterOrAdder = allMethods.indexOf(entityRelationshipDetail.getSetterOrAdder().get());
            int indexOfUnlinker = allMethods.indexOf(entityRelationshipDetail.getUnlinkerMethod().get());

            if (indexOfGetter != indexOfSetterOrAdder - 1) {
                errors.add(entityDetail.getEntityClass().getName() + ": " + entityRelationshipDetail.getSetterOrAdder().get().getName()
                    + " must be placed just after " + entityRelationshipDetail.getGetterMethod().get().getName());
            }

            if (indexOfSetterOrAdder != indexOfUnlinker - 1) {
                errors.add(entityDetail.getEntityClass().getName() + ": " + entityRelationshipDetail.getUnlinkerMethod().get().getName()
                    + " must be placed just after " + entityRelationshipDetail.getSetterOrAdder().get().getName());
            }
        }

        return errors;
    }

    private List<String> checkIfAllMethodsFromGivenClassDoNotHaveCollectionsPassedToSetters(Class<?> clazz) {
        List<String> errors = new ArrayList<>();

        stream(clazz.getDeclaredMethods())
            .filter(method -> method.getName().startsWith(SETTER_PREFIX))
            .forEach(method -> stream(method.getParameterTypes())
                .filter(Collection.class::isAssignableFrom)
                .forEach(parameterType ->
                    errors.add(clazz.getName() + ": Do not pass collection to setter method: " + method.getName() + ", use adder instead.")));

        return errors;
    }

    private List<String> checkIfAllEnumsFromGivenClassArePersistedAsStrings(Class<?> clazz) {
        List<String> errors = new ArrayList<>();

        for (Field field : clazz.getDeclaredFields()) {
            Class<? extends Enum<?>> enumFieldType = getEnumTypeFromEnumFieldOrCollectionOfEnumsField(field);
            if (enumFieldType != null && !isEnumeratedAnnotationWithStringValuePresent(field)) {
                errors.add(clazz.getName() + ": Field: " + field.getName() + " must have @Enumerated(STRING) annotation.");
            }
        }

        return errors;
    }
}
