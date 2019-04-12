/*
 * Copyright (c) 2018-2019 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.dto.ODataId;
import lombok.Data;

import javax.validation.constraints.NotNull;
import java.util.ArrayList;
import java.util.Collection;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@Data
public class EventRecord {
    @JsonProperty("MessageArgs")
    private final Collection<String> messageArgs = new ArrayList<>();

    @NotNull
    @JsonProperty("EventType")
    private EventType eventType;

    @NotNull
    @JsonProperty("EventId")
    private String eventId;

    @NotNull
    @JsonProperty("EventTimestamp")
    private String eventTimestamp;

    @NotNull
    @JsonProperty("Severity")
    private String severity;

    @JsonProperty("Message")
    private String message;

    @JsonProperty("MessageId")
    private String messageId;

    @JsonInclude(NON_NULL)
    @JsonProperty("Context")
    private String context;

    @NotNull
    @JsonProperty("OriginOfCondition")
    private ODataId originOfCondition;

    private void addMessageArg(String messageArg) {
        messageArgs.add(messageArg);
    }

    private void addMessageArgs(Collection<String> messageArgs) {
        this.messageArgs.addAll(messageArgs);
    }
}
