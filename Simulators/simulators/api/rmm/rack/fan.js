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
const uuid = require('uuid');

class Fan {
    constructor(baseUri, id, uLocation=1, xlocation = 1){
        this.baseUri = baseUri;
        this.id = id;
        this.uuid = uuid.v1();
        this.ulocation = uLocation;
        this.xlocation = xlocation;
    }

    get Id() {
        return this.id;
    }

    get Uri() {
        return this.baseUri + '/ThermalZone#/Fans/' + this.Id;
    }

    Json(){
        return {
            '@odata.id': this.Uri,
            'Name': 'Fan 1',
            'UUID': this.uuid,
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'ReadingRPM': 0,
            'CreatedDate': '2014-09-11T01:15:16+08:00',
            'UpdatedDate': '2014-09-15T03:12:16+08:00',
            'RackLocation': {
                'RackUnits': 'OU',
                'XLocation': this.xlocation,
                'ULocation': this.ulocation,
                'UHeight': 8
            }
        }
    }
}

exports.Fan = Fan;
