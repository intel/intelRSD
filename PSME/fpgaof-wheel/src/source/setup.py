"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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

from setuptools import setup
from fpgaof_helpers.constants import Constants

setup(
    name='FPGAoF Wheel',
    description='FPGAoF Wheel package written in Python',
    version=Constants.FPGAOF_WHEEL_VERSION,
    author='Tomasz (tmacieje) Maciejewski, '
           'Szymon (szielonk) Zielonka',
    author_email='tomasz.maciejewski@intel.com,'
                 'szymon.zielonka@intel.com',
    classifiers=[
        'Development Status :: 1 - Alpha',
        'Environment :: Console',
        'Programming Language :: Python :: 2.7'
    ],
    packages=['fpgaof_main',
              'fpgaof_actions',
              'fpgaof_helpers',
              ],
    url='',
    install_requires=['requests==2.21.0'],
    license='',
    entry_points={
        'console_scripts': [
            'fpgaof-wheel = fpgaof_main.main:main',
        ],
    },
)
