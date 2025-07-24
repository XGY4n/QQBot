cmake_minimum_required(VERSION 3.10)

project(QQBot VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PYTHON_DIR "${CMAKE_SOURCE_DIR}/Python")
set(DLL_DIR "${CMAKE_SOURCE_DIR}/dll")
set(PYTHON_INCLUDE_DIR "D:/workspace/Python/Python39/include" CACHE PATH "Python include path")

set(INI_DIR "${CMAKE_SOURCE_DIR}/ini")

cmake_host_system_information(RESULT CPU_NUMBER_OF_LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
message(STATUS "Number of logical CPU cores: ${CPU_NUMBER_OF_LOGICAL_CORES}")

include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${PYTHON_INCLUDE_DIR}"
)

link_directories("${CMAKE_SOURCE_DIR}/libs")

add_definitions(-D_WINSOCKAPI_)
add_definitions(-D_CLASS_LOG_)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp" "src/*.h")

add_executable(QQBot ${SOURCES} ${HEADERS})
source_group(TREE "${CMAKE_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})

target_compile_definitions(QQBot PRIVATE UNICODE _UNICODE)

target_link_libraries(QQBot
    "python39_d.lib"
)

if(CPU_NUMBER_OF_LOGICAL_CORES)
  target_compile_options(QQBot PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/MP${CPU_NUMBER_OF_LOGICAL_CORES}>)
endif()

target_compile_options(QQBot PRIVATE
    $<$<CONFIG:Debug>:/ZI>
)

add_custom_command(TARGET QQBot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL_DIR}/python3.dll" "$<TARGET_FILE_DIR:QQBot>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL_DIR}/python39.dll" "$<TARGET_FILE_DIR:QQBot>"
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
