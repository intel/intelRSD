PSME Agent Framework
====================

1. Information
--------------

PSME Agent Framework - Application

2. Build project
----------------

Dependencies:

    gcc = 4.9.2
    clang = 3.6.0
    ccache >= 3.1.9
    cmake >= 2.8.12
    make >= 3.82
    valgrind >= 3.9.0
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

7. Workarounds
--------------

To resolve issue with *unreachable-code* warning generate from default
switch case, use the newest Clang release. Example for Fedora 20:

    wget http://llvm.org/releases/3.5.1/clang+llvm-3.5.1-i686-fedora20.tar.xz
    tar xvf clang+llvm-3.5.1-i686-fedora20.tar.xz
    export PATH=path_to_clang/clang+llvm-3.5.1-i686-fedora20/bin:$PATH

The newest Clang releases can be download from http://llvm.org/releases/download.html
