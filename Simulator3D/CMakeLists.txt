cmake_minimum_required(VERSION 3.10)


project(Simulator3D LANGUAGES CXX C)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


# set files to link

set(SRCDIR "Simulator3D")

set(SOURCE_FILES
	${SRCDIR}/Shader.h
	${SRCDIR}/Camera.h
	${SRCDIR}/ParticleStructures.h
	${SRCDIR}/Utils.h 	${SRCDIR}/Utils.cpp
	${SRCDIR}/SimVisualizer.h 	${SRCDIR}/SimVisualizer.cpp
	${SRCDIR}/Simulator_3D.h 	${SRCDIR}/Simulator_3D.cpp


	${SRCDIR}/IO/FrameSBF.h
	${SRCDIR}/IO/WriteSBF.h 	${SRCDIR}/IO/WriteSBF.cpp
	${SRCDIR}/IO/ReadSBF.h 	${SRCDIR}/IO/ReadSBF.cpp
	${SRCDIR}/glad/glad.c

	)



# Add projects

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(OpenMP REQUIRED)

# Add headers
set(HEADERS_DIR ../Libraries/includes)

# link

add_executable(Simulator3D ${SRCDIR}/EntryPoint.cpp ${SOURCE_FILES})

target_include_directories(Simulator3D PUBLIC ${SRCDIR})

target_link_libraries(Simulator3D glfw)

target_include_directories(Simulator3D PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(Simulator3D ${OPENGL_gl_LIBRARY})

target_include_directories(Simulator3D PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(Simulator3D ${Boost_LIBRARIES})

target_link_libraries(Simulator3D OpenMP::OpenMP_CXX)



target_include_directories(Simulator3D PUBLIC ${HEADERS_DIR})



# copy files

set(SHADER_DIR "${CMAKE_SOURCE_DIR}/Simulator3D/shaders")
set(SHADER_SHADOW_DIR "${CMAKE_SOURCE_DIR}/Simulator3D/shadersShadows")


set(SHADER_DST "$<TARGET_FILE_DIR:Simulator3D>/shaders")
set(SHADER_SHADOW_DST "$<TARGET_FILE_DIR:Simulator3D>/shadersShadows")

add_custom_command(
        TARGET Simulator3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
        "Linking: ${SHADER_DIR} to ${SHADER_DST}, and ${SHADER_SHADOW_DIR} to ${SHADER_SHADOW_DST}"
    )


add_custom_command(
        TARGET Simulator3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${SHADER_DIR}" "${SHADER_DST}"
        )


add_custom_command(
        TARGET Simulator3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${SHADER_SHADOW_DIR}" "${SHADER_SHADOW_DST}"
        )
