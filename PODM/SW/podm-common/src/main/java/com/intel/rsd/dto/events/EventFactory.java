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

package com.intel.rsd.dto.events;

import javax.validation.ConstraintViolation;
import javax.validation.ConstraintViolationException;
import javax.validation.Validation;
import javax.validation.Validator;
import java.time.ZonedDateTime;
import java.util.Collection;
import java.util.Set;

import static com.intel.rsd.dto.ODataId.odataId;
import static java.time.format.DateTimeFormatter.ISO_OFFSET_DATE_TIME;

public class EventFactory {
    private static final Validator VALIDATOR = Validation.buildDefaultValidatorFactory().getValidator();

    public static Event createEvent(Collection<EventRecord> eventRecords) {
        Event event = new Event();
        event.setEventRecords(eventRecords);

        Set<ConstraintViolation<Event>> violations = VALIDATOR.validate(event);
        if (!violations.isEmpty()) {
            throw new ConstraintViolationException("One or more given arguments have illegal values", violations);
        }
        return event;
    }

    public static EventRecord createEventRecord(EventType eventType, String eventId, String severity, String context, String originOfCondition) {
        EventRecord eventRecord = new EventRecord();
        eventRecord.setEventType(eventType);
        eventRecord.setEventId(eventId);
        eventRecord.setEventTimestamp(ISO_OFFSET_DATE_TIME.format(ZonedDateTime.now()));
        eventRecord.setContext(context);
        eventRecord.setSeverity(severity);
        eventRecord.setOriginOfCondition(odataId(originOfCondition));

        Set<ConstraintViolation<EventRecord>> violations = VALIDATOR.validate(eventRecord);
        if (!violations.isEmpty()) {
            throw new ConstraintViolationException("One or more given arguments have illegal values", violations);
        }
        return eventRecord;
    }
}
