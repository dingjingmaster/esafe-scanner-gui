project(app)

include(ui/ui.cmake)
include(3thrd/3thrd.cmake)

FILE(GLOB MAIN_SRC ${CMAKE_SOURCE_DIR}/app/main.cpp)

add_executable(esafe-scanner-gui ${CMAKE_SOURCE_DIR}/app/main.cpp
    ${SINGLEAPP_SRC} ${UI_SRC} ${CMAKE_SOURCE_DIR}/app/res.qrc)
target_link_directories(esafe-scanner-gui PUBLIC
    ${CMAKE_SOURCE_DIR}/app/
    ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}
)
target_link_libraries(esafe-scanner-gui Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets)
