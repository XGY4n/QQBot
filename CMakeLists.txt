cmake_minimum_required(VERSION 3.10)

project(QQBot VERSION 1.0)

if(NOT DEFINED _BLOCK)
    set(_BLOCK 0)
endif()

if(NOT (_BLOCK STREQUAL "0" OR _BLOCK STREQUAL "1"))
    message(FATAL_ERROR "_BLOCK must be either 0 or 1")
endif()

message(STATUS "The value of _BLOCK is: ${_BLOCK}")

add_definitions(-D_BLOCK=${_BLOCK})

if(NOT DEFINED _CLASS_LOG_)
    set(_CLASS_LOG_ 1)
endif()
add_definitions(-D_CLASS_LOG_=${_CLASS_LOG_})
add_definitions(-D_WINSOCKAPI_)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(CPU_NUMBER_OF_LOGICAL_CORES)
    target_compile_options(QQBot PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/MP${CPU_NUMBER_OF_LOGICAL_CORES}>)
endif()

set(PYTHON_INCLUDE "" CACHE PATH "Path to Python include directory")
set(PYTHON_LIBDIR "" CACHE PATH "Path to Python lib directory")
set(PYTHON_LIBNAME "" CACHE STRING "Python library name (e.g. python39, python310)")
set(PYTHON_DLL "" CACHE PATH "Path to Python dll directory")
set(PROTOBUF_PATH "" CACHE PATH "Path to protobuf directory")

set(PYTHON_DIR "${CMAKE_SOURCE_DIR}/Python")

cmake_host_system_information(RESULT CPU_NUMBER_OF_LOGICAL_CORES QUERY NUMBER_OF_LOGICAL_CORES)
message(STATUS "Number of logical CPU cores: ${CPU_NUMBER_OF_LOGICAL_CORES}")

# -------------------------------
# Python
# -------------------------------
include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${PYTHON_INCLUDE}"
)

link_directories("${PYTHON_LIBDIR}")


# -------------------------------
# Protobuf-lib
# -------------------------------
if(PROTOBUF_PATH)
    set(Protobuf_INCLUDE_DIR "${PROTOBUF_PATH}/include")
    include_directories(${Protobuf_INCLUDE_DIR})
	
	set(Protobuf_PROTOC_EXECUTABLE "${PROTOBUF_PATH}/bin/protoc.exe")
	
	
    set(Protobuf_DEBUG_LIB "${PROTOBUF_PATH}/lib/libprotobufd.lib")
    set(Protobuf_RELEASE_LIB "${PROTOBUF_PATH}/lib/libprotobuf.lib")

    set(Protoc_DEBUG_LIB "${PROTOBUF_PATH}/lib/libprotocd.lib")
    set(Protoc_RELEASE_LIB "${PROTOBUF_PATH}/lib/libprotoc.lib")

    set(Protobuf_LITE_DEBUG_LIB "${PROTOBUF_PATH}/lib/libprotobuf-lited.lib")
    set(Protobuf_LITE_RELEASE_LIB "${PROTOBUF_PATH}/lib/libprotobuf-lite.lib")

    set(Protobuf_DLL_DEBUG "${PROTOBUF_PATH}/bin/libprotobufd.dll")
    set(Protobuf_DLL_RELEASE "${PROTOBUF_PATH}/bin/libprotobuf.dll")
    set(Protoc_DLL_DEBUG "${PROTOBUF_PATH}/bin/libprotocd.dll")
    set(Protoc_DLL_RELEASE "${PROTOBUF_PATH}/bin/libprotoc.dll")
endif()

