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

const OdataIdArray = require('../../common/odataIdArray').OdataIdArray;

class Temperature {
    constructor(baseUri, id){
        this.baseUri = baseUri;
        this.id = id;
        this.relatedItems = new OdataIdArray();
    }

    Id() {
        return this.id;
    }

    GetBaseUri() {
        return this.baseUri + '#/Temperatures/' + this.Id();
    }

    Json(){
        return {
            '@odata.id': this.GetBaseUri(),
            'MemberId': this.Id(),
            'Name': this.Name || 'Drawer inlet Temp',
            'SensorNumber': 42,
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'ReadingCelsius': 21,
            'UpperThresholdNonCritical': 42,
            'UpperThresholdCritical': 42,
            'UpperThresholdFatal': 42,
            'LowerThresholdNonCritical': 42,
            'LowerThresholdCritical': 5,
            'LowerThresholdFatal': 42,
            'MinReadingRange': 0,
            'MaxReadingRange': 200,
            'PhysicalContext': 'Intake',
            'RelatedItem': this.relatedItems
        }
    }

    AddToRelatedItems(element) {
        this.relatedItems.AddWithODataId(element.GetBaseUri());
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }
}

exports.Temperature = Temperature;
