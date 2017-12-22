"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

from pip.req import parse_requirements
from setuptools import setup

# load dependencies from external files
reqs = []
for e in ['../utils/requirements.txt', '../utils/requirements-no-deps.txt']:
    install_reqs = parse_requirements(e, session='hack')
    reqs.extend([str(ir.req) for ir in install_reqs])

setup(
    name='CTS',
    version='2.2.0.172.0',
    packages=['main',
              'cts_core',
              'cts_core.commons',
              'cts_core.metadata',
              'cts_core.metadata.model',
              'cts_core.metadata.model.metadata_types',
              'cts_core.metadata.model.metadata_types.primitive_types',
              'cts_core.metadata.primitive_types_helpers',
              'cts_core.metadata.diff',
              'cts_core.discovery',
              'cts_core.validation',
              'cts_core.validation.get',
              'cts_core.validation.hierarchy',
              'cts_core.validation.patch',
              'cts_core.validation.patch.nontrivial_property',
              'cts_framework',
              'cts_framework.db',
              'cts_framework.db.dao',
              'cts_framework.db.model',
              'cts_framework.actions',
              'cts_framework.actions.tests',
              'cts_framework.actions.status',
              'cts_framework.actions.status.results_formatters',
              'cts_framework.actions.status.results_formatters.html_static_files',
              'cts_framework.actions.execute',
              'cts_framework.actions.replay',
              'cts_framework.actions.sos',
              'cts_framework.actions.metadata',
              'cts_framework.commons',
              'cts_framework.helpers',
              'cts_framework.helpers.python',
              'cts_framework.helpers.vars',
              'cts_framework.logging',
              'cts_framework.configuration',
              'cts_framework.tests_running',
              'cts_framework.tests_managing',
              'cts_framework.tests_managing.test_suite',
              'cts_framework.tests_managing.test_script',
              'cts_framework.tests_managing.test_package',

              'tests_packages',
              'tests_packages.Rack_Scale_2_1_POD_Manager.required',
              'tests_packages.Rack_Scale_2_1_PSME.required',
              'tests_packages.Rack_Scale_2_1_RMM.required',
              'tests_packages.Rack_Scale_2_1_Storage_Services.required',

              'tests_packages.Rack_Scale_2_2_POD_Manager.required',
              'tests_packages.Rack_Scale_2_2_PSME.required',
              'tests_packages.Rack_Scale_2_2_RMM.required',
              'tests_packages.Rack_Scale_2_2_Storage_Services.required',

              'metadata',

              'metadata.2_1_2.PODM',
              'metadata.2_1_2.PSME',
              'metadata.2_1_2.RMM',
              'metadata.2_1_2.SS',

              'metadata.2_1_3.PODM',
              'metadata.2_1_3.PSME',
              'metadata.2_1_3.RMM',
              'metadata.2_1_3.SS',

              'metadata.2_1_4.PODM',
              'metadata.2_1_4.PSME',
              'metadata.2_1_4.RMM',
              'metadata.2_1_4.SS',

              'metadata.2_2.PODM',
              'metadata.2_2.PSME',
              'metadata.2_2.RMM',
              'metadata.2_2.SS'
              ],

    package_data={'cts_framework.actions.status.results_formatters.html_static_files': ['*'],
                  'cts_framework.actions.sos': ['*.sh'],

                  'tests_packages.Rack_Scale_2_1_POD_Manager.required': ['*.py'],
                  'tests_packages.Rack_Scale_2_1_PSME.required': ['*.py'],
                  'tests_packages.Rack_Scale_2_1_RMM.required': ['*.py'],
                  'tests_packages.Rack_Scale_2_1_Storage_Services.required': ['*.py'],

                  'tests_packages.Rack_Scale_2_2_POD_Manager.required': ['*.py'],
                  'tests_packages.Rack_Scale_2_2_PSME.required': ['*.py'],
                  'tests_packages.Rack_Scale_2_2_RMM.required': ['*.py'],
                  'tests_packages.Rack_Scale_2_2_Storage_Services.required': ['*.py'],

                  'metadata.2_1_2.PODM': ['*'],
                  'metadata.2_1_2.PSME': ['*'],
                  'metadata.2_1_2.RMM': ['*'],
                  'metadata.2_1_2.SS': ['*'],

                  'metadata.2_1_3.PODM': ['*'],
                  'metadata.2_1_3.PSME': ['*'],
                  'metadata.2_1_3.RMM': ['*'],
                  'metadata.2_1_3.SS': ['*'],

                  'metadata.2_1_4.PODM': ['*'],
                  'metadata.2_1_4.PSME': ['*'],
                  'metadata.2_1_4.RMM': ['*'],
                  'metadata.2_1_4.SS': ['*'],

                  'metadata.2_2.PODM': ['*'],
                  'metadata.2_2.PSME': ['*'],
                  'metadata.2_2.RMM': ['*'],
                  'metadata.2_2.SS': ['*']
                  },

    url='',
    install_requires=reqs,
    license='',
    description='Conformance Test Suite',
    entry_points={
        'console_scripts': [
            'cts = main.main:main',
        ],
    },
)
