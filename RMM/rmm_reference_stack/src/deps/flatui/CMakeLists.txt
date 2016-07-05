IF(NOT (EXISTS "${ROOT_DIR}/src/core/restd/web/res/flatUI"))
	IF(NOT (EXISTS "${ROOT_DIR}/src/deps/flatui/2.2.1.zip"))
	    message(STATUS "Dependent library checking ...
    -- Doesn't find flatui 2.2.1.zip
    -- Please manually download install it to directory ${ROOT_DIR}/src/deps/flatui/
    -- Reference download site: https://github.com/designmodo/Flat-UI/archive/2.2.1.zip")
    	message(FATAL_ERROR "Missing flatui 2.2.1.zip")
	ENDIF(NOT (EXISTS "${ROOT_DIR}/src/deps/flatui/2.2.1.zip"))

	IF(NOT (EXISTS "${ROOT_DIR}/src/deps/flatui/bootstrap-3.0.3-dist.zip"))
	    message(STATUS "Dependent library checking ...
    -- Doesn't find bootstrap-3.0.3-dist.zip
    -- Please manually download install it to directory ${ROOT_DIR}/src/deps/flatui/
    -- Reference download site: https://github.com/twbs/bootstrap/releases/download/v3.0.3/bootstrap-3.0.3-dist.zip")
    	message(FATAL_ERROR "Missing bootstrap-3.0.3-dist.zip")
	ENDIF(NOT (EXISTS "${ROOT_DIR}/src/deps/flatui/bootstrap-3.0.3-dist.zip"))

	IF(NOT (EXISTS "${ROOT_DIR}/src/deps/flatui/jquery-2.0.0.min.js"))
	    message(STATUS "Dependent library checking ...
    -- Doesn't find jquery-2.0.0.min.js
    -- Please manually download install it to directory ${ROOT_DIR}/src/deps/flatui/
    -- Reference download site: http://code.jquery.com/jquery-2.0.0.min.js")
    	message(FATAL_ERROR "Missing jquery-2.0.0.min.js")
	ENDIF(NOT (EXISTS "${ROOT_DIR}/src/deps/flatui/jquery-2.0.0.min.js"))

	exec_program("rm -rf ${ROOT_DIR}/src/deps/flatui/Flat-UI-2.2.1")
	exec_program("cd ${ROOT_DIR}/src/deps/flatui/;unzip 2.2.1.zip || exit -1; mv Flat-UI-2.2.1 ${ROOT_DIR}/src/core/restd/web/res/flatUI")
	exec_program("cd ${ROOT_DIR}/src/deps/flatui/;unzip bootstrap-3.0.3-dist.zip; cp dist/js/bootstrap.min.js ${ROOT_DIR}/src/core/restd/web/res/flatUI/dist/js/vendor/") 
	exec_program("rm -rf ${ROOT_DIR}/src/deps/flatui/dist/")
	exec_program("cd ${ROOT_DIR}/src/deps/flatui/;cp jquery-2.0.0.min.js ${ROOT_DIR}/src/core/restd/web/res/flatUI/dist/js/vendor/")
ENDIF(NOT (EXISTS "${ROOT_DIR}/src/core/restd/web/res/flatUI"))
