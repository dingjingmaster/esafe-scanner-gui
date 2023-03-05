project(dsip_sit LANGUAGES CXX)

cmake_minimum_required(VERSION 3.5)

set(DEBUG OFF)
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
    -w -g
    -D QT_NO_KEYWORDS
    -D QT_MESSAGELOGCONTEXT
    -D APP_NAME=\\"dsip_sit\\"
    -D LOCAL_SOCKET_NAME=\\"dsip_sit\\"
)

if(DEBUG)
    add_definitions(-D DB_PATH=\\"${CMAKE_SOURCE_DIR}/demo/scan\\" -DDEBUG=1)
else()
    add_definitions(-DDEBUG=0 -D DB_PATH=\\"/usr/local/ultrasec/dat/db_task/EstDlpSEDataBase.db\\")
endif()



find_package (PkgConfig)
find_package(Qt5 COMPONENTS Core Network Widgets Gui DBus X11Extras REQUIRED)

pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
#pkg_check_modules(PROTOBUF REQUIRED protobuf)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "current platform: Linux ")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "current platform: Windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    MESSAGE(STATUS "current platform: Mac OS X")
else()
    MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

cmake_host_system_information(RESULT OS QUERY OS_NAME)
MESSAGE(STATUS "OS name: ${OS}")

cmake_host_system_information(RESULT RELEASE QUERY OS_RELEASE)
MESSAGE(STATUS "OS release: ${RELEASE}")

execute_process(COMMAND bash -c "qmake --version | grep -i 'qt version' | awk '{print $4}' | tr '\n' ' '|sed 's/ //g'" OUTPUT_VARIABLE CUR_QT_VERSION)
MESSAGE(STATUS "Qt Version: ${CUR_QT_VERSION}")

execute_process(COMMAND uname -m OUTPUT_VARIABLE OS_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
MESSAGE(STATUS "OS arch: '${OS_ARCH}'")

execute_process(COMMAND bash -c "cat /etc/os-release|grep -E '^NAME'|awk -F'=' '{print $2}'|sed 's/\"//g'|tr '\n' ' '|sed 's/ //g'" OUTPUT_VARIABLE DISTRIB)
MESSAGE(STATUS "OS distribute: '${DISTRIB}'")

message(STATUS "Kernel headers: ${KERNEL_HEADER}")

string(COMPARE EQUAL "${DISTRIB}" "uos" IS_UOS)
string(COMPARE EQUAL "${DISTRIB}" "kylin" IS_KYLIN)

if (IS_UOS)
    add_definitions(-D IS_UOS)
else(IS_KYLIN)
    add_definitions(-D IS_KYLIN)
endif ()

add_subdirectory(app)
#add_subdirectory(demo)
#add_subdirectory(tools)

