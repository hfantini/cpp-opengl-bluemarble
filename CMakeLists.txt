cmake_minimum_required(VERSION 3.12)

project(CPP-OPENGL-BLUEMARBLE)

add_executable(BLUEMARBLE ./src/main.cpp "src/camera.cpp")

target_include_directories(BLUEMARBLE PRIVATE 
	libs/glm/include
	libs/stb/include
	libs/glfw/include
	libs/glew/include
)

target_link_directories(BLUEMARBLE PRIVATE
	libs/glfw/bin
	libs/glew/bin
)

target_link_libraries(BLUEMARBLE PRIVATE
	glfw3.lib
	glew32.lib
	opengl32.lib
)

add_custom_command(TARGET BLUEMARBLE POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/libs/glew/bin/glew32.dll" "${CMAKE_BINARY_DIR}/glew32.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/shaders" "${CMAKE_BINARY_DIR}/shaders"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/textures" "${CMAKE_BINARY_DIR}/textures"
)