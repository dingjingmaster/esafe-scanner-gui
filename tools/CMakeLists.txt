include(${CMAKE_SOURCE_DIR}/app/3thrd/3thrd.cmake)

file(GLOB MD5_SRC
        ${CMAKE_SOURCE_DIR}/tools/md5/main.cpp

        ${CMAKE_SOURCE_DIR}/tools/md5/main-window.h
        ${CMAKE_SOURCE_DIR}/tools/md5/main-window.cpp
        )

add_executable(dsip_md5 ${MD5_SRC} ${CMAKE_SOURCE_DIR}/app/res.qrc ${SINGLEAPP_SRC})
target_link_libraries(dsip_md5 Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets Qt5::DBus ${GIO_LIBRARIES})
