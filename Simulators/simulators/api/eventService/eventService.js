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

const Collection = require('./../common/collection').Collection;
const Subscription = require('./subscription').Subscription;
const BaseAsset = require('../common/baseAsset').BaseAsset;

class EventService extends BaseAsset {
    constructor(mock, serviceRoot, odataType = '#EventService.v1_0_0.EventService') {
        super(mock, serviceRoot);
        this.odataType = odataType;
        this.CreateCollections();
        this.MockGetWithDynamicBody();
    }

    get Subscriptions() {
        return this.subscriptionCollection.GetElements();
    }

    GetBaseUri() {
        return '/redfish/v1/EventService';
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': this.GetBaseUri() + '/$metadata#EventService.EventService',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': this.odataType,
            'Id': 'EventService',
            'Name': 'Event Service',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'ServiceEnabled': true,
            'DeliveryRetryAttempts': 3,
            'DeliveryRetryIntervalSeconds': 60,
            'EventTypesForSubscription': [
                'StatusChange',
                'ResourceUpdated',
                'ResourceAdded',
                'ResourceRemoved',
                'Alert'
            ],
            'Subscriptions': {
                '@odata.id': this.GetBaseUri() + '/Subscriptions'
            }
        };
    }

    CreateCollections() {
        this.subscriptionCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#EventDestinationCollection.EventDestinationCollection',
            '@odata.type': '#EventDestinationCollection.EventDestinationCollection',
            '@odata.id': this.GetBaseUri() + '/Subscriptions',
            'Name': 'Event Subscriptions Collection',
            'Context': this,
            'Resource': Subscription
        });

        this.subscriptionCollection.EnablePostAndDelete((body) => {
            var subscription = this.subscriptionCollection.Add();
            subscription.Destination = body.Destination;
            subscription.EventTypes = body.EventTypes;
            subscription.Context = body.Context;
            subscription.Protocol = body.Protocol;
            return subscription;
        });
    }
}

exports.EventService = EventService;
