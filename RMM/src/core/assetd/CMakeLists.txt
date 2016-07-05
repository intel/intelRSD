SET(TARGET Assetd)

SET(SRC_LIST attribute.c handler.c main.c map.c utils.c)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)

ADD_EXECUTABLE(${TARGET} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${TARGET} librmm_init.so librmm_utils.a libcurl.a libssl.a libcrypto.a libdl.so librmm_jsonrpcapi.so libpthread.so libm.so librmm_cfg.so librmm_wrap.so librmm_log.so librmm_jsonrpc.so librmm_json.so librmm_redfish.so librmm_securec.so libuuid.a)

