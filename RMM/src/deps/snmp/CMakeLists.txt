IF(NOT (EXISTS "${ROOT_DIR}/src/deps/snmp/net-snmp-5.7.3.tar.gz"))
	message(STATUS "Dependent library checking ...
	-- Doesn't find net-snmp-5.7.3.tar.gz
	-- Please manually download install it to directory ${ROOT_DIR}/src/deps/snmp/
	-- Reference download site: http://sourceforge.net/projects/net-snmp/files/net-snmp/5.7.3/")
	message(FATAL_ERROR "Missing net-snmp-5.7.3.tar.gz")
ENDIF(NOT (EXISTS "${ROOT_DIR}/src/deps/snmp/net-snmp-5.7.3.tar.gz"))

ExternalProject_Add(snmp
  INSTALL_DIR ${PROJECT_BINARY_DIR}
  PREFIX ${PROJECT_BINARY_DIR}/snmp_build
  URL ${CMAKE_SOURCE_DIR}/src/deps/snmp/net-snmp-5.7.3.tar.gz
  SOURCE_DIR ${PROJECT_BINARY_DIR}/snmp
  CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/snmp/configure ${HOST} --with-default-snmp-version=3  --with-sys-contact="" --with-sys-location="" --with-logfile=/var/log/snmpd.log --with-persistent-directory=/var/net-snmp --disable-embedded-perl --disable-perl-cc-checks --enable-shared=no --enable-static=yes --prefix=${PROJECT_BINARY_DIR}/snmp_build LDFLAGS=-L${PROJECT_BINARY_DIR}/ssl_build/lib LIBS=-lcrypto #LIBS=-ldl
  BUILD_COMMAND make
  BUILD_IN_SOURCE 1
)

ExternalProject_Add_Step(snmp install_snmp
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/sbin/snmpd  ${PROJECT_BINARY_DIR}/bin
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/sbin/snmptrapd   ${PROJECT_BINARY_DIR}/bin
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/bin/snmpget   ${PROJECT_BINARY_DIR}/bin
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/bin/snmpset  ${PROJECT_BINARY_DIR}/bin
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/bin/net-snmp-config  ${PROJECT_BINARY_DIR}/bin
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/bin/snmptranslate  ${PROJECT_BINARY_DIR}/bin
  COMMAND rm -rf ${PROJECT_BINARY_DIR}/include/net-snmp
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libnetsnmp.a
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libnetsnmptrapd.a
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libnetsnmpagent.a
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libnetsnmpmibs.a
  COMMAND rm -f ${PROJECT_BINARY_DIR}/lib/libnetsnmphelpers.a
  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/include
  COMMAND mkdir -p ${PROJECT_BINARY_DIR}/lib
  COMMAND cp -rf ${PROJECT_BINARY_DIR}/snmp_build/include/net-snmp  ${PROJECT_BINARY_DIR}/include/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/lib/libnetsnmptrapd.a  ${PROJECT_BINARY_DIR}/lib/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/lib/libnetsnmp.a  ${PROJECT_BINARY_DIR}/lib/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/lib/libnetsnmpagent.a  ${PROJECT_BINARY_DIR}/lib/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/lib/libnetsnmpmibs.a  ${PROJECT_BINARY_DIR}/lib/
  COMMAND cp -f ${PROJECT_BINARY_DIR}/snmp_build/lib/libnetsnmphelpers.a  ${PROJECT_BINARY_DIR}/lib/
  DEPENDEES install
)
