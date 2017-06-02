# Copyright (c) 2016-2017 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


from data_model.protocols import Protocols
from data_model.service_description import ServiceDescription
from data_model.service_localization import ServiceLocalization
from data_model.service_parameters import ServiceParameters


class ServiceRetriever:
    _pod_manager_host = '127.0.0.1'
    _pod_manager_root_path = '/redfish/v1'
    _pod_manager_service_type = 'podm'
    _default_port = '8443'

    _default_protocol = Protocols.HTTPS

    @classmethod
    def retrieve_service_descriptions(cls):
        pod_manager_service_description = ServiceDescription(
            ServiceLocalization(ServiceRetriever._pod_manager_host, ServiceRetriever._default_port),
            ServiceParameters(
                ServiceRetriever._pod_manager_service_type,
                ServiceRetriever._pod_manager_root_path,
                ServiceRetriever._default_protocol
            )
        )

        descriptions = cls._additional_work()
        descriptions.insert(0, pod_manager_service_description)
        return descriptions

    @staticmethod
    def _additional_work():
        pass
