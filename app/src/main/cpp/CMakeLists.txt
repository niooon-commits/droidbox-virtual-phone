cmake_minimum_required(VERSION 3.22.1)

project("droidbox_native" CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directory pointing to virtual_android_os engine
set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../virtual_android_os)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ENGINE_DIR}/include
)

add_library(
    droidbox_native
    SHARED
    native-lib.cpp
    ${ENGINE_DIR}/src/virtual_os.cpp
    ${ENGINE_DIR}/src/namespace_isolator.cpp
    ${ENGINE_DIR}/src/binder_hook.cpp
    ${ENGINE_DIR}/src/virtual_fs.cpp
    ${ENGINE_DIR}/src/display_compositor.cpp
    ${ENGINE_DIR}/src/apk_manager.cpp
    ${ENGINE_DIR}/src/gms_core_bridge.cpp
    ${ENGINE_DIR}/src/input_system.cpp
)

find_library(log-lib log)
find_library(android-lib android)

target_link_libraries(
    droidbox_native
    ${log-lib}
    ${android-lib}
)
