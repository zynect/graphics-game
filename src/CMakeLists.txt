SET(pwd ${CMAKE_CURRENT_LIST_DIR})

SET(src "")
AUX_SOURCE_DIRECTORY(${pwd} src)
add_executable(game ${src})
message(STATUS "game added ${src}")

target_link_libraries(game ${stdgl_libraries})
FIND_PACKAGE(JPEG REQUIRED)
TARGET_LINK_LIBRARIES(game ${JPEG_LIBRARIES})
