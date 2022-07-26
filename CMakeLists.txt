project(esafe-scanner-gui LANGUAGES CXX)

cmake_minimum_required(VERSION 3.5)

set(DEBUG ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(
    -w
    -D QT_NO_KEYWORDS
    -D APP_NAME=\\"esafe-scanner-gui\\"
    -D LOCAL_SOCKET_NAME=\\"esafe-scanner-gui\\"
)

if(DEBUG)
    add_definitions(-D DB_PATH=\\"${CMAKE_SOURCE_DIR}/demo/scan\\")
else()
    add_definitions(-D DB_PATH=\\"\\")
endif()



find_package (PkgConfig)
find_package(Qt5 COMPONENTS Core Network Widgets Gui REQUIRED)

pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
pkg_check_modules(PROTOBUF REQUIRED protobuf)

add_subdirectory(app)
add_subdirectory(demo)

