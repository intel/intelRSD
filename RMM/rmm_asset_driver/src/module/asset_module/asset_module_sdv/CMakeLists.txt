option (RACK_PTAS "Enable Rack PTAS" OFF)

execute_process(COMMAND cp ${CMAKE_SOURCE_DIR}/src/module/asset_module/ptas_sdk/libptassdk.so ${PROJECT_BINARY_DIR}/lib/
               COMMAND cp ${CMAKE_SOURCE_DIR}/src/module/asset_module/ptas_daemon/OEM_coe_file ${PROJECT_BINARY_DIR}/bin/
               COMMAND cp ${CMAKE_SOURCE_DIR}/src/module/asset_module/ptas_daemon/ptasd ${PROJECT_BINARY_DIR}/bin/)


SET(TARGET AssetModule)

SET(SRC_LIST action.c cm.c cm_update.c dzone.c general.c main.c notify.c ptas.c pzone.c tzone.c ../asset_module_common/gami.c)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)

ADD_EXECUTABLE(${TARGET} ${SRC_LIST})

if (RACK_PTAS)
    TARGET_LINK_LIBRARIES(${TARGET} librmm_init.so librmm_utils.a librmm_redfish.so librmm_jsonrpcapi.so librmm_jsonrpc.so librmm_json.so  libpthread.so libm.so librmm_cfg.so  librmm_wrap.so 
	librmm_log.so librmm_encrypter.so librmm_securec.so libcurl.a libssl.a libcrypto.a libdl.so libuuid.a libptassdk.so)
else ()
    TARGET_LINK_LIBRARIES(${TARGET} librmm_init.so librmm_utils.a librmm_redfish.so librmm_jsonrpcapi.so librmm_jsonrpc.so librmm_json.so  libpthread.so libm.so librmm_cfg.so  librmm_wrap.so 
	librmm_log.so librmm_encrypter.so librmm_securec.so libcurl.a libssl.a libcrypto.a libdl.so libuuid.a)
endif ()