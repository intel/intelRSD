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

const uuid = require('uuid');
const https = require('https');
const url = require('url');

class EventTrigger {
    constructor(eventService) {
        this.eventService = eventService;
    }

    get EventService() {
        return this.eventService;
    }

    CreateJsonTemplate(subscription, originOfCondition, typeOfEvent) {
        return {
            '@odata.context': '/redfish/v1/$metadata#EventService/Members/Events/Members/' + subscription.Id,
            '@odata.id': '/redfish/v1/EventService/Events/' + subscription.Id,
            '@odata.type': '#Event.v1_0_0.Event',
            'Id': subscription.Id.toString(),
            'Name': subscription.Name,
            'Description': subscription.Description,
            'Events': [
                {
                    'EventType': typeOfEvent,
                    'EventId': '',
                    'Severity': '',
                    'EventTimestamp': '',
                    'Message': '',
                    'MessageId': uuid.v1(),
                    'MessageArgs': [],
                    'OriginOfCondition': {'@odata.id': originOfCondition},
                    'Context': subscription.Context
                }
            ]
        };
    }

    FireResourceAddedEvent(originOfCondition) {
        return this.FireEvent(originOfCondition, 'ResourceAdded');
    }

    FireResourceUpdatedEvent(originOfCondition) {
        return this.FireEvent(originOfCondition, 'ResourceUpdated');
    }

    FireResourceRemovedEvent(originOfCondition) {
        return this.FireEvent(originOfCondition, 'ResourceRemoved');
    }

    FireStatusChangedEvent(originOfCondition) {
        return this.FireEvent(originOfCondition, 'StatusChange');
    }

    FireEvent(originOfCondition, typeOfEvent) {
        var sentEvents = [];
        var subs = this.EventService.Subscriptions;
        if (typeof subs !== 'undefined') {
            var self = this;
            subs.forEach(function (sub) {
                var eventJson = self.CreateJsonTemplate(sub, originOfCondition, typeOfEvent);

                var requestUrl = url.parse(sub.Destination);

                var request = new https.request({
                    protocol: 'https:',
                    hostname: requestUrl.hostname,
                    port: requestUrl.port,
                    path: requestUrl.pathname,
                    method: "POST",
                    rejectUnauthorized: false,
                    strictSSL: false,
                    headers: {
                        "Content-Type": "application/json; charset=utf-8"
                    }
                });

                request.on('error', (err) => {
                    throw err;
                });

                request.end(JSON.stringify(eventJson));

                sentEvents.push({
                    Destination: sub.Destination,
                    Event: eventJson
                });
            });
        }

        return sentEvents;
    }
}

exports.EventTrigger = EventTrigger;
