SET(TARGET_MEM Memdbd)
SET(TARGET_DMP dumpmemdb)
SET(TARGET_TEST memdbtest)

SET(SRC_MEM main.c event.c node.c handle.c snap.c memdb_log.c memdb_jrpc.c)
SET(SRC_DMP dump.c)
SET(SRC_TEST test.c)

SET(LIBS ${LIBS}-lpthread -lrt)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
SET(MEMDB_NEED_LIBS librmm_init.so librmm_utils.a librmm_redfish.so librmm_jsonrpcapi.so librmm_jsonrpc.so librmm_json.so librmm_log.so librmm_cfg.so librmm_securec.so libcurl.a libssl.a libcrypto.a libdl.so)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)

ADD_EXECUTABLE(${TARGET_MEM} ${SRC_MEM})
TARGET_LINK_LIBRARIES(${TARGET_MEM} ${MEMDB_NEED_LIBS})

ADD_EXECUTABLE(${TARGET_DMP} ${SRC_DMP})
TARGET_LINK_LIBRARIES(${TARGET_DMP} ${MEMDB_NEED_LIBS})

ADD_EXECUTABLE(${TARGET_TEST} ${SRC_TEST})
TARGET_LINK_LIBRARIES(${TARGET_TEST} ${MEMDB_NEED_LIBS})
