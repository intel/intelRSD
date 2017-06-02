/*
 * Copyright (c) 2016-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const BaseAsset = require('../common/baseAsset').BaseAsset;

class MessageRegistry extends BaseAsset {
    constructor(mock, serviceRoot, odataType = '#MessageRegistry.v1_0_0.MessageRegistry') {
        super(mock, serviceRoot);
        this.odataType = odataType;
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return '/redfish/v1/MessageRegistry';
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': this.GetBaseUri() + '/$metadata#MessageRegistry',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': this.odataType,
            'Language': 'en_US',
            'RegistryPrefix': 'RMM',
            'RegistryVersion': '1.0.0',
            'OwningEntity': 'Intel',
            'Messages': {
                'MSGRackStatusChange': {
                    'Description': 'Rack Status Change',
                    'Message': 'Rack status change',
                    'Severity': 'N/A',
                    'NumberOfArgs': 1,
                    'ParamTypes': ['string']
                },
                'MSGRackResourceUpdated': {
                    'Description': 'Rack Info Update',
                    'Message': 'Rack updated.',
                    'Severity': 'N/A',
                    'NumberOfArgs': 1,
                    'ParamTypes': ['string']
                },
                'MSGPsuStatusChange': {
                    'Description': 'PSU Status Change',
                    'Severity': 'N/A',
                    'NumberOfArgs': 2,
                    'ParamTypes': ['string', 'string']
                }
            }
        }
    }
}

exports.MessageRegistry = MessageRegistry;