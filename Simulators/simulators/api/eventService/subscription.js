/*
 * Copyright (c) 2015-2017 Intel Corporation
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

const BaseAsset = require('../common/baseAsset').BaseAsset;

class Subscription extends BaseAsset {
    constructor(mock, eventService, id) {
        super(mock, eventService, id);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return '/redfish/v1/EventService/Subscriptions/' + this.Id;
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#EventDestination.EventDestination',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#EventDestination.v1_0_0.EventDestination',
            'Id': this.Id.toString(),
            'Name': this.Name || '',
            'Destination': this.Destination || '',
            'EventTypes': this.EventTypes || [],
            'Context': this.Context || '',
            'Protocol': this.Protocol || ''
        };
    }

    set Name(name) {
        this.name = name;
    }

    get Name() {
        return this.name;
    }

    set Destination(destination) {
        this.destination = destination;
    }

    get Destination() {
        return this.destination;
    }

    set Description(description) {
        this.description = description;
    }

    get Description() {
        return this.description;
    }

    get EventTypes() {
        return this.eventTypes;
    }

    set EventTypes(eventTypes) {
        this.eventTypes = eventTypes;
    }

    set Context(context) {
        this.context = context;
    }

    get Context() {
        return this.context;
    }

    get Protocol() {
        return this.protocol;
    }

    set Protocol(protocol) {
        this.protocol = protocol;
    }
}

exports.Subscription = Subscription;
