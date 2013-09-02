cmake_minimum_required (VERSION 2.6)
project(trillek-client-core)

set(CMAKE_MODULE_PATH ${trillek-client-core_SOURCE_DIR}/cmake)

set(TRILLEK_INCLUDE_DIR ${trillek-client-core_SOURCE_DIR}/include)

message(STATUS ${PROJECT_SOURCE_DIR})
message(STATUS ${PROJECT_BINARY_DIR})
message(STATUS "Adding Source and Header Files")
set(src_dir dir)
FILE(GLOB_RECURSE tmp_raw_src_list src/*.cpp)
SET(src_list ${tmp_raw_src_list})
FILE(GLOB_RECURSE tmp_raw_src_list src/*.c)
SET(src_list ${src_list} ${tmp_raw_src_list})
FILE(GLOB_RECURSE tmp_raw_src_list src/*.cc)
SET(src_list ${src_list} ${tmp_raw_src_list})
FILE(GLOB_RECURSE tmp_raw_src_list include/*.h)
SET(src_list ${src_list} ${tmp_raw_src_list})
FILE(GLOB_RECURSE tmp_raw_src_list include/*.hpp)
SET(src_list ${src_list} ${tmp_raw_src_list})

#In case you want to profile, comment out otherwise
SET(CMAKE_CXX_FLAGS "-pg")  

if(src_list)
	add_executable(trillek-client-core ${src_list})
	
	include_directories(trillek-client-core
		${TRILLEK_INCLUDE_DIR}
		${OPENGL_INCLUDE_DIR}
		${SFML_INCLUDE_DIR}
		${GLEW_INCLUDE_DIR}
	)

	target_link_libraries(trillek-client-core
		${OPENGL_LIBRARIES}
		${SFML_LIBS}
		${GLEW_LIBS}
	)
	set_target_properties(trillek-client-core PROPERTIES LINKER_LANGUAGE CXX)
    add_definitions("-std=c++11 -Wall -Wextra")
	if(WIN32)
		target_link_libraries(trillek-client-core 
            "sfml-graphics" 
            "sfml-audio" 
            "sfml-network" 
            "sfml-window" 
            "sfml-system" 
            "opengl32" 
            "GLEW32" 
            "stdc++")   #add windows assimp here
	else(WIN32)
		target_link_libraries(trillek-client-core 
            "sfml-graphics" 
            "sfml-audio" 
            "sfml-network" 
            "sfml-window" 
            "sfml-system" 
            "GL" 
            "GLEW" 
            "stdc++" 
            "jpeg" 
            "assimp")
	endif()
else(src_list)
	message(WARNING "No Source or Header files found")
endif()
message(STATUS ${PROJECT_SOURCE_DIR})
add_custom_command(	TARGET trillek-client-core PRE_BUILD
					${CMAKE_COMMAND} -E copy
					"${PROJECT_SOURCE_DIR}/test.cfg"
					"${PROJECT_BINARY_DIR}")