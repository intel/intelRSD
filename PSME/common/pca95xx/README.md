LICENSE
=======

 | Copyright (c) 2015-2016 Intel Corporation
 |
 | Licensed under the Apache License, Version 2.0 (the "License");
 | you may not use this file except in compliance with the License.
 | You may obtain a copy of the License at
 |
 |    http://www.apache.org/licenses/LICENSE-2.0
 |
 | Unless required by applicable law or agreed to in writing, software
 | distributed under the License is distributed on an "AS IS" BASIS,
 | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 | See the License for the specific language governing permissions and
 | limitations under the License.

1. Build
--------

To build from sources:

    mkdir build
    cd build
    cmake ..
    make

If you prefer to use Clang compiler rather than GCC:

    export CC=clang
    export CXX=clang++

To check code with Clang static analyzer:

    scan-build make

2. New PCA95xx models
---------------------

Add new PCA95xx models in *models* directory and register in pca95xx_dev_table
located in pca95xx_common.c source file.

3. Usage
--------

Code example:

    #include "pca95xx.h"

    /* Init API and choose PCA9505 */
    struct pca95xx pca95xx;
    pca95xx_init(&pca95xx, PCA9505);

    /* Choose second I2C bus and set slave address to 0x21 */
    pca95xx_set_i2c_bus_number(&pca95xx, 1);
    pca95xx_set_i2c_slave_address(&pca95xx, 0x21);

    /* Write 0xAA to output at bank 1 */
    pca95xx_set_output(&pca95xx, 1, 0xAA);

4. I2C-stub
-----------

Load required I2C modules. For i2c-stub pass I2C slave address through
chip_addr parameter:

    modprobe i2c-dev
    modprobe i2c-stub chip_addr=0x21

Install I2C-tools. Fedora:

    yum install i2c-tools

Check if device exist. Example I2C3 (/dev/i2c-3):

    i2cdetect 3

Write some data. I2C bus 3, slave address 0x21, register address 0, data 0x55:

    i2cset 3 0x21 0 0x55

Read data back:

    i2cget 3 0x21 0
