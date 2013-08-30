message(STATUS "Searching for sfml headers")
find_path(SFML_INCLUDE_DIR
    NAMES SFML/OpenGL.hpp
    HINTS
    PATH_SUFFIXES include Headers
)

message(STATUS "Searching for sfml-window.lib")
find_library(SFML_WINDOW_LIB
    NAMES sfml-window
	HINTS
    PATH_SUFFIXES lib lib64
)

message(STATUS "Searching for sfml-system.lib")
find_library(SFML_SYSTEM_LIB
    NAMES sfml-system
    HINTS
    PATH_SUFFIXES lib lib64
)


if (SFML_SYSTEM_LIB)
    if (SFML_WINDOW_LIB)
        set (SFML_LIBS "${SFML_WINDOW_LIB}" "${SFML_SYSTEM_LIB}")
	else(SFML_WINDOW_LIB)
		message(FATAL_ERROR "sfml-window.lib couldn't be found")
	endif(SFML_WINDOW_LIB)
else(SFML_SYSTEM_LIB)
		message(FATAL_ERROR "sfml-system.lib couldn't be found")
endif(SFML_SYSTEM_LIB)