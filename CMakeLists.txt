cmake_minimum_required(VERSION 3.10)

project(QQBot VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Python 配置由外部输入
set(PYTHON_INCLUDE "" CACHE PATH "Path to Python include directory")
set(PYTHON_LIBDIR "" CACHE PATH "Path to Python lib directory")
set(PYTHON_LIBNAME "" CACHE STRING "Python library name (e.g. python39, python310)")
set(PYTHON_DLL "" CACHE PATH "Path to Python dll directory")

set(PYTHON_DIR "${CMAKE_SOURCE_DIR}/Python")
set(INI_DIR "${CMAKE_SOURCE_DIR}/ini")

cmake_host_system_information(RESULT CPU_NUMBER_OF_LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
message(STATUS "Number of logical CPU cores: ${CPU_NUMBER_OF_LOGICAL_CORES}")

include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${PYTHON_INCLUDE}"
)

link_directories("${PYTHON_LIBDIR}")

add_definitions(-D_WINSOCKAPI_)
add_definitions(-D_CLASS_LOG_)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp" "src/*.h")

add_executable(QQBot ${SOURCES} ${HEADERS})
source_group(TREE "${CMAKE_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})

target_compile_definitions(QQBot PRIVATE UNICODE _UNICODE)

target_link_libraries(QQBot
    ${PYTHON_LIBNAME}
)

if(CPU_NUMBER_OF_LOGICAL_CORES)
  target_compile_options(QQBot PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/MP${CPU_NUMBER_OF_LOGICAL_CORES}>)
endif()

target_compile_options(QQBot PRIVATE
    $<$<CONFIG:Debug>:/ZI>
)

add_custom_command(TARGET QQBot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PYTHON_DLL}" "$<TARGET_FILE_DIR:QQBot>"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_DIR}" "$<TARGET_FILE_DIR:QQBot>/Python"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_DIR}" "${CMAKE_BINARY_DIR}/Python"
    COMMENT "Copying runtime files to output directory"
)

foreach(source_file ${SOURCES} ${HEADERS})
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${source_file}")
    get_filename_component(dir "${rel_path}" DIRECTORY)
    string(REPLACE "/" "\\" group "${dir}")
    source_group("${group}" FILES "${source_file}")
endforeach()
