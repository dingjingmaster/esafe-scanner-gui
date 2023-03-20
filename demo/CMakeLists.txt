project(demo LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/app/db/db.cmake)
include(${CMAKE_SOURCE_DIR}/app/view/view.cmake)
include(${CMAKE_SOURCE_DIR}/app/model/model.cmake)
include(${CMAKE_SOURCE_DIR}/app/utils/utils.cmake)
include(${CMAKE_SOURCE_DIR}/app/3thrd/singleton/singleton.cmake)

add_executable (demo-singleton-app
    ${CMAKE_SOURCE_DIR}/demo/demo-singleton-app.cpp
    ${SINGLEAPP_SRC})
target_include_directories(demo-singleton-app PUBLIC
    ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
target_link_libraries (demo-singleton-app
    Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets)


add_executable (demo-scanner-task-model
    ${CMAKE_SOURCE_DIR}/demo/demo-scanner-task-model.cpp
    ${MODEL_SRC} ${VIEW_SRC} ${UTILS_SRC} ${DB_SRC})
target_include_directories(demo-scanner-task-model PUBLIC
    ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS} ${PROTOBUF_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
target_link_libraries (demo-scanner-task-model
    Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets ${SQLITE3_LIBRARIES} ${GIO_LIBRARIES})

add_executable (demo-scan-task-bentch
    ${CMAKE_SOURCE_DIR}/demo/demo-scan-task-bentch.cpp
    ${MODEL_SRC} ${VIEW_SRC} ${UTILS_SRC} ${DB_SRC})
target_include_directories(demo-scan-task-bentch PUBLIC
    ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
target_link_libraries (demo-scan-task-bentch
    Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets ${SQLITE3_LIBRARIES} ${GIO_LIBRARIES})

add_executable (demo-scan-result-bentch
    ${CMAKE_SOURCE_DIR}/demo/demo-scan-result-bentch.cpp
    ${MODEL_SRC} ${VIEW_SRC} ${UTILS_SRC} ${DB_SRC})
target_include_directories(demo-scan-result-bentch PUBLIC
    ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
target_link_libraries (demo-scan-result-bentch
    Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets ${SQLITE3_LIBRARIES} ${GIO_LIBRARIES})



# add_executable (demo-server
#     ${CMAKE_SOURCE_DIR}/demo/server.cpp
#     ${UTILS_SRC} ${MODEL_SRC})
#
# target_include_directories(demo-server PUBLIC
#     ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS} ${PROTOBUF_INCLUDE_DIRS})
#target_link_libraries (demo-server
#     Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets ${SQLITE3_LIBRARIES} ${PROTOBUF_LIBRARIES})
