@dir common/agent-framework

PSME Generic Asset Management Interface Framework
====================

1. Information
--------------
The PSME provides a brand new modular design to support variety of custom hardware. Apart from the generic and reference platform hardware the user may easily extend the GAMI Layer by integrating custom drivers. The following GAMI Framework provides the neccessary routines to help the user integrate custom drivers with the PSME like eventing, logging and configuration.

![Figure 1.1](../../../../images/agent_framework.png "PSME GAMI Framework")

Figure shows a structure of PSME GAMI Framework. Blocks marked in gray are generic components that may be used in any PSME driver implementation. Block marked in orange is user provided software that integrates driver with hardware.

### 1.1 JSON-RPC Stack ###

JSON-RPC Stack enables HTTP interface between PSME REST Server and PSME Driver. JSON-RCP 2.0 protocol is currently supported. Protocol  specification may be found on [json-rpc.org/wiki/specification](http://json-rpc.org/wiki/specification).

List of currently supported commands.

1. Compute

+ GetComponents

  JSON-RPC request example: { "jsonrpc" : "2.0", "method" : "getComponents", "params" : { "id" : ComponentUUID }, "id" : 1 }

  ComponentUUID - This parameter is optional. If field is empty, list of all components will be returned.

+ GetComponentInfo

  JSON-RPC request example: { "jsonrpc" : "2.0", "method" : "getComponentInfo", "params" : { "id" : ComponentUUID }, "id" : 1 }

+ GetManagerInfo

  JSON-RPC request example: { "jsonrpc" : "2.0", "method" : "getManagerInfo", "params" : { "id" : ComponentUUID }, "id" : 1 }

+ GetCapabilities

  JSON-RPC request example: { "jsonrpc" : "2.0", "method" : "getCapabilities", "params" : {}, "id" : 1 }

+ GetPrcoessorInfo

  JSON-RPC request example: { "jsonrpc" : "2.0", "method" : "getProcessorInfo", "params" : { "id" : ComponentUUID, "slot" : SlotID }, "id" : 1 }

+ GetProcessorSlotCount

  JSON-RPC request example: { "jsonrpc" : "2.0", "method" : "getProcessorSlotCount", "params" : { "id" : ComponentUUID }, "id" : 1 }

+ GerMemoryInfo

  JSON-RPC request example: { "jsonrpc" : "2.0", "method" : "getMemoryInfo", "params" : { "id" : ComponentUUID, "slot" : SlotID }, "id" : 1 }

+ GetMemorySlotCount

  JSON-RPC request example: { "jsonrpc" : "2.0", "method" : "getMemorySlotCount", "params" : { "id" : ComponentUUID }, "id" : 1 }

### 1.2 Command Engine ###

Command Engine is a software module that provides generic structures to define hardware or interface specific commands.

### 1.3 Registration ###

Registration mechanism is used to establish communication with PSME REST Server. Driver is useless unless registration process is completed.

### 1.4 Eventing ###

Driver sends notifications to PSME REST Server when hot-plug or hot-unplag events occur.

### 1.5 State Machine ###

State Machine is a mechanism that monitors state of each of modules.

### 1.6 Logging ###

Logging format and output destination can be configured in JSON configuration file. The following example describes logger configuration.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"logger" : {
    "rest" : {
        "level" : "DEBUG",
        "streams" : [
            {
                "type" : "FILE",
                "file" : "path_to_file"
            },
            {
                "type" : "STDOUT"
            }
        ],
        "timeformat" : "DATE_NS",
        "color" : true,
        "output" : true,
        "tagging" : true,
        "moredebug" : true
    },
    "core" : { ... },
    "registration" : { ... }
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### 1.7 Configuration ###

Framework provides JSON based configuration interface.

List of configuration flags is shown below.
| Section      | Flag     | Description                                                                                      | Default value | Format  | Required |
| :----------: | :------: | :-------------------------------------------------------------------------------------------     | :-----------: | :----:  | :------: |
| server       | port     | JSON-RPC commands access port                                                                    | 7777          | Integer | yes      |
| registration | ipv4     | Registration IPv4 address of PSME REST Server                                                    | localhost     | String  | yes      |
| registration | port     | Registration TCP port of PSME REST Server                                                        | 8383          | Integer | yes      |
| registration | interval | Registration interval in seconds                                                                 | 3             | Integer | yes      |
| modules      |          | List of modules to be discovered. The structure of particular module is implementation specific. |               | Array   | yes      |
| logger       |          | Logger configuration described in 1.6                                                            |               | Array   | no       |

Configuration example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{
  "server" : {
      "port" : 1234
  },
  "registration" : {
      "ipv4" : "localhost",
      "port" : 5678,
      "interval" : 4
  },
  "logger" : {
  },
  "modules" : [
      {
          "key1" : "value1",
          "key2" : 1,
          "key3" : [ 1, 2 ,3 4]
      }
  ]
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If configuration file is not provided, application starts with defaults.

2. Build project
----------------

Dependencies:

    gcc = 4.9.2
    clang = 3.6.0
    ccache >= 3.1.9
    cmake >= 2.8.12
    make >= 3.82
    gcov >= 4.8.3
    lcov >= 1.11
    scan-build

Build release:

    mkdir build.release
    cd build.release
    cmake ..
    make

Build debug:

    mkdir build.debug
    cd build.debug
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    make

3. Unit testing
---------------

Run unit testing (all build types):

    ctest

All tests results are stored under Testing directory:

    build/Testing/Temporary

4. Code coverage
----------------

Build with code coverage (only GCC):

    mkdir build.coverage
    cd build.coverage
    cmake -DCMAKE_BUILD_TYPE=Coverage ..
    make

Run code coverage. It will run also unit tests to collect code coverage traces:

    make code-coverage

Show code coverage results:

    YOUR_WEB_BROWSER code_coverage/html/index.html

5. Static analyzer
------------------

Run Clang static analyzer:

    export CCC_CC=clang
    export CCC_CXX=clang++
    export CC=/usr/libexec/clang-analyzer/scan-build/ccc-analyzer
    export CXX=/usr/libexec/clang-analyzer/scan-build/c++-analyzer
    mkdir build.release
    cd build.release
    cmake ..
    scan-build make

Always do make clean before rerun scan-build make. Reports can be viewed by
scan-view tool.

6. Documentation
----------------

Generate documentation:

    make doc-generate

Open HTML documentation:

    YOUR_WEB_BROWSER doc/html/index.html

7. Cross compilation
--------------------

Buildroot is needed for ARM cross compilation. For that, change current
directory to tools subdirectory and clone buildroot project using git:

    cd project_path/tools
    git clone git://git.buildroot.net/buildroot
    cd buildroot
    make menuconfig

7.1 Minimal configuration for build buildroot toolchain
---------------------------------------

Example for i.MX287 target:

+ Change Target options:
  +  Target Architecture: ARM (little endian)
  +  Target Binary Format: ELF
  +  Target Architecture Variant: arm926t
  +  Target ABI: EABI
  +  Floating point strategy: Soft float
  +  ARM instruction set: ARM
+ Toolchain:
  +  C library: glibc
  +  glibc versio: 2.20
  +  Binutils version: 2.24
  +  GCC compiler version: 4.9.x
  +  Enable C++ support
  +  Enable MMU support

Next:

    < Save >
    < Exit >
    make -j8

7.2 Using buildroot toolchain for cross compilation
---------------------------------------------------

    cd project_path
    mkdir build
    cd build
    cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Platform/Linux-buildroot-arm.cmake ..
    make -j8
