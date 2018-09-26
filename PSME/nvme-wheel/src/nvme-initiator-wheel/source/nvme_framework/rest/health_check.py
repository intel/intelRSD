"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2018 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""
import os

from flask import Flask, jsonify, Response
from time import time

import nvme_framework.configuration.config_manager as cfg
from nvme_framework.helpers.database import get_last_timestamp

app = Flask(__name__)


# Environment settings
# NVME_MODE=[PRODUCTION/DEBUG] Default (if empty): PRODUCTION
# NVME_PORT=port_number Default (if empty): 8082
class HealthCheck:
    def __init__(self):
        self.config_manager = cfg.Config()
        self.rest_port = self.get_port()
        self.in_production_mode = self.is_production_mode_enabled()

    def build_report(self):
        report = dict()

        report['status'] = "ok" if self.check_timestamps() else "error"
        report['last-timestamp'] = get_last_timestamp()
        report['cron-value'] = self.get_wait_time()

        if not self.in_production_mode:
            report['number-of-targets'] = self.get_number_of_connected_targets()
            report['connected-targets'] = self.get_connected_targets()
        return report

    def get_health(self):
        if not self.in_production_mode:
            return jsonify(HealthCheck().build_report())

        if not self.check_timestamps():
            return Response('', status=503)  # Service Unavailable
        return Response('', status=204)  # No content

    def get_wait_time(self):
        # Increase time of wait, connecting target can take some time
        # Default value is 5 sec * 6 = 30 sec
        return self.config_manager.get_cron_time() * 6

    def get_connected_targets(self):
        return self.config_manager.get_available_targets()

    def get_number_of_connected_targets(self):
        return len(self.config_manager.get_available_targets())

    def check_timestamps(self):
        return int(time()) - get_last_timestamp() <= self.get_wait_time()

    @staticmethod
    def is_production_mode_enabled():
        mode = os.getenv('NVME_MODE')
        if str(mode).upper() == "DEBUG":
            return False
        return True

    @staticmethod
    def get_port():
        port = os.getenv('NVME_PORT')
        if port is None:
            return 8082
        return port

    @staticmethod
    @app.route('/health', methods=['GET'])
    def health():
        return HealthCheck().get_health()


def server_runner():
    app.run(port=HealthCheck.get_port())


if __name__ == "__main__":
    server_runner()
