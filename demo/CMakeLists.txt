project(demo LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/app/3thrd/singleton/singleton.cmake)

add_executable (demo-singleton-app
    ${CMAKE_SOURCE_DIR}/demo/demo-singleton-app.cpp
    ${SINGLEAPP_SRC})

target_include_directories(demo-singleton-app PUBLIC ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries (demo-singleton-app Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets)
