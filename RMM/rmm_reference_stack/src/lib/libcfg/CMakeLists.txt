SET(TARGET_LIB rmm_cfg)
SET(TARGET_TEST test_rmmcfg)

SET(SRC_LIB cfg.c)
SET(SRC_TEST cfg.c test.c)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)

ADD_LIBRARY(${TARGET_LIB} SHARED ${SRC_LIB})
TARGET_LINK_LIBRARIES(${TARGET_LIB} librmm_json.so librmm_utils.a librmm_encrypter.so librmm_securec.so)

ADD_EXECUTABLE(${TARGET_TEST} ${SRC_TEST})
TARGET_LINK_LIBRARIES(${TARGET_TEST} librmm_json.so libcurl.a libcrypto.a libssl.a librmm_securec.so librmm_encrypter.so librmm_utils.a libpthread.so)
