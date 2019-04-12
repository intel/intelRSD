/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.types;

import com.intel.rsd.nodecomposer.utils.StringRepresentation;

import java.io.Serializable;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Objects;

import static com.intel.rsd.nodecomposer.types.EnumeratedType.stringToEnum;
import static java.lang.String.format;
import static java.util.Objects.hash;
import static java.util.Optional.ofNullable;
import static org.apache.commons.lang3.StringUtils.trimToNull;

/**
 * Provides representation of State, Health and HealthRollup objects.
 */
public class Status implements Serializable {
    public static final String STATE_PROPERTY = "State";
    public static final String HEALTH_PROPERTY = "Health";
    public static final String HEALTH_ROLLUP_PROPERTY = "HealthRollup";

    private static final long serialVersionUID = -1239512910107787280L;

    private State state;
    private Health health;
    private Health healthRollup;

    public Status(State state, Health health, Health healthRollup) {
        this.state = state;
        this.health = health;
        this.healthRollup = healthRollup;
    }

    /**
     * Creates Status object instance from string
     *
     * @param statusString string representation of Status, e.g. "State=Enabled,Health=OK,HealthRollup=OK"
     * @return Status object instance
     * @throws IllegalArgumentException if statusString cannot be parsed
     */
    public static Status statusFromString(String statusString) {
        if (trimToNull(statusString) == null) {
            return new Status(null, null, null);
        }

        try {
            Map<String, String> map = splitStatusString(statusString);
            return statusFromMap(map);
        } catch (IllegalArgumentException e) {
            throw new IllegalArgumentException(format("Cannot create Status instance for string: %s, details: %s", statusString, e.getMessage()), e);
        }
    }

    /**
     * Creates Status object instance from map
     *
     * @param statusMap map representation of Status
     * @return Status object instance
     * @throws IllegalArgumentException if Status object instance cannot be created
     */
    public static Status statusFromMap(Map<String, String> statusMap) {
        if (statusMap == null || statusMap.isEmpty()) {
            return new Status(null, null, null);
        }

        State state = ofNullable(statusMap.get(STATE_PROPERTY)).map(Status::mapState).orElse(null);
        Health health = ofNullable(statusMap.get(HEALTH_PROPERTY)).map(Status::mapHealth).orElse(null);
        Health healthRollup = ofNullable(statusMap.get(HEALTH_ROLLUP_PROPERTY)).map(Status::mapHealth).orElse(null);

        return new Status(state, health, healthRollup);
    }

    private static Map<String, String> splitStatusString(String statusString) {
        try {
            return StringRepresentation.toMap(statusString, true, true);
        } catch (IllegalArgumentException e) {
            throw new IllegalArgumentException("Error while parsing: " + statusString, e);
        }
    }

    private static Health mapHealth(String healthString) {
        return stringToEnum(Health.class, healthString);
    }

    private static State mapState(String stateString) {
        return stringToEnum(State.class, stateString);
    }

    public Health getHealth() {
        return health;
    }

    public State getState() {
        return state;
    }

    public Health getHealthRollup() {
        return healthRollup;
    }

    @Override
    public String toString() {
        return trimToNull(StringRepresentation.fromMap(toMap()));
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        Status status = (Status) o;
        return Objects.equals(state, status.state)
            && Objects.equals(health, status.health)
            && Objects.equals(healthRollup, status.healthRollup);
    }

    @Override
    public int hashCode() {
        return hash(state, health, healthRollup);
    }

    private Map<String, String> toMap() {
        Map<String, String> propertiesMap = new LinkedHashMap<>();

        if (state != null) {
            propertiesMap.put(STATE_PROPERTY, state.getValue());
        }

        if (health != null) {
            propertiesMap.put(HEALTH_PROPERTY, health.name());
        }

        if (healthRollup != null) {
            propertiesMap.put(HEALTH_ROLLUP_PROPERTY, healthRollup.name());
        }

        return propertiesMap;
    }
}
