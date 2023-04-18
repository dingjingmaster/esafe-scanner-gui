cmake_minimum_required(VERSION 3.5)

project(dsip_sit LANGUAGES CXX)

set(DEBUG OFF)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(OS              "")
set(OS_ARCH         "")
set(RELEASE         "")
set(KERNEL_HEADER   "")
set(CUR_QT_VERSION  "")
set(ROOT_FS         "/")


set(IS_UOS          0)
set(IS_KYLIN        0)

set(IS_X86          0)
set(IS_ARM          0)

set(IS_ABC          0)
set(IS_UNIVERSAL    0)


set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(
        -w -g
        -DDEBUG=0
        -D QT_NO_KEYWORDS
        -D QT_MESSAGELOGCONTEXT
        -D APP_NAME=\\"dsip_sit\\"
        -D LOCAL_SOCKET_NAME=\\"dsip_sit\\"
        -D DB_PATH=\\"/usr/local/ultrasec/dat/db_task/EstDlpSEDataBase.db\\"
)

option(BUILD_DJ "是否开启手动编译" OFF)
if (NOT BUILD_DJ)
    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        MESSAGE(STATUS "current platform: Linux")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
        MESSAGE(STATUS "current platform: Windows")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        MESSAGE(STATUS "current platform: Mac OS X")
    else()
        MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
    endif()

    cmake_host_system_information(RESULT OS QUERY OS_NAME)
    cmake_host_system_information(RESULT RELEASE QUERY OS_RELEASE)

    execute_process(COMMAND uname -m OUTPUT_VARIABLE OS_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND bash -c "qmake --version | grep -i 'qt version' | awk '{print $4}' | tr '\n' ' '|sed 's/ //g'" OUTPUT_VARIABLE CUR_QT_VERSION)
    execute_process(COMMAND bash -c "cat /etc/os-release|grep -E '^NAME'|awk -F'=' '{print $2}'|sed 's/\"//g'|tr '\n' ' '|sed 's/ //g'" OUTPUT_VARIABLE DISTRIB)

    string(COMPARE EQUAL "${DISTRIB}" "uos" IS_UOS)
    string(COMPARE EQUAL "${DISTRIB}" "kylin" IS_KYLIN)

    string(COMPARE EQUAL "${OS_ARCH}" "aarch" IS_ARM)
    string(COMPARE EQUAL "${OS_ARCH}" "x86_64" IS_X86)
else ()
    option(DJ_IS_UOS   "is uos"    OFF)
    if (DJ_IS_UOS)
        set(IS_UOS 1)
    endif ()
    option(DJ_IS_KYLIN "is kylin"  OFF)
    if (DJ_IS_KYLIN)
        set(IS_KYLIN 1)
    endif ()
    option(DJ_IS_X86   ""  OFF)
    if (DJ_IS_X86)
        set(IS_X86 1)
    endif ()
    option(DJ_IS_ARM   ""  OFF)
    if (DJ_IS_ARM)
        set(IS_ARM 1)
    endif ()
    option(DJ_IS_ABC "" OFF)
    if (DJ_IS_ABC)
        set(IS_ABC 1)
    endif ()
    option(DJ_IS_UNIVERSAL "" OFF)
    if (DJ_IS_UNIVERSAL)
        set(IS_UNIVERSAL 1)
    endif ()

    option(DJ_OS "" "Unknown")
    set(OS ${DJ_OS})

    option(DJ_OS_ARCH "" "Unknown")
    set(OS_ARCH ${DJ_OS_ARCH})

    option(DJ_RELEASE "" "Unknown")
    set(RELEASE ${DJ_RELEASE})

    option(DJ_QT_VERSION "" "Unknown")
    set(CUR_QT_VERSION ${DJ_QT_VERSION})

    option(DJ_ROOT_FS "" "Unknown")
    set(ROOT_FS ${DJ_ROOT_FS})
    set(CMAKE_SYSROOT "${ROOT_FS}")
    add_definitions(--sysroot=${ROOT_FS})

    option(DJ_COMPILER"" "Unknown")
    if (${OS_ARCH} MATCHES aarch)
        message(STATUS "aarch")
        set(CMAKE_C_COMPILER "${DJ_COMPILER}/bin/arm-none-linux-gnueabihf-gcc")
        set(CMAKE_CXX_COMPILER "${DJ_COMPILER}/bin/arm-none-linux-gnueabihf-g++")
    elseif (${OS_ARCH} MATCHES x86_64)
        message(STATUS "x86_64")
        set(CMAKE_C_COMPILER "${DJ_COMPILER}/bin/gcc")
        set(CMAKE_CXX_COMPILER "${DJ_COMPILER}/bin/g++")
    endif ()
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

    set(CMAKE_PREFIX_PATH "${ROOT_FS}/usr/lib/${OS_ARCH}-linux-gnu/cmake")
    message(STATUS "${ROOT_FS}/usr/lib/${OS_ARCH}-linux-gnu/cmake")
endif ()


find_package (PkgConfig)
find_package(Qt5 COMPONENTS Core Network Widgets Gui DBus X11Extras REQUIRED)

pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)


if (IS_UOS)
    add_definitions(-D IS_UOS)
else(IS_KYLIN)
    add_definitions(-D IS_KYLIN)
endif ()

MESSAGE("")
MESSAGE(STATUS "OS name                 : ${OS}")
MESSAGE(STATUS "OS arch                 : ${OS_ARCH}")
MESSAGE(STATUS "OS release              : ${RELEASE}")
MESSAGE(STATUS "Qt Version              : ${CUR_QT_VERSION}")
MESSAGE(STATUS "Root Filesystem         : ${ROOT_FS}")
MESSAGE(STATUS "gcc                     : ${CMAKE_C_COMPILER}")
MESSAGE(STATUS "g++                     : ${CMAKE_CXX_COMPILER}")
MESSAGE("")
MESSAGE(STATUS "IS_UOS                  : ${IS_UOS}")
MESSAGE(STATUS "IS_KYLIN                : ${IS_KYLIN}")
MESSAGE(STATUS "IS_X86                  : ${IS_X86}")
MESSAGE(STATUS "IS_ARM                  : ${IS_ARM}")
MESSAGE(STATUS "IS_ABC                  : ${IS_ABC}")
MESSAGE(STATUS "IS_UNIVERSAL            : ${IS_UNIVERSAL}")
MESSAGE("")
MESSAGE(STATUS "Qt5 Core headers        : ${Qt5Core_INCLUDE_DIRS}")
MESSAGE(STATUS "Qt5 Network headers     : ${Qt5Network_INCLUDE_DIRS}")
MESSAGE(STATUS "Qt5 Widgets headers     : ${Qt5Widgets_INCLUDE_DIRS}")
MESSAGE(STATUS "Qt5 Gui headers         : ${Qt5Gui_INCLUDE_DIRS}")
MESSAGE(STATUS "Qt5 DBus headers        : ${Qt5DBus_INCLUDE_DIRS}")
MESSAGE(STATUS "Qt5 X11Extras headers   : ${Qt5X11Extras_INCLUDE_DIRS}")
MESSAGE(STATUS "gio headers             : ${GIO_INCLUDE_DIRS}")
MESSAGE("")

add_subdirectory(app)
#add_subdirectory(demo)
#add_subdirectory(tools)

