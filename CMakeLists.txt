cmake_minimum_required(VERSION 3.10)

project(QQBot VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(PYTHON_DIR "${CMAKE_SOURCE_DIR}/Python")  
set(DLL_DIR "${CMAKE_SOURCE_DIR}/dll")

set(INI_DIR "${CMAKE_SOURCE_DIR}/ini")
set(OUT_DIR1 "$<TARGET_FILE_DIR:QQBot>") # build/Debug/
set(OUT_DIR2 "${CMAKE_BINARY_DIR}")  # build

include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "D:/workspace/Python/Python39/include"
)
link_directories("${CMAKE_SOURCE_DIR}/libs")


add_definitions(-D_WINSOCKAPI_)
add_definitions(-D_CLASS_LOG_)


file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp" "src/*.h")

add_executable(QQBot ${SOURCES} ${HEADERS})
target_compile_definitions(QQBot PRIVATE UNICODE _UNICODE)
target_link_libraries(QQBot
    "python39_d.lib"  
)

add_custom_command(TARGET QQBot POST_BUILD
    # ---cp build/Debug/ ---
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL_DIR}/python3.dll" ${OUT_DIR1}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL_DIR}/python39.dll" ${OUT_DIR1}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_DIR}" "${OUT_DIR1}/Python"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${INI_DIR}" "${OUT_DIR1}/ini" 
    # --- cp build/ ---
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL_DIR}/python3.dll" ${OUT_DIR2}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL_DIR}/python39.dll" ${OUT_DIR2}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PYTHON_DIR}" "${OUT_DIR2}/Python"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${INI_DIR}" "${OUT_DIR2}/ini"
    COMMENT "Copying runtime files to build/Debug and build/"
)


foreach(source_file ${SOURCES} ${HEADERS})
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${source_file}")
    get_filename_component(dir "${rel_path}" DIRECTORY)
    string(REPLACE "/" "\\" group "${dir}")
    source_group("${group}" FILES "${source_file}")
endforeach()