message(STATUS "Protobuf protoc executable: ${Protobuf_PROTOC_EXECUTABLE}")
message(STATUS "Protobuf include dir: ${Protobuf_INCLUDE_DIR}")
message(STATUS "Protobuf debug lib: ${Protobuf_DEBUG_LIB}")
message(STATUS "Protobuf release lib: ${Protobuf_RELEASE_LIB}")
message(STATUS "Protoc debug lib: ${Protoc_DEBUG_LIB}")
message(STATUS "Protoc release lib: ${Protoc_RELEASE_LIB}")
message(STATUS "Protobuf lite debug lib: ${Protobuf_LITE_DEBUG_LIB}")
message(STATUS "Protobuf lite release lib: ${Protobuf_LITE_RELEASE_LIB}")
message(STATUS "Protobuf DLL debug: ${Protobuf_DLL_DEBUG}")
message(STATUS "Protobuf DLL release: ${Protobuf_DLL_RELEASE}")
message(STATUS "Protoc DLL debug: ${Protoc_DLL_DEBUG}")
message(STATUS "Protoc DLL release: ${Protoc_DLL_RELEASE}")
# -------------------------------
# c++
# -------------------------------
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp" "src/*.h")

add_executable(QQBot ${SOURCES} ${HEADERS})
source_group(TREE "${CMAKE_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADERS})

target_compile_definitions(QQBot PRIVATE UNICODE _UNICODE)
target_compile_definitions(QQBot PRIVATE PROTOBUF_USE_DLLS)

target_link_libraries(QQBot
    ${PYTHON_LIBNAME}
    debug   ${Protobuf_DEBUG_LIB}
    optimized ${Protobuf_RELEASE_LIB}
    debug   ${Protobuf_LITE_DEBUG_LIB}
    optimized ${Protobuf_LITE_RELEASE_LIB}
    debug   ${Protoc_DEBUG_LIB}
    optimized ${Protoc_RELEASE_LIB}
)

target_compile_options(QQBot PRIVATE $<$<CONFIG:Debug>:/ZI>)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Python")

foreach(source_file ${SOURCES} ${HEADERS})
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${source_file}")
    get_filename_component(dir "${rel_path}" DIRECTORY)
    string(REPLACE "/" "\\" group "${dir}")
    source_group("${group}" FILES "${source_file}")
endforeach()

# -------------------------------
# Protobuf-GENERATED
# -------------------------------
file(GLOB PROTO_FILES "${CMAKE_SOURCE_DIR}/proto/*.proto")

# 生成目录
set(PROTO_OUT_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${PROTO_OUT_DIR})

foreach(proto_file ${PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME_WE)
    set(pb_h "${PROTO_OUT_DIR}/${proto_name}.pb.h")
    set(pb_cc "${PROTO_OUT_DIR}/${proto_name}.pb.cc")
    set(pb_py "${PROTO_OUT_DIR}/${proto_name}_pb2.py")

    add_custom_command(
        OUTPUT ${pb_h} ${pb_cc} ${pb_py}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROTO_OUT_DIR}  
        COMMAND ${Protobuf_PROTOC_EXECUTABLE} -I ${CMAKE_SOURCE_DIR}/proto --cpp_out=${PROTO_OUT_DIR} ${proto_file}
        COMMAND ${Protobuf_PROTOC_EXECUTABLE} -I ${CMAKE_SOURCE_DIR}/proto --python_out=${PROTO_OUT_DIR} ${proto_file}
        MAIN_DEPENDENCY ${proto_file}
        VERBATIM
    )

    list(APPEND PROTO_SRCS ${pb_cc})
    list(APPEND PROTO_HDRS ${pb_h})
	list(APPEND GENERATED_PY_FILES ${pb_py})
endforeach()

target_include_directories(QQBot PRIVATE ${PROTO_OUT_DIR})
target_sources(QQBot PRIVATE ${PROTO_SRCS} ${PROTO_HDRS})

# -------------------------------
# file
# -------------------------------
add_custom_command(TARGET QQBot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
		"$<TARGET_FILE_DIR:QQBot>/Python"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PYTHON_DLL}" "$<TARGET_FILE_DIR:QQBot>"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_DIR}" "$<TARGET_FILE_DIR:QQBot>/Python"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_DIR}" "${CMAKE_BINARY_DIR}/Python"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${Protobuf_DLL_DEBUG},${Protobuf_DLL_RELEASE}>"
        "$<TARGET_FILE_DIR:QQBot>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<IF:$<CONFIG:Debug>,${Protoc_DLL_DEBUG},${Protoc_DLL_RELEASE}>"
        "$<TARGET_FILE_DIR:QQBot>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GENERATED_PY_FILES}" "$<TARGET_FILE_DIR:QQBot>/Python"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GENERATED_PY_FILES}" "${CMAKE_BINARY_DIR}/Python"
    COMMENT "Copying runtime files to output directory"
)