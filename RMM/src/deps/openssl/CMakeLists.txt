IF(NOT (EXISTS "${ROOT_DIR}/src/deps/openssl/openssl-1.0.1u.tar.gz"))
    message(STATUS "Dependent library checking ...
        -- Doesn't find openssl-1.0.1u.tar.gz
        -- Please manually download and install it to directory ${ROOT_DIR}/src/deps/openssl/
        -- Reference download site: http://www.openssl.org/source/old/1.0.1/openssl-1.0.1u.tar.gz")
    message(FATAL_ERROR "Missing openssl-1.0.1u.tar.gz")
ENDIF(NOT (EXISTS "${ROOT_DIR}/src/deps/openssl/openssl-1.0.1u.tar.gz"))

IF(ARM)
  SET (OPENSSL_HOST linux-generic32 no-asm shared)
ELSE()
  SET (OPENSSL_HOST linux-x86_64 no-asm shared)
ENDIF()

ExternalProject_Add(openssl
  INSTALL_DIR ${PROJECT_BINARY_DIR}
  PREFIX ${PROJECT_BINARY_DIR}/ssl_build
  URL ${CMAKE_SOURCE_DIR}/src/deps/openssl/openssl-1.0.1u.tar.gz
  SOURCE_DIR ${PROJECT_BINARY_DIR}/ssl
  CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/ssl/Configure -fPIC ${OPENSSL_HOST} no-shared --prefix=${PROJECT_BINARY_DIR}/ssl_build --openssldir=${PROJECT_BINARY_DIR}/ssl_build
  BUILD_COMMAND make
  BUILD_IN_SOURCE 1
)

SET (LIB_CRYPTO libcrypto.a)
SET (LIB_SSL libssl.a)

ExternalProject_Add_Step(openssl install_ssl
  COMMAND rm -rf ${PROJECT_BINARY_DIR}/include/openssl
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/${LIB_CRYPTO}
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/${LIB_SSL}
  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/include
  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/lib
  COMMAND cp -rf ${PROJECT_BINARY_DIR}/ssl_build/include/openssl ${PROJECT_BINARY_DIR}/include/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/ssl_build/lib/${LIB_CRYPTO}  ${PROJECT_BINARY_DIR}/lib/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/ssl_build/lib/${LIB_SSL}  ${PROJECT_BINARY_DIR}/lib/
  DEPENDEES install
)

