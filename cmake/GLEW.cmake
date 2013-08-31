message(STATUS "Searching for glew headers")
find_path(GLEW_INCLUDE_DIR
    NAMES GL/glew.h
    HINTS
    PATH_SUFFIXES include Headers
)

message(STATUS "Searching for libglew")
find_library(LIBGLEW
    NAMES libglew32
	HINTS
    PATH_SUFFIXES lib lib64
)
 

message(STATUS "Searching for libglew")
find_library(OPENGL32
    NAMES OpenGL32
	HINTS
    PATH_SUFFIXES lib lib64
)
 
if (LIBGLEW)
        set (GLEW_LIBS "${LIBGLEW}" "${OPENGL32}")
else(LIBGLEW)
		message(FATAL_ERROR "libglew couldn't be found")
endif(LIBGLEW)