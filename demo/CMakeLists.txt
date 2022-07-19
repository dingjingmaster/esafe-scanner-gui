project(demo LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/app/model/model.cmake)
include(${CMAKE_SOURCE_DIR}/app/3thrd/singleton/singleton.cmake)

add_executable (demo-singleton-app
    ${CMAKE_SOURCE_DIR}/demo/demo-singleton-app.cpp
    ${SINGLEAPP_SRC})

target_include_directories(demo-singleton-app PUBLIC ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries (demo-singleton-app Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets)


add_executable (demo-scanner-task-model
    ${CMAKE_SOURCE_DIR}/demo/demo-scanner-task-model.cpp
    ${MODEL_SRC})

target_include_directories(demo-scanner-task-model PUBLIC ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries (demo-scanner-task-model Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets)
