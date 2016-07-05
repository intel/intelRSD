IF(NOT (EXISTS "${ROOT_DIR}/src/deps/libcurl/curl-7.40.0.tar.bz2"))
    message(STATUS "Dependent library checking ...
	-- Doesn't find curl-7.40.0.tar.bz2
	-- Please manually download and install it to directory ${ROOT_DIR}/src/deps/libcurl/
	-- Reference download site: http://curl.haxx.se/download/curl-7.40.0.tar.bz2")
    message(FATAL_ERROR "Missing curl-7.40.0.tar.bz2")
ENDIF(NOT (EXISTS "${ROOT_DIR}/src/deps/libcurl/curl-7.40.0.tar.bz2"))


ExternalProject_Add(libcurl
  INSTALL_DIR ${PROJECT_BINARY_DIR}
  PREFIX ${PROJECT_BINARY_DIR}/libcurl_build
  URL ${CMAKE_SOURCE_DIR}/src/deps/libcurl/curl-7.40.0.tar.bz2
  SOURCE_DIR ${PROJECT_BINARY_DIR}/libcurl
  CONFIGURE_COMMAND  ${PROJECT_BINARY_DIR}/libcurl/configure --enable-shared=no --with-ssl=${PROJECT_BINARY_DIR}/ssl_build ${HOST} --prefix=${PROJECT_BINARY_DIR}/libcurl_build --disable-ldap --disable-ldaps --without-librtmp LIBS=-ldl --without-zlib --without-libidn
  BUILD_COMMAND make;
  BUILD_IN_SOURCE 1
)

ExternalProject_Add_Step(libcurl install_libcurl
  COMMAND rm -f ${PROJECT_BINARY_DIR}/include/curl/*.h
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libcurl.so
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libcurl.so.4
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libcurl.so.4.3.0
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libcurl.a
  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/include/curl
  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/lib
  COMMAND cp -rf ${PROJECT_BINARY_DIR}/libcurl_build/include/curl/  ${PROJECT_BINARY_DIR}/include/
#  COMMAND cp -f ${PROJECT_BINARY_DIR}/libcurl_build/lib/libcurl.so.4.3.0  ${PROJECT_BINARY_DIR}/lib/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/libcurl_build/lib/libcurl.a  ${PROJECT_BINARY_DIR}/lib/
#  COMMAND cp -d ${PROJECT_BINARY_DIR}/libcurl_build/lib/libcurl.so  ${PROJECT_BINARY_DIR}/lib/

  DEPENDEES install
)

