IF(NOT (EXISTS "${ROOT_DIR}/src/deps/stunnel/stunnel-5.20.tar.gz"))
    message(STATUS "Dependent library checking ...
    -- Doesn't find stunnel-5.20.tar.gz
    -- Please manually download install it to directory ${ROOT_DIR}/src/deps/stunnel/
    -- Reference download site: http://www.usenix.org.uk/mirrors/stunnel/archive/5.x/stunnel-5.20.tar.gz")
    message(FATAL_ERROR "Missing stunnel-5.20.tar.gz")
ENDIF(NOT (EXISTS "${ROOT_DIR}/src/deps/stunnel/stunnel-5.20.tar.gz"))

SET (CFLAGS "-Wl,-rpath,/usr/local/lib/ -Wl,-rpath-link,${ROOT_DIR}/build/lib")

ExternalProject_Add(stunnel
  INSTALL_DIR ${PROJECT_BINARY_DIR}
  PREFIX ${PROJECT_BINARY_DIR}/stunnel_build
  URL ${CMAKE_SOURCE_DIR}/src/deps/stunnel/stunnel-5.20.tar.gz
  SOURCE_DIR ${PROJECT_BINARY_DIR}/stunnel
  CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/stunnel/configure ${HOST} --prefix=${PROJECT_BINARY_DIR}/stunnel_build --with-ssl=${PROJECT_BINARY_DIR}/ssl_build
  BUILD_COMMAND make
  BUILD_IN_SOURCE 1
)

ExternalProject_Add_Step(stunnel install_stunnel
  COMMAND cp -f ${PROJECT_BINARY_DIR}/stunnel_build/bin/stunnel  ${PROJECT_BINARY_DIR}/bin
  DEPENDEES install
)

