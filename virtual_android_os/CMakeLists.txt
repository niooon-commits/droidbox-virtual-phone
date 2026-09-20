cmake_minimum_required(VERSION 3.18.1)
project(VirtualAndroidEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags for Linux and Android NDK
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread -fPIC")

# Android NDK definitions
if(ANDROID)
    add_definitions(-D__ANDROID__ -DANDROID_PLATFORM_LEVEL=29)
    find_library(LOG_LIB log)
    find_library(ANDROID_LIB android)
    find_library(EGL_LIB EGL)
    find_library(GLES_LIB GLESv3)
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Header files
set(HEADERS
    include/virtual_os.hpp
    include/namespace_isolator.hpp
    include/binder_hook.hpp
    include/virtual_fs.hpp
    include/display_compositor.hpp
    include/apk_manager.hpp
    include/gms_core_bridge.hpp
    include/input_system.hpp
)

# Source files
set(SOURCES
    src/virtual_os.cpp
    src/namespace_isolator.cpp
    src/binder_hook.cpp
    src/virtual_fs.cpp
    src/display_compositor.cpp
    src/apk_manager.cpp
    src/gms_core_bridge.cpp
    src/input_system.cpp
    src/main.cpp
)

# Build static library for embedding into Android APK via JNI
add_library(virtual_android_engine_static STATIC ${SOURCES} ${HEADERS})

# Build shared library (.so) for JNI native loading
add_library(virtual_android_engine SHARED ${SOURCES} ${HEADERS})

# Build standalone CLI executable for daemon/testing (can run via Termux or adb shell)
add_executable(vphone_daemon src/main.cpp ${SOURCES} ${HEADERS})

if(ANDROID)
    target_link_libraries(virtual_android_engine
        ${LOG_LIB}
        ${ANDROID_LIB}
        ${EGL_LIB}
        ${GLES_LIB}
    )
    target_link_libraries(vphone_daemon
        ${LOG_LIB}
        ${ANDROID_LIB}
    )
endif()
