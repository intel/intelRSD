ExternalProject_Add(zlib
  INSTALL_DIR ${PROJECT_BINARY_DIR}
  PREFIX ${PROJECT_BINARY_DIR}/zlib_build
  URL ${CMAKE_SOURCE_DIR}/src/deps/zlib/zlib-1.2.8.tar.gz
  SOURCE_DIR ${PROJECT_BINARY_DIR}/zlib
  CONFIGURE_COMMAND ${ZLIB_CC} ${PROJECT_BINARY_DIR}/zlib/configure --prefix=${PROJECT_BINARY_DIR}/zlib_build
  BUILD_COMMAND make
  BUILD_IN_SOURCE 1
)

ExternalProject_Add_Step(zlib install_zlib
  COMMAND rm -f ${PROJECT_BINARY_DIR}/include/zconf.h
  COMMAND rm -f ${PROJECT_BINARY_DIR}/include/zlib.h
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libz.so
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libz.a
  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/include
  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/lib
  COMMAND cp -f ${PROJECT_BINARY_DIR}/zlib_build/include/zconf.h ${PROJECT_BINARY_DIR}/include/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/zlib_build/include/zlib.h ${PROJECT_BINARY_DIR}/include/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/zlib_build/lib/libz.so.1.2.8  ${PROJECT_BINARY_DIR}/lib/libz.so
  COMMAND cp -f ${PROJECT_BINARY_DIR}/zlib_build/lib/libz.a  ${PROJECT_BINARY_DIR}/lib/
  DEPENDEES install
)

