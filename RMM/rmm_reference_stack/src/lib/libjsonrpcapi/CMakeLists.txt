SET(TARGET_LIB rmm_jsonrpcapi)
SET(TARGET_TEST test_jipmi)
SET(TARGET_TESTD test_jipmid)

SET(SRC_LIB jipmi.c memdb.c jsonrpcapi.c assetd_socket.c assetd_api.c registerd_api.c parser.c asset_module_api.c asset_module_socket.c registerd_socket.c utils.c)
SET(SRC_TEST testjipmi.c)
SET(SRC_TESTD testjipmid.c)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)

ADD_LIBRARY(${TARGET_LIB} SHARED ${SRC_LIB})
TARGET_LINK_LIBRARIES(${TARGET_LIB} librmm_log.so libpthread.so librmm_utils.a librt.so librmm_jsonrpc.so)

ADD_EXECUTABLE(${TARGET_TEST} ${SRC_TEST})
TARGET_LINK_LIBRARIES(${TARGET_TEST} librmm_json.so librmm_log.so librmm_utils.a librmm_jsonrpc.so librmm_jsonrpcapi.so libssl.a libcrypto.a librmm_cfg.so libcurl.a librmm_securec.so)

ADD_EXECUTABLE(${TARGET_TESTD} ${SRC_TESTD})
TARGET_LINK_LIBRARIES(${TARGET_TESTD} librmm_json.so librmm_log.so librmm_utils.a librmm_jsonrpc.so librmm_jsonrpcapi.so libssl.a libcrypto.a librmm_cfg.so libcurl.a )
