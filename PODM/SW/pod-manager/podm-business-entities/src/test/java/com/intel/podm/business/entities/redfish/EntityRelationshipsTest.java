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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.redfish.EntityDetail.EntityRelationshipDetail;
import com.intel.podm.business.entities.redfish.EntityDetail.SpiedEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import org.mockito.invocation.InvocationOnMock;
import org.testng.annotations.Test;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.business.entities.redfish.EntityDetailsGatherer.getEmbedabbles;
import static com.intel.podm.business.entities.redfish.EntityDetailsGatherer.getEntityDetails;
import static com.intel.podm.business.entities.redfish.EntityRelationshipHelper.SETTER_PREFIX;
import static com.intel.podm.business.entities.redfish.EntityRelationshipHelper.getEntityDetailByEntityType;
import static com.intel.podm.business.entities.redfish.EntityRelationshipHelper.invokeGetMethodAndReturnEntityContained;
import static com.intel.podm.business.entities.redfish.EntityRelationshipHelper.unproxy;
import static com.intel.podm.business.entities.redfish.EntityRelationshipHelper.validateIfGetMethodReturnsNullOrEmptyCollection;
import static java.util.Arrays.stream;
import static java.util.Collections.sort;
import static java.util.stream.Collectors.toList;
import static org.apache.commons.lang.exception.ExceptionUtils.getStackTrace;
import static org.mockito.Mockito.mockingDetails;
import static org.testng.Assert.assertTrue;
import static org.testng.Assert.fail;

public class EntityRelationshipsTest {
    private static final Set<String> ENTITIES_PACKAGES = new HashSet<String>() {{
        add("com.intel.podm.business.entities.redfish");
        add("com.intel.podm.business.entities.redfish.base");
    }};
    private static final String EMBEDDABLES_PACKAGE = "com.intel.podm.business.entities.redfish.embeddables";

    @Test
    public void allEntityRelationshipsShouldHaveProperMethods() {
        List<String> errors = new ArrayList<>();

        Set<EntityDetail> entityDetails = getEntityDetails();
        assertTrue(entityDetails.size() > 0, "No entities were found in current classpath");

        for (EntityDetail entityDetail : entityDetails) {
            errors.addAll(entityDetail.validate());
        }

        if (!errors.isEmpty()) {
            sort(errors);
            String error = "\n" + errors.size() + " problem(s) found in Entity relationships:\n" + String.join("\n", errors);
            fail(error);
        }
    }

    @Test
    public void allEmbeddablesShouldHaveEqualsAndHashCodeImplemented() {
        List<String> errors = new ArrayList<>();

        Set<Object> embeddables = getEmbedabbles();
        assertTrue(embeddables.size() > 0, "No embeddables were found in current classpath");

        for (Object embeddable : embeddables) {
            errors.addAll(checkIfEqualsAndHashCodeAreDeclared(embeddable));
        }

        if (!errors.isEmpty()) {
            sort(errors);
            String error = "\n" + errors.size() + " problem(s) found in Embeddables:\n" + String.join("\n", errors);
            fail(error);
        }
    }

    @Test
    public void allEntitiesShouldBeInProperPackage() {
        List<String> errors = new ArrayList<>();

        for (EntityDetail entityDetail : getEntityDetails()) {
            if (!ENTITIES_PACKAGES.contains(entityDetail.getEntityClass().getPackage().getName())) {
                errors.add(entityDetail.getEntityClass().getName() + ": Class must be located directly in one of the following packages: "
                    + ENTITIES_PACKAGES + ".");
            }
        }

        if (!errors.isEmpty()) {
            sort(errors);
            String error = "\n" + errors.size() + " problem(s) found in Entity package location:\n" + String.join("\n", errors);
            fail(error);
        }
    }

    @Test
    public void allEmbeddablesShouldBeInProperPackage() {
        List<String> errors = new ArrayList<>();

        for (Object embeddable : getEmbedabbles()) {
            if (!EMBEDDABLES_PACKAGE.equals(embeddable.getClass().getPackage().getName())) {
                errors.add(embeddable.getClass().getName() + ": Class must be located directly in following package: " + EMBEDDABLES_PACKAGE + ".");
            }
        }

        if (!errors.isEmpty()) {
            sort(errors);
            String error = "\n" + errors.size() + " problem(s) found in Entity package location:\n" + String.join("\n", errors);
            fail(error);
        }
    }

    @Test(dependsOnMethods = {"allEntityRelationshipsShouldHaveProperMethods"})
    public void allUnlinkersShouldBeCalledInPreRemoveMethod() {
        List<String> errors = new ArrayList<>();

        for (EntityDetail entityDetail : getEntityDetails()) {
            errors.addAll(checkIfAllUnlinkersWereCalled(entityDetail));
        }

        if (!errors.isEmpty()) {
            sort(errors);
            String error = "\n" + errors.size() + " problem(s) found in Entity relationships:\n" + String.join("\n", errors);
            fail(error);
        }
    }

    @Test(dependsOnMethods = {"allEntityRelationshipsShouldHaveProperMethods"})
    public void allEntityRelationshipsShouldHaveMethodsInProperOrder() {
        List<String> errors = new ArrayList<>();

        for (EntityDetail entityDetail : getEntityDetails()) {
            errors.addAll(checkIfAllRelationshipsHaveMethodsInProperOrder(entityDetail));
        }

        if (!errors.isEmpty()) {
            sort(errors);
            String error = "\n" + errors.size() + " problem(s) found with methods order:\n" + String.join("\n", errors);
            fail(error);
        }
    }

    @Test
    public void collectionsMustNotBeenPassedToSetters() {
        List<String> errors = new ArrayList<>();

        for (EntityDetail entityDetail : getEntityDetails()) {
            errors.addAll(checkIfAllMethodsFromGivenClassDoNotHaveCollectionsPassedToSetters(entityDetail.getEntityClass()));
        }
        for (Object embeddable : getEmbedabbles()) {
            errors.addAll(checkIfAllMethodsFromGivenClassDoNotHaveCollectionsPassedToSetters(embeddable.getClass()));
        }

        if (!errors.isEmpty()) {
            sort(errors);
            String error = "\n" + errors.size() + " problem(s) found with collections passed to setters:\n" + String.join("\n", errors);
            fail(error);
        }
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
        List<String> errors = new ArrayList<>();
        for (Entity spiedRelatedEntity : spiedRelatedEntities) {
            errors.add(typeOfSpiedEntity.getName() + ": Mandatory proper unlinking with "
                + unproxy(spiedRelatedEntity.getClass()).getSimpleName() + " was not done in @PreRemove method(s).");
        }

        return errors;
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

    private List<String> checkIfAllMethodsFromGivenClassDoNotHaveCollectionsPassedToSetters(Class clazz) {
        List<String> errors = new ArrayList<>();

        stream(clazz.getDeclaredMethods())
            .filter(method -> method.getName().startsWith(SETTER_PREFIX))
            .forEach(method -> stream(method.getParameterTypes())
                .filter(Collection.class::isAssignableFrom)
                .forEach(parameterType ->
                    errors.add(clazz.getName() + ": Do not pass collection to setter method: " + method.getName() + ", use adder instead.")));

        return errors;
    }
}
