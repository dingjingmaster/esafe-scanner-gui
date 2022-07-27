project(app)

include(ui/ui.cmake)
include(view/view.cmake)
include(3thrd/3thrd.cmake)
include(utils/utils.cmake)
include(model/model.cmake)

FILE(GLOB MAIN_SRC ${CMAKE_SOURCE_DIR}/app/main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/app/
    ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS} ${PROTOBUF_INCLUDE_DIRS})

add_executable(esafe-scanner-gui
    ${MAIN_SRC} ${CMAKE_SOURCE_DIR}/app/res.qrc
    ${SINGLEAPP_SRC} ${UI_SRC} ${VIEW_SRC} ${MODEL_SRC} ${UTILS_SRC} ${QGSETTINGS_SRC})

target_link_libraries(esafe-scanner-gui
    Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets Qt5::DBus ${SQLITE3_LIBRARIES} ${GIO_LIBRARIES} ${PROTOBUF_LIBRARIES})

