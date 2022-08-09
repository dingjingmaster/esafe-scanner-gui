#execute_process(COMMAND protoc
#    --cpp_out=${CMAKE_SOURCE_DIR}/app/utils
#    -I${CMAKE_SOURCE_DIR}/app/utils
#    message-with-fp.proto
#)

file(GLOB UTILS_SRC
    ${CMAKE_SOURCE_DIR}/app/utils/tools.h
    ${CMAKE_SOURCE_DIR}/app/utils/tools.cpp

    ${CMAKE_SOURCE_DIR}/app/utils/scan-task-helper.h
    ${CMAKE_SOURCE_DIR}/app/utils/scan-task-helper.cpp

    ${CMAKE_SOURCE_DIR}/app/utils/scan-result-helper.h
    ${CMAKE_SOURCE_DIR}/app/utils/scan-result-helper.cpp

    ${CMAKE_SOURCE_DIR}/app/utils/export-scan-result.h
    ${CMAKE_SOURCE_DIR}/app/utils/export-scan-result.cpp

#    ${CMAKE_SOURCE_DIR}/app/utils/message-with-fp.pb.h
#    ${CMAKE_SOURCE_DIR}/app/utils/message-with-fp.pb.cc

#    ${CMAKE_SOURCE_DIR}/app/utils/notify-to-filter.h
#    ${CMAKE_SOURCE_DIR}/app/utils/notify-to-filter.cpp
)
