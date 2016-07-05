IF(NOT (EXISTS "${ROOT_DIR}/src/deps/uuid/uuid-1.6.2.tar.gz"))
    message(STATUS "Dependent library checking ...
	-- Doesn't find uuid-1.6.2.tar.gz
	-- Please manually download and install it to directory ${ROOT_DIR}/src/deps/uuid/
	-- Reference download site: ftp://ftp.ossp.org/pkg/lib/uuid/uuid-1.6.2.tar.gz")
    message(FATAL_ERROR "Missing uuid-1.6.2.tar.gz")
ENDIF(NOT (EXISTS "${ROOT_DIR}/src/deps/uuid/uuid-1.6.2.tar.gz"))

IF(ARM)
  SET (CV_COPY ac_cv_va_copy=yes && patch -d ${PROJECT_BINARY_DIR}/uuid -p1 < ${CMAKE_SOURCE_DIR}/src/deps/uuid/uuid.patch)
#  SET (CV_COPY ac_cv_va_copy=yes)
ENDIF()

ExternalProject_Add(uuid
  INSTALL_DIR ${PROJECT_BINARY_DIR}
  PREFIX ${PROJECT_BINARY_DIR}/uuid_build
  URL ${CMAKE_SOURCE_DIR}/src/deps/uuid/uuid-1.6.2.tar.gz
  SOURCE_DIR ${PROJECT_BINARY_DIR}/uuid
  CONFIGURE_COMMAND  ${PROJECT_BINARY_DIR}/uuid/configure ${HOST} --prefix=${PROJECT_BINARY_DIR}/uuid_build --with-pic ${CV_COPY}
  BUILD_COMMAND make;
  BUILD_IN_SOURCE 1
)

ExternalProject_Add_Step(uuid install_uuid
  COMMAND rm -f ${PROJECT_BINARY_DIR}/include/uuid.h
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libuuid.so
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libuuid.so.16
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libuuid.so.16.0.22
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libuuid.a
  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/include
  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/lib
  COMMAND cp -rf ${PROJECT_BINARY_DIR}/uuid_build/include  ${PROJECT_BINARY_DIR}/
#  COMMAND cp -f ${PROJECT_BINARY_DIR}/uuid_build/lib/libuuid.so.16.0.22  ${PROJECT_BINARY_DIR}/lib/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/uuid_build/lib/libuuid.a  ${PROJECT_BINARY_DIR}/lib/
#  COMMAND cp -d ${PROJECT_BINARY_DIR}/uuid_build/lib/libuuid.so.16  ${PROJECT_BINARY_DIR}/lib/
#  COMMAND cp -d ${PROJECT_BINARY_DIR}/uuid_build/lib/libuuid.so  ${PROJECT_BINARY_DIR}/lib/

  DEPENDEES install
)

