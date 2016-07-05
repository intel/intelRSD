SET(TARGET_LIB rmm_init)

SET(SRC_LIB libinit.c)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)

ADD_LIBRARY(${TARGET_LIB} SHARED ${SRC_LIB})
TARGET_LINK_LIBRARIES(${TARGET_LIB} librmm_log.so libpthread.so librmm_utils.a librt.so librmm_jsonrpc.so librmm_jsonrpcapi.so librmm_redfish.so)
