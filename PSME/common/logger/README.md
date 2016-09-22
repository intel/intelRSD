README
======

1. Information
--------------

Logger library that use threads for all IO operations and time consuming log
formatting.

* support for C/C++, one common library with separated interfaces
* write log messages to file, standard output, standard error output or
  send through UDP/TCP socket
* the same log message can be write parallel to many different streams
* create different logger objects for different submodules
* logger buffer object for easy join separate messages in one
* log message levels
* log message tagging
* log message time stamp with formatting option
* log message coloring with special ANSI codes
* log message can be enabled/disabled on the fly
* logger object can write to many stream objects
* stream object can handle many logger objects
* independent options for all logger and stream objects that can be change
  on the fly

2. Build logger
---------------

Dependencies:

    pthread (until C libraries doesn't support full C11 standard with threads)
    GCC/Clang
    cmake
    make

Build logger library:

    mkdir build
    cd build
    cmake ..
    make

Logger library:

    build/lib/liblogger.a

Logger examples:

    build/examples

Logger headers for C application:

    src/logger.h
    src/logger_stream.h
    src/logger_buffer.h

Logger headers for C++ application:

    src/logger.hpp
    src/logger_stream.hpp

3. Usage
--------

Simple initialization in C:

    #include "logger.h"
    #include "logger_stream.h"

    #include <stdlib.h>     // atexit

    /* Common practise is create easy to manage macros */
    #define LOGUSR          g_logusr
    #define LOGOUT          g_logout
    #define LOGFILE         "/var/log/example.log"

    /* Extern these handlers in your project */
    struct logger *LOGUSR = NULL;
    struct logger_stream *LOGOUT = NULL;

    static void logger_cleanup(void);

    /* Use this function at the start point in your application.
     * Setting last parameter to NULL will configure logger and stream output
     * to default settings with enable output, tagging log messages,
     * more debug information like file name, function name and line number.
     * Colors are disabled when setting to default because files don't like
     * extra characters that cannot print :)
     *
     * First parameter is tag ID string, this is optional. You can set it to
     * NULL, logger won't print tag in log message.
     *
     * You must add logger stream object to logger object. Logger stream can
     * handle various output like standard output, files and remote UDP!
     * Logger objects prepare log message for logger stream objects.
     * You can add many diffrent streams to logger and create many logger
     * objects for various submodules with diffrent unique settings!
     * */
    void logger_init(void) {
        LOGOUT = logger_stream_create(LOGGER_STREAM_FILE, "log file", NULL);
        logger_stream_open_file(LOGOUT, LOGFILE);

        LOGUSR = logger_create("USR", NULL);
        logger_add_stream(LOGUSR, LOGOUT);

        atexit(logger_cleanup);
    }

    /* It will be called automatically before application exit */
    static void logger_cleanup(void) {
        logger_destroy(LOGUSR);
        logger_stream_destroy(LOGOUT);
    }

C++ version:

    #include "logger.hpp"
    #include "logger_stream.hpp"

    #include <cstdlib>      // atexit

    using namespace logger_cpp;

    #define LOGUSR          g_logusr
    #define LOGOUT          g_logout
    #define LOGFILE         "/var/log/example.log"

    /* Extern these handlers in your project */
    Logger* LOGUSR = nullptr;
    Stream* LOGOUT = nullptr;

    static void logger_cleanup();

    /* Put this function at the begining after the main */
    void logger_init() {
        LOGOUT = new Stream(Stream::Type::FILE, "stdout");
        LOGOUT->open_file(LOGFILE);

        LOGUSR = new Logger("USR");
        LOGUSR->add_stream(LOGOUT);

        atexit(logger_cleanup);
    }

    static void logger_cleanup() {
        delete LOGUSR;
        delete LOGOUT;
    }

Sample log:

    #include "logger.h"

    log_info(LOGUSR, "Sample text %d\n", 7);

Sample log output:

    2014-07-28 15:23:41.284651 - INFO - Sample text 7

Too see how proper use logger library in your application please see
examples/logger_example_streams.c source code for reference example.

4. Log buffer
-------------

Include required headers

    #include "logger_buffer.h"

Create logger buffer object (dynamic allocation):

    struct logger_buffer *logbuf = logbuf_create();

Write some message to the logger buffer:

    logbuf_write(logbuf, "Sample text ");
    logbuf_write(logbuf, "%s ", "data contain");
    logbuf_vector(logbuf, data, 2);

When you finished, write message through logger methods using logbuf_string:

    logbuf_warning(LOGUSR, "%s", logbuf_string(logbuf));

You must always provide to destroy logger buffer object before you exit:

    logbuf_destroy(logbuf);

Sample log output:

    2014-07-28 15:23:41.284651 - ALERT - Sample text data contain [2][AA CC]

5. Tests
--------

Building automatically after building logger library.

6. Documentation
----------------

Building automatically after building logger library.
