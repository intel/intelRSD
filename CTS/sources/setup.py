"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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

from setuptools import setup

try:  # for pip >= 10
    from pip._internal.req import parse_requirements
except ImportError:  # for pip <= 9.0.3
    from pip.req import parse_requirements

# load dependencies from external files
requirements = []
for e in ['../utils/requirements.txt', '../utils/requirements-no-deps.txt']:
    install_reqs = parse_requirements(e, session='hack')
    requirements.extend([str(ir.req) for ir in install_reqs])

setup(
    name='CTS',
    version='2.5.0.31.0',
    packages=['main',
              'cts_core',
              'cts_core.commons',
              'cts_core.measure',
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
              'cts_framework.actions.scenario',
              'cts_framework.actions.sos',
              'cts_framework.actions.update',
              'cts_framework.actions.metadata',
              'cts_framework.actions.metadata.template',
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
              'cts_framework.validations',

              'tests_packages',
              'tests_packages.Rack_Scale_2_5_POD_Manager.required',
              'tests_packages.Rack_Scale_2_5_PSME.required',
              'tests_packages.Rack_Scale_2_5_RMM.required',
              'tests_packages.Rack_Scale_2_5_Storage_Services.required',
              'tests_packages.Rack_Scale_2_5_Scenario.required',
              'tests_packages.Rack_Scale_2_5_Profiles.required',

              'tests_packages.Rack_Scale_2_5_POD_Manager.optional',
              'tests_packages.Rack_Scale_2_5_PSME.optional',
              'tests_packages.Rack_Scale_2_5_RMM.optional',
              'tests_packages.Rack_Scale_2_5_Storage_Services.optional',
              'tests_packages.Rack_Scale_2_5_Scenario.optional',
              'tests_packages.Rack_Scale_2_5_Profiles.optional',

              'tests_packages.Rack_Scale_2_5_POD_Manager.required.tests_archeo_podm',
              'tests_packages.Rack_Scale_2_5_PSME.required.tests_archeo_psme',
              'tests_packages.Rack_Scale_2_5_Storage_Services.required.tests_archeo_ss',

              'metadata',
              'metadata.2_5.PODM',
              'metadata.2_5.PSME',
              'metadata.2_5.RMM',
              'metadata.2_5.SS',
              'metadata.2_5.SCENARIO',
              'metadata.2_5.RACKSCALE',

              'metadata.redfish.2018_1',
              'metadata.redfish.2018_2',
              'metadata.redfish.2018_3',
              'metadata.redfish.2019_1'
              ],

    package_data={'main': ['VERSION'],
                  'cts_framework.actions.metadata.template': ['*.html', '*.png'],
                  'cts_framework.actions.status.results_formatters.html_static_files': ['*'],
                  'cts_framework.actions.sos': ['*.sh'],

                  'tests_packages.Rack_Scale_2_5_POD_Manager.required': ['*.py'],
                  'tests_packages.Rack_Scale_2_5_PSME.required': ['*.py'],
                  'tests_packages.Rack_Scale_2_5_RMM.required': ['*.py'],
                  'tests_packages.Rack_Scale_2_5_Storage_Services.required': ['*.py'],
                  'tests_packages.Rack_Scale_2_5_Scenario.required': ['*.py', '*.action'],
                  'tests_packages.Rack_Scale_2_5_Profiles.required': ['*.py'],

                  'tests_packages.Rack_Scale_2_5_POD_Manager.optional': ['*.py'],
                  'tests_packages.Rack_Scale_2_5_PSME.optional': ['*.py'],
                  'tests_packages.Rack_Scale_2_5_RMM.optional': ['*.py'],
                  'tests_packages.Rack_Scale_2_5_Storage_Services.optional': ['*.py'],
                  'tests_packages.Rack_Scale_2_5_Scenario.optional': ['*.py', '*.action'],
                  'tests_packages.Rack_Scale_2_5_Profiles.optional': ['*.py'],

                  'tests_packages.Rack_Scale_2_5_POD_Manager.required.tests_archeo_podm': ['*'],
                  'tests_packages.Rack_Scale_2_5_PSME.required.tests_archeo_psme': ['*'],
                  'tests_packages.Rack_Scale_2_5_Storage_Services.required.tests_archeo_ss': ['*'],

                  'metadata.2_5.PODM': ['*'],
                  'metadata.2_5.PSME': ['*'],
                  'metadata.2_5.RMM': ['*'],
                  'metadata.2_5.SS': ['*'],
                  'metadata.2_5.SCENARIO': ['*'],
                  'metadata.2_5.RACKSCALE': ['*'],

                  'metadata.redfish.2018_1': ['*'],
                  'metadata.redfish.2018_2': ['*'],
                  'metadata.redfish.2018_3': ['*'],
                  'metadata.redfish.2019_1': ['*']
                  },

    url='',
    install_requires=requirements,
    license='',
    description='Conformance Test Suite',
    entry_points={
        'console_scripts': [
            'cts = main.main:main',
        ],
    },
)
