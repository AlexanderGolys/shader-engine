if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
	add_definitions( "/MP" )
endif()


add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
	-DSPDLOG_COMPILED_LIB
)

### GLFW ###

set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
add_subdirectory (glfw-3.1.2)
add_subdirectory (spdlog-1.x)

include_directories(
	glfw-3.1.2/include/GLFW/
	glew-2.1.0/include/
	spdlog-1.x/include/
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
	-lGL -lGLU -lXrandr -lXext -lX11 -lrt
	${CMAKE_DL_LIBS}
	${GLFW_LIBRARIES}
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

set(GLEW_SOURCE glew-2.1.0/src/glew.c)
set(GLEW_HEADERS glew-2.1.0/include/GL/glew.h)

add_library(
		GLEW_1130 STATIC
		${GLEW_SOURCE}
		${GLEW_HEADERS}
)

target_link_libraries(GLEW_1130
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)
