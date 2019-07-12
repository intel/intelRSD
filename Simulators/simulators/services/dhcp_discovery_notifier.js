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
const fs = require('fs');

class DhcpDiscoveryNotifier {
    constructor(type, location) {
        this._serviceType = type;
        this._serviceLocation = location;

        this._servicesListFilePath = '/tmp/services.list';
    }

    start() {
        console.log("Running DHCP discovery for " + this._serviceLocation);

        var services = [];
        if (fs.existsSync(this._servicesListFilePath)) {
            var servicesFile = fs.readFileSync(this._servicesListFilePath, 'utf-8');
            services = servicesFile.split('\n');
        }

        var line = this._serviceLocation + " " + this._serviceType + '\n';

        if (services.indexOf(line) === -1) {
            fs.appendFileSync(this._servicesListFilePath, line, 'utf-8');
        }
    }

    stop() {
        if (!fs.existsSync(this._servicesListFilePath)) {
            return;
        }

        var servicesFile = fs.readFileSync(this._servicesListFilePath, 'utf-8');
        var services = servicesFile.split('\n');

        var line = this._serviceLocation + " " + this._serviceType;

        removeOccurrences(line, services);
        fs.writeFileSync(this._servicesListFilePath, services.join('\n'), 'utf-8', function (err) {
            if (err) throw err;
        });
    }
}

var removeOccurrences = function (what, from) {
    var index = from.indexOf(what);
    while (index !== -1) {
        from.splice(index, 1);
        index = from.indexOf(what);
    }
};

exports.DhcpDiscoveryNotifier = DhcpDiscoveryNotifier;