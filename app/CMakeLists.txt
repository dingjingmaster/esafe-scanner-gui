project(app)

include(ui/ui.cmake)
include(db/db.cmake)
include(view/view.cmake)
include(style/style.cmake)
include(3thrd/3thrd.cmake)
include(utils/utils.cmake)
include(model/model.cmake)
include(widget/widget.cmake)
include(threads/threads.cmake)

FILE(GLOB MAIN_SRC ${CMAKE_SOURCE_DIR}/app/main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/app/
    ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})

add_executable(dsip_sit
    ${MAIN_SRC} ${CMAKE_SOURCE_DIR}/app/res.qrc
    ${SINGLEAPP_SRC} ${UI_SRC} ${VIEW_SRC} ${MODEL_SRC} ${UTILS_SRC} ${QGSETTINGS_SRC} ${STYLE_SRC} ${DB_SRC} ${WIDGET_SRC} ${THREADS_SRC})
#target_compile_options(dsip_sit PUBLIC -O0 -finput-charset=utf8 -fexec-charset=utf8)
target_link_libraries(dsip_sit Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets Qt5::DBus ${SQLITE3_LIBRARIES} ${GIO_LIBRARIES})

