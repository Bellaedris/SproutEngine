cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project(SproutEngine)

set(CMAKE_CXX_STANDARD 17) # this does nothing for MSVC, use target_compile_options below
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# fix for a weird bug that caused windows.h to expand "max" and "min" keywords into macros.
add_definitions(-DNOMINMAX)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

link_directories(${CMAKE_SOURCE_DIR}/lib)

# find the required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
# find_package(SOIL REQUIRED)
# message(STATUS "Found SOIL in ${SOIL_INCLUDE_DIR}")
# find_package(GLEW REQUIRED)
# message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIR}")
if(APPLE)
  find_package(Freetype REQUIRED)
  message(STATUS "Found Freetype in ${FREETYPE_INCLUDE_DIRS}")
endif(APPLE)

if(WIN32)
  set(LIBS glfw3 opengl32 assimp freetype irrKlang)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)
  # note that the order is important for setting the libs
  # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
  set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype ${ASSIMP_LIBRARY})
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} ${FREETYPE_LIBRARIES})
  set(LIBS ${LIBS} ${APPLE_LIBS})
else()
  set(LIBS )
endif(WIN32)

set(PROJECTS
    tests
    framebuffers
    physics
    raytracer
    PBR
    editor
)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

# first create relevant static libraries required for other projects
add_library(STB_IMAGE "src/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

#core sprout engine target
file(GLOB_RECURSE CORE_SOURCES "includes/sprout_engine/*.cpp")

add_library(
        SproutEngineCore
        ${CORE_SOURCES}
)
set(LIBS ${LIBS} SproutEngineCore)

file(GLOB IMGUI_SOURCES "includes/imgui/*.cpp")
add_library(
        IMGUI
        ${IMGUI_SOURCES}
)
set(LIBS ${LIBS} IMGUI)

macro(makeLink src dest target)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}  DEPENDS  ${dest} COMMENT "mklink ${src} -> ${dest}")
endmacro()

function(create_project_from_sources project)
	file(GLOB_RECURSE SOURCE
            "src/${project}/*.h"
            "src/${project}/*.cpp"
            "src/${project}/*.vs"
            "src/${project}/*.fs"
            "src/${project}/*.tcs"
            "src/${project}/*.tes"
            "src/${project}/*.gs"
            "src/${project}/*.comp"
    )
	if (project STREQUAL "")
		SET(replaced "")
		string(REPLACE "/" "_" replaced ${project})
		set(NAME ${replaced})
	else()
		set(NAME "SproutEngine__${project}")
	endif()
    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} ${LIBS})
    if(MSVC)
		target_compile_options(${NAME} PRIVATE /std:c++17 /MP)
        target_link_options(${NAME} PUBLIC /ignore:4099)
    endif(MSVC)
    if(WIN32)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${project}")
        set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${project}/Debug")
    elseif(UNIX AND NOT APPLE)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${project}")
    elseif(APPLE)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${project}")
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${project}")
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${project}")
    endif(WIN32)
    # copy shader files to build directory
    file(GLOB SHADERS
             "shaders/**/*.vs"
             "shaders/**/*.frag"
             "shaders/**/*.fs"
             "shaders/**/*.tcs"
             "shaders/**/*.tes"
             "shaders/**/*.gs"
             "shaders/**/*.comp"
    )
	# copy dlls
	file(GLOB DLLS "dlls/*.dll")
    foreach(SHADER ${SHADERS})
        if(WIN32)
            # configure_file(${SHADER} "test")
            add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>)
			add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${DLLS} $<TARGET_FILE_DIR:${NAME}>)
        elseif(UNIX AND NOT APPLE)
            file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/${project})
        elseif(APPLE)
            # create symbolic link for *.vs *.fs *.gs
            get_filename_component(SHADERNAME ${SHADER} NAME)
            makeLink(${SHADER} ${CMAKE_SOURCE_DIR}/bin/${project}/${SHADERNAME} ${NAME})
        endif(WIN32)
    endforeach(SHADER)
    # if compiling for visual studio, also use configure file for each project (specifically to set up working directory)
    if(MSVC)
        configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user @ONLY)
    endif(MSVC)
endfunction()

# then create a project file per tutorial
foreach(PROJECT ${PROJECTS})
		create_project_from_sources(${PROJECT})
endforeach(PROJECT)

include_directories(${CMAKE_SOURCE_DIR}/includes)

# Tests execuables
enable_testing()

file(GLOB_RECURSE TESTS "tests/Test*.cpp")

add_executable(TestsSproutCore ${TESTS})

target_link_libraries(
        TestsSproutCore
        GTest::gtest_main
        ${LIBS}
)

include(GoogleTest)
gtest_discover_tests(TestsSproutCore)